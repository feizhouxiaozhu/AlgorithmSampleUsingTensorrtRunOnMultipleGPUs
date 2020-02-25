





#ifndef YGLWIDGET_H
#define YGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "utils/yimage.h"

class YGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    YGLWidget(QWidget* parent = Q_NULLPTR);
    ~YGLWidget();

public slots:
    void playback(YImage *image);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
    GLuint                m_textures;
    YImage               *m_nv_image;
    QOpenGLShaderProgram *m_glProgram;
};

#endif // YGLWIDGET_H
