#ifndef __SHADOWMAP__
#define __SHADOWMAP__

#include <tucano.hpp>
#include <camera.hpp>

using namespace Tucano;

namespace Effects
{

/**
 * @brief Renders a mesh using a Edge shader.
 */
class ShadowMap : public Effect
{

private:

    /// Edge Shader
    Shader depth_shader;

    /// Save coord, normal and color to FBO
    //Shader deferred_shader;

    /// Compose Shader
    Shader compose_shader;

	/// Default color
	Eigen::Vector4f default_color;

    /// Framebuffer to store coord/normal buffer
    Framebuffer* fbo;

    //Mesh quad;

    int depthTextureID, normalTextureID, colorTextureID;

    bool hasFbo;

public:

    /**
     * @brief Default constructor.
     */
    ShadowMap (void)
    {
		default_color << 0.7, 0.7, 0.7, 1.0;
        depthTextureID = 0;
        normalTextureID = 1;
        colorTextureID = 2;
        hasFbo = false;
        fbo = NULL;
    }

    /**
     * @brief Default destructor
     */
    virtual ~ShadowMap (void) {}

    /**
     * @brief Load and initialize shaders
     */
    virtual void initialize (void)
    {
        // searches in default shader directory (/shaders) for shader files phongShader.(vert,frag,geom,comp)
        loadShader(depth_shader, "depthLight");
        loadShader(compose_shader, "composeShadowmap");
        
        //quad.createQuad();
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
    void mapShadows (Mesh& mesh, const Trackball& camera, const Trackball& light)
    {

        // Bind buffer to store coord, normal and color information
        fbo->clearAttachments();
        fbo->bindRenderBuffers(depthTextureID, normalTextureID, colorTextureID);

        depth_shader.bind();
        depth_shader.setUniform("projectionMatrix", light.getProjectionMatrix());
        depth_shader.setUniform("modelMatrix",mesh.getModelMatrix());
        depth_shader.setUniform("viewMatrix", light.getViewMatrix());

        mesh.setAttributeLocation(depth_shader);
        mesh.render();

        depth_shader.unbind();
        fbo->unbind();
        //fbo->clearDepth();

    }

    /** * @brief Render the mesh given a camera and light, using a Edge shader 
     * @param mesh Given mesh
     * @param camera Given camera 
     * @param lightTrackball Given light camera 
     */
    void render (Tucano::Mesh& mesh, const Tucano::Trackball& camera, const Tucano::Trackball& light)
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
        mapShadows(mesh, camera, light);

        //second pass

        // sets all uniform variables for the phong shader
        compose_shader.bind();
        
        compose_shader.setUniform("shadowMap", fbo->bindAttachment(depthTextureID));
        compose_shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
        compose_shader.setUniform("modelMatrix",mesh.getModelMatrix());
        compose_shader.setUniform("viewMatrix", camera.getViewMatrix());
        compose_shader.setUniform("lightViewMatrix", light.getViewMatrix());
        compose_shader.setUniform("default_color", default_color);
        compose_shader.setUniform("lightProjectionMatrix", light.getProjectionMatrix());
        compose_shader.setUniform("lightViewMatrix", light.getViewMatrix());

        mesh.render();

        compose_shader.unbind();
        fbo->unbind();


    }


};

}


#endif
