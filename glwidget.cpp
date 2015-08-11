#include "glwidget.hpp"
#include <QDebug>

void GLWidget::initialize (void)
{
    // initialize the shader effect
    //edge.setShadersDir("../shaders/");
    //edge.initialize();
    shadowmap.setShadersDir("../shaders/");
    shadowmap.initialize();
    //voxelight.setShadersDir("../shaders/");
    //voxelight.initialize();

    // initialize the widget, camera and light trackball, and opens default mesh
    Tucano::QtTrackballWidget::initialize();
    //Tucano::QtTrackballWidget::openMesh("../../resources/models/rapanui_160k_retry2.obj");
    Tucano::QtTrackballWidget::openMesh("../../resources/models/sponza_ok.ply");
}

void GLWidget::paintGL (void)
{
    makeCurrent();

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //edge.render(mesh, camera, light_trackball);
    //voxelight.render(mesh, camera, light_trackball);
    shadowmap.render(mesh, camera, light_trackball);

    camera.render();
}

void GLWidget::keyPressEvent (QKeyEvent * key){
    setFocus();
    int pressed = key->key();
    int modifiers = key->modifiers();

    switch(pressed)
    {
        case Qt::Key_F5:
            //edge.reloadShaders();
            //voxelight.reloadShaders();
            shadowmap.reloadShaders();
            break;
        default:
            QGLWidget::keyPressEvent(key);

    }

}