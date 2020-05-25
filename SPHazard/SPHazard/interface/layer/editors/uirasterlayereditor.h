#pragma once

#include "uilayereditor.h"
#include "uirasterstreamlayer.h"
#include "multipolygon.h"
#include "polygon.h"
#include "iogdal.h"

class UIStreamRasterLayer;



class UIRasterLayerEditor : public UILayerEditor
{


private:


    UIStreamRasterLayer * m_RasterLayer;
    cTMap * m_OriginalMap = nullptr;
    cTMap * m_Map = nullptr;

    bool m_Dragging = false;
    SPHVector3 m_DragOrigin;
    bool m_HasSquare = false;
    QList<SPHVector2> m_PolygonPoints;
    SPHMultiPolygon *m_PolygonFill = nullptr;
    std::vector<float> Polygonpx;
    std::vector<float> Polygonpy;
    std::vector<float> m_PolygonFillX;
    std::vector<float> m_PolygonFillY;

    QList<RasterChangeList> m_Edits;

    bool m_HasDoneEnter = false;
    bool m_HasDoneCtrlZ = false;
    bool m_HasDoneMouseClick = false;
    bool m_HasDoneMouseDoubleClick = false;
    bool m_HasDoneEscape = false;

public:

    UIRasterLayerEditor(UILayer * rl);



    void OnPrepare(OpenGLCLManager * m)
    {
        m_IsPrepared = true;
    }
    void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }

    inline void OnParameterChanged(QString name, QString value) override
    {
        m_Dragging = false;
        if(name.compare("Shape") == 0)
        {

            m_HasDoneEscape = false;
            m_HasDoneEnter = false;
            m_HasDoneCtrlZ = false;
            m_HasDoneMouseClick = false;
            m_HasDoneMouseDoubleClick = false;
        }

    }


    void OnDraw(OpenGLCLManager * m,GeoWindowState s);

    void OnDestroy(OpenGLCLManager * m)
    {

    }
    void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm);

    void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }

    void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    void OnDraw3DGeoPost(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    void OnDrawPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}


    void OnFrame(float dt, GLFWwindow*w)
    {



    }

    inline void OnKeyPressed(int button, int action, int modifiers) override
    {

        if(modifiers & GLFW_MOD_CONTROL)
        {

            if(button == GLFW_KEY_Z && action == GLFW_PRESS)
            {
                m_HasDoneCtrlZ = true;
            }
        }

        if(button == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            m_HasDoneEscape = true;
        }

        if(button == GLFW_KEY_ENTER && action == GLFW_PRESS)
        {
            m_HasDoneEnter= true;
        }

    }

    inline void OnGeoMousePressed(int button, int action,GeoWindowState s, SPHVector3 pos) override
    {

        QString edit_shapetype_string = GetParameterValue("Shape");

        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            if(m_Dragging == false)
            {
                m_Dragging = true;

                if(edit_shapetype_string.compare("Rectangle") == 0 )
                {
                    m_HasSquare = true;
                }
                m_DragOrigin= pos;

            }

        }

        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            m_HasDoneMouseClick = true;
            if( action == GLFW_RELEASE ){
                    static auto before = std::chrono::system_clock::now();
                    auto now = std::chrono::system_clock::now();
                    double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();
                    before = now;
                    if(diff_ms>10 && diff_ms<200){
                        m_HasDoneMouseDoubleClick = true;
                    }
                }

            if(m_Dragging == true)
            {
                m_Dragging = false;
            }

            if(edit_shapetype_string.compare("Polygon") == 0 || edit_shapetype_string.compare("Lines") == 0)
            {

                m_PolygonPoints.append(SPHVector2(pos.x,pos.z));

                if(m_PolygonPoints.length() > 1)
                {
                    Polygonpx.clear();
                    Polygonpy.clear();

                    for(int i = 0; i < m_PolygonPoints.length(); i++)
                    {
                        Polygonpx.push_back(m_PolygonPoints.at(i).x);
                        Polygonpy.push_back(m_PolygonPoints.at(i).y);
                    }
                }else
                {
                    Polygonpx.clear();
                    Polygonpy.clear();
                }

                if(m_PolygonPoints.length() > 2)
                {
                    SPHPolygon * p = new SPHPolygon();
                    Ring * r = new Ring();
                    for(int i = 0; i < m_PolygonPoints.length(); i++)
                    {
                        r->AddVertex(m_PolygonPoints.at(i).x,m_PolygonPoints.at(i).y);
                    }
                    p->SetExtRing(r);
                    if(m_PolygonFill != nullptr)
                    {
                        m_PolygonFill->Destroy();
                        delete m_PolygonFill;
                        m_PolygonFill = nullptr;
                    }
                    m_PolygonFill = p->Triangulate();
                    m_PolygonFillX.clear();
                    m_PolygonFillY.clear();
                    for(int i = 0; i < m_PolygonFill->GetPolygonCount(); i++)
                    {
                        SPHPolygon * p = m_PolygonFill->GetPolygon(i);
                        if(p->GetExtRing()->GetVertexCount() > 2)
                        {
                            double * data = p->GetExtRing()->GetVertexData();
                            m_PolygonFillX.push_back(data[0]);
                            m_PolygonFillY.push_back(data[1]);
                            m_PolygonFillX.push_back(data[2]);
                            m_PolygonFillY.push_back(data[3]);
                            m_PolygonFillX.push_back(data[4]);
                            m_PolygonFillY.push_back(data[5]);
                        }
                    }
                }else
                {
                    m_PolygonFillX.clear();
                    m_PolygonFillY.clear();
                }
            }




            m_Dragging = false;
        }

        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {

            if(edit_shapetype_string.compare("Rectangle(drag)") == 0 )
            {
                m_HasSquare = false;
            }

            if(edit_shapetype_string.compare("Polygon") == 0 ||edit_shapetype_string.compare("Lines") == 0  )
            {
                if(m_PolygonPoints.length() > 0)
                {
                    m_PolygonPoints.removeLast();
                }
                if(m_PolygonPoints.length() > 1)
                {

                    Polygonpx.clear();
                    Polygonpy.clear();

                    for(int i = 0; i < m_PolygonPoints.length(); i++)
                    {
                        Polygonpx.push_back(m_PolygonPoints.at(i).x);
                        Polygonpy.push_back(m_PolygonPoints.at(i).y);
                    }
                }else
                {
                    Polygonpx.clear();
                    Polygonpy.clear();
                }
                if(m_PolygonPoints.length() > 2)
                {
                    SPHPolygon * p = new SPHPolygon();
                    Ring * r = new Ring();
                    for(int i = 0; i < m_PolygonPoints.length(); i++)
                    {
                        r->AddVertex(m_PolygonPoints.at(i).x,m_PolygonPoints.at(i).y);
                    }
                    p->SetExtRing(r);
                    if(m_PolygonFill != nullptr)
                    {
                        m_PolygonFill->Destroy();
                        delete m_PolygonFill;
                        m_PolygonFill = nullptr;
                    }
                    m_PolygonFill = p->Triangulate();
                    m_PolygonFillX.clear();
                    m_PolygonFillY.clear();
                    for(int i = 0; i < m_PolygonFill->GetPolygonCount(); i++)
                    {
                        SPHPolygon * p = m_PolygonFill->GetPolygon(i);
                        if(p->GetExtRing()->GetVertexCount() > 2)
                        {
                            double * data = p->GetExtRing()->GetVertexData();
                            m_PolygonFillX.push_back(data[0]);
                            m_PolygonFillY.push_back(data[1]);
                            m_PolygonFillX.push_back(data[2]);
                            m_PolygonFillY.push_back(data[3]);
                            m_PolygonFillX.push_back(data[4]);
                            m_PolygonFillY.push_back(data[5]);
                        }
                    }
                }else
                {
                    m_PolygonFillX.clear();
                    m_PolygonFillY.clear();
                }
            }
        }
    }


    inline void OnSave() override
    {

        //save raster using known file path
        AS_SaveMapToFileAbsolute(m_Map,GetCurrentFilePath());

        for(int r = 0; r < m_Map->nrRows(); r++)
        {
            for(int c = 0; c < m_Map->nrCols(); c++)
            {
                m_OriginalMap->Drc = m_Map->Drc;
            }
        }

        m_IsChanged = false;
    }

    inline void OnSaveAs(QString path) override
    {
        //save as using the provided path
        m_Layer->SetFilePath(path);
        m_FilePath = m_Layer->GetFilePath();
        AS_SaveMapToFileAbsolute(m_Map,m_FilePath);

        for(int r = 0; r < m_Map->nrRows(); r++)
        {
            for(int c = 0; c < m_Map->nrCols(); c++)
            {
                m_OriginalMap->Drc = m_Map->Drc;
            }
        }

        m_IsChanged = false;
    }

    inline MatrixTable * GetAttributes() override
    {


        return nullptr;
    }

    inline void SetAttributes(MatrixTable * t) override
    {


        m_IsChanged = true;
    }


    inline void OnClose();

};
