#ifndef OPENGLCLTEXTURE_H
#define OPENGLCLTEXTURE_H


#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "OpenGLUtil.h"

#include <QList>
#include <QString>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "map.h"
#include "qimage.h"
#include "QGLWidget"

class OpenGLCLTexture
{
public:
    QImage m_Image;

    cl::ImageGL m_texcl;
    cl::size_t<3> m_dims;
    GLuint m_FramebufferName;
    GLuint m_texgl;
    bool m_IsFrambuffer;

public:
    OpenGLCLTexture();

    inline int Create2D(cl::Context &c, int width, int height,int component = GL_RGBA, int datatype = GL_FLOAT, int format =GL_RGBA, bool render_target = false)
    {

        int errCode = 0;
        m_texgl = createTexture2D(width,height);
        m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = width;
        m_dims[1] = height;
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int Create2DRF32Linear(cl::Context &c, int width, int height,float value_init = 0, bool render_target = false)
    {
        std::vector<float> data(width * height , value_init);

        int errCode = 0;
        m_texgl = createTexture2DLinear(width,height,data.data(),GL_RED,GL_FLOAT,GL_R32F);
        m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = width;
        m_dims[1] = height;
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int Create2DRF32(cl::Context &c, int width, int height,float value_init = 0, bool render_target = false, bool use_cl = true)
    {
        std::vector<float> data(width * height , value_init);

        int errCode = 0;
        m_texgl = createTexture2D(width,height,data.data(),GL_RED,GL_FLOAT,GL_R32F);
        if(use_cl)
        {
            m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        }
        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = width;
        m_dims[1] = height;
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int FillData2DRF32(void * data)
    {
        glad_glBindTexture(GL_TEXTURE_2D, this->m_texgl);
        glad_glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_dims[0],m_dims[1],GL_RED,GL_FLOAT,data);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        return 0;
    }

    inline int Create2DFromFile(QString path)
    {


        bool load = m_Image.load(path);
        if(!load)
        {
            SPH_ERROR("Could not load texture " + path);
            return 1;
        }

        m_Image = QGLWidget::convertToGLFormat(m_Image);



        glad_glEnable(GL_TEXTURE_2D); // Enable texturing

        glad_glActiveTexture(GL_TEXTURE0);
        glad_glGenTextures(1, &m_texgl); // Obtain an id for the texture
        glad_glBindTexture(GL_TEXTURE_2D, m_texgl); // Set as the current texture

        glad_glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        m_dims[0] = m_Image.width();
        m_dims[1] = m_Image.height();
        m_dims[2] = 0;

        glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Image.width(), m_Image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Image.bits());

        glad_glGenerateMipmap(GL_TEXTURE_2D);

        glad_glBindTexture(GL_TEXTURE_2D, m_texgl);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glad_glBindTexture(GL_TEXTURE_2D, 0);
        return 0;
    }

    inline int Create2DFromColor(cl::Context &c, int r, int g, int b, int a = 255)
    {



        return 0;
    }

    inline int Create2DFromMap(cl::Context &c, cTMap * map, bool render_target = false, bool flipv = false)
    {

        int errCode = 0;

        if(flipv)
        {
            std::vector<float> data(map->nrCols() * map->nrRows() , 0.0);

            FOR_ROW_COL_MV(map)
            {
                data[r * map->nrCols() + c] = map->data[map->nrRows() -1 -r][ c];
            }
            m_texgl = createTexture2D(map->nrCols(),map->nrRows(), data.data(),GL_RED,GL_FLOAT,GL_R32F);

        }else
        {
            m_texgl = createTexture2D(map->nrCols(),map->nrRows(), map->data.data(),GL_RED,GL_FLOAT,GL_R32F);
        }

        glad_glFinish();
        try
        {
            std::cout << "create CL Image " << (&c) << std::endl;
            m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        }catch(...)
        {
            std::cout<<"Error converting GL Image to CL Texture, is OpenGL Device equal to OpenCL Device?"<<errCode<<std::endl;
            return 250;
        }

        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = map->nrCols();
        m_dims[1] = map->nrRows();
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int Create2DFromData(cl::Context &c, float * data, int width, int height)
    {



        return 0;
    }
    inline int Create2DFromData(cl::Context &c, char * data, int width, int height)
    {



        return 0;
    }



    inline void Destroy()
    {
        glad_glDeleteTextures(1,&m_texgl);
    }
};

#endif // OPENGLCLTEXTURE_H
