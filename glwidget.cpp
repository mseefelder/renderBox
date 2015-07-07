#include "glwidget.hpp"
#include <QDebug>

void GLWidget::initialize (void)
{
    // initialize the shader effect
    edge.setShadersDir("../shaders/");
    edge.initialize();

    // initialize the widget, camera and light trackball, and opens default mesh
    Tucano::QtTrackballWidget::initialize();
    Tucano::QtTrackballWidget::openMesh("../../resources/tucano/samples/models/toy.ply");
}

void GLWidget::paintGL (void)
{
    makeCurrent();

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    edge.render(mesh, camera, light_trackball);

    camera.render();
}

void GLWidget::keyPressEvent (QKeyEvent * key){
    setFocus();
    int pressed = key->key();
    int modifiers = key->modifiers();

    switch(pressed)
    {
        case Qt::Key_F5:
            edge.reloadShaders();
            break;
        default:
            QGLWidget::keyPressEvent(key);

    }

}