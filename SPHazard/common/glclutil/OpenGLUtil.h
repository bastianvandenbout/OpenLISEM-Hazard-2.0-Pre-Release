#ifndef __OPENGL_UTIL_H__
#define __OPENGL_UTIL_H__

#include <windows.h>
#include <glad/glad.h>

typedef struct {
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint geometry = 0;
    GLuint tc = 0;
    GLuint te = 0;
} shaders_t;

char* loadFile(const char *fname, GLint &fSize);

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader);

void printLinkInfoLog(GLint prog);

shaders_t loadShaders(const char * vert_path, const char * frag_path);

void attachAndLinkProgram( GLuint program, shaders_t shaders);

GLuint initShaders(const char* vshaderpath, const char* fshaderpath);
GLuint initShaders(const char* vshaderpath, const char* fshaderpath,const char* gshaderpath,const char* tcshaderpath,const char* teshaderpath);

GLuint createTexture2D(int width, int height, void* data=0,int component = GL_RGBA, int datatype = GL_FLOAT, int format =GL_RGBA);
GLuint createTexture2DLinear(int width, int height, void* data=0,int component = GL_RGBA, int datatype = GL_FLOAT, int format =GL_RGBA);

GLuint createBuffer(int size, const float* data, GLenum usage);
GLuint createBuffer(int size, const int* data, GLenum usage);


#endif//__OPENGL_UTIL_H__
