#ifndef OPENGLTEXTUREPAINTER_H
#define OPENGLTEXTUREPAINTER_H

#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <map>
#include "sph_vector2.h"
#include <QList>
#include <iostream>
#include <QApplication>
#include "spherror.h"
#include "openglprogram.h"
#include "sph_vector3.h"
#include "sph_vector4.h"
#include "sph_matrix4x4.h"

class OpenGLTexturePainter
{
public:

    QString m_AssetDir;

    inline OpenGLTexturePainter()
    {

    }


    GLuint m_RenderTarget;
    int m_RenderTargetWidth;
    int m_RenderTargetHeight;

    inline void UpdateRenderTargetProperties(GLuint render_target, int width, int height)
    {
        m_RenderTarget = render_target;
        m_RenderTargetWidth = width;
        m_RenderTargetHeight = height;

    }

    inline void LoadShaders()
    {

    }

    inline void Destroy()
    {

    }
};

#endif // OPENGLTEXTUREPAINTER_H
