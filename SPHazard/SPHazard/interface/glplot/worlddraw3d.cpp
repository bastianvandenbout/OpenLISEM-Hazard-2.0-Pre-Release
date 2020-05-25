#include "worldwindow.h"


void WorldWindow::DrawToFrameBuffer3D(GeoWindowState s)
{




    DrawColoredTerrain3D(&s); //this one fills the 2d to 3d render targets

    //fill elevation model

    for(int i = 0; i < m_2D3DRenderDTargets.length(); i++)
    {
        m_ElevationProvider->SetElevationModel(i,m_OpenGLCLManager,m_2D3DRenderDTargets.at(i),m_2D3DRenderTargetBoundingBox.at(i));
    }

    s.GL_FrameBuffer = s.GL_PrimaryFrameBuffer;

    m_3DRenderTarget->ClearAll();

    glad_glBindFramebuffer(GL_FRAMEBUFFER, m_3DRenderTarget->GetFrameBuffer());
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glClearColor(0.5,0.5,0.5,1.0);

    glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_FrameBuffer->GetFrameBuffer());
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glDisable(GL_DEPTH_TEST);
    glad_glClearColor(0.5,0.5,0.5,1.0);
    glad_glViewport(0,0,m_CurrentDrawWindowState.scr_pixwidth,m_CurrentDrawWindowState.scr_pixheight);

    s.GL_FrameBuffer->ClearAll();
    s.GL_FrameBuffer->SetAsTarget();
    DrawBaseLayers3D(s);


    m_3DRenderTarget->BlitToTexture();

    if(m_OpenGLCLManager->GL_GLOBAL.Height != m_3DScreenPosX->nrRows() || m_OpenGLCLManager->GL_GLOBAL.Width != m_3DScreenPosX->nrCols())
    {
        s.ScreenPosX = nullptr;
        s.ScreenPosY = nullptr;
        s.ScreenPosZ = nullptr;

        s.ScreenPosXTex = -1;
        s.ScreenPosYTex = -1;
        s.ScreenPosZTex = -1;

        s.ScreenNormalXTex = -1;
        s.ScreenNormalYTex = -1;
        s.ScreenNormalZTex = -1;

    }else {


        m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(1),m_3DScreenPosZ);
        m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(2),m_3DScreenPosY);
        m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(3),m_3DScreenPosX);

        s.ScreenPosX = m_3DScreenPosX;
        s.ScreenPosY = m_3DScreenPosY;
        s.ScreenPosZ = m_3DScreenPosZ;

        s.ScreenPosXTex =m_3DRenderTarget->GetTexture(1);
        s.ScreenPosYTex =m_3DRenderTarget->GetTexture(2);
        s.ScreenPosZTex =m_3DRenderTarget->GetTexture(3);

        s.ScreenNormalXTex =m_3DRenderTarget->GetTexture(4);
        s.ScreenNormalYTex =m_3DRenderTarget->GetTexture(5);
        s.ScreenNormalZTex =m_3DRenderTarget->GetTexture(6);
    }



    Draw3DObjects(s);
    m_3DRenderTarget->BlitToTexture();

    s.GL_FrameBuffer->SetDefaultTarget();
    glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_FrameBuffer->GetFrameBuffer());


    glad_glBindFramebuffer(GL_FRAMEBUFFER, m_OpenGLCLManager->GetFrameBuffer()->GetFrameBuffer());
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glad_glDrawBuffers(1, DrawBuffers);

    glad_glViewport(0,0,m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);

    glad_glDisable(GL_DEPTH_TEST);
    // bind shader
    glad_glUseProgram(m_OpenGLCLManager->m_GLProgram_CopyText->m_program);
    // get uniform locations
    int mat_loc = glad_glGetUniformLocation(m_OpenGLCLManager->m_GLProgram_CopyText->m_program,"matrix");
    int tex_loc = glad_glGetUniformLocation(m_OpenGLCLManager->m_GLProgram_CopyText->m_program,"tex");
    // bind texture

    glad_glUniform1i(tex_loc,0);
    glad_glActiveTexture(GL_TEXTURE0);
    glad_glBindTexture(GL_TEXTURE_2D, m_3DRenderTarget->GetTexture(0));//m_2D3DRenderDTargets.at(0)->GetTexture(0));//
    // set project matrix
    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
    // now render stuff
    glad_glBindVertexArray(m_OpenGLCLManager->m_Quad->m_vao);
    glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
    glad_glBindVertexArray(0);





    GeoWindowState s2 = s;
    s2.GL_FrameBuffer = m_OpenGLCLManager->GetFrameBuffer();

    Draw3DPost(s2);


}

void WorldWindow::DrawBaseLayers3D(GeoWindowState s)
{
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPreDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPreDraw3D(m_OpenGLCLManager,s, m_TransformManager);
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
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }

}


void WorldWindow::DrawColoredTerrain3D(GeoWindowState *s)
{

    s->GL_FrameBuffer3DWindow.clear();
    s->GL_FrameBuffer3DColor.clear();
    s->GL_FrameBuffer3DElevation.clear();

    m_2D3DRenderTargetBoundingBox.clear();

    float scalebase = 1.0;

    for(int i = 0; i < m_2D3DRenderTargetScales.length(); i++)
    {

        if(i == 0)
        {
            scalebase = m_2D3DRenderTargetScales.at(i);
        }
        OpenGLCLMSAARenderTarget * TargetC = m_2D3DRenderCTargets.at(i);
        OpenGLCLMSAARenderTarget * TargetD = m_2D3DRenderDTargets.at(i);
        float TargetScale = m_2D3DRenderTargetScales.at(i);

        //find bounding box to look at
        BoundingBox lookbb = m_Camera3D->Get2DViewEquivalent(m_ElevationProvider);
        float cx = lookbb.GetCenterX();
        float cy = lookbb.GetCenterY();
        float sx = lookbb.GetSizeX();
        float sy = lookbb.GetSizeY();
        SPHVector3 C_Pos = m_Camera3D->GetPosition();

        float sxn = sx * TargetScale;
        float syn = sy * TargetScale;

        sxn = std::max(sxn,syn);
        syn = sxn;

        SPHVector2 cn = SPHVector2(C_Pos.X(),C_Pos.Z());// + ((SPHVector2(C_Pos.X(),C_Pos.Z())-SPHVector2(cx,cy)) * TargetScale);

        float cxn = cn.X();
        float cyn = cn.Y();
        //create new GeoWindowStates for each 2d render target

        GeoWindowState sn = *s;


        sn.scale = sxn;
        sn.width = sxn;
        sn.height = syn;
        sn.tlx = cxn - 0.5 * sxn;
        sn.brx = cxn + 0.5 * sxn;
        sn.tly = cyn - 0.5 * syn;
        sn.bry = cyn + 0.5 * syn;
        sn.scr_pixwidth = TargetC->GetWidth();
        sn.scr_pixheight = TargetC->GetHeight();
        sn.scr_width = TargetC->GetWidth();
        sn.scr_height = TargetC->GetHeight();
        sn.draw_ui = false;
        sn.draw_legends = false;
        sn.GL_FrameBuffer = TargetC;
        sn.ui_scale2d3d = scalebase/m_2D3DRenderTargetScales.at(i);
        //update gl transformer

        m_2D3DTransformManager.at(i)->UpdateFromWindowState(sn);

        //draw to this

        DrawToFrameBuffer2D(sn,m_2D3DTransformManager.at(i));
        TargetC->BlitToTexture();

        sn.GL_FrameBuffer = TargetD;
        sn.scr_pixwidth = TargetD->GetWidth();
        sn.scr_pixheight = TargetD->GetHeight();
        sn.scr_width = TargetD->GetWidth();
        sn.scr_height = TargetD->GetHeight();
        //draw to this

        DrawToFrameBuffer2DElevation(sn,m_2D3DTransformManager.at(i));
        TargetD->BlitToTexture();

        //store it in our current GeoWindowState
        s->GL_FrameBuffer3DColor.append(TargetC);
        s->GL_FrameBuffer3DElevation.append(TargetD);
        s->GL_FrameBuffer3DWindow.append(BoundingBox(sn.tlx,sn.brx,sn.tly,sn.bry));

        m_2D3DRenderTargetBoundingBox.append(BoundingBox(sn.tlx,sn.brx,sn.tly,sn.bry));

        m_ElevationProvider->SetElevationModel(i,m_OpenGLCLManager,TargetD,BoundingBox(sn.tlx,sn.brx,sn.tly,sn.bry));
    }

    s->m_2D3DRenderTargetBoundingBox = m_2D3DRenderTargetBoundingBox;
}


void WorldWindow::Draw3DObjects(GeoWindowState s)
{
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DGeo(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }


    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DGeo(m_OpenGLCLManager,s, m_TransformManager);
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
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DTransparancyDepthMap(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                   l->OnDraw3DTransparancyDepthMap(m_OpenGLCLManager,s, m_TransformManager);
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
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DTransparentLayer(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                   l->OnDraw3DTransparentLayer(m_OpenGLCLManager,s, m_TransformManager);
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
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPostDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPostDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }


}


void WorldWindow::Draw3DPost(GeoWindowState s)
{

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DPostProcess(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DPostProcess(m_OpenGLCLManager,s, m_TransformManager);
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
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DPostPostProcess(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3DPostPostProcess(m_OpenGLCLManager,s, m_TransformManager);
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
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && !l->IsDrawAsDEM()  && !l->IsDrawAsHillShade())
            {
                if(l->DrawLegend() && s.draw_legends && s.draw_ui)
                {
                    l->OnDrawLegend(m_OpenGLCLManager,s);
                    s.legendindex ++;
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
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                if(l->DrawLegend() && s.draw_legends && s.draw_ui)
                {
                    l->OnDrawLegend(m_OpenGLCLManager,s);
                    s.legendindex ++;
                }
            }
        }
    }
}
