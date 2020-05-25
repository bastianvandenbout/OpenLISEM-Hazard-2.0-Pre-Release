#include "uivectorlayer.h"
#include "shapeediting.h"

UIVectorLayerEditor::UIVectorLayerEditor(UILayer * rl) : UILayerEditor(rl)
{

    m_VectorLayer =dynamic_cast<UIVectorLayer*>( rl);
    m_VectorLayer->SetDrawPoints(true);
    m_FilePath = m_VectorLayer->GetFilePath();

    m_Shapes = m_VectorLayer->GetShapeFile();
    m_OriginalShapes = new ShapeFile();
    m_OriginalShapes->CopyFrom(m_Shapes);

    AddParameter(UI_PARAMETER_TYPE_GROUP,"Vector Tool","","");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Type","Add/Remove Shape|Edit Shape|Move Shape|Select Feature|Add/Remove Shape To Feature|Edit Attributes","Move Shape");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Shape","Polygon|Rectangle|Circle|Lines|Rectangle (Outline)|Circle (Outline)|Point","Rectangle");
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Value","Value used as parameter to the chosen function","1",-1e31,1e31);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Circle Points","Number of points used to build a polygon approximating a circle","3",3,10000);
    AddParameter(UI_PARAMETER_TYPE_BOOL,"Ask For Attributes","Show dialog to enter attributes when adding a feature","true");

    std::cout << "edit feature count " << m_Shapes->GetLayer(0)->GetFeatureCount() << std::endl;
}


void UIVectorLayerEditor::OnDraw(OpenGLCLManager * m,GeoWindowState state)
{

    if(m_StartedDragging)
    {
        m_DragPrevious = m_DragOrigin;

    }
    if(m_Dragging)
    {
        m_DragDelta = SPHVector3(state.MouseGeoPosX,state.MouseGeoPosY,state.MouseGeoPosZ) - m_DragPrevious;

    }

    QString edit_type_string = GetParameterValue("Type");
    QString edit_shape_string = GetParameterValue("Shape");

    SPHVector2 locn = m_Transformer->Transform(SPHVector2(state.MouseGeoPosX,state.MouseGeoPosZ));

    double tolerence = 0.02 *std::min(state.width,state.height);
    QList<Feature *> features = m_Shapes->GetFeaturesAt(locn,tolerence);

    SPHShape ** s_close = new (SPHShape*)();
    Feature ** f_close = new (Feature*)();

    m_closestVertex = m_Shapes->GetClosestVertex(locn,nullptr,f_close,s_close);

    m_closestEdgeLocation = m_Shapes->GetClosestEdgeLocation(locn);


    SPHVector2 locn1 = m_Transformerinv->Transform(m_closestVertex);
    SPHVector2 locn2 = m_Transformerinv->Transform(m_closestEdgeLocation);

    float tlX_vertex = state.scr_width * (locn1.x - state.tlx)/state.width;
    float tlY_vertex = (state.scr_height * (locn1.y - state.tly)/state.height) ;

    float tlX_edge = state.scr_width * (locn2.x - state.tlx)/state.width;
    float tlY_edge = (state.scr_height * (locn2.y - state.tly)/state.height) ;

    if(features.length() > 0)
    {
        m_VectorLayer->DrawFeature(features.at(0),state,m);
    }

    bool dont_ctrlz = false;
    bool edited = false;

    if(edit_type_string.compare("Add/Remove Shape") == 0 || edit_type_string.compare("Add/Remove Shape To Feature") == 0)
    {

        if(this->m_HasDoneEnter || (this->m_HasDoneMouseClick &&edit_shape_string.compare("Point") == 0) )
        {
            if(!m_HasDoneCtrlZ)
            {
                if(!(m_Shapes == nullptr))
                {
                    if(m_Shapes->GetLayerCount() > 0)
                    {
                        if(edit_shape_string.compare("Polygon") == 0 && Polygonpx.size() > 2)
                        {


                            int success;
                            QList<QString> attrvalues;

                            if(m_AskAttributes)
                            {
                                MatrixTable * attroptions = new MatrixTable();
                                attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                                for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                                {
                                    attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                    attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                                }

                                MatrixTable * attroptionsn = m->DoTableDialog(attroptions);

                                success = m->GetResultingMatrixTableCode();
                                attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                                std::cout << "add polygon, " << success << std::endl;
                                delete attroptions;
                                delete attroptionsn;

                            }else {
                                success = 1;
                            }

                            if(success)
                            {
                                Feature * fnew = new Feature();
                                SPHPolygon * snew = new SPHPolygon();

                                Ring * ring = new Ring();
                                for(int k = 0; k < Polygonpx.size(); k++)
                                {
                                    SPHVector2 point = m_Transformer->Transform(SPHVector2(Polygonpx.at(k),Polygonpy.at(k)));
                                    ring->AddVertex(point.x,point.y);
                                }
                                snew->SetExtRing(ring);
                                fnew->AddShape(snew);

                                std::cout << "Layer Count " << m_Shapes->GetLayerCount()<< std::endl;
                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    std::cout << "add " << std::endl;

                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }


                                m_PolygonPoints.clear();
                                m_PolygonFillX.clear();
                                m_PolygonFillY.clear();
                                Polygonpx.clear();
                                Polygonpy.clear();

                                if(m_PolygonFill != nullptr)
                                {
                                    m_PolygonFill->Destroy();
                                    delete m_PolygonFill;
                                    m_PolygonFill = nullptr;
                                }
                            }
                        }else if(m_HasSquare && (edit_shape_string.compare("Rectangle") == 0 || edit_shape_string.compare("Rectangle (Outline)") == 0))
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = m->DoTableDialog(attroptions);

                            int success = m->GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                SPHVector3 endpos = m_Dragging? SPHVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                                SPHVector2 br = m_Transformer->Transform(SPHVector2(endpos.x,endpos.z));
                                SPHVector2 tl = m_Transformer->Transform(SPHVector2(m_DragOrigin.x,m_DragOrigin.z));

                                if(m_Shapes->GetLayerCount()>0)
                                {

                                    Feature * fnew = new Feature();
                                    if(edit_shape_string.compare("Rectangle") == 0)
                                    {
                                        SPHPolygon * snew = new SPHPolygon();
                                        snew->SetAsSquare(tl,br);
                                        fnew->AddShape(snew);
                                    }else {
                                        SPHLine * snew = new SPHLine();
                                        snew->SetAsSquare(tl,br);
                                        fnew->AddShape(snew);
                                    }

                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }

                                m_HasSquare = false;

                            }

                        }else if(m_HasCircle && (edit_shape_string.compare("Circle") == 0 || edit_shape_string.compare("Circle (Outline)") == 0))
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = m->DoTableDialog(attroptions);

                            int success = m->GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                //draw square from origin position to current position

                                SPHVector3 endpos = m_Dragging? SPHVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                                SPHVector2 br = m_Transformer->Transform(SPHVector2(endpos.x,endpos.z));
                                SPHVector2 tl = m_Transformer->Transform(SPHVector2(m_DragOrigin.x,m_DragOrigin.z));

                                float r = sqrt((tl.y - br.y)*(tl.y - br.y) + (tl.x - br.x)*(tl.x - br.x));

                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    Feature * fnew = new Feature();
                                    if(edit_shape_string.compare("Circle") == 0)
                                    {
                                        SPHPolygon * snew = new SPHPolygon();
                                        snew->SetAsRegularNGon(tl,r);
                                        fnew->AddShape(snew);
                                    }else {
                                        SPHLine * snew = new SPHLine();
                                        snew->SetAsRegularNGon(tl,r);
                                        fnew->AddShape(snew);
                                    }
                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }
                                m_HasCircle = false;

                            }

                        }else if(edit_shape_string.compare("Lines") == 0 && Polygonpx.size() > 1)
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = m->DoTableDialog(attroptions);

                            int success = m->GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    Feature * fnew = new Feature();
                                    SPHLine* snew = new SPHLine();


                                    for(int k = 0; k < Polygonpx.size(); k++)
                                    {
                                        SPHVector2 point = m_Transformer->Transform(SPHVector2(Polygonpx.at(k),Polygonpy.at(k)));
                                        snew->AddVertex(point.x,point.y);
                                    }
                                    fnew->AddShape(snew);

                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }

                                m_PolygonPoints.clear();
                                m_PolygonFillX.clear();
                                m_PolygonFillY.clear();
                                Polygonpx.clear();
                                Polygonpy.clear();

                                if(m_PolygonFill != nullptr)
                                {
                                    m_PolygonFill->Destroy();
                                    delete m_PolygonFill;
                                    m_PolygonFill = nullptr;
                                }

                            }


                        }else if(edit_shape_string.compare("Point") == 0)
                        {
                            MatrixTable * attroptions = new MatrixTable();
                            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
                            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
                            {
                                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
                            }
                            MatrixTable * attroptionsn = m->DoTableDialog(attroptions);

                            int success = m->GetResultingMatrixTableCode();
                            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

                            delete attroptions;
                            delete attroptionsn;
                            if(success)
                            {
                                SPHVector2 p = m_Transformer->Transform(SPHVector2(state.MouseGeoPosX,state.MouseGeoPosZ));
                                if(m_Shapes->GetLayerCount()>0)
                                {
                                    Feature * fnew = new Feature();
                                    SPHPoint * point = new SPHPoint();
                                    point->SetPos(p.x,p.y);
                                    ShapeLayerChange lc = ShapeLayerChangeAddFeature(m_Shapes,m_Shapes->GetLayer(0),fnew,attrvalues);
                                    ApplyShapeLayerChange(m_Shapes,lc);
                                    m_Edits.append(lc);
                                    edited = true;
                                    m_VectorLayer->NotifyDataChange();

                                }
                            }

                        }
                    }
                }
            }else
            {
                dont_ctrlz = true;
            }
        }

    }else if(edit_type_string.compare("Edit Shape") == 0 || edit_type_string.compare("Move Shape") == 0)
    {

        if(m_StartedDragging)
        {
            //starting drag, see if we meet criteria
            if(!((*s_close == nullptr)|| (*f_close == nullptr)))
            {
                SPHShape * shapeclose = *s_close;
                Feature * featureclose = *f_close;

                float pixdist_vertex = sqrt((tlX_vertex - state.MousePosX)*(tlX_vertex - state.MousePosX) + (tlY_vertex - state.MousePosY)*(tlY_vertex - state.MousePosY));

                if(edit_type_string.compare("Edit Shape") == 0)
                {
                    if(pixdist_vertex < 100 )
                    {
                        SetSelected(featureclose,shapeclose);

                        if(s_close != nullptr)
                        {
                                if(*s_close != nullptr)
                                {
                                    (*s_close)->GetClosestVertexP(locn,x1t,y1t);
                                }
                        }

                    }else
                    {
                        SetSelected(nullptr, nullptr);
                    }
                }else if(edit_type_string.compare("Move Shape") == 0)
                {
                    if(features.length() > 0)
                    {
                        for(int i = 0; i < features.at(0)->GetShapeCount(); i++)
                        {
                            if(features.at(0)->GetShape(i)->Contains(locn,tolerence))
                            {
                                SetSelected(features.at(0),features.at(0)->GetShape(i));
                                break;
                            }
                        }
                    }else
                    {
                        SetSelected(nullptr, nullptr);
                    }
                }
            }else
            {
                SetSelected(nullptr, nullptr);
            }
        }
        if(m_Dragging)
        {
            //during drag, update the selected shape only
            SPHVector2 pos_dragold = SPHVector2(m_DragPrevious.X(),m_DragPrevious.Z());
            SPHVector2 pos_dragcurrent = SPHVector2(state.MouseGeoPosX,state.MouseGeoPosZ);

            SPHVector2 pos_dragoldn = m_Transformer->Transform(pos_dragold);
            SPHVector2 pos_dragcurrentn = m_Transformer->Transform(pos_dragcurrent);

            SPHVector2 dragv = pos_dragcurrentn - pos_dragoldn;
            SPHShape * shapeclose = GetSelectedShapeC();
            if(shapeclose != nullptr)
            {
                if(edit_type_string.compare("Edit Shape") == 0)
                {
                    shapeclose->MoveVertex(*x1t,*y1t,dragv);
                }else if(edit_type_string.compare("Move Shape") == 0)
                {
                    shapeclose->Move(dragv);
                }
            }
        }

        if(m_StoppedDragging)
        {
            //when stopping the drag, apply the changes to the shapefile

            SPHShape * shapeclose = GetSelectedShapeC();
            SPHShape * shapecloseorg = GetSelectedShape();
            Feature * featureclose = GetSelectedFeature();

            if(shapeclose != nullptr && featureclose != nullptr)
            {
                SPHShape * news= shapeclose->Copy();
                ShapeLayerChange lc = ShapeLayerChangeReplaceShape(m_Shapes,shapecloseorg,news);
                ApplyShapeLayerChange(m_Shapes,lc);
                m_Edits.append(lc);
                edited = true;
                m_VectorLayer->NotifyDataChange();

                SetSelected(featureclose,news);
            }
        }

    }else if(edit_type_string.compare("Select Feature") == 0)
    {

        if(m_HasDoneDelete)
        {
            Feature*f = GetSelectedFeature();
            if(f != nullptr)
            {
                ShapeLayerChange lc = ShapeLayerChangeRemoveFeature(m_Shapes,f);
                ApplyShapeLayerChange(m_Shapes,lc);
                m_Edits.append(lc);
                edited = true;
                m_VectorLayer->NotifyDataChange();
            }
        }


    }else if(edit_type_string.compare("Edit Attributes") == 0)
    {


        if(m_DoAlterAttributeDialog)
        {
            MatrixTable * attroptions = new MatrixTable();
            attroptions->SetSize(m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2,3);
            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
            {
                attroptions->SetValue(i+2,0,QString::number(m_Shapes->GetLayer(0)->GetAttributeList(i)->m_type));
                attroptions->SetValue(i+2,1,m_Shapes->GetLayer(0)->GetAttributeName(i));
            }
            MatrixTable * attroptionsn = m->DoTableDialog(attroptions);

            int success = m->GetResultingMatrixTableCode();
            QList<QString> attrvalues = attroptionsn->GetVerticalStringList(2,2,m_Shapes->GetLayer(0)->GetNumberOfAttributes()+2-1);

            delete attroptions;
            delete attroptionsn;

            m_DoAlterAttributeDialog = false;
            m_DoAlterAttributeFeature = nullptr;
        }
    }

    bool undid = false;
    if(m_HasDoneCtrlZ && !dont_ctrlz)
    {
        if(m_Edits.length() > 0)
        {
            UndoShapeLayerChange(m_Shapes,m_Edits.at(m_Edits.length()-1));
            m_Edits.removeAt(m_Edits.length()-1);
            undid = true;
            edited = true;
            m_VectorLayer->NotifyDataChange();
        }


    }

    if(edited)
    {
        m_IsChanged = true;
    }

    m_HasDoneDelete = false;
    m_HasDoneEnter = false;
    m_HasDoneCtrlZ = false;
    m_HasDoneMouseClick = false;
    m_HasDoneRightMouseClick = false;
    m_HasDoneEscape = false;
    m_HasDoneMouseDoubleClick = false;

    m_StartedDragging = false;
    m_StoppedDragging = false;

    if(m_Dragging)
    {
        m_DragPrevious = SPHVector3(state.MouseGeoPosX,state.MouseGeoPosY,state.MouseGeoPosZ);
    }

    delete s_close;
    delete f_close;

}



void UIVectorLayerEditor::OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm)
{

    SPHVector2 locn = m_Transformer->Transform(SPHVector2(state.MouseGeoPosX,state.MouseGeoPosZ));

    double tolerence = 0.02 *std::min(state.width,state.height);
    QList<Feature *> features = m_Shapes->GetFeaturesAt(locn,tolerence);

    SPHVector2 locn1 = m_Transformerinv->Transform(m_closestVertex);
    SPHVector2 locn2 = m_Transformerinv->Transform(m_closestEdgeLocation);

    float tlX_vertex = state.scr_width * (locn1.x - state.tlx)/state.width;
    float tlY_vertex = (state.scr_height * (locn1.y - state.tly)/state.height) ;

    m->m_ShapePainter->DrawSquare(tlX_vertex -5,tlY_vertex-5,10,10,SPHVector4(1.0,1.0,1.0,0.75));

    float tlX_edge = state.scr_width * (locn2.x - state.tlx)/state.width;
    float tlY_edge = (state.scr_height * (locn2.y - state.tly)/state.height) ;

    m->m_ShapePainter->DrawSquare(tlX_edge -5,tlY_edge-5,10,10,SPHVector4(1.0,1.0,1.0,0.75));

    if(features.length() > 0)
    {
        m_VectorLayer->DrawFeature(features.at(0),state,m,SPHVector4(1.0,1.0,1.0,0.2));
    }

    if(m_SelectedShape != nullptr)
    {
        m_VectorLayer->DrawShape(m_SelectedShapeC,state,m,SPHVector4(1.0,0.6,0.3,0.2));
    }
    if(m_SelectedShapeC != nullptr)
    {
         m_VectorLayer->DrawShape(m_SelectedShapeC,state,m,SPHVector4(1.0,0.6,0.3,0.75));
    }
    if(m_SelectedFeature != nullptr && m_SelectedFeature != features.at(0))
    {
        m_VectorLayer->DrawFeature(m_SelectedFeature,state,m,SPHVector4(1.0,1.0,1.0,0.4));
    }

    if(IsDraw() && m_Shapes != nullptr)
    {
        QString edit_type_string = GetParameterValue("Type");
        QString edit_shape_string = GetParameterValue("Shape");


        if(edit_type_string.compare("Add/Remove Shape") == 0 || edit_type_string.compare("Add/Remove Shape To Feature") == 0)
        {

            if(edit_shape_string.compare("Polygon") == 0)
            {

                {
                    if(Polygonpx.size() > 0)
                    {
                        std::vector<float> plx;
                        std::vector<float> ply;

                        for(int i = 0; i < Polygonpx.size(); i++)
                        {
                            float tlX = state.scr_width * (Polygonpx.at(i) - state.tlx)/state.width;
                            float tlY = (state.scr_height * (Polygonpy.at(i)- state.tly)/state.height) ;

                            plx.push_back(tlX);
                            ply.push_back(tlY);
                        }

                        float mtlX = state.scr_width * (state.MouseGeoPosX - state.tlx)/state.width;
                        float mtlY = (state.scr_height * (state.MouseGeoPosZ- state.tly)/state.height) ;

                        plx.push_back(mtlX);
                        ply.push_back(mtlY);

                        m->m_ShapePainter->DrawLines(plx.data(),ply.data(),std::max(1.0,5.0 * state.ui_scale2d3d),Polygonpx.size()+1,SPHVector4(0.0,0.0,0.0,0.5));
                    }

                    if(m_PolygonFillX.size() > 0)
                    {
                        {
                            std::vector<float> plx;
                            std::vector<float> ply;

                            for(int i = 0; i < m_PolygonFillX.size(); i++)
                            {
                                float tlX = state.scr_width * (m_PolygonFillX.at(i) - state.tlx)/state.width;
                                float tlY = (state.scr_height * (m_PolygonFillY.at(i)- state.tly)/state.height) ;

                                plx.push_back(tlX);
                                ply.push_back(tlY);
                            }
                            m->m_ShapePainter->DrawPolygon(plx.data(),ply.data(),m_PolygonFillX.size(),SPHVector4(1.0,1.0,1.0,0.75));
                        }
                    }
                }

            }else if(edit_shape_string.compare("Rectangle") == 0 || edit_shape_string.compare("Rectangle (Outline)") == 0)
            {
                if(m_HasSquare)
                {
                    //draw square from origin position to current position

                    SPHVector3 endpos = m_Dragging? SPHVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                    float tlX = state.scr_width * (endpos.x - state.tlx)/state.width;
                    float tlY = (state.scr_height * (endpos.z - state.tly)/state.height) ;

                    float brX = state.scr_width * (m_DragOrigin.x - state.tlx)/state.width;
                    float brY = (state.scr_height * (m_DragOrigin.z- state.tly)/state.height) ;

                    if(tlX > brX)
                    {
                        float temp = brX;
                        brX = tlX;
                        tlX = temp;
                    }
                    if(tlY > brY)
                    {
                        float temp = brY;
                        brY = tlY;
                        tlY = temp;
                    }

                    m->m_ShapePainter->DrawSquare(tlX,tlY,brX-tlX,brY-tlY,SPHVector4(1.0,1.0,1.0,0.75));

                }


            }else if(edit_shape_string.compare("Circle") == 0 || edit_shape_string.compare("Circle (Outline)") == 0)
            {
                if(m_HasCircle)
                {
                    //draw square from origin position to current position

                    SPHVector3 endpos = m_Dragging? SPHVector3(state.MouseGeoPosX,0.0,state.MouseGeoPosZ):m_DragEnd;

                    float brX = state.scr_width * (endpos.x - state.tlx)/state.width;
                    float brY = (state.scr_height * (endpos.z - state.tly)/state.height) ;

                    float tlX = state.scr_width * (m_DragOrigin.x - state.tlx)/state.width;
                    float tlY = (state.scr_height * (m_DragOrigin.z- state.tly)/state.height) ;


                    float r = sqrt((tlY - brY)*(tlY - brY) + (tlX - brX)*(tlX - brX));

                    m->m_ShapePainter->DrawRegularNGon(tlX,tlY,r,25,SPHVector4(1.0,1.0,1.0,0.75));

                }

            }else if(edit_shape_string.compare("Lines") == 0)
            {
                if(Polygonpx.size() > 0)
                {
                    std::vector<float> plx;
                    std::vector<float> ply;

                    for(int i = 0; i < Polygonpx.size(); i++)
                    {
                        float tlX = state.scr_width * (Polygonpx.at(i) - state.tlx)/state.width;
                        float tlY = (state.scr_height * (Polygonpy.at(i)- state.tly)/state.height) ;

                        plx.push_back(tlX);
                        ply.push_back(tlY);
                    }

                    float mtlX = state.scr_width * (state.MouseGeoPosX - state.tlx)/state.width;
                    float mtlY = (state.scr_height * (state.MouseGeoPosZ- state.tly)/state.height) ;

                    plx.push_back(mtlX);
                    ply.push_back(mtlY);

                    m->m_ShapePainter->DrawLines(plx.data(),ply.data(),std::max(1.0,5.0 * state.ui_scale2d3d),Polygonpx.size()+1,SPHVector4(1.0,1.0,1.0,0.75));
                }

            }if(edit_shape_string.compare("Point") == 0)
            {


            }

        }else if(edit_type_string.compare("Edit Shape") == 0)
        {


        }else if(edit_type_string.compare("Move Shape") == 0)
        {



        }else if(edit_type_string.compare("Select Feature") == 0)
        {


        }


    }
}


void UIVectorLayerEditor::OnSave()
{


    m_OriginalShapes->Destroy();
    delete m_OriginalShapes;
    m_OriginalShapes = new ShapeFile();
    m_OriginalShapes->CopyFrom(m_Shapes);

    AS_SaveVectorToFileAbsolute(m_Shapes,GetCurrentFilePath());


    m_IsChanged = false;
    m_VectorLayer->NotifyDataChange();
}

void UIVectorLayerEditor::OnSaveAs(QString path)
{
    m_OriginalShapes->Destroy();
    delete m_OriginalShapes;
    m_OriginalShapes = new ShapeFile();
    m_OriginalShapes->CopyFrom(m_Shapes);

    m_Layer->SetFilePath(path);
    m_FilePath = m_Layer->GetFilePath();

    AS_SaveVectorToFileAbsolute(m_Shapes,m_FilePath);

    m_IsChanged = false;
    m_VectorLayer->NotifyDataChange();
}



void UIVectorLayerEditor::OnClose()
{

    m_Shapes->Destroy();
    delete m_Shapes;
    m_Shapes = nullptr;
    m_VectorLayer->m_Shapes = m_OriginalShapes;
    m_VectorLayer->SetDrawPoints(false);
    m_VectorLayer->NotifyDataChange();
    m_IsPrepared = false;
}
