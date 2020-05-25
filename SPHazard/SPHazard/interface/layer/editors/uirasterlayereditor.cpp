#include "uirasterlayereditor.h"

UIRasterLayerEditor::UIRasterLayerEditor(UILayer * rl) : UILayerEditor(rl)
{

    m_RasterLayer =dynamic_cast<UIStreamRasterLayer*>( rl);

    m_FilePath = m_RasterLayer->GetFilePath();

    m_OriginalMap = m_RasterLayer->GetMap()->GetCopy();
    m_Map = m_RasterLayer->GetMap();

    AddParameter(UI_PARAMETER_TYPE_GROUP,"Draw Tool","","");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Type","Paint|Add|Subtract|Multiply|Converge|Blur","Paint");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Shape","Rectangle|Ellips|Rectangle(drag)|Lines|Polygon","Rectangle");
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Size X","Horizontal extent of the brush shape",QString::number(m_Map->cellSize() * 5.0),0,1000000.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Size Y","Vertical extent of the brush shape",QString::number(m_Map->cellSize() * 5.0),0,1000000.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Feather","Softness of the brush edge","0",0.0,1.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Value","Value to either set or add/subtract/multiply per second","1",-1e31,1e31);


}

void UIRasterLayerEditor::OnDraw(OpenGLCLManager * m,GeoWindowState state)
{

    QString edit_shapetype_string = GetParameterValue("Shape");


    GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(state.projection,m_RasterLayer->GetProjection());

    SPHVector2 MousePosLoc = transformer->Transform(SPHVector2(state.MouseGeoPosX,state.MouseGeoPosZ));


    float edit_cx = MousePosLoc.x;
    float edit_cy = MousePosLoc.y;
    float edit_sx = GetParameterValueDouble("Shape Size X");
    float edit_sy = GetParameterValueDouble("Shape Size Y");
    float edit_ff = GetParameterValueDouble("Shape Feather");
    float edit_value = GetParameterValueDouble("Value");
    float edit_value2 = 0.95;

    RasterAlterFunc f;

    QString PaintFunction = GetParameterValue("Type");

    if(PaintFunction.compare("Paint") == 0)
    {
        f = PaintToRasterCell;
    }else if(PaintFunction.compare("Add") == 0)
    {
        f = AddToRasterCell;
    }else if(PaintFunction.compare("Subtract") == 0)
    {
        f = AddToRasterCell;
        edit_value = edit_value * -1.0;
    }else if(PaintFunction.compare("Multiply") == 0)
    {
        f = MutiplyToRasterCell;
    }else if(PaintFunction.compare("Converge") == 0)
    {
        f = ConvergeRasterCell;
    }else if(PaintFunction.compare("Blur") == 0)
    {
        f = BlurRasterCell;
    }

    bool dont_ctrlz = false;
    bool edited = false;


    if(edit_shapetype_string.compare("Rectangle") == 0 || edit_shapetype_string.compare("Ellips") == 0)
    {
        static auto before = std::chrono::system_clock::now();
        auto now = std::chrono::system_clock::now();
        double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();
        if((m_Dragging && diff_ms > 30)|| m_HasDoneMouseClick )
        {
            before = now;

            if(!m_HasDoneCtrlZ)
            {
                if(edit_shapetype_string.compare("Rectangle") == 0)
                {
                    RasterCoordinateList rl = RasterizeRectangle(m_Map,edit_cx,edit_cy,edit_sx,edit_sy,edit_ff);
                    m_Edits.append(RasterPaint(m_Map,rl,f,edit_value,edit_value2));
                    edited = true;
                    //signal the raster layer to update the opengl texture from the map
                    //and re-calculate the minimum and maximum value
                    m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                }else {
                    RasterCoordinateList rl = RasterizeRectangle(m_Map,edit_cx,edit_cy,edit_sx,edit_sy,edit_ff);
                    m_Edits.append(RasterPaint(m_Map,rl,f,edit_value,edit_value2));
                    edited = true;
                    //signal the raster layer to update the opengl texture from the map
                    //and re-calculate the minimum and maximum value
                    m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
                }

            }else
            {
                dont_ctrlz = true;
            }

        }
    }else if(edit_shapetype_string.compare("Rectangle(drag)") == 0 )
        {
            if(m_HasSquare)
            {
                if(m_HasDoneEnter)
                {

                    if(!m_HasDoneCtrlZ)
                    {


                        //draw square from origin position to current position



                        SPHVector2 tr = transformer->Transform(SPHVector2(state.MouseGeoPosX ,state.MouseGeoPosZ));

                        float tlX = tr.x;
                        float tlY = tr.y;

                        SPHVector2 tr2 = transformer->Transform(SPHVector2(m_DragOrigin.x,m_DragOrigin.z));

                        float brX = tr2.x;
                        float brY = tr2.y;

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

                        RasterCoordinateList rl = RasterizeRectangle(m_Map,(tlX +  brX)*0.5,(tlY +  brY)*0.5,(brX -  tlX),(brY - tlY),edit_ff);
                        m_Edits.append(RasterPaint(m_Map,rl,f,edit_value,edit_value2));
                        edited = true;
                        //signal the raster layer to update the opengl texture from the map
                        //and re-calculate the minimum and maximum value
                        m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());

                        m_HasSquare = false;
                    }else
                    {
                        dont_ctrlz = true;
                    }
                }


            }

    }else if(edit_shapetype_string.compare("Lines") == 0)
    {

        if(Polygonpx.size() > 0)
        {

            if(m_HasDoneEnter)
            {
                if(!m_HasDoneCtrlZ)
                {

                    std::vector<float> plx;
                    std::vector<float> ply;

                    for(int i = 0; i < Polygonpx.size(); i++)
                    {
                        float tlX = Polygonpx.at(i);
                        float tlY = Polygonpy.at(i);

                        SPHVector2 tr = transformer->Transform(SPHVector2(tlX,tlY));

                        plx.push_back(tr.x);
                        ply.push_back(tr.y);
                    }

                    RasterCoordinateList rl = RasterizeLines(m_Map,plx.data(),ply.data(),plx.size(),edit_sx,edit_ff);
                    m_Edits.append(RasterPaint(m_Map,rl,f,edit_value,edit_value2));
                    edited = true;
                    //signal the raster layer to update the opengl texture from the map
                    //and re-calculate the minimum and maximum value
                    m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());

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

                }else
                {
                    dont_ctrlz = true;
                }

            }
        }

    }else if(edit_shapetype_string.compare("Polygon") == 0)
    {

        if(m_PolygonFillX.size() > 0)
        {
           if(m_HasDoneEnter)
           {
                if(!m_HasDoneCtrlZ)
                {

                    {
                        std::vector<float> plx;
                        std::vector<float> ply;

                        for(int i = 0; i < m_PolygonFillX.size(); i++)
                        {
                            float tlX = m_PolygonFillX.at(i);
                            float tlY = m_PolygonFillY.at(i);

                            SPHVector2 tr = transformer->Transform(SPHVector2(tlX,tlY));

                            plx.push_back(tr.x);
                            ply.push_back(tr.y);

                            RasterCoordinateList rl = RasterizeTriangles(m_Map,plx.data(),ply.data(),plx.size());
                            m_Edits.append(RasterPaint(m_Map,rl,f,edit_value,edit_value2));
                            edited = true;
                            //signal the raster layer to update the opengl texture from the map
                            //and re-calculate the minimum and maximum value
                            m_RasterLayer->NotifyDataChange(rl.GetBoundingBox());
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

                }else
                {
                    dont_ctrlz = true;
                }
            }

        }
    }


    bool undid = false;
    if(m_HasDoneCtrlZ && !dont_ctrlz )
    {

        if(m_Edits.length() > 0)
        {

            undid = true;

            RasterUndo(m_Map,m_Edits.at(m_Edits.length()-1));
            //signal the raster layer to update the opengl texture from the map
            //and re-calculate the minimum and maximum value
            m_RasterLayer->NotifyDataChange(m_Edits.at(m_Edits.length()-1).GetBoundingBox());
            m_Edits.removeAt(m_Edits.length()-1);
        }
    }

    if(edited || undid)
    {
        m_IsChanged = true;
    }



    m_HasDoneEnter = false;
    m_HasDoneCtrlZ = false;
    m_HasDoneMouseClick = false;
    m_HasDoneEscape = false;
    m_HasDoneMouseDoubleClick = false;

    delete transformer;

}


void UIRasterLayerEditor::OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm)
{

    if(IsDraw() && m_Map != nullptr)
    {
        QString edit_shapetype_string = GetParameterValue("Shape");

        if(edit_shapetype_string.compare("Rectangle") == 0 || edit_shapetype_string.compare("Ellips") == 0)
        {
            WorldGLTransform * gltransform = tm->Get(state.projection,m_RasterLayer->GetProjection());


            //Get style
            SPHStyle s = m_RasterLayer->GetStyle();
            BoundingBox bb = m_RasterLayer->GetBoundingBox();

            float hmax = -1e31f;
            float hmin = 1e31f;
            float havg = 0.0f;
            float n = 0.0;

            GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(state.projection,m_RasterLayer->GetProjection());

            SPHVector2 MousePosLoc = transformer->Transform(SPHVector2(state.MouseGeoPosX,state.MouseGeoPosZ));

            delete transformer;

            float edit_cx = MousePosLoc.x;
            float edit_cy = MousePosLoc.y;
            float edit_sx = GetParameterValueDouble("Shape Size X");
            float edit_sy = GetParameterValueDouble("Shape Size Y");
            float edit_ff = GetParameterValueDouble("Shape Feather");
            float edit_value = GetParameterValueDouble("Value");

            int edit_shapetype = 0;
            if(edit_shapetype_string.compare("Rectangle") == 0)
            {
                edit_shapetype = 0;
            }else if(edit_shapetype_string.compare("Ellips") == 0)
            {
                edit_shapetype = 1;
            }

            if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
            {
                hmax = s.m_Intervalb1.GetMax() ;
                hmin = s.m_Intervalb1.GetMin() ;
            }

            //layer geometry
            float l_width = ((float)(m_Map->nrCols()))*m_Map->data.cell_sizeX();
            float l_height = ((float)(m_Map->nrRows()))*m_Map->data.cell_sizeY();
            float l_cx = m_Map->data.west() + 0.5f * l_width;
            float l_cy = m_Map->data.north()+ 0.5f * l_height;

            //set shader uniform values
            OpenGLProgram * program = m->m_GLProgram_uimapshape;

            // bind shader
            glad_glUseProgram(program->m_program);
            // get uniform locations

            int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
            int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
            int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
            int tex_loc = glad_glGetUniformLocation(program->m_program,"tex");
            int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
            int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
            int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
            int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
            int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
            int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"edit_shapetype"),edit_shapetype);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_cx"),edit_cx);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_cy"),edit_cy);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_sx"),edit_sx);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_sy"),edit_sy);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_ff"),edit_ff);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"edit_value"),edit_value);


            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_Map->AS_IsLDD? 1:0);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(m_Map->nrCols()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(m_Map->nrRows()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(m_Map->cellSizeX()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(m_Map->cellSizeY()));
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx"),l_cx);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy"),l_cy);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrwidth"),state.scr_width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrheight"),state.scr_height);

            for(int i = 0; i <SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb1.m_Gradient_Stops.at(i));
                    ColorF c = s.m_ColorGradientb1.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }


            // bind texture
            glad_glUniform1i(tex_loc,0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,m_RasterLayer->GetMainTexture()->m_texgl);

            if(gltransform != nullptr)
            {
                if(!gltransform->IsGeneric())
                {
                    BoundingBox b = gltransform->GetBoundingBox();

                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),b.GetSizeX());
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),b.GetSizeY());
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),b.GetCenterX());
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),b.GetCenterY());

                    glad_glUniform1i(istransformed_loc,1);
                    glad_glUniform1i(istransformedf_loc,1);

                    glad_glUniform1i(tex_x_loc,1);
                    glad_glActiveTexture(GL_TEXTURE1);
                    glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                    glad_glUniform1i(tex_y_loc,2);
                    glad_glActiveTexture(GL_TEXTURE2);
                    glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureY()->m_texgl);
                }else {

                    glad_glUniform1i(istransformed_loc,0);
                    glad_glUniform1i(istransformedf_loc,0);
                }

            }else
            {
                glad_glUniform1i(istransformed_loc,0);
                glad_glUniform1i(istransformedf_loc,0);
            }

            glad_glUniform1i(israw_loc,0);



            glad_glUniform1f(h_max_loc,hmax);
            glad_glUniform1f(h_min_loc,hmin);

            // set project matrix
            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
            glad_glUniform1i(islegend_loc,0);
            glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

            // now render stuff
            glad_glBindVertexArray(m->m_Quad->m_vao);
            glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);
        }

        if(edit_shapetype_string.compare("Rectangle(drag)") == 0 )
        {
            if(m_HasSquare)
            {
                //draw square from origin position to current position

                float tlX = state.scr_width * (state.MouseGeoPosX - state.tlx)/state.width;
                float tlY = (state.scr_height * (state.MouseGeoPosZ- state.tly)/state.height) ;

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

        }else if(edit_shapetype_string.compare("Lines") == 0)
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

        }else if(edit_shapetype_string.compare("Polygon") == 0)
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
        }

    }

}

inline void UIRasterLayerEditor::OnClose()
{
    for(int r = 0; r < m_Map->nrRows(); r++)
    {
        for(int c = 0; c < m_Map->nrCols(); c++)
        {
            m_Map->Drc = m_OriginalMap->Drc;
        }
    }
    m_RasterLayer->NotifyDataChange(BoundingBox(0,m_Map->nrRows()-1,0,m_Map->nrCols()-1));

}
