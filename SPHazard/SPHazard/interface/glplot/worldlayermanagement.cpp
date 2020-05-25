#include "worldwindow.h"
#include "uirasterstreamlayer.h"
#include "rasterdataprovider.h"
#include "gl3docean.h"
#include "uicloudlayer.h"


void WorldWindow::AddOceanLayer()
{
    UILayer * ml2 = new UIOceanLayer();
    AddUILayer(ml2,true);
}

void WorldWindow::AddSkyBoxLayer()
{
    UILayer * ml2 = new UISkyBoxLayer();
    AddUILayer(ml2,true);
}

void WorldWindow::AddCloudLayer()
{
    UILayer * ml2 = new UICloudLayer();
    AddUILayer(ml2,true);
}



int WorldWindow::AddUILayer(UILayer *ML, bool emitsignal)
{
    m_UILayerMutex.lock();

    if(GetUILayers().length() == 0)
    {
        if(ML->IsGeo())
        {
            LookAt((UIGeoLayer*)(ML));
        }
    }

    //find new id number that is the lowest available positive integer
    int id = 0;

    if (m_UILayerList.length() > 0) {
        int * il = new int[m_UILayerList.length()];

        for(int i = m_UILayerList.length()-1; i > -1; i--)
        {
            il[i] = m_UILayerList.at(i)->GetID();
        }

        id = SPHazardMath::findMissingPositive(il,m_UILayerList.length());
        delete[] il;

    }else {
        id = 1;
    }

    ML->SetID(id);
    m_UILayerList.append(ML);
    m_UILayersChanged = true;

    m_UILayerMutex.unlock();

    if(emitsignal)
    {
        emit OnMapsChanged();
    }
    return 0;
}
UILayer * WorldWindow::GetUILayerFromName(QString name)
{

    m_UILayerMutex.lock();
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(name.compare(m_UILayerList.at(i)->GetName())== 0)
        {
            m_UILayerMutex.unlock();
            return m_UILayerList.at(i);
        }
    }

    m_UILayerMutex.unlock();

    return nullptr;
}

UILayer * WorldWindow::GetUILayerFromID(int id)
{

    m_UILayerMutex.lock();
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(id == m_UILayerList.at(i)->GetID())
        {
            m_UILayerMutex.unlock();
            return m_UILayerList.at(i);
        }
    }

    m_UILayerMutex.unlock();

    return nullptr;
}

UILayer * WorldWindow::GetUILayerFromFile(QString path)
{

    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());
    /*QList<cTMap *> _M;

    try
    {
        _M = readRasterList(path);
    }
    catch (int e)
    {
       return nullptr;
    }

    UILayer * ret;

    if(_M.length() == 1)
    else if(_M.length() > 1)
        {
            UIMultiRasterLayer *HS = new UIMultiRasterLayer(_M,filename,true,path);
            HS->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_MULTIRASTER),true);
            ret = HS;
        }*/


    {
        RasterDataProvider * rdp = new RasterDataProvider(QList<QString>({path}));
        bool setmem = rdp->SetInMemory();

        if(rdp->Exists())
        {
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,path,false);
            if(rdp->GetBandCount() > 1)
            {
                ret->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_MULTIRASTER),true);
            }else
            {
                ret->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERUI),true);
            }
            return ret;

        }else {
            return nullptr;
        }


    }


}

UILayer * WorldWindow::GetUIStreamLayerFromFile(QString path)
{
    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());

    RasterDataProvider * rdp = new RasterDataProvider(QList<QString>({path}));

    if(rdp->Exists())
    {
        UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,path,false);
        if(rdp->GetBandCount() > 1)
        {
            ret->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_MULTIRASTER),true);
        }else
        {
            ret->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERUI),true);
        }
        return ret;

    }else {
        return nullptr;
    }

}

UILayer * WorldWindow::GetUIDuoLayerFromFile(QList<QString> paths)
{
    if(paths.length() > 0)
    {
        QFileInfo fileInfo(paths.at(0));
        QString filename(fileInfo.fileName());
        QString filedir(fileInfo.dir().path());

        RasterDataProvider * rdp = new RasterDataProvider(paths,true);

        if(rdp->Exists())
        {
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,paths.at(0),false);
            ret->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_DUORASTER),false);

            return ret;

        }else {
            return nullptr;
        }
    }

    return nullptr;
}


UILayer * WorldWindow::GetUITimeSeriesLayerFromFile(QList<QString> paths)
{
    if(paths.length() > 0)
    {
        QFileInfo fileInfo(paths.at(0));
        QString filename(fileInfo.fileName());
        QString filedir(fileInfo.dir().path());

        RasterDataProvider * rdp = new RasterDataProvider(paths);

        if(rdp->Exists())
        {
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,paths.at(0),false);
            if(rdp->GetBandCount() > 1)
            {
                ret->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_MULTIRASTER),false);
            }else
            {
                ret->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERUI),false);
            }
            return ret;

        }else {
            return nullptr;
        }
    }

    return nullptr;
}


UILayer * WorldWindow::GetUILayerFromMap(cTMap * map, QString name, bool native)
{
    if(map == nullptr)
    {
        return nullptr;
    }
    UIRasterLayer *HS = new UIRasterLayer(map,name,false,"",native);
    HS->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERUI),true);
    return HS;

}

UILayer * WorldWindow::GetUIVectorLayerFromFile(QString path)
{

    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());
    ShapeFile * _M;

    try
    {
        _M = LoadVector(path);
    }
    catch (int e)
    {
       return nullptr;
    }

    UIVectorLayer *HS = new UIVectorLayer(_M,filename,true,path);
    HS->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_VECTORUI),true);

    return HS;

}

UILayer * WorldWindow::GetUIPointCloudLayerFromFile(QString path)
{

    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());

    PointCloud * _M;
    try
    {
        _M = ReadPointCloud(path);
    }
    catch (int e)
    {
       return nullptr;
    }

    UIPointCloudLayer *HS = new UIPointCloudLayer(_M,filename,true,path);
    HS->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_POINTCLOUDUI),true);

    return HS;

}

UILayer * WorldWindow::GetUILayerFromShapeFile(ShapeFile * map, QString name, bool native)
{
    if(map == nullptr)
    {
        return nullptr;
    }
    UIVectorLayer *HS = new UIVectorLayer(map,name,false,"",native);
    HS->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_VECTORUI),true);
    return HS;

}

UILayer * WorldWindow::GetGoogleLayer()
{
    UIWebTileLayer * HS = new UIWebTileLayer(GetDefaultTileServer(TILESERVER_GOOGLEEARTH));
    return HS;
}
UILayer * WorldWindow::GetGoogleDEMLayer()
{
    UIWebTileLayer * HS = new UIWebTileLayer(GetDefaultTileServer(TILESERVER_GOOGLEELEVATION));
    return HS;
}
void WorldWindow::RemoveUILayer(UILayer *ML, bool emitsignal)
{
    ML->SetRemove();
}

void WorldWindow::ReplaceUILayer(UILayer *ML, UILayer *ML2)
{
    ML->SetReplace(ML2);
}
void WorldWindow::SetUILayerAs(UILayer *ML,  bool emitsignal)
{
    m_UILayerMutex.lock();
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->GetID() == ML->GetID())
        {
            m_UILayerList.replace(i,ML);
            break;
        }
    }
    m_UILayerMutex.unlock();

    if(emitsignal)
    {
        emit OnMapsChanged();
    }

}
QList<UILayer *> WorldWindow::GetUILayers()
{
    QList<UILayer *> ls;
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            ls.append(m_UILayerList.at(i));
        }
    }
    return ls;
}

void WorldWindow::SetLayerOrder(QList<UILayer *> newlist, bool emitsignal)
{
    m_UILayerMutex.lock();

    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            m_UILayerList.removeAt(i);
        }
    }

    for(int i = 0; i < newlist.length(); i++)
    {
        m_UILayerList.append(newlist.at(i));
    }

    m_UILayerMutex.unlock();

    if(emitsignal)
    {
        emit OnMapsChanged();
    }

}

int WorldWindow::GetUILayerCount()
{
    return m_UILayerList.length();
}

UILayer* WorldWindow::GetUILayer(int i)
{
    return m_UILayerList.at(i);
}

void WorldWindow::AddNativeUILayers()
{


    if(!m_Model->m_MemorySave)
    {
        UIStreamRasterLayer *HS = GetUIModelRasterLayer(m_Model->HILLSHADE,m_Model->TUI_HILLSHADE, "Hillshade","HS (model)",true);
        HS->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERHS),true);
        AddUILayer(HS,false);

        LookAt(HS);
    }

    if(!m_Model->m_MemorySave)
    {
        if((m_Model->m_DoSlopeFailure || m_Model->m_DoInitialSolids))
        {
            /*UIStreamRasterLayer *WH = GetUIModelRasterLayer(m_Model->MUIHALL,m_Model->TUI_HALL, "Water Height (m)","WH (Model)",true);
            WH->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERWH),true);
            AddUILayer(WH,false);

            UIStreamRasterLayer *SH = GetUIModelRasterLayer(m_Model->MUIHS,m_Model->TUI_HS, "Solid Height (m)","SH (Model)",true);
            SH->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERWH),true);
            AddUILayer(SH,false);*/

            UIStreamRasterLayer * WH = GetUIModelDuoRasterLayer(m_Model->MUIH,m_Model->TUI_H,m_Model->MUIHS,m_Model->TUI_HS,"Flow","Flow height and content (Model)",true);
            SPHStyle s = GetStyleDefault(SPH_STYLE_DEFAULT_DUORASTER);
            s.m_IsFractional = true;
            s.m_Intervalb2.SetMax(0.25);
            WH->SetStyle(s,true);
            AddUILayer(WH,false);

        }else {

            UIStreamRasterLayer *WH = GetUIModelRasterLayer(m_Model->MUIH,m_Model->TUI_H, "Water Height (m)","WH (Model)",true);
            WH->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERWH),true);
            AddUILayer(WH,false);
        }
    }else
    {
        UIStreamRasterLayer *WH = GetUIModelRasterLayer(m_Model->MUIH,m_Model->TUI_H, "Water Height (m)","WH (Model)",true);
        WH->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERWH),true);
        AddUILayer(WH,false);

        LookAt(WH);
    }


    if(!m_Model->m_MemorySave)
    {
        UIStreamRasterLayer * WV = GetUIModelDuoRasterLayer(m_Model->MUIUX,m_Model->TUI_UX,m_Model->MUIUY,m_Model->TUI_UY,"Velocity","Flow velocity (Model)",true);
        WV->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_DUORASTER_VEL),true);
        AddUILayer(WV,false);

    }

    if(m_Model->m_DoSlopeFailure || m_Model->m_DoInitialSolids)
    {

    }

    if(!m_Model->m_MemorySave)
    {
        UIStreamRasterLayer *UI = GetUIModelRasterLayer(m_Model->MUI,m_Model->TUI_UI, "User Map","UI (Model)",true);
        UI->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERUI),true);
        UI->SetDraw(false);



        AddUILayer(UI,false);
    }


    if(m_Model->m_DoParticleSolids && (m_Model->m_DoInitialSolids || (m_Model->m_DoSlopeFailure && m_Model->m_DoHydrology && m_Model->m_DoSlopeStability)))
    {
           m_Model->m_DataPos;

           UIModelPointLayer *UIP = new UIModelPointLayer(m_Model->m_DataPos,m_Model->DEM->west(),m_Model->DEM->north());
           UIP->SetStyle(GetStyleDefault(SPH_STYLE_DEFAULT_RASTERUI),true);

           AddUILayer(UIP,false);
    }


    emit OnMapsChanged();



}

void WorldWindow::RemoveNativeUILayers()
{
    m_UILayerMutex.lock();

    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->IsModel() == true)
        {
            m_UILayerList.removeAt(i);
        }
    }

    m_UILayersChanged = true;
    m_UILayerMutex.unlock();
    {
        emit OnMapsChanged();
    }
}



//control of the display geolocation

BoundingBox WorldWindow::GetLook()
{
    return BoundingBox(m_CurrentWindowState.tlx,m_CurrentWindowState.brx,m_CurrentWindowState.tly,m_CurrentWindowState.bry);
}

void WorldWindow::LookAt(UIGeoLayer* geolayer)
{
    BoundingBox bb_orgref = geolayer->GetBoundingBox();


    GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(geolayer->GetProjection(),m_CurrentWindowState.projection);

    BoundingBox bb_newref = transformer->Transform(bb_orgref);

    delete transformer;

    LookAt(bb_newref);
}

void WorldWindow::LookAt(BoundingBox b)
{
    m_Camera2D->LookAt(b);
    m_Camera3D->PlaceAndLookAtAuto(m_Camera2D->GetBoundingBox());

    //get screen dim (min 1)
    float scw = std::max(1.0f,((float)(m_OpenGLCLManager->GL_GLOBAL.Width)));
    float sch = std::max(1.0f,(float)( m_OpenGLCLManager->GL_GLOBAL.Height));

    m_CurrentWindowState.translation_x = b.GetCenterX();
    m_CurrentWindowState.translation_y = b.GetCenterY();

    m_CurrentWindowState.scale = b.GetSizeX();

    m_CurrentWindowState.width = m_CurrentWindowState.scale;
    float ratiohw = (sch/scw);
    m_CurrentWindowState.height = m_CurrentWindowState.scale * ratiohw;

    m_CurrentWindowState.tlx = m_CurrentWindowState.translation_x - 0.5f *m_CurrentWindowState.width;
    m_CurrentWindowState.brx = m_CurrentWindowState.translation_x + 0.5f *m_CurrentWindowState.width;
    m_CurrentWindowState.tly = m_CurrentWindowState.translation_y - 0.5f *m_CurrentWindowState.height;
    m_CurrentWindowState.bry = m_CurrentWindowState.translation_y + 0.5f *m_CurrentWindowState.height;

}

