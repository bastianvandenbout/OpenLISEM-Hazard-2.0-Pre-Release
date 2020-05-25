#pragma once

#include "uigeolayer.h"
#include "uivectorlayereditor.h"

class UIVectorLayerEditor;


class UIVectorLayer : public UIGeoLayer
{

public:
    ShapeFile * m_Shapes = nullptr;
private:
    QList<GLGeometryList *> m_GLGeometryList;
    QList<GLGeometryList *> m_GLTRGeometryList;
    QList<OpenGLGeometry *> m_GLPointSimpleList;
    QList<OpenGLGeometry *> m_GLPointFillList;
    QList<OpenGLGeometry *> m_GLPointLineList;
    QList<OpenGLGeometry *> m_GLLineLineList;
    QList<OpenGLGeometry *> m_GLPolygonFillList;
    QList<OpenGLGeometry *> m_GLPolygonLineList;

    bool m_createdGeometry = false;
    bool m_DrawPoints = false;
    bool m_ShapesChanged = false;
    OpenGLProgram * m_Program = nullptr;


public:

    inline UIVectorLayer()
    {

    }

    inline UIVectorLayer(ShapeFile * shapes, QString name, bool file = false, QString filepath = "", bool native = false) : UIGeoLayer()
    {
        Initialize(shapes,name,file,filepath,native);
    }

    inline void Initialize(ShapeFile * shapes, QString name, bool file = false, QString filepath = "", bool native = false)
    {
        UIGeoLayer::Initialize(shapes->GetProjection(),shapes->GetBoundingBox(),name,file,filepath,false);
        m_IsLayerSaveAble = file;
        m_IsNative = native;
        m_Shapes = shapes;
        m_RequiresCRSGLTransform = false;
        m_Editable = true;
        m_ShapesChanged = true;
        m_Style.m_StyleSimpleGradient = false;
        m_Style.m_StyleSimpleRange = false;
        m_IsPrepared = false;

    }
    inline ~UIVectorLayer()
    {

    }



    inline void SaveLayer(QJsonObject * obj) override
    {
        if(!m_IsNative && m_IsFile)
        {
            SPHSerialize::FromString(obj,"FilePath",m_File);
            SPHSerialize::FromString(obj,"Name",m_Name);
            SPHSerialize::FromString(obj,"ShapeFile",m_File);
            QJsonObject obj_style;
            m_Style.Save(&obj_style);

            obj->insert("Style",obj_style);

            QString proj;
            SPHSerialize::ToString(obj,"CRS",proj);
            m_Projection.FromString(proj);
        }
    }

    inline void RestoreLayer(QJsonObject * obj)
    {

        QString path;
        QString name;
        SPHSerialize::ToString(obj,"ShapeFile",path);
        SPHSerialize::ToString(obj,"Name",name);

        ShapeFile * _M;
        bool error = false;
        try
        {
            _M = LoadVector(path);
        }
        catch (int e)
        {
           error = true;
        }

        if(!error && _M != nullptr)
        {
            Initialize(_M,name,true,path,false);

            QJsonValue sval = (*obj)["Style"];
            if(sval.isObject() && !sval.isUndefined())
            {
                SPHStyle st;
                QJsonObject o =sval.toObject();
                st.Restore(&o);
                this->m_Style.CopyFrom(st);
            }

            QString proj;
            SPHSerialize::ToString(obj,"CRS",proj);
            GeoProjection p;
            p.FromString(proj);
            this->SetProjection(p);
        }else
        {
            m_Exists = false;
        }
    }

    inline QString layertypeName()
    {
        return "VectorLayer";
    }

    inline ShapeFile * GetShapeFile()
    {
        return m_Shapes;
    }


    inline QList<ShapeFile*> GetShapeFiles() override
    {
        QList<ShapeFile *> l = QList<ShapeFile *>();
        l.append(m_Shapes);
        return l;

    }

    inline void SetDrawPoints(bool x)
    {
        m_DrawPoints = x;
    }

    inline void OnDraw(OpenGLCLManager * m, GeoWindowState state) override
    {
        if(m_ShapesChanged)
        {
            DestroyGLBuffers();
            DestroyShapeData();
            GetShapeData();
            TransformShapeData(state);
            CreateGLBuffers();
            m_ShapesChanged = false;
        }
    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm) override
    {

        SPHStyle s = GetStyle();


        glad_glDisable(GL_DEPTH_TEST);

        for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
        {
            GLGeometryList *GLGeometryRaw = m_GLGeometryList.at(i);

            QList<OpenGLGeometry *> glgeomlist = {m_GLPointFillList.at(i),m_GLPointLineList.at(i),m_GLLineLineList.at(i),m_GLPolygonLineList.at(i),m_GLPolygonFillList.at(i)};
            QList<OpenGLProgram *> programlist = {m->m_GLProgram_uivectorp,m->m_GLProgram_uivectorl,m->m_GLProgram_uivectorl,m->m_GLProgram_uivectorl,m->m_GLProgram_uivectorp};
            QList<bool> is_line_list = {false,true,true,true,false};
            QList<bool> is_point_part_list = {true,true,false,false,false};
            QList<SPHLineStyle> ls_list = {s.m_PointLineStyle,s.m_PointLineStyle,s.m_LineLineStyle,s.m_PolygonLineStyle,s.m_PolygonLineStyle};
            QList<SPHFillStyle> fs_list = {s.m_PointFillStyle,s.m_PointFillStyle,s.m_PointFillStyle,s.m_PolygonFillStyle,s.m_PolygonFillStyle};

            for(int j = glgeomlist.length()-1; j > -1; j--)
            {
                SPHLineStyle ls = ls_list.at(j);
                SPHFillStyle fs = fs_list.at(j);
                bool is_line = is_line_list.at(j);
                bool is_point_part = is_point_part_list.at(j);
                OpenGLGeometry * GLGeometry = glgeomlist.at(j);
                if(GLGeometry != nullptr)
                {
                    OpenGLProgram * program = programlist.at(j);

                    // bind shader
                    glad_glUseProgram(program->m_program);

                    int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                    int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                    int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                    int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");

                    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                    glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());
                    glad_glUniform1f(h_max_loc,1.0f);
                    glad_glUniform1f(h_min_loc,0.0f);

                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);

                    if(is_line)
                    {
                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),ls.m_Color1.r,ls.m_Color1.g,ls.m_Color1.b,ls.m_HasBackGroundColor? ls.m_Color1.a : 0.0f);
                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),ls.m_Color2.r,ls.m_Color2.g,ls.m_Color2.b,ls.m_Color2.a);
                        float*pat = new float[16];
                        for(int k = 0; k < 16; k++)
                        {
                            pat[k] = 0.0f;
                        }
                        float pat_total = 0.0f;
                        for(int k = 0; k < ls.pattern.length(); k++)
                        {
                            if(ls.pattern.at(k) > 0)
                            {
                                pat_total += ls.pattern.at(k);
                                pat[k] = pat_total;
                            }
                            if(k == 15)
                            {
                                break;
                            }
                        }
                        glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"pattern"),1,GL_FALSE,pat);

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"patterntotal"),pat_total);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),ls.m_LineWMax * state.ui_scale2d3d);
                        delete[] pat;
                    }else {

                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),fs.m_Color1.r,fs.m_Color1.g,fs.m_Color1.b,fs.m_HasBackGroundColor? fs.m_Color1.a : 0.0f);
                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),fs.m_Color2.r,fs.m_Color2.g,fs.m_Color2.b,(fs.m_IsLines || fs.m_IsShapes)?fs.m_Color2.a : 0.0f);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),fs.m_LineSize * state.ui_scale2d3d);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineangle"),fs.m_LineAngle * state.ui_scale2d3d);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineseperation"),fs.m_LineSeperation * state.ui_scale2d3d);
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"type"),fs.m_IsLines?1:0);
                    }

                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);

                    if(is_point_part)
                    {
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"relative_size"),s.m_PointSize);
                    }else {
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);
                    }



                    for(int k = 0; k <SPHAZARD_SHADER_GRADIENT_NCOLORS; k++)
                    {
                        QString is = QString::number(k);
                        int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                        int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                        if(k < s.m_ColorGradientb1.m_Gradient_Stops.length())
                        {
                            glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb1.m_Gradient_Stops.at(k));
                            ColorF c = s.m_ColorGradientb1.m_Gradient_Colors.at(k);
                            glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                        }else {
                            glad_glUniform1f(colorstop_i_loc,1e30f);
                            glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                        }
                    }

                    //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

                    glad_glBindVertexArray(GLGeometry->m_vao);
                    glad_glDrawElements(GL_TRIANGLES,GLGeometry->m_indexlength,GL_UNSIGNED_INT,0);
                    glad_glBindVertexArray(0);

                    //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                }
            }
        }


        if(m_DrawPoints)
        {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {

                    {

                        OpenGLGeometry * GLGeometry = m_GLPointSimpleList.at(i);
                        if(GLGeometry != nullptr)
                        {
                            OpenGLProgram * program = m->m_GLProgram_uipointsimple;

                            // bind shader
                            glad_glUseProgram(program->m_program);

                            int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                            int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                            int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");

                            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                            glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());
                            glad_glUniform1f(h_max_loc,1.0f);
                            glad_glUniform1f(h_min_loc,0.0f);

                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);

                            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),1.0,0.0,0.0,1.0);
                            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),1.0,0.0,0.0,1.0);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),1 * state.ui_scale2d3d);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineangle"),1 * state.ui_scale2d3d);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineseperation"),1 * state.ui_scale2d3d);
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"type"),0);

                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);

                            glad_glPointSize(5);
                            glad_glBindVertexArray(GLGeometry->m_vao);
                            glad_glDrawElements(GL_POINTS,GLGeometry->m_indexlength,GL_UNSIGNED_INT,0);
                            glad_glBindVertexArray(0);
                        }

                    }
                }
        }

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_IsPrepared = true;
    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        UIGeoLayer::OnCRSChanged(m,s,tm);

        if(m_ShapesChanged)
        {
            DestroyGLBuffers();
            DestroyShapeData();
            GetShapeData();
            TransformShapeData(s);
            CreateGLBuffers();
            m_ShapesChanged = false;
        }

        TransformShapeData(s);
        DestroyGLBuffers();
        CreateGLBuffers();
        m_IsCRSChanged = false;
    }

    inline void DestroyShapeData()
    {
        if(m_Shapes!= nullptr)
        {
            for(int i = 0; i < m_GLGeometryList.length(); i++)
            {
                if(m_GLGeometryList.at(i) != nullptr)
                {
                    delete m_GLGeometryList.at(i);
                }
            }
            m_GLGeometryList.clear();
            for(int i = 0; i < m_GLTRGeometryList.length(); i++)
            {
                if(m_GLTRGeometryList.at(i) != nullptr)
                {
                    delete m_GLTRGeometryList.at(i);
                }
            }
            m_GLTRGeometryList.clear();
        }
    }

    inline void GetShapeData()
    {
        if(m_Shapes!= nullptr)
        {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {
                ShapeLayer * l = m_Shapes->GetLayer(i);
                GLGeometryList *GLGeometry = l->GetGLGeometryList();
                GLGeometryList *GLTRGeometry = GLGeometry->CopyGeomOnly();
                m_GLGeometryList.append(GLGeometry);
                m_GLTRGeometryList.append(GLTRGeometry);
            }
        }
    }

    inline void TransformShapeData(GeoWindowState s)
    {


            //GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(GetProjection(),s.projection);

            //if(!transformer->IsGeneric())
            {
                for(int i = 0; i < m_GLTRGeometryList.length(); i++)
                {

                    m_GLTRGeometryList.at(i)->CopyGeomOnlyFrom(m_GLGeometryList.at(i));
                    m_GLTRGeometryList.at(i)->Transform(m_Transformerinv);
                }
            }
    }

    inline void DestroyGLBuffers()
    {
        for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
        {
            GLGeometryList *GLGeometry = m_GLTRGeometryList.at(i);

            if(m_GLPointSimpleList.at(i) != nullptr)
            {
                m_GLPointSimpleList.at(i)->Destroy();
                delete m_GLPointSimpleList.at(i);
            }
            m_GLPointSimpleList.clear();
            if(m_GLPointLineList.at(i) != nullptr)
            {
                m_GLPointLineList.at(i)->Destroy();
                delete m_GLPointLineList.at(i);
            }
            m_GLPointLineList.clear();
            if(m_GLPointFillList.at(i) != nullptr)
            {
                m_GLPointFillList.at(i)->Destroy();
                delete m_GLPointFillList.at(i);
            }
            m_GLPointFillList.clear();
            if(m_GLLineLineList.at(i) != nullptr)
            {
                m_GLLineLineList.at(i)->Destroy();
                delete m_GLLineLineList.at(i);
            }
            m_GLLineLineList.clear();
            if(m_GLPolygonLineList.at(i) != nullptr)
            {
                m_GLPolygonLineList.at(i)->Destroy();
                delete m_GLPolygonLineList.at(i);
            }
            m_GLPolygonLineList.clear();
            if(m_GLPolygonFillList.at(i) != nullptr)
            {
                m_GLPolygonFillList.at(i)->Destroy();
                delete m_GLPolygonFillList.at(i);
            }
            m_GLPolygonFillList.clear();
        }
        m_createdGeometry = false;
    }

    inline void CreateGLBuffers()
    {
        if(!m_createdGeometry)
        {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {
                GLGeometryList *GLGeometry = m_GLTRGeometryList.at(i);

                if(GLGeometry->VertexPoint_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromPoints(GLGeometry->VertexPoint_x.data(),GLGeometry->VertexPoint_y.data(),GLGeometry->VertexPoint_x.size(),true,GLGeometry->Vertex_Point_attr.data(),GLGeometry->Vertex_Point_index.data());
                    m_GLPointSimpleList.append(geom);
                }else {
                    m_GLPointSimpleList.append(nullptr);
                }

                if(GLGeometry->Point_Lines_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromLinesWithOrigin(GLGeometry->Point_Lines_x.data(),GLGeometry->Point_Lines_y.data(),GLGeometry->Point_Lines_connect.data(),GLGeometry->Point_Lines_rx.data(),GLGeometry->Point_Lines_ry.data(),GLGeometry->Point_Lines_x.size(),1,true,GLGeometry->Point_Lines_attr.data(),GLGeometry->Point_Lines_index.data());
                    m_GLPointLineList.append(geom);
                }else {
                    m_GLPointLineList.append(nullptr);
                }
                if(GLGeometry->Point_Triangles_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromTrianglesWithOrigin(GLGeometry->Point_Triangles_x.data(),GLGeometry->Point_Triangles_y.data(),GLGeometry->Point_Triangles_rx.data(),GLGeometry->Point_Triangles_ry.data(),GLGeometry->Point_Triangles_x.size(),true, GLGeometry->Point_Triangles_attr.data(),GLGeometry->Point_Triangles_index.data());
                    m_GLPointFillList.append(geom);

                }else {
                    m_GLPointFillList.append(nullptr);
                }
                if(GLGeometry->Line_Lines_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromLines(GLGeometry->Line_Lines_x.data(),GLGeometry->Line_Lines_y.data(),GLGeometry->Line_Lines_connect.data(),GLGeometry->Line_Lines_x.size(),1,true,GLGeometry->Line_Lines_attr.data(),GLGeometry->Line_Lines_index.data());
                    m_GLLineLineList.append(geom);
                }else {
                    m_GLLineLineList.append(nullptr);
                }
                if(GLGeometry->Polygon_Lines_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromLines(GLGeometry->Polygon_Lines_x.data(),GLGeometry->Polygon_Lines_y.data(),GLGeometry->Polygon_Lines_connect.data(),GLGeometry->Polygon_Lines_x.size(),1,true,GLGeometry->Polygon_Lines_attr.data(),GLGeometry->Polygon_Lines_index.data());
                    m_GLPolygonLineList.append(geom);
                }else {
                    m_GLPolygonLineList.append(nullptr);
                }
                if(GLGeometry->Polygon_Triangles_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromTriangles(GLGeometry->Polygon_Triangles_x.data(),GLGeometry->Polygon_Triangles_y.data(),GLGeometry->Polygon_Triangles_x.size(),true, GLGeometry->Polygon_Triangles_attr.data(),GLGeometry->Polygon_Triangles_index.data());
                    m_GLPolygonFillList.append(geom);
                }else {
                    m_GLPolygonFillList.append(nullptr);
                }
            }
        }else {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {


                GLGeometryList *GLGeometry = m_GLTRGeometryList.at(i);

                if(m_GLPointSimpleList.at(i) != nullptr)
                {
                    m_GLPointSimpleList.at(i)->FillFromPoints(GLGeometry->VertexPoint_x.data(),GLGeometry->VertexPoint_y.data(),GLGeometry->VertexPoint_x.size(),true,GLGeometry->Vertex_Point_attr.data(),GLGeometry->Vertex_Point_index.data());
                }
                if(m_GLPointLineList.at(i) != nullptr)
                {
                    m_GLPointLineList.at(i)->FillFromLinesWithOrigin(GLGeometry->Point_Lines_x.data(),GLGeometry->Point_Lines_y.data(),GLGeometry->Point_Lines_connect.data(),GLGeometry->Point_Lines_rx.data(),GLGeometry->Point_Lines_ry.data(),GLGeometry->Point_Lines_x.size(),1,true,GLGeometry->Point_Lines_attr.data(),GLGeometry->Point_Lines_index.data());
                }
                if(m_GLPointFillList.at(i) != nullptr)
                {
                    m_GLPointFillList.at(i)->FillFromTrianglesWithOrigin(GLGeometry->Point_Triangles_x.data(),GLGeometry->Point_Triangles_y.data(),GLGeometry->Point_Triangles_rx.data(),GLGeometry->Point_Triangles_ry.data(),GLGeometry->Point_Triangles_x.size(),true, GLGeometry->Point_Triangles_attr.data(),GLGeometry->Point_Triangles_index.data());
                }
                if(m_GLLineLineList.at(i) != nullptr)
                {
                    m_GLLineLineList.at(i)->FillFromLines(GLGeometry->Line_Lines_x.data(),GLGeometry->Line_Lines_y.data(),GLGeometry->Line_Lines_connect.data(),GLGeometry->Line_Lines_x.size(),1,true,GLGeometry->Line_Lines_attr.data(),GLGeometry->Line_Lines_index.data());
                }
                if(m_GLPolygonLineList.at(i) != nullptr)
                {

                    m_GLPolygonLineList.at(i)->FillFromLines(GLGeometry->Polygon_Lines_x.data(),GLGeometry->Polygon_Lines_y.data(),GLGeometry->Polygon_Lines_connect.data(),GLGeometry->Polygon_Lines_x.size(),1,true,GLGeometry->Polygon_Lines_attr.data(),GLGeometry->Polygon_Lines_index.data());
                }
                if(m_GLPolygonFillList.at(i) != nullptr)
                {

                    m_GLPolygonFillList.at(i)->FillFromTriangles(GLGeometry->Polygon_Triangles_x.data(),GLGeometry->Polygon_Triangles_y.data(),GLGeometry->Polygon_Triangles_x.size(),true, GLGeometry->Polygon_Triangles_attr.data(),GLGeometry->Polygon_Triangles_index.data());
                }
            }
        }
        m_createdGeometry = true;
    }

    inline void DrawFeature(Feature * s,GeoWindowState state, OpenGLCLManager * m, SPHVector4 color = SPHVector4(1.0,1.0,1.0,0.3))
    {
        if(s == nullptr)
        {
            return;
        }

        for(int i =0; i < s->GetShapeCount(); i++)
        {
            DrawShape(s->GetShape(i),state,m,color);
        }
    }

    inline void DrawShape(SPHShape * s,GeoWindowState state,OpenGLCLManager * m,SPHVector4 color = SPHVector4(1.0,1.0,1.0,0.3))
    {
        if(s == nullptr)
        {
            return;
        }

        GLGeometryList * gl = new GLGeometryList();
        Feature::AddShapeToGlGeometryList(gl,0,s);

        //GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(GetProjection(),state.projection);

        gl->Transform(m_Transformerinv);
        gl->TransformToScreen(BoundingBox(state.tlx,state.brx,state.tly,state.bry),state.scr_pixwidth,state.scr_pixheight);

        m->m_ShapePainter->DrawPolygon(gl->Polygon_Triangles_x.data(),gl->Polygon_Triangles_y.data(),gl->Polygon_Triangles_x.size(),color);
        m->m_ShapePainter->DrawLines(gl->Polygon_Lines_x.data(),gl->Polygon_Lines_y.data(),5,gl->Polygon_Lines_x.size(),color);
        m->m_ShapePainter->DrawLines(gl->Line_Lines_x.data(),gl->Line_Lines_y.data(),5,gl->Line_Lines_x.size(),color);

        for(int i = 0; i < gl->PointPoint_x.size(); i++)
        {
            m->m_ShapePainter->DrawSquare(gl->PointPoint_x.at(i) -5,gl->PointPoint_y.at(i) -5,10,10,color);
        }
        delete gl;

    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {
        DestroyShapeData();
        DestroyGLBuffers();

        m_IsPrepared = false;
    }

    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline virtual LayerProbeResult Probe(SPHVector2 Pos, GeoProjection proj, double tolerence)
    {
        LayerProbeResult p;
        p.hit = false;
        //convert location to local coordinate system
        //GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        SPHVector2 locn = m_Transformer->Transform(Pos);


        //then if it is within the bounding box of the raster
        BoundingBox b = GetBoundingBox();
        if(b.Contains(locn.x,locn.y))
        {
            QList<Feature *> features = m_Shapes->GetFeaturesAt(locn, tolerence);

            if(features.length() > 0)
            {
                MatrixTable *t = m_Shapes->GetFeatureAttributes(features.at(0));
                p.hit = true;
                for(int i = 0; i < t->GetNumberOfRows(); i++)
                {
                    p.AttrNames.append(t->GetValueStringQ(i,0));
                    p.AttrValues.append(t->GetValueStringQ(i,1));
                }
                delete t;
            }
        }

        return p;
    }

    inline void NotifyDataChange()
    {
        m_ShapesChanged = true;
    }


    inline virtual QList<QString> GetAllAttributes()
    {
        QList<QString> names;
        if(m_Shapes->GetLayerCount() > 0)
        {
            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
            {
                names.append(m_Shapes->GetLayer(0)->GetAttributeName(i));
            }

        }
        return names;
    }

    inline virtual QList<QString> GetNumberAttributes()
    {
        QList<QString> names;
        if(m_Shapes->GetLayerCount() > 0)
        {
            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
            {
                if(m_Shapes->GetLayer(0)->GetAttributeList(i)->isNumberType())
                {
                    names.append(m_Shapes->GetLayer(0)->GetAttributeName(i));
                }

            }

        }
        return names;
    }

    UILayerEditor* GetEditor();

};
