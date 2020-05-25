#include "worldwindow.h"
#include "gdal_version.h"



void WorldWindow::Draw()
{

    qint64 timenew =  m_time.nsecsElapsed();
    float timedif = float(timenew - m_timeold)/1000000000.0f;
    m_GLDT = std::max(0.00001f,timedif);
    if(dt_first)
    {
        m_GLDT = 0.00001f;
        dt_first = false;
    }else {
        m_GLT += m_GLDT;
    }
    if(draw_first)
    {
        draw_first = false;

        //do initial calculations for drawing code

        //generate framebuffers that are used for drawing 2d content and rendering this onto 3d geometry


        OpenGLCLMSAARenderTarget * targetc = new OpenGLCLMSAARenderTarget();
        targetc->CreateRGBA8(1500,1500);
        OpenGLCLMSAARenderTarget * targetd = new OpenGLCLMSAARenderTarget();
        targetd->CreateR32(1000,1000);
        m_2D3DRenderCTargets.append(targetc);
        m_2D3DRenderDTargets.append(targetd);
        m_2D3DRenderTargetScales.append(6.0);
        m_2D3DTransformManager.append( new WorldGLTransformManager(m_OpenGLCLManager));
        m_ElevationProvider->AddElevationModel(1000,1000);

        targetc = new OpenGLCLMSAARenderTarget();
        targetc->CreateRGBA8(1500,1500);
        targetd = new OpenGLCLMSAARenderTarget();
        targetd->CreateR32(1000,1000);
        m_2D3DRenderCTargets.append(targetc);
        m_2D3DRenderDTargets.append(targetd);
        m_2D3DRenderTargetScales.append(40.0);
        m_2D3DTransformManager.append( new WorldGLTransformManager(m_OpenGLCLManager));
        m_ElevationProvider->AddElevationModel(1000,1000);

        targetc = new OpenGLCLMSAARenderTarget();
        targetc->CreateRGBA8(1000,1000);
        targetd = new OpenGLCLMSAARenderTarget();
        targetd->CreateR32(1000,1000);
        m_2D3DRenderCTargets.append(targetc);
        m_2D3DRenderDTargets.append(targetd);
        m_2D3DRenderTargetScales.append(250.0);
        m_2D3DTransformManager.append( new WorldGLTransformManager(m_OpenGLCLManager));
        m_ElevationProvider->AddElevationModel(1000,1000);

        m_3DLayerRenderTargetC = new OpenGLCLMSAARenderTarget();
        m_3DLayerRenderTargetC->CreateRGBA8(1500,1500);
        m_3DLayerRenderTargetD = new OpenGLCLMSAARenderTarget();
        m_3DLayerRenderTargetD->CreateR32(1000,1000);

        m_3DRenderTarget = new OpenGLCLMSAARenderTarget();
        m_TDRenderTarget = new OpenGLCLMSAARenderTarget();

        m_3DRenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,4,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,6);
        m_TDRenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,4,GL_R32F,GL_R32F,GL_RED, GL_FLOAT,0,0);

        MaskedRaster<float> raster_data(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosX = new cTMap(std::move(raster_data),"","");
        m_3DScreenPosX->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data2(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosY = new cTMap(std::move(raster_data2),"","");
        m_3DScreenPosY->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data3(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosZ = new cTMap(std::move(raster_data3),"","");
        m_3DScreenPosZ->AS_IsSingleValue = false;

    }

    m_3DRenderTarget->Resize(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);
    m_TDRenderTarget->Resize(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);

    if(m_OpenGLCLManager->GL_GLOBAL.Height != m_3DScreenPosX->nrRows() || m_OpenGLCLManager->GL_GLOBAL.Width != m_3DScreenPosX->nrCols())
    {
        delete m_3DScreenPosX;
        delete m_3DScreenPosY;
        delete m_3DScreenPosZ;
        m_3DScreenPosX= nullptr;
        m_3DScreenPosY= nullptr;
        m_3DScreenPosZ= nullptr;

        MaskedRaster<float> raster_data(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosX = new cTMap(std::move(raster_data),"","");
        m_3DScreenPosX->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data2(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosY = new cTMap(std::move(raster_data2),"","");
        m_3DScreenPosY->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data3(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosZ = new cTMap(std::move(raster_data3),"","");
        m_3DScreenPosZ->AS_IsSingleValue = false;
    }else {

        //m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(1),m_3DScreenPosX);
        //m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(2),m_3DScreenPosY);
        //m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(3),m_3DScreenPosZ);
    }
    //std::cout << "fps : " << 1.0f/timedif << std::endl;

    m_timeold = timenew;

    //layer management


    m_Model->UILayerRemoveMutex.lock();


    if(m_Model->m_UIMapsSet)
    {

        if(m_Model->m_RequiresUIReset)
        {

            m_Model->m_RequiresUIReset = false;

            RemoveNativeUILayers();
            AddNativeUILayers();

        }
    }else if(m_Model->m_RequiresUIReset)
    {

        m_Model->m_RequiresUIReset = false;

        RemoveNativeUILayers();
    }


    m_UILayerMutex.lock();


    //Update Camera properties
    UpdateCamera();



    bool removed = false;
    for(int i = m_UILayerList.length()-1; i >-1 ; i--)
    {
        UILayer * l = m_UILayerList.at(i);

        if(l->ShouldBeRemoved() )
        {
            removed = true;
            l->OnDestroy(m_OpenGLCLManager);
            m_UILayerList.removeAt(i);
        }

    }

    if(m_LayerEditor != nullptr)
    {
        UILayerEditor * l = m_LayerEditor;

        if(l->ShouldBeRemoved() )
        {
            removed = true;
            l->OnClose();
            l->OnDestroy(m_OpenGLCLManager);
            delete m_LayerEditor;
            m_LayerEditor = nullptr;

        }
    }

    if(removed)
    {
        emit OnMapsChanged();
    }


    //update GeoWindowState
    bool need_redraw = UpdateCurrentWindowState();



    m_CRSMutex.lock();

    //check if projection of either the layer or the window has changed
    //if so, call the relevant function for the layer class (OnCRSChanged())
    m_CurrentWindowState.projection = m_CurrentProjection;

    bool crs_doneeditor = false;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);
        if(m_CRSChanged || l->IsCRSChanged())
        {
            l->OnCRSChanged(m_OpenGLCLManager,m_CurrentWindowState,m_TransformManager);
            if(m_LayerEditor != nullptr)
            {
                if(m_LayerEditor->GetLayer() == l)
                {
                    m_LayerEditor->OnCRSChanged(m_OpenGLCLManager,m_CurrentWindowState,m_TransformManager);
                    crs_doneeditor = true;
                }
            }
        }
    }
    if(m_LayerEditor != nullptr && !crs_doneeditor && m_CRSChanged)
    {
        m_LayerEditor->OnCRSChanged(m_OpenGLCLManager,m_CurrentWindowState,m_TransformManager);
    }
    m_CRSChanged = false;

    //now update the opengl crs transforming textures based on the current layers and CRS
    UpdateTransformers();

    m_CRSMutex.unlock();


    //actually draw the crs transformers
    m_TransformManager->UpdateFromWindowState(this->m_CurrentWindowState);

    m_CurrentDrawWindowState = m_CurrentWindowState;

    //m_CurrentDrawWindowState.m_FocusLocations =  m_CurrentWindowState.m_FocusLocations;
    //m_CurrentDrawWindowState.m_FocusSquare = m_CurrentWindowState.m_FocusSquare;


    m_CurrentDrawWindowState.GL_FrameBuffer = m_Draw3D? m_3DRenderTarget:m_OpenGLCLManager->GetFrameBuffer();
    m_CurrentDrawWindowState.GL_PrimaryFrameBuffer = m_Draw3D? m_3DRenderTarget :m_OpenGLCLManager->GetFrameBuffer();


   MouseStateMutex.lock();

    //send all input signals to the child objects (layers, editors etc..)


    InputToLayers();

    //reset some values in the mouse state

    m_MouseState.PosOld_x = m_MouseState.Pos_x;
    m_MouseState.PosOld_y = m_MouseState.Pos_y;
    m_MouseState.Drag_x = 0.0f;
    m_MouseState.Drag_y = 0.0f;
    m_MouseState.scrollold = m_MouseState.scroll;
    m_MouseState.Move_X = 0.0f;
    m_MouseState.Move_Y = 0.0f;
    m_MouseState.MoveScroll_X = 0.0f;
    m_MouseState.MoveScroll_Y = 0.0f;
    m_MouseState.MouseButtonEventsShift.clear();
    m_MouseState.MouseButtonEvents.clear();
    m_MouseState.MouseButtonKeyAction.clear();
    m_MouseState.KeyEventShift.clear();
    m_MouseState.KeyEvents.clear();
    m_MouseState.KeyAction.clear();
    m_MouseState.KeyMods.clear();
    MouseStateMutex.unlock();



    //OnDraw callback

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser() )
        {
            UILayer * l = m_UILayerList.at(i);
            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                l->OnDraw(m_OpenGLCLManager,m_CurrentWindowState);
            }
        }
    }

    if(m_LayerEditor != nullptr)
    {
        m_LayerEditor->OnDraw(m_OpenGLCLManager,m_CurrentWindowState);
    }


    //now depending on what kind of window we are drawing (3d, globe or 2d) call the relevant function
    if(!m_Draw3D)
    {

        DrawToFrameBuffer2D(m_CurrentDrawWindowState);

    }else {
        DrawToFrameBuffer3D(m_CurrentDrawWindowState);
    }



    m_UILayerMutex.unlock();


    m_Model->UILayerRemoveMutex.unlock();

}



void WorldWindow::SetDraw3D(bool d)
{
    m_UILayerMutex.lock();
    m_Draw3D = d;
    m_UILayerMutex.unlock();
}

void WorldWindow::SetDraw3DGlobe(bool d)
{
    m_UILayerMutex.lock();
    m_Draw3DGlobe = d;
    m_UILayerMutex.unlock();
}

void WorldWindow::SetUIDraw(bool d)
{
     m_UILayerMutex.lock();
     m_DrawUI = d;
     m_UILayerMutex.unlock();
}
void WorldWindow::SetLinesDraw(bool d)
{
     m_UILayerMutex.lock();
     m_DrawLines = d;
     m_UILayerMutex.unlock();
}

void WorldWindow::SetUIScale(double d)
{
     m_UILayerMutex.lock();
     m_DrawUIScale = d;
     m_UILayerMutex.unlock();

}
void WorldWindow::SetLegendDraw(bool d)
{
    m_UILayerMutex.lock();
    m_DrawLegends = d;
    m_UILayerMutex.unlock();
}
GeoProjection WorldWindow::GetCurrentProjection()
{
    return m_CurrentWindowState.projection;
}
void WorldWindow::SetCurrentProjection(GeoProjection p, bool forceupdate)
{
    if(!m_CurrentWindowState.projection.IsEqualTo(p) || forceupdate)
    {
        m_CRSMutex.lock();

        m_CRSChanged =true;
        m_CurrentProjection = p;

        m_FocusMutex.lock();
        m_FocusSquare.clear();
        m_FocusLocations.clear();
        m_FocusMutex.unlock();

        m_CRSMutex.unlock();
    }
}

GeoWindowState WorldWindow::GetCurrentWindowState()
{
    return m_CurrentWindowState;
}

void WorldWindow::SignalClose()
{
        glfwSetWindowShouldClose(m_OpenGLCLManager->window,GL_TRUE);
}

void WorldWindow::SetModel( SPHModel * m)
{
    m_Model = m;

}
