#pragma once

#include "uilayereditor.h"

#include "uilayereditor.h"
#include "uivectorlayer.h"
#include "shapefile.h"



class UIVectorLayer;

class UIVectorLayerEditor : public UILayerEditor
{


private:

    UIVectorLayer * m_VectorLayer;

    ShapeFile * m_OriginalShapes;
    ShapeFile * m_Shapes;

    Feature * m_SelectedFeature = nullptr;
    SPHShape * m_SelectedShape = nullptr;
    SPHShape * m_SelectedShapeC = nullptr;

    double ** x1t = new (double*);
    double ** y1t = new (double*);
    double ** x2t = new (double*);
    double ** y2t = new (double*);

    bool m_StartedDragging = false;
    bool m_StoppedDragging = false;
    bool m_Dragging = false;
    SPHVector3 m_DragOrigin;
    SPHVector3 m_DragPrevious;
    SPHVector3 m_DragDelta;
    SPHVector3 m_DragEnd;
    bool m_HasSquare = false;
    bool m_HasCircle = false;
    QList<SPHVector2> m_PolygonPoints;
    SPHMultiPolygon *m_PolygonFill = nullptr;
    std::vector<float> Polygonpx;
    std::vector<float> Polygonpy;
    std::vector<float> m_PolygonFillX;
    std::vector<float> m_PolygonFillY;

    QList<ShapeLayerChange> m_Edits;

    bool m_AskAttributes;

    SPHVector2 m_closestVertex;
    SPHVector2 m_closestEdgeLocation;

    bool m_HasDoneDelete = false;
    bool m_HasDoneEnter = false;
    bool m_HasDoneCtrlZ = false;
    bool m_HasDoneMouseClick = false;
    bool m_HasDoneRightMouseClick = false;
    bool m_HasDoneMouseDoubleClick = false;
    bool m_HasDoneEscape = false;


    bool m_DoAlterAttributeDialog = false;
    Feature * m_DoAlterAttributeFeature = nullptr;
public:

    UIVectorLayerEditor(UILayer * rl);


    void OnPrepare(OpenGLCLManager * m)
    {
        UILayerEditor::OnPrepare(m);
        m_IsPrepared = true;
    }
    void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
        UILayerEditor::OnCRSChanged(m,s,tm);
    }

    inline SPHShape * GetSelectedShape()
    {
        return m_SelectedShape;
    }
    inline SPHShape * GetSelectedShapeC()
    {
        return m_SelectedShape;
    }


    inline Feature * GetSelectedFeature()
    {
        return m_SelectedFeature;
    }

    inline void SetSelected(Feature * f, SPHShape * s)
    {

        m_SelectedShape = s;
        m_SelectedFeature = f;
        if( m_SelectedShapeC != nullptr)
        {
            delete m_SelectedShapeC;
            m_SelectedShapeC = nullptr;
        }
        if(s!= nullptr)
        {
            m_SelectedShapeC = s->Copy();
        }else
        {
            m_SelectedShapeC = nullptr;
        }
    }


    inline void OnParameterChanged(QString name, QString value) override
    {
        m_Dragging = false;
        if(name.compare("Shape") == 0 || name.compare("Type") == 0)
        {

            m_HasDoneEscape = false;
            m_HasDoneDelete = false;
            m_HasDoneEnter = false;
            m_HasDoneCtrlZ = false;
            m_HasDoneMouseClick = false;
            m_HasDoneMouseDoubleClick = false;
            m_StartedDragging = false;

            SetSelected(nullptr,nullptr);
        }

        if(name.compare("Ask For Attributes") == 0)
        {
            m_AskAttributes = value.toInt();
        }
    }

    void OnDraw(OpenGLCLManager * m, GeoWindowState s );

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

        if(button == GLFW_KEY_DELETE && action == GLFW_PRESS)
        {
            m_HasDoneDelete= true;
        }

    }


    inline void OnGeoMousePressed(int button, int action,GeoWindowState s, SPHVector3 pos) override
    {

        QString edit_type_string = GetParameterValue("Type");
        QString edit_shapetype_string = GetParameterValue("Shape");

        if(edit_type_string.compare("Select Feature") == 0 || edit_type_string.compare("Edit Attributes") == 0)
        {
            if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {
                SPHVector2 locn = m_Transformer->Transform(SPHVector2(s.MouseGeoPosX,s.MouseGeoPosZ));

                double tolerence = 0.02 *std::min(s.width,s.height);
                QList<Feature *> features = m_Shapes->GetFeaturesAt(locn,tolerence);

                if(features.length() > 0)
                {

                    if(edit_type_string.compare("Edit Attributes") == 0)
                    {
                        m_DoAlterAttributeDialog = true;
                        m_DoAlterAttributeFeature = features.at(0);
                    }

                    for(int i = 0; i < features.at(0)->GetShapeCount(); i++)
                    {
                        SPHShape * s = features.at(0)->GetShape(i);
                        if(s->Contains(locn,tolerence))
                        {
                            SetSelected(features.at(0),s);

                            break;
                        }
                    }
                }else {

                    SetSelected(nullptr,nullptr);

                }


                if(m_Dragging == false)
                {
                    m_StartedDragging = true;
                    m_Dragging = true;
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
            }

            if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
            {

                SetSelected(nullptr,nullptr);
                m_HasDoneRightMouseClick = true;

            }


        }else if(edit_type_string.compare("Edit Shape") == 0 || edit_type_string.compare("Move Shape") == 0)
        {

            if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {

                if(m_Dragging == false)
                {
                    m_StartedDragging = true;
                    m_Dragging = true;

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
                    m_StoppedDragging = true;
                    m_DragEnd = pos;
                }
            }

            if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
            {
                SetSelected(nullptr,nullptr);
                m_HasDoneRightMouseClick = true;

            }

        }if(edit_type_string.compare("Add/Remove Shape") == 0 || edit_type_string.compare("Add/Remove Shape As Feature") == 0)
        {
            if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {
                if(m_Dragging == false)
                {
                    m_StartedDragging = true;
                    m_Dragging = true;

                    if(edit_shapetype_string.compare("Circle (Outline)") == 0 ||edit_shapetype_string.compare("Circle") == 0 )
                    {
                        m_HasCircle = true;
                    }

                    if(edit_shapetype_string.compare("Rectangle") == 0 ||edit_shapetype_string.compare("Rectangle (Outline)") == 0 )
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


                    m_DragEnd = pos;
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
                if(edit_shapetype_string.compare("Circle (Outline)") == 0 ||edit_shapetype_string.compare("Circle") == 0 )
                {
                    m_HasCircle = false;
                }

                if(edit_shapetype_string.compare("Rectangle") == 0 ||edit_shapetype_string.compare("Rectangle (Outline)") == 0 )
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
    }


    void OnFrame(float dt, GLFWwindow*w)
    {

    }

    void OnSave() override;

    void OnSaveAs(QString path) override;

    inline void OnClose();

    inline MatrixTable * GetAttributes() override
    {
        if(m_Shapes != nullptr)
        {
            if(m_Shapes->GetLayerCount() > 0)
            {
                return m_Shapes->GetLayer(0)->GetAttributeTable();
            }
        }

        return nullptr;
    }

    inline void SetAttributes(MatrixTable * t) override
    {


        m_IsChanged = true;
    }


};
