#ifndef UIMULTIRASTERLAYER_H
#define UIMULTIRASTERLAYER_H

#include "multimap.h"
#include "uigeolayer.h"

class UIMultiRasterLayer : public UIGeoLayer
{
private:


    QList<OpenGLCLTexture *> m_Textures;
    bool m_createdTexture = false;
    cTMultiMap * m_MMap;
    QList<cTMap *> m_MapList;

    OpenGLProgram * m_Program = nullptr;

    bool m_MinMaxSet = false;
    QList<float> m_Val_Min;
    QList<float> m_Val_Max;
    QList<float> m_Val_Avg;

public:


    inline UIMultiRasterLayer(QList<cTMap * > maps, QString name, bool file = false, QString filepath = "", bool native = false) : UIGeoLayer(maps.at(0)->GetProjection(), maps.at(0)->GetBoundingBox(), name,file,filepath,false)
    {
        m_IsNative = native;
        m_MMap = new cTMultiMap(maps);
        m_MapList = maps;
        m_RequiresCRSGLTransform = true;
        m_Style.m_StyleSimpleGradient = false;
        m_Style.m_StyleSimpleRange = false;
        m_Style.m_Bands = maps.length();

        for(int i = 0; i < m_MMap->GetNumberOfBands(); i++)
        {
            m_Val_Avg.append(0);
            m_Val_Min.append(0);
            m_Val_Max.append(0);
        }

        UpdateMinMax();

    }


    inline ~UIMultiRasterLayer()
    {

    }

    inline QString layertypeName()
    {
        return "MultiRasterLayer";
    }


    inline void UpdateMinMax()
    {
        for(int i = 0; i < m_MMap->GetNumberOfBands(); i++)
        {
            cTMap * m_Map =m_MMap->GetBand(i);
            if(m_Map != nullptr)
            {
                float hmax = -1e31f;
                float hmin = 1e31f;
                float havg = 0.0f;
                float n = 0.0;

                if(IsDynamic() || !( m_MinMaxSet))
                {

                    FOR_ROW_COL_MV(m_Map)
                    {
                        if(std::isfinite(m_Map->data[r][c]) && !std::isnan((m_Map->data[r][c])))
                        {
                            n++;
                            havg += (m_Map->data[r][c]);
                            hmax = std::max(hmax,m_Map->data[r][c]);
                            hmin = std::min(hmin,m_Map->data[r][c]);
                        }
                    }

                    m_Val_Avg.replace(i,havg/n);
                    m_Val_Min.replace(i,hmin);
                    m_Val_Max.replace(i,hmax);


                }
            }else
            {
                m_Val_Avg.replace(i,0);
                m_Val_Min.replace(i,0);
                m_Val_Max.replace(i,0);
            }

        }

        m_MinMaxSet = true;
    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm) override
    {
        if(m_createdTexture)
        {
                if(IsDraw() && m_MMap != nullptr)
                {
                    WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                    //Get style
                    SPHStyle s = GetStyle();
                    BoundingBox bb = GetBoundingBox();


                    int index1 = s.m_IndexB1;
                    int index2 = s.m_IndexB2;
                    int index3 = s.m_IndexB3;

                    cTMap * m_Map1 = m_MMap->GetBand(index1);
                    cTMap * m_Map2 = m_MMap->GetBand(index2);
                    cTMap * m_Map3 = m_MMap->GetBand(index3);
                    cTMap * m_Map;

                    if(m_Map1 == nullptr && m_Map2 == nullptr && m_Map3 == nullptr)
                    {
                        return;
                    }else
                    {
                        if(m_Map1 != nullptr)
                        {
                            m_Map =m_Map1;
                        }else if(m_Map2 != nullptr)
                        {
                            m_Map = m_Map2;
                        }else
                        {
                            m_Map = m_Map3;
                        }
                    }

                    OpenGLCLTexture * m_Texture1 = m_Textures.at(index1);
                    OpenGLCLTexture * m_Texture2 = m_Textures.at(index2);
                    OpenGLCLTexture * m_Texture3 = m_Textures.at(index3);
                    float hmax1 = -1e31f;
                    float hmin1 = 1e31f;
                    float hmax2 = -1e31f;
                    float hmin2 = 1e31f;
                    float hmax3 = -1e31f;
                    float hmin3 = 1e31f;


                    UpdateMinMax();

                    hmax1 = m_Val_Max.at(index1);
                    hmin1 = m_Val_Min.at(index1);
                    hmax2 = m_Val_Max.at(index2);
                    hmin2 = m_Val_Min.at(index2);
                    hmax3 = m_Val_Max.at(index3);
                    hmin3 = m_Val_Min.at(index3);

                    if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                    {
                        hmax1 = s.m_Intervalb1.GetMax() ;
                        hmin1 = s.m_Intervalb1.GetMin() ;
                    }
                    if(!(s.m_Intervalb2.GetMax()  == 0.0f && s.m_Intervalb2.GetMin()  == 0.0f) && s.m_Intervalb2.GetMax()  > s.m_Intervalb2.GetMin() )
                    {
                        hmax2 = s.m_Intervalb2.GetMax() ;
                        hmin2 = s.m_Intervalb2.GetMin() ;
                    }
                    if(!(s.m_Intervalb3.GetMax()  == 0.0f && s.m_Intervalb3.GetMin()  == 0.0f) && s.m_Intervalb3.GetMax()  > s.m_Intervalb3.GetMin() )
                    {
                        hmax3 = s.m_Intervalb3.GetMax() ;
                        hmin3 = s.m_Intervalb3.GetMin() ;
                    }

                    //layer geometry
                    float l_width = ((float)(m_Map1->nrCols()))*m_Map1->data.cell_sizeX();
                    float l_height = ((float)(m_Map1->nrRows()))*m_Map1->data.cell_sizeY();
                    float l_cx = m_Map1->data.west() + 0.5f * l_width;
                    float l_cy = m_Map1->data.north()+ 0.5f * l_height;

                    //set shader uniform values
                    OpenGLProgram * program = m->m_GLProgram_uimultimap;

                    // bind shader
                    glad_glUseProgram(program->m_program);
                    // get uniform locations


                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h1max"),hmax1);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h1min"),hmin1);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h2max"),hmax2);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h2min"),hmin2);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h3max"),hmax3);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h3min"),hmin3);


                    int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                    int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                    int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                    int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                    int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                    int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                    int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");

                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),0);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(m_Map1->nrCols()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(m_Map1->nrRows()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(m_Map1->cellSizeX()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(m_Map1->cellSizeY()));
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
                    if(m_Texture1 != nullptr)
                    {
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex1"),0);
                        glad_glActiveTexture(GL_TEXTURE0);
                        glad_glBindTexture(GL_TEXTURE_2D,m_Texture1->m_texgl);
                    }

                    if(m_Texture2 != nullptr)
                    {

                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex2"),1);
                        glad_glActiveTexture(GL_TEXTURE1);
                        glad_glBindTexture(GL_TEXTURE_2D,m_Texture2->m_texgl);
                    }
                    if(m_Texture3 != nullptr)
                    {
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex3"),2);
                        glad_glActiveTexture(GL_TEXTURE2);
                        glad_glBindTexture(GL_TEXTURE_2D,m_Texture3->m_texgl);
                    }

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

                            glad_glUniform1i(tex_x_loc,3);
                            glad_glActiveTexture(GL_TEXTURE3);
                            glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                            glad_glUniform1i(tex_y_loc,4);
                            glad_glActiveTexture(GL_TEXTURE4);
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




                    // set project matrix
                    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                    glad_glUniform1i(islegend_loc,0);
                    glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                    // now render stuff
                    glad_glBindVertexArray(m->m_Quad->m_vao);
                    glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                    glad_glBindVertexArray(0);
                }

        }
    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {

        if(!m_createdTexture)
        {
            for(int i = 0; i < m_MMap->GetNumberOfBands(); i++)
            {
                cTMap * m_Map =m_MMap->GetBand(i);

                if(m_Map != nullptr)
                {
                    OpenGLCLTexture * _T = m->GetMCLGLTexture(m_Map,false,false,true);
                    m_Textures.append(_T);
                    m_createdTexture = true;
                }else
                {
                    m_Textures.append(nullptr);
                }
            }

        }

        m_IsPrepared = true;
    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        m_IsCRSChanged = false;
    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_IsPrepared = false;
    }

    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline double GetMinimumValue(int i) override
    {
        return m_Val_Min.at(i);
    }

    inline double GetAverageValue(int i) override
    {
        return m_Val_Avg.at(i);
    }

    inline double GetMaximumValue(int i) override
    {
        return m_Val_Max.at(i);
    }



    inline LayerInfo GetInfo()
    {
        LayerInfo l;
        l.Add("Type","Raster Layer");
        l.Add("FileName",m_File);
        l.Add("Name",m_Name);
        l.Add(UIGeoLayer::GetInfo());
        for(int i = 0; i < m_MMap->GetNumberOfBands(); i++)
        {
            cTMap * m_Map =m_MMap->GetBand(i);

            if(m_Map != nullptr)
            {
                l.Add("Band " + QString::number(i) +":"," ");
                l.Add("Nr of Rows", QString::number(m_Map->nrRows()));
                l.Add("Nr of Cols", QString::number(m_Map->nrCols()));
                l.Add("CellSize x", QString::number(m_Map->cellSizeX()));
                l.Add("CellSize y", QString::number(m_Map->cellSizeY()));
            }else
            {
                l.Add("Band " + QString::number(i) +":","ERROR");
            }
        }

        return l;
    }


    inline void SetStyle(SPHStyle s, bool OverWriteAll = false) override
    {
        UILayer::SetStyle(s,OverWriteAll);

        m_Style.m_Bands = m_MMap->GetNumberOfBands();
    }

};


#endif // UIMULTIRASTERLAYER_H
