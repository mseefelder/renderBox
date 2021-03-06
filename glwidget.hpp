#ifndef __GLWIDGET__
#define __GLWIDGET__

#include <GL/glew.h>

#include <phongshader.hpp>
#include <utils/qttrackballwidget.hpp>
#include "shadowmap.hpp"

using namespace std;

class GLWidget : public Tucano::QtTrackballWidget
{
    Q_OBJECT

public:

    explicit GLWidget(QWidget *parent) : Tucano::QtTrackballWidget(parent) {}
    ~GLWidget() {}
    
    /**
     * @brief Initializes the shader effect
     * @todo remove hard coded mesh file!!
     */
    void initialize();

    /**
     * Repaints screen buffer.
     **/
    virtual void paintGL();




signals:
    
public slots:
    
private:
    //Edge effect
    //Effects::Edge edge;

    //Shadowmapping
    Effects::ShadowMap shadowmap;

    //Voxel shadowing effect
    //Effects::Voxelight voxelight;

protected:

    void keyPressEvent (QKeyEvent * key);
    
};

#endif // GLWIDGET
