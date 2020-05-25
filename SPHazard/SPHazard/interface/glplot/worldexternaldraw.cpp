#include "worldwindow.h"

QImage *WorldWindow::DoExternalDraw(GeoWindowState s)
{

    m_OpenGLCLManager->m_GLOutputUIMutex.lock();
    m_OpenGLCLManager->m_GLMutex.lock();

    m_CurrentDrawWindowState = s;


    glfwMakeContextCurrent(m_OpenGLCLManager->window);

    OpenGLCLMSAARenderTarget * Target = new OpenGLCLMSAARenderTarget();

    Target->Create(s.scr_pixwidth,s.scr_pixheight,4,GL_RGB8,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE);

    m_CurrentDrawWindowState.GL_FrameBuffer = Target;
    m_CurrentDrawWindowState.GL_PrimaryFrameBuffer = Target;

    this->DrawToFrameBuffer2D(m_CurrentDrawWindowState);
    Target->BlitToTexture();

    QImage * Im = Target->ToQImage();

    Target->Destroy();

    glfwMakeContextCurrent(NULL);

    m_OpenGLCLManager->m_GLOutputUIMutex.unlock();
    m_OpenGLCLManager->m_GLMutex.unlock();

    return Im;
}
