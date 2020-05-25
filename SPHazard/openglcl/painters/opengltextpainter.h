#ifndef OPENGLTEXTPAINTER_H
#define OPENGLTEXTPAINTER_H

#include <ft2build.h>
#include FT_FREETYPE_H

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

struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    SPHVector2 Size;       // Size of glyph
    SPHVector2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
    float      size_org;
};

struct Font {

    QString FontName;
    QString FontSize;
    bool is_bold;
    bool is_italic;
    FT_Face * face = NULL;

    std::map<GLchar, Character> Glyphs_Rendered;


};


class OpenGLTextPainter
{
public:

    QList<Font*> m_Fonts;
    std::map<GLchar, Character> Glyphs_Rendered;

    FT_Library ft;

    Font*m_MainFont;

    QString m_FontDir;
    QString m_AssetDir;
    OpenGLProgram * m_GLProgram_DrawText;

    GLuint VAO, VBO;

    inline OpenGLTextPainter()
    {
        //load the freetype font libary. We will use this to pre-render glyphs that we draw with opengl.
        //we have our own copy of the font for portability and customizability for users

        SPH_DEBUG("Loading Freetype Font Library and Fonts");

        if (FT_Init_FreeType(&ft))
        {
            SPH_ERROR("ERROR::FREETYPE: Could not init FreeType Library");
            throw 1;
        }

        m_FontDir = QCoreApplication::applicationDirPath()+"/fonts/";
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



    QList<FT_Face> m_Faces;
    QList<Font *> m_Fonts_Rendered;

    inline void LoadDefaultFontsAndShaders()
    {
        m_MainFont = LoadFont(m_FontDir + "roboto/Roboto-Regular.ttf");
        m_Fonts.append(m_MainFont);

        m_GLProgram_DrawText = new OpenGLProgram();
        m_GLProgram_DrawText->LoadProgram(m_AssetDir+ "UITextDraw.vert", m_AssetDir+ "UITextDraw.frag");

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

    inline Font * LoadFont(QString file, int size = 12, bool bold = false, bool italic = false)
    {
        SPH_DEBUG("Loading Font: " + file);

        Font * ret = new Font();

        FT_Face face;
        if (FT_New_Face(ft, QString(file).toStdString().c_str(), 0, &face))
        {
            SPH_ERROR("ERROR::FREETYPE: Failed to load font");
            throw 1;
        }

        std::map<GLchar, Character> Characters;

        glad_glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

        FT_Set_Pixel_Sizes(face, 48, 48);

        for (GLubyte c = 0; c < 128; c++)
        {
            int err = FT_Load_Char(face, c, FT_LOAD_RENDER);
            // Load character glyph
            if (err)
            {
                SPH_ERROR("ERROR::FREETYTPE: Failed to load Glyph, code: " + QString::number(err));
                continue;
            }
            // Generate texture
            GLuint texture;
            glad_glGenTextures(1, &texture);
            glad_glBindTexture(GL_TEXTURE_2D, texture);
            glad_glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // Set texture options
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //std::cout << "store character " <<c << " "<< face->glyph->bitmap.width << " "<< face->glyph->bitmap.rows << " "<<  face->glyph->advance.x << std::endl;
            // Now store character for later use
            Character character = {
                texture,
                SPHVector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                SPHVector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>(face->glyph->advance.x),
                48
            };

            ret->Glyphs_Rendered.insert(std::pair<GLchar, Character>(c, character));
        }

        return ret;
    }

    inline Font * GetFont(QString Name, bool bold, bool italic)
    {

        return nullptr;
    }

    inline void DrawString(QString in_text, Font * f, float pos_x, float pos_y, SPHVector4 color = SPHVector4(1.0,1.0,1.0,1.0), float size = 12)
    {
        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);

        Font * useFont = nullptr;
        if(f == nullptr)
        {
            useFont = m_MainFont;
        }else
        {
            useFont = f;
        }

        float x = pos_x;
        float y = pos_y;

        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawText->m_program);

        SPHMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawText->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawText->m_program, "textColor"), color.x, color.y, color.z,color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        std::string text = in_text.toStdString();

        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = useFont->Glyphs_Rendered[*c];

            float scale = size/ch.size_org;

            GLfloat xpos = x + ch.Bearing.x * scale;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;

            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };

            /*float dev = 500.0;
            GLfloat vertices[6][4] = {
                { xpos/dev,     (ypos + h)/dev,   0.0, 0.0 },
                { xpos/dev,     ypos/dev,       0.0, 1.0 },
                { (xpos + w)/dev, ypos/dev,       1.0, 1.0 },

                { xpos/dev,     (ypos + h)/dev,   0.0, 0.0 },
                { (xpos + w)/dev, ypos/dev,       1.0, 1.0 },
                { (xpos + w)/dev, (ypos + h)/dev,   1.0, 0.0 }
            };*/
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glad_glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;
    }

    inline void Destroy()
    {
        for(int i = 0; i < m_Fonts.length(); i++)
        {
            Font * f = m_Fonts.at(i);
            if(f != nullptr)
            {
                FT_Face * fa = (f->face);
                if(fa != nullptr)
                {
                    FT_Done_Face(*fa);
                }
                f->Glyphs_Rendered.clear();
                SAFE_DELETE(f);
            }
        }
        m_Fonts.clear();
        m_MainFont = nullptr;
        FT_Done_FreeType(ft);
    }


};

#endif // OPENGLTEXTPAINTER_H
