#include "worldwindow.h"

bool WorldWindow::UpdateCurrentWindowState()
{

    //we have to update the m_CurrentWindowState;

    bool redrawreq = false;




    //get screen dim (min 1)
    float scw = std::max(1.0f,((float)(m_OpenGLCLManager->GL_GLOBAL.Width)));
    float sch = std::max(1.0f,(float)( m_OpenGLCLManager->GL_GLOBAL.Height));

    //get scroll and drag
    float dscroll = m_MouseState.scroll-m_MouseState.scrollold;

    float dragx = m_MouseState.Drag_x;
    float dragy = m_MouseState.Drag_y;

    if(dscroll != 0.0f || dragx != 0.0f || dragy != 0.0f)
    {
        redrawreq = true;
    }


    m_CurrentWindowState.FocusLayer = m_UIFocusLayer;



    ////
    ///
    /// ALL THE 2D VIEW PARAMETERS
    ///
    ////

    //set the CurrentWindowState based on the added scroll and drag

    m_CurrentWindowState.scale = m_Camera2D->GetSize().X();

    //now calculate the CRS width and height of the total window
    m_CurrentWindowState.width = m_CurrentWindowState.scale;
    float ratiohw = (sch/scw);
    m_CurrentWindowState.height = m_CurrentWindowState.scale * ratiohw;

    //CRS to pixel conversion value
    float CRStoPixX = m_CurrentWindowState.width/(scw);
    float CRStoPixY = m_CurrentWindowState.height/(sch);

    //then based on the CRS width and height and pixel width and heigt
    //estimate CRS drag

    float CRSDragX = dragx *CRStoPixX;
    float CRSDragY = dragy *CRStoPixY;

    //add this drag to the translation
    m_CurrentWindowState.translation_x =m_Camera2D->cx;
    m_CurrentWindowState.translation_y =m_Camera2D->cy;

    //calculate tl and br coords
    m_CurrentWindowState.tlx = m_Camera2D->tlx;
    m_CurrentWindowState.brx = m_Camera2D->brx;
    m_CurrentWindowState.tly = m_Camera2D->tly;
    m_CurrentWindowState.bry = m_Camera2D->bry;

    //pixel widths and ratios
    m_CurrentWindowState.scr_ratio = sch/scw;
    m_CurrentWindowState.scr_pixwidth = scw;
    m_CurrentWindowState.scr_pixheight = sch;
    m_CurrentWindowState.scr_width = scw;
    m_CurrentWindowState.scr_height = sch;




    float width = m_CurrentWindowState.scr_pixwidth;
    float height = m_CurrentWindowState.scr_pixheight;

    ////
    ///
    /// ALL THE 3D VIEW PARAMETERS
    ///
    ////

    m_CurrentWindowState.Camera3D = m_Camera3D;

    //SPHVector3 CameraPosition;
    //SPHVector3 CameraDir;
    //SPHVector3 CameraUpDir;

    //float fov;

    //SPHMatrix4x4 MatrixProjection;
    //SPHMatrix4x4 MatrixProjectionNoTranslation;

    ////
    ///
    /// ALL THE UI PARAMETERS
    ///
    ////


    m_FocusMutex.lock();

    m_CurrentWindowState.m_FocusLocations = this->m_FocusLocations;
    m_CurrentWindowState.m_FocusSquare = this->m_FocusSquare;

    m_FocusMutex.unlock();

    int max_legend = 4;

    m_CurrentDrawWindowState.legendindex = 0;
    m_CurrentDrawWindowState.legendtotal = 0;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);

        if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && !l->IsDrawAsDEM() && !l->IsDrawAsHillShade())
        {
            if(m_UILayerList.at(i)->DrawLegend())
            {
                m_CurrentDrawWindowState.legendtotal ++;
            }
        }
    }

    m_CurrentDrawWindowState.legendtotal = std::min(m_CurrentDrawWindowState.legendtotal,max_legend);


    //just some guess of what a nice ui  scaling would be
    float framewidth = std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
    float ui_ticktextscale = std::max(0.3f,0.5f * framewidth/25.0f);
    m_CurrentWindowState.ui_scale = ui_ticktextscale * m_DrawUIScale;
    m_CurrentWindowState.ui_scalem = m_DrawUIScale;
    m_CurrentWindowState.ui_scale2d3d = 1.0;

    m_CurrentWindowState.draw_lines = m_DrawLines;
    m_CurrentWindowState.draw_ui = m_DrawUI;
    m_CurrentWindowState.draw_legends = m_DrawLegends;
    m_CurrentWindowState.is_3d = m_Draw3D;
    m_CurrentWindowState.SunDir =  m_SunDir;
    m_CurrentWindowState.is_globe = m_Draw3DGlobe;
    m_CurrentWindowState.m_time = m_GLT;

    m_CurrentWindowState.m_2D3DRenderTargetScales = m_2D3DRenderTargetScales;
    m_CurrentWindowState.m_2D3DRenderTargetBoundingBox.clear();


    m_CurrentWindowState.GL_FrameBuffer3DLayerColor = m_3DLayerRenderTargetC;
    m_CurrentWindowState.GL_FrameBuffer3DLayerElevation = m_3DLayerRenderTargetD;

    ////
    ///
    /// ALL THE MOUSE AND INPUT PARAMETERS
    ///
    ////

    //get current mouse position

    //m_Camera3D->GetRayFromScreen()
    //SPHVector4 hit = m_ElevationProvider->RayCast();

    if(m_3DScreenPosX != nullptr && m_3DScreenPosY != nullptr && m_3DScreenPosZ != nullptr)
    {
        if(m_Draw3D)
        {
            int r =(int)m_MouseState.Pos_y;
            int c = (int)m_MouseState.Pos_x;
            if(r > -1 && r < m_3DScreenPosX->nrRows() && c > -1 && c <m_3DScreenPosX->nrCols())
            {

                m_CurrentWindowState.MouseGeoPosX = m_Camera3D->GetPosition().x +  m_3DScreenPosX->data[m_3DScreenPosX->nrRows()-(int)m_MouseState.Pos_y-1][(int)m_MouseState.Pos_x];
                m_CurrentWindowState.MouseGeoPosY = m_3DScreenPosY->data[m_3DScreenPosX->nrRows()-(int)m_MouseState.Pos_y-1][(int)m_MouseState.Pos_x];
                m_CurrentWindowState.MouseGeoPosZ = m_Camera3D->GetPosition().z +  m_3DScreenPosZ->data[m_3DScreenPosX->nrRows()-(int)m_MouseState.Pos_y-1][(int)m_MouseState.Pos_x];

                if(m_CurrentWindowState.MouseGeoPosX > -1e25)
                {
                    m_CurrentWindowState.MouseHit = true;
                }
            }else {
                m_CurrentWindowState.MouseHit = false;
                m_CurrentWindowState.MouseGeoPosX = 0;
                m_CurrentWindowState.MouseGeoPosY = 0;
                m_CurrentWindowState.MouseGeoPosZ = 0;
            }
        }else
        {
            float mousefracx = m_MouseState.Pos_x/scw;
            float mousefracy = m_MouseState.Pos_y/sch;
            m_CurrentWindowState.MouseHit = true;

            m_CurrentWindowState.MouseGeoPosX = m_CurrentWindowState.tlx + mousefracx * (m_CurrentWindowState.brx - m_CurrentWindowState.tlx);
            m_CurrentWindowState.MouseGeoPosY = 0;
            m_CurrentWindowState.MouseGeoPosZ = m_CurrentWindowState.bry + mousefracy * (m_CurrentWindowState.tly - m_CurrentWindowState.bry);
        }

    }else {

        m_CurrentWindowState.MouseHit = false;
        m_CurrentWindowState.MouseGeoPosX = 0;
        m_CurrentWindowState.MouseGeoPosY = 0;
        m_CurrentWindowState.MouseGeoPosZ = 0;
    }


    m_CurrentWindowState.MousePosX = m_MouseState.Pos_x;
    m_CurrentWindowState.MousePosY = sch - 1 - m_MouseState.Pos_y;



    ///
    /// OPENGL RELATED STUFF
    ///
    ////





    ////
    ///
    /// GENERAL
    ///
    ////

    m_CurrentWindowState.no_async = false;

    return redrawreq;
}

