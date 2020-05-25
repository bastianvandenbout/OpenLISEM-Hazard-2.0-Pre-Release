#include "worldwindow.h"


void WorldWindow::DrawToFrameBuffer2D(GeoWindowState S, WorldGLTransformManager * glt)
{

    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }

    GLuint FB = S.GL_FrameBuffer->GetFrameBuffer();

    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);

    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
    glad_glViewport(0,0,S.scr_pixwidth,S.scr_pixheight);
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glDisable(GL_DEPTH_TEST);
    glad_glClearColor(0.1,0.1,0.1,1.0);

    DrawGeoLayers2DColor(S,glt);
    DrawUILayers(S,glt);

    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
}


void WorldWindow::DrawToFrameBuffer2DElevation(GeoWindowState S, WorldGLTransformManager * glt)
{
    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }

    GLuint FB = S.GL_FrameBuffer->GetFrameBuffer();

    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);

    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glDisable(GL_DEPTH_TEST);

    static GLfloat mv[] = {-1e30f, 0.0f, 0.0f, 1.0f};
    glad_glClearBufferfv(GL_COLOR, 0, mv);

    glad_glViewport(0,0,S.scr_pixwidth,S.scr_pixheight);

    DrawGeoLayers2DElevation(S,glt);
    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);



}

void WorldWindow::DrawGeoLayers2DColor(GeoWindowState S, WorldGLTransformManager * glt)
{
    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }


    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser() )
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
                l->OnCRSChanged(m_OpenGLCLManager,S,glt);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && (!l->IsDrawAsDEM() || l->IsDrawAsHillShade()))
            {
                l->OnDrawGeo(m_OpenGLCLManager,S,glt);
            }
        }
    }

    if(m_LayerEditor != nullptr)
    {

        if(m_LayerEditor->IsPrepared() == false)
        {
            m_LayerEditor->OnPrepare(m_OpenGLCLManager);
            m_LayerEditor->OnCRSChanged(m_OpenGLCLManager,S,glt);
        }
        if(m_LayerEditor->ShouldBeRemoved() == false && m_LayerEditor->Exists() && m_LayerEditor->IsDraw())
        {
            m_LayerEditor->OnDrawGeo(m_OpenGLCLManager,S,glt);
        }
    }

}
void WorldWindow::DrawGeoLayers2DElevation(GeoWindowState S, WorldGLTransformManager * glt)
{
    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }


    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser() )
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && l->IsDrawAsDEM())
            {
                l->OnDrawGeoElevation(m_OpenGLCLManager,S, glt);
            }
        }
    }



}
void WorldWindow::DrawUILayers(GeoWindowState S, WorldGLTransformManager * glt)
{

    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }

    S.legendtotal = 0;
    S.legendindex = 0;

    //all legends
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && !l->IsDrawAsDEM() && !l->IsDrawAsHillShade())
            {
                if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                {
                    S.legendtotal = S.legendtotal + 1;
                }
            }
        }
    }

    //all ui layers

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                l->OnDrawGeo(m_OpenGLCLManager,S, glt);
            }
        }
    }

    //all legends
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && !l->IsDrawAsDEM()  && !l->IsDrawAsHillShade())
            {
                if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                {
                    l->OnDrawLegend(m_OpenGLCLManager,S);
                    S.legendindex ++;
                }
            }
        }
    }

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                {
                    l->OnDrawLegend(m_OpenGLCLManager,S);
                    S.legendindex ++;
                }
            }
        }
    }
}
