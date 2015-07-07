/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EDGE__
#define __EDGE__

#include <tucano.hpp>
#include <camera.hpp>

using namespace Tucano;

namespace Effects
{

/**
 * @brief Renders a mesh using a Edge shader.
 */
class Edge : public Effect
{

private:

    /// Edge Shader
    Shader edge_shader;

    /// Save coord, normal and color to FBO
    Shader deferred_shader;

    /// Compose Shader
    Shader compose_shader;

	/// Default color
	Eigen::Vector4f default_color;

    /// Framebuffer to store coord/normal buffer
    Framebuffer* fbo;

    Mesh quad;

    int depthTextureID, normalTextureID, colorTextureID, edgeTextureID;

    bool hasFbo;

public:

    /**
     * @brief Default constructor.
     */
    Edge (void)
    {
		default_color << 0.7, 0.7, 0.7, 1.0;
        depthTextureID = 0;
        normalTextureID = 1;
        colorTextureID = 2;
        edgeTextureID = 3;
        hasFbo = false;
        fbo = NULL;
    }

    /**
     * @brief Default destructor
     */
    virtual ~Edge (void) {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files phongShader.(vert,frag,geom,comp)
        loadShader(edge_shader, "edge");
        loadShader(compose_shader, "compose");
        loadShader(deferred_shader, "viewspacebuffer");
        
        quad.createQuad();
    }

	/**
	* @brief Sets the default color, usually used for meshes without color attribute
	*/
	void setDefaultColor ( Eigen::Vector4f& color )
	{
		default_color = color;
	}

    /**
     * @brief First pass of the Screen Space Edge Rendering, writes coords, normals and colors to a buffer.
     * @param mesh Mesh to be rendered.
     * @param camera_trackball A pointer to the camera trackball object.
     * @param light_trackball A pointer to the light trackball object.
     */
    void createViewSpaceBuffer (Mesh& mesh, const Trackball& camera_trackball, const Trackball& light_trackball)
    {

        // Bind buffer to store coord, normal and color information
        fbo->clearAttachments();
        fbo->bindRenderBuffers(depthTextureID, normalTextureID, colorTextureID);

        deferred_shader.bind();
        deferred_shader.setUniform("projectionMatrix", camera_trackball.getProjectionMatrix());
        deferred_shader.setUniform("modelMatrix",mesh.getModelMatrix());
        deferred_shader.setUniform("viewMatrix", camera_trackball.getViewMatrix());
        deferred_shader.setUniform("lightViewMatrix", light_trackball.getViewMatrix());
        deferred_shader.setUniform("has_color", mesh.hasAttribute("in_Color"));

        mesh.setAttributeLocation(deferred_shader);
        mesh.render();

        deferred_shader.unbind();
        fbo->unbind();
        fbo->clearDepth();

    }

    /** * @brief Render the mesh given a camera and light, using a Edge shader 
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void render (Tucano::Mesh& mesh, const Tucano::Trackball& camera, const Tucano::Trackball& lightTrackball)
    {

        Eigen::Vector4f viewport = camera.getViewport();
        Eigen::Vector2i viewport_size = camera.getViewportSize();
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        // check if viewport was modified, if so, regenerate fbo
        if (!hasFbo || fbo->getWidth() != viewport_size[0] || fbo->getHeight() != viewport_size[1])
        {
            if (fbo)
                delete fbo;
            fbo = new Framebuffer(viewport_size[0], viewport_size[1], 4, GL_TEXTURE_2D, GL_RGBA32F, GL_RGBA, GL_FLOAT);
            hasFbo = true;
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(1.0, 1.0, 1.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // first pass
        createViewSpaceBuffer (mesh, camera, lightTrackball);

        //second pass
        fbo->bindRenderBuffer(edgeTextureID);
        edge_shader.bind();

        // sets all uniform variables for the phong shader
        
        edge_shader.setUniform("normals", fbo->bindAttachment(normalTextureID));
        edge_shader.setUniform("depth", fbo->bindAttachment(depthTextureID));
        
        quad.render();

        edge_shader.unbind();
        fbo->unbind();

        //third pass (composite)
        compose_shader.bind();
        
        compose_shader.setUniform("edges", fbo->bindAttachment(edgeTextureID));
        compose_shader.setUniform("colors", fbo->bindAttachment(colorTextureID));
        quad.render();

        compose_shader.unbind();
        fbo->unbind();


    }


};

}


#endif
