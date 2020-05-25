#ifndef OPENGLSHAPEPAINTER_H
#define OPENGLSHAPEPAINTER_H

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
#include "math.h"
#include "sphcolorgradient.h"

class OpenGLShapePainter
{
public:


    QString m_AssetDir;

    inline OpenGLShapePainter()
    {

        m_AssetDir = QCoreApplication::applicationDirPath()+"/kernels/";


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

    GLuint VAO, VBO;

    OpenGLProgram * m_GLProgram_DrawPolygon;
    OpenGLProgram * m_GLProgram_DrawPolygonGradient;
    OpenGLProgram * m_GLProgram_DrawLine;

    inline void LoadShaders()
    {
        m_GLProgram_DrawLine = new OpenGLProgram();
        m_GLProgram_DrawLine->LoadProgram(m_AssetDir+ "UILineDraw.vert", m_AssetDir+ "UILineDraw.frag");

        m_GLProgram_DrawPolygon = new OpenGLProgram();
        m_GLProgram_DrawPolygon->LoadProgram(m_AssetDir+ "UIPolygonDraw.vert", m_AssetDir+ "UIPolygonDraw.frag");

        m_GLProgram_DrawPolygonGradient = new OpenGLProgram();
        m_GLProgram_DrawPolygonGradient->LoadProgram(m_AssetDir+ "UIGradientDraw.vert", m_AssetDir+ "UIGradientDraw.frag");


        glad_glGenVertexArrays(1, &VAO);
        glad_glGenBuffers(1, &VBO);
        glad_glBindVertexArray(VAO);
        glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glad_glEnableVertexAttribArray(0);
        glad_glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        glad_glBindVertexArray(0);


    }

    inline void DrawPoints()
    {

    }

    inline void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3,SPHVector4 color)
    {

    }

    inline void DrawSquareRot(float xmid, float ymid, float width, float height, float rotation,SPHVector4 color)
    {


    }


    inline void DrawPolygonGradient(float * x, float *y, int length,SPHColorGradient * grad, SPHVector2 dir = SPHVector2(0.0,1.0))
    {

        length = 3*(length / 3);
        if(!(length > 3))
        {
            return;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);

        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawPolygonGradient->m_program);

        SPHMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform1f(glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program, "alpha"), 1.0);


        for(int i = 0; i <SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            QString is = QString::number(i);
            int colorstop_i_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop_"+ is).toStdString().c_str());
            int colorstop_ci_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop_c"+ is).toStdString().c_str());

            if(i < grad->m_Gradient_Stops.length())
            {
                glad_glUniform1f(colorstop_i_loc,grad->m_Gradient_Stops.at(i));
                ColorF c = grad->m_Gradient_Colors.at(i);
                glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
            }else {
                glad_glUniform1f(colorstop_i_loc,1e30f);
                glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
            }
        }


        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        float x_mindot = x[0];
        float y_mindot = y[0];
        float mindot = 1e30f;
        float maxdot = -1e30f;

        for(int i = 0; i < length; i++)
        {
            float dot = SPHVector2(x[i],y[i]).dot(dir);
            if(dot < mindot)
            {
                mindot = dot;
                x_mindot = x[i];
                y_mindot = y[i];
            }
            if(dot > maxdot)
            {
                maxdot = dot;
            }
        }

        for(int i = 0; i < length; i = i+6)
        {
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, 0);

            if( i + 3 <  length)
            {
                // Update VBO for each character
                GLfloat vertices[6][4] = {
                    { x[i],    y[i],        std::fabs(SPHVector2(x[i]-x_mindot,y[i]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 },
                    { x[i+1],  y[i+1],      std::fabs(SPHVector2(x[i+1]-x_mindot,y[i+1]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },
                    { x[i+2],  y[i+2],      std::fabs(SPHVector2(x[i+2]-x_mindot,y[i+2]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },

                    { x[i+3],  y[i+3],      std::fabs(SPHVector2(x[i+3]-x_mindot,y[i+3]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 },
                    { x[i+4],  y[i+4],      std::fabs(SPHVector2(x[i+4]-x_mindot,y[i+4]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },
                    { x[i+5],  y[i+5],      std::fabs(SPHVector2(x[i+5]-x_mindot,y[i+5]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 }
                  };

               // Update content of VBO memory
               glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
               glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
               glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            }else {
                // Update VBO for each character
                GLfloat vertices[6][4] = {
                    { x[i],    y[i],        std::fabs(SPHVector2(x[i]-x_mindot,y[i]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 },
                    { x[i+1],  y[i+1],      std::fabs(SPHVector2(x[i+1]-x_mindot,y[i+1]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },
                    { x[i+2],  y[i+2],      std::fabs(SPHVector2(x[i+2]-x_mindot,y[i+2]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },

                    { 0.0f,  0.0f,      0.0, 0.0 },
                    { 0.0f,  0.0f,      1.0, 1.0 },
                    { 0.0f,  0.0f,      1.0, 0.0 }
                  };

               // Update content of VBO memory
               glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
               glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
               glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

            }

            // Render quad
            if( i + 3 <  length)
            {
                glad_glDrawArrays(GL_TRIANGLES, 0, 6);
            }else {
                glad_glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;
    }


    inline void DrawSquare(float xlt, float ylt, float width, float height,SPHVector4 color)
    {
        float * x = new float[6];
        float * y = new float[6];

        x[0] = xlt;
        y[0] = ylt + height;
        x[1] = xlt;
        y[1] = ylt;
        x[2] = xlt + width;
        y[2] = ylt;

        x[3] = xlt + width;
        y[3] = ylt;
        x[4] = xlt + width;
        y[4] = ylt + height;
        x[5] = xlt;
        y[5] = ylt + height;

        DrawPolygon(x,y,6,color);

        delete[] x;
        delete[] y;
    }

    inline void DrawSquareGradient(float xlt, float ylt, float width, float height, SPHColorGradient * grad, SPHVector2 dir = SPHVector2(0,1.0))
    {
        float * x = new float[6];
        float * y = new float[6];

        x[0] = xlt;
        y[0] = ylt + height;
        x[1] = xlt;
        y[1] = ylt;
        x[2] = xlt + width;
        y[2] = ylt;

        x[3] = xlt + width;
        y[3] = ylt;
        x[4] = xlt + width;
        y[4] = ylt + height;
        x[5] = xlt;
        y[5] = ylt + height;

        DrawPolygonGradient(x,y,6,grad,dir);

        delete[] x;
        delete[] y;

    }

    inline void DrawSquareLine(float xlt, float ylt, float width, float height, float lwidth, SPHVector4 color)
    {
        float * x = new float[5];
        float * y = new float[5];

        x[0] = xlt;
        y[0] = ylt;
        x[1] = xlt + width;
        y[1] = ylt;
        x[2] = xlt + width;
        y[2] = ylt + height;

        x[3] = xlt;
        y[3] = ylt + height;
        x[4] = xlt;
        y[4] = ylt;

        DrawLines(x,y,lwidth,5,color);

        delete[] x;
        delete[] y;
    }

    inline void DrawLine(float x1, float y1, float x2, float y2, float lwidth, SPHVector4 color)
    {
        float * x = new float[2];
        float * y = new float[2];

        x[0] = x1;
        y[0] = y1;
        x[1] = x2;
        y[1] = y2;

        DrawLines(x,y,lwidth,2,color);

        delete[] x;
        delete[] y;
    }

    inline void DrawSquare(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,SPHVector4 color)
    {


    }

    inline void DrawPolygon(float * x, float *y, int length,SPHVector4 color)
    {

        length = 3*(length / 3);
        if(!(length > 3))
        {
            return;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawPolygon->m_program);

        SPHMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygon->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawPolygon->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        for(int i = 0; i < length; i = i+6)
        {
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, 0);

            if( i + 3 <  length)
            {
                // Update VBO for each character
                GLfloat vertices[6][4] = {
                    { x[i],    y[i],        0.0, 0.0 },
                    { x[i+1],  y[i+1],       0.0, 1.0 },
                    { x[i+2],  y[i+2],       1.0, 1.0 },

                    { x[i+3],  y[i+3],      0.0, 0.0 },
                    { x[i+4],  y[i+4],      1.0, 1.0 },
                    { x[i+5],  y[i+5],      1.0, 0.0 }
                  };

               // Update content of VBO memory
               glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
               glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
               glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            }else {
                // Update VBO for each character
                GLfloat vertices[6][4] = {
                    { x[i],    y[i],        0.0, 0.0 },
                    { x[i+1],  y[i+1],       0.0, 1.0 },
                    { x[i+2],  y[i+2],       1.0, 1.0 },

                    { 0.0f,  0.0f,      0.0, 0.0 },
                    { 0.0f,  0.0f,      1.0, 1.0 },
                    { 0.0f,  0.0f,      1.0, 0.0 }
                  };

               // Update content of VBO memory
               glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
               glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
               glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

            }

            // Render quad
            if( i + 3 <  length)
            {
                glad_glDrawArrays(GL_TRIANGLES, 0, 6);
            }else {
                glad_glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;
    }

    inline void DrawLinesInduvidual(float * x, float *y, float width, int length, SPHVector4 color)
    {
        if(!(length > 1))
        {
            return;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawLine->m_program);

        SPHMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        for(int i = 0; i < length-1; i = i+2)
        {
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, 0);

                float dx = x[i] - x[i+1];
                float dy = y[i] - y[i+1];

                float l = sqrt(dx*dx + dy*dy);
                dx = dx/l;
                dy = dy/l;
                if(l > 0.01)
                {
                    float dxe = 0.5f * width *(-dy);
                    float dye = 0.5f * width *(dx);

                    // Update VBO for each character
                    GLfloat vertices[6][4] = {
                        { x[i] - dxe,    y[i] - dye,        0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,       1.0, 1.0 },
                        { x[i+1] - dxe,  y[i+1] - dye,       0.0, 1.0 },

                        { x[i+1] + dxe,  y[i+1] + dye,      1.0, 1.0},
                        { x[i+1] - dxe,  y[i+1] - dye,      0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,      1.0, 0.0 }

                    };

                    // Update content of VBO memory
                    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


                    // Render quad
                    glad_glDrawArrays(GL_TRIANGLES, 0, 6);
                }

        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;


    }

    inline void DrawLines(float * x, float *y, float width, int length, SPHVector4 color)
    {
        if(!(length > 1))
        {
            return;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawLine->m_program);

        SPHMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        for(int i = 0; i < length-1; i = i+1)
        {
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, 0);

                float dx = x[i] - x[i+1];
                float dy = y[i] - y[i+1];

                float l = sqrt(dx*dx + dy*dy);
                dx = dx/l;
                dy = dy/l;
                if(l > 0.01)
                {
                    float dxe = 0.5f * width *(-dy);
                    float dye = 0.5f * width *(dx);

                    // Update VBO for each character
                    GLfloat vertices[6][4] = {
                        { x[i] - dxe,    y[i] - dye,        0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,       1.0, 1.0 },
                        { x[i+1] - dxe,  y[i+1] - dye,       0.0, 1.0 },

                        { x[i+1] + dxe,  y[i+1] + dye,      1.0, 1.0},
                        { x[i+1] - dxe,  y[i+1] - dye,      0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,      1.0, 0.0 }

                    };

                    // Update content of VBO memory
                    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


                    // Render quad
                    glad_glDrawArrays(GL_TRIANGLES, 0, 6);
                }

        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;

    }

    inline void DrawRegularNGon(float x, float y, float r, int npoints, SPHVector4 color)
    {

        std::vector<float> v_x;
        std::vector<float> v_y;

        for(int i = 0; i < npoints; i++)
        {
            float thetai = float(i) * 2.0f * 3.14159f/((float)(npoints));
            float thetaip = float(i+1) * 2.0f * 3.14159f/((float)(npoints));

            float x_i = x + r*sin(thetai);
            float y_i = y + r*cos(thetai);

            float x_ip = x + r*sin(thetaip);
            float y_ip = y + r*cos(thetaip);

            bool connect = (i > 0);

            v_x.push_back(x_i);
            v_y.push_back(y_i);
            v_x.push_back(x_ip);
            v_y.push_back(y_ip);
            v_x.push_back(x);
            v_y.push_back(y);
        }

        DrawPolygon(v_x.data(),v_y.data(),v_x.size(),color);

    }

    inline void DrawRegularNGonOutline(float x, float y, float r, float width, int npoints, SPHVector4 color)
    {
        std::vector<float> v_x;
        std::vector<float> v_y;

        for(int i = 0; i < npoints; i++)
        {
            float thetai = float(i) * 2.0f * 3.14159f/((float)(npoints));
            float thetaip = float(i+1) * 2.0f * 3.14159f/((float)(npoints));

            float x_i = x + r*sin(thetai);
            float y_i = y + r*cos(thetai);

            float x_ip = x + r*sin(thetaip);
            float y_ip = y + r*cos(thetaip);

            bool connect = (i > 0);

            v_x.push_back(x_i);
            v_y.push_back(y_i);
            v_x.push_back(x_ip);
            v_y.push_back(y_ip);
        }

        DrawLines(v_x.data(),v_y.data(),v_x.size(),width,color);

    }

    inline void Destroy()
    {
        //delete shaders and buffer object


    }

};

#endif // OPENGLSHAPEPAINTER_H
