





#include "yglwidget.h"

YGLWidget::YGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m_nv_image  = nullptr;
    m_glProgram = nullptr;
}

YGLWidget::~YGLWidget()
{
    makeCurrent();

    if (NULL != m_glProgram) {
        delete m_glProgram;
    }
    doneCurrent();
}

void YGLWidget::playback(YImage *image)
{
    m_nv_image = image;
    update();
}

void YGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertices;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = vertices;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
            "   //vec3 color = texture2D(texture, texc.st).rgb;\n"
            "	gl_FragColor = texture2D(texture, texc.st).bgra;\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    m_glProgram = new QOpenGLShaderProgram();
    m_glProgram->addShader(vshader);
    m_glProgram->addShader(fshader);
    m_glProgram->link();

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_textures);
    glBindTexture(GL_TEXTURE_2D, m_textures);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    m_glProgram->setUniformValue("texture", m_textures);
}

void YGLWidget::paintGL()
{
    if (nullptr == m_nv_image) {
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textures);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nv_image->getPixelWidth(), m_nv_image->getPixelHeight(),
                 0, GL_RGB, GL_UNSIGNED_BYTE, m_nv_image->getData());
#if 0
    glBegin(GL_QUADS);
    glVertex2f(-1, -1); glTexCoord2f(1, 1);
    glVertex2f(1, -1); glTexCoord2f(1, 0);
    glVertex2f(1, 1); glTexCoord2f(0, 0);
    glVertex2f(-1, 1); glTexCoord2f(0, 1);
    glEnd();

    glDrawArrays(GL_TRIANGLE_STRIP, 0 ,4);
#else
    m_glProgram->bind();

    float vertices_value[] = {-1, -1, 1, -1, -1, 1, 1, 1};
    GLint vertices_location = m_glProgram->attributeLocation("vertices");
    m_glProgram->enableAttributeArray(vertices_location);
    m_glProgram->setAttributeArray(vertices_location, GL_FLOAT, vertices_value, 2);

    float texc_value[] = {0, 1, 1, 1, 0, 0, 1, 0};
    GLint texCoord_location = m_glProgram->attributeLocation("texCoord");
    m_glProgram->enableAttributeArray(texCoord_location);
    m_glProgram->setAttributeArray(texCoord_location, GL_FLOAT, texc_value, 2);

    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();

    m_glProgram->disableAttributeArray(vertices_location);
    m_glProgram->disableAttributeArray(texCoord_location);
    m_glProgram->release();
#endif

    glBindTexture(GL_TEXTURE_2D, 0);
}

void YGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}
