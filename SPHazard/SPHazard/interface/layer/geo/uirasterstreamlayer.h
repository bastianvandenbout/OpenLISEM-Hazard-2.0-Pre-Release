#pragma once

#include "uigeolayer.h"
#include "uirasterlayereditor.h"
#include "rasterdataprovider.h"
#include "QMutex"


#include "uirasterlayereditor.h"


class RasterStreamBuffer
{
public:

     OpenGLCLTexture * GLTexture;
     cTMap * Map;

     GeoProjection projection;
     float tlx = 0.0;
     float tly = 0.0;
     float brx = 0.0;
     float bry = 0.0;
     float width = 0.0;
     float height = 0.0;
     int sizex = 0;
     int sizey = 0;

     int band = 0;

     GeoProjection projection_old;
     float tlx_old = 0.0;
     float tly_old = 0.0;
     float brx_old = 0.0;
     float bry_old = 0.0;
     float width_old = 0.0;
     float height_old = 0.0;

     float cellsize_x = 1.0;
     float cellsize_y = 1.0;

     float cellsize_x_old = 1.0;
     float cellsize_y_old = 1.0;

     bool bufferused = false;
     bool write_done = true;
     bool update_gpu = false;

     BoundingBox box;

     BoundingBox box_old;

     QMutex * m_MapMutex;
     QMutex * m_SignMutex;


     inline RasterStreamBuffer(OpenGLCLManager * m, int pix_x, int pix_y, BoundingBox b, GeoProjection p, int in_band)
     {
         int rows = std::max(10,(int)(pix_y * 1.2));
         int cols = std::max(10,(int)(pix_x * 1.2));
         sizex = cols;
         sizey = rows;
         MaskedRaster<float> raster_data(rows,cols,0.0f,0.0f,1.0f);
         Map = new cTMap(std::move(raster_data),"","");
         GLTexture = m->GetMCLGLTexture(Map,false,false,false);

         band = in_band;

         box = b;
         box_old = b;
         m_MapMutex = new QMutex();
         m_SignMutex = new QMutex();
         //set projection and boundingbox
         projection = p;
         projection_old = p;
         BoundingBox b2 = b;
         b2.Scale(1.0);
         tlx = b2.GetMinX();
         brx = b2.GetMaxX();
         tly = b2.GetMinY();
         bry = b2.GetMaxY();

         width = brx - tlx;
         height = bry - tly;

         tlx_old = tlx;
         brx_old = brx;
         tly_old = tly;
         bry_old = bry;

         width_old = width;
         height_old = height;


         cellsize_x = b2.GetSizeX()/(double(Map->nrCols()));
         cellsize_y = b2.GetSizeY()/(double(Map->nrRows()));
         Map->data._cell_sizeX = b2.GetSizeX()/(double(Map->nrCols()));
         Map->data._cell_sizeY = b2.GetSizeY()/(double(Map->nrRows()));

         cellsize_x_old = cellsize_x;
         cellsize_y_old = cellsize_y;

     }

     inline void SetFrom(BoundingBox b, GeoProjection p, int in_band)
     {

         band = in_band;

         tlx_old = tlx;
         brx_old = brx;
         tly_old = tly;
         bry_old = bry;

         width_old = width;
         height_old = height;

         projection_old = projection;

         BoundingBox b2 = b;
         b2.Scale(1.0);
         tlx = b2.GetMinX();
         brx = b2.GetMaxX();
         tly = b2.GetMinY();
         bry = b2.GetMaxY();

         width = brx - tlx;
         height = bry - tly;

         projection = p;

         Map->data._west = b2.GetMinX();
         Map->data._north = b2.GetMinY();

         cellsize_x_old = cellsize_x;
         cellsize_y_old = cellsize_y;

         cellsize_x = b2.GetSizeX()/(double(Map->nrCols()));
         cellsize_y = b2.GetSizeY()/(double(Map->nrRows()));
         Map->data._cell_sizeX = b2.GetSizeX()/(double(Map->nrCols()));
         Map->data._cell_sizeY = b2.GetSizeY()/(double(Map->nrRows()));

     }

     inline void Destroy(OpenGLCLManager * m)
     {
        delete m_MapMutex;
        delete m_SignMutex;
        delete Map;
        Map = nullptr;
        GLTexture->Destroy();
     }
};


class UIRasterLayerEditor;


class UIStreamRasterLayer : public UIGeoLayer
{
private:



    //maps for each band that can be used to draw from memory
    QList<OpenGLCLTexture * > m_Textures;
    bool m_CreatedTextures = false;
    int m_TextureIndex = 0;

    int m_CurrentTime = 0;
    int m_CurrentTimeIndex = 0;
    bool m_DataHasChanged = false;

    //rgb buffers that are filled with stream data, red green blue for each drawing that has to be done for a frame
    QList<RasterStreamBuffer*> m_Buffersr;
    QList<RasterStreamBuffer*> m_Buffersg;
    QList<RasterStreamBuffer*> m_Buffersb;

    bool m_IsInMemory = true;
    RasterDataProvider * m_RDP = nullptr;

    OpenGLProgram * m_Program = nullptr;



    QList<BoundingBox> m_DataEdits;

public:

    inline UIStreamRasterLayer() : UIGeoLayer()
    {
        m_RequiresCRSGLTransform = true;
        m_Style.m_HasDuoBand = false;
        m_Style.m_HasTripleBand = false;
        m_Style.m_HasSingleBand = true;
        m_Style.m_StyleSimpleGradient = true;
        m_Style.m_StyleSimpleRange = true;
    }

    inline UIStreamRasterLayer(RasterDataProvider * map, QString name, bool file = false, QString filepath = "", bool native = false) : UIGeoLayer()
    {
        m_RequiresCRSGLTransform = true;
        m_Style.m_HasDuoBand = false;
        m_Style.m_HasTripleBand = false;
        m_Style.m_HasSingleBand = true;
        m_Style.m_StyleSimpleGradient = true;
        m_Style.m_StyleSimpleRange = true;

        Initialize(map,name,file,filepath,native);

    }

    inline void Initialize(RasterDataProvider * map,QString name,bool file = false, QString filepath = "", bool native = false)
    {
        m_RequiresCRSGLTransform = true;
        m_Style.m_HasDuoBand = false;
        m_Style.m_HasTripleBand = false;
        m_Style.m_HasSingleBand = true;
        m_Style.m_StyleSimpleGradient = true;
        m_Style.m_StyleSimpleRange = true;


        UIGeoLayer::Initialize(map->GetProjection(), map->GetBoundingBox(), name,file,filepath,false);

        m_IsInMemory = map->IsMemoryMap();

        if(map->IsEditable())
        {
            m_Editable = true;
        }
        m_IsNative = native;
        m_RDP = map;
        m_CouldBeDEM = true;


        if(m_RDP->HasTime())
        {
            m_IsTimeSeriesAble = true;
        }

        if(!map->IsDuoMap() && map->GetBandCount() == 1)
        {
            m_HasLegend = true;
            m_DrawLegend = true;
        }
        if(map->IsDuoMap())
        {
            m_Style.m_HasDuoBand = true;
            m_Style.m_HasTripleBand = false;
            m_Style.m_HasSingleBand = false;
            m_Style.m_StyleSimpleGradient = false;
            m_Style.m_StyleSimpleRange = false;

        }else if(map->GetBandCount() == 1)
        {
            m_Style.m_HasDuoBand = false;
            m_Style.m_HasTripleBand = false;
            m_Style.m_HasSingleBand = true;
            m_Style.m_StyleSimpleGradient = true;
            m_Style.m_StyleSimpleRange = true;
        }else {
            m_Style.m_HasDuoBand = false;
            m_Style.m_HasTripleBand = true;
            m_Style.m_HasSingleBand = false;
            m_Style.m_StyleSimpleGradient = false;
            m_Style.m_StyleSimpleRange = false;
            m_Style.m_Bands = map->GetBandCount();
        }

        if(map->HasTime())
        {
            m_Style.m_HasTime = true;
            m_Style.m_TimeMax = map->GetMaxTime();
            m_Style.m_TimeMin = map->GetMinTime();
        }

    }

    inline ~UIStreamRasterLayer()
    {

    }

    inline QString layertypeName() override
    {
        return "RasterStreamLayer";
    }

    inline bool IsLayerSaveAble() override
    {
        return (!IsNative()) && m_RDP->IsSaveAble();
    }


    inline void SaveLayer(QJsonObject * obj) override
    {
        if(m_RDP->IsSaveAble())
        {
            QJsonObject obj_rdp;
            m_RDP->Save(&obj_rdp);

            obj->insert("RasterDataProvider",obj_rdp);
            SPHSerialize::FromBool(obj,"File",m_IsNative);
            SPHSerialize::FromString(obj,"FilePath",m_File);
            SPHSerialize::FromString(obj,"Name",m_Name);

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

        if(m_RDP != nullptr)
        {
            delete m_RDP;
        }

        m_RDP = new RasterDataProvider();
        QJsonValue rdpval = (*obj)["RasterDataProvider"];
        if(rdpval.isObject() && !rdpval.isUndefined())
        {
            QJsonObject o = rdpval.toObject();
            m_RDP->load(&o);
        }

        SPHSerialize::ToBool(obj,"File",m_IsNative);
        SPHSerialize::ToString(obj,"FilePath",m_File);
        SPHSerialize::ToString(obj,"Name",m_Name);



        if(!m_RDP->Exists())
        {
            m_Exists = false;
        }else
        {

            Initialize(m_RDP,m_Name,true,m_File,false);

            QJsonValue sval = (*obj)["Style"];
            if(sval.isObject() && !sval.isUndefined())
            {
                std::cout << "layer restore 2" << std::endl;

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
        }
    }




    inline QList<cTMap *> GetMaps() override
    {

        QList<cTMap *> l = QList<cTMap *>();
        //l.append((m_Map));
        return l;
    }


    inline void UpdateGLData()
    {

        for(int i = 0; i < m_DataEdits.length(); i++)
        {
            cTMap * m_Map = m_RDP->GetMemoryMap(0,0);
            OpenGLCLTexture * m_Texture = m_Textures.at(0);

            BoundingBox replacebb = m_DataEdits.at(i);

            int minr = replacebb.GetMinX();
            int minc = replacebb.GetMinY();
            int sizer = replacebb.GetSizeX();
            int sizec = replacebb.GetSizeY();

            minr = std::max(0,std::min(m_Map->nrRows()-1,minr));
            minc = std::max(0,std::min(m_Map->nrCols()-1,minc));
            sizer = std::max(0,std::min(m_Map->nrRows()-minr,sizer));
            sizec = std::max(0,std::min(m_Map->nrCols()-minc,sizec));


            MaskedRaster<float> datareplace = MaskedRaster<float>(sizer,sizec,0.0,0.0,1.0);

            for(int r = minr; r < minr + sizer; r++)
            {
                for(int c = minc; c < minc + sizec; c++)
                {
                    datareplace[r - minr][c - minc] = m_Map->data[r][c];
                }

            }
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);
            glad_glTexSubImage2D(GL_TEXTURE_2D,0,minc,minr,sizec,sizer,GL_RED,GL_FLOAT,datareplace.data());
            glad_glBindTexture(GL_TEXTURE_2D,0);
        }

        m_DataEdits.clear();
    }

    RasterStreamBuffer *  Fill_RasterStreamBuffers(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm, QList<RasterStreamBuffer*> &Buffers, int band)
    {
        WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

        BoundingBox b;
        if(!gltransform->IsGeneric())
        {
            b = gltransform->GetBoundingBox();
        }else
        {
            b.Set(state.tlx,state.brx,state.bry,state.tly);
        }

        if(!b.AllFinite())
        {
            b = GetBoundingBox();
        }

        //check if we are out of sight
        if(GetBoundingBox().Overlaps(b))
        {
            BoundingBox bz = GetBoundingBox();

            float pixy = std::max(10,(int)(state.scr_pixwidth * 1.2));
            float pixx = std::max(10,(int)(state.scr_pixheight * 1.2));

            float ratiox = (b.GetSizeX()/pixx)/(std::fabs(m_RDP->GetCellSizeX(band)));
            float ratioy = (b.GetSizeY()/pixy)/(std::fabs(m_RDP->GetCellSizeY(band)));

            if(ratiox < 1.0 || ratioy < 1.0)
            {
                if(ratiox < ratioy)
                {
                    b.ScaleX(1.0/std::max(0.00001f,ratiox));
                    b.ScaleY(1.0/std::max(0.00001f,ratiox));
                }else
                {
                    b.ScaleX(1.0/std::max(0.00001f,ratioy));
                    b.ScaleY(1.0/std::max(0.00001f,ratioy));

                }
            }

            //limit box of view for the map to the map extent

            BoundingBox blim = bz;
            blim.MoveInto(b);
            blim.And(b);


            BoundingBox bfinal = blim;




            //check if the stream buffer is available for this geowindowstate
            RasterStreamBuffer * rsb = nullptr;

            if(!m_DataHasChanged)
            {
                for(int i = 0; i < Buffers.length(); i++)
                {
                    RasterStreamBuffer * rsb_i = Buffers.at(i);

                    //check if this buffer is good in terms of pixels

                    if(!rsb_i->bufferused && band == rsb_i->band)
                    {
                        if(rsb_i->sizex >= std::max(10,(int)(0.95 * state.scr_pixwidth)) && rsb_i->sizey >= std::max(10,(int)(0.95 * state.scr_pixheight)) )
                        {
                            if(rsb_i->sizex <= std::max(10,(int)(1.35 * state.scr_pixwidth)) && rsb_i->sizey <= std::max(10,(int)(1.35 * state.scr_pixheight)) )
                            {
                                //is it inside?
                                if(!(bfinal.GetMinX() < rsb_i->tlx) && !(bfinal.GetMinY() < rsb_i->tly) && !(bfinal.GetMaxX() > rsb_i->brx) && !(bfinal.GetMaxY() > rsb_i->bry))
                                {
                                    //does it cover at least 90 percent
                                    if(bfinal.GetSizeX() > 0.75*rsb_i->width && bfinal.GetSizeY() > 0.75*rsb_i->height)
                                    {
                                       //std::cout << "found buffer" << std::endl;
                                        rsb = rsb_i;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                }
            }


            //not found
            if(rsb == nullptr)
            {

                bfinal.Scale(1.2);
                bfinal.And(GetBoundingBox());


                //check for one with correct pixels, as to not reallocate memory
                for(int i = 0; i < Buffers.length(); i++)
                {
                    //std::cout << "check reuse buffer " << i << std::endl;
                    RasterStreamBuffer * rsb_i = Buffers.at(i);

                    //check if this buffer is good in terms of pixels

                    if(!rsb_i->bufferused)
                    {
                        //enough pixels?
                        if(rsb_i->sizex >= std::max(10,(int)(0.95 * state.scr_pixwidth)) && rsb_i->sizey >= std::max(10,(int)(0.95 * state.scr_pixheight)) )
                        {
                            std::cout << 1 << std::endl;
                            if(rsb_i->sizex <= std::max(10,(int)(1.35 * state.scr_pixwidth)) && rsb_i->sizey <= std::max(10,(int)(1.35 * state.scr_pixheight)) )
                            {
                                //std::cout << "reuse buffer " << std::endl;
                                //reuse
                                rsb = rsb_i;
                                break;

                            }
                        }
                    }
                }

                //make new one
                if(rsb == nullptr)
                {
                    //std::cout << "create  buffer " << std::endl;
                    //create new buffer object

                    rsb = new RasterStreamBuffer(m,state.scr_pixwidth,state.scr_pixheight,bfinal,GetProjection(),band);
                    Buffers.prepend(rsb);

                }else {

                    rsb->SetFrom(bfinal,GetProjection(),band);
                }

                rsb->m_SignMutex->lock();
                if(rsb->write_done)
                {
                    rsb->m_SignMutex->unlock();

                    //std::cout << "ask for rewrite ----------------------###################" << std::endl;
                    //refill values
                    rsb->write_done = false;
                    rsb->update_gpu = true;
                    //request fill for this object from rasterdataprovider
                    m_RDP->FillValuesToRaster(bfinal,rsb->Map,rsb->m_MapMutex,&(rsb->write_done),rsb->m_SignMutex,band,m_CurrentTimeIndex);

                }else
                {
                    rsb->m_SignMutex->unlock();
                }
            }

            rsb->m_SignMutex->lock();

            rsb->bufferused = true;


            //check if we have access to the memory
            if(rsb->write_done)
            {

                //now check if the data from the cpu needs to be refreshed to gpu

                if(rsb->update_gpu)
                {
                    rsb->update_gpu = false;

                    glad_glBindTexture(GL_TEXTURE_2D,rsb->GLTexture->m_texgl);
                    glad_glTexSubImage2D(GL_TEXTURE_2D,0,0,0,rsb->Map->nrCols(),rsb->Map->nrRows(),GL_RED,GL_FLOAT,rsb->Map->data[0]);
                    glad_glBindTexture(GL_TEXTURE_2D,0);
                }

            }

            rsb->m_SignMutex->unlock();

            return rsb;
        }

        return nullptr;
    }


    void Draw_Raster(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm,bool raw_value, float zscale = 1.0f)
    {

        WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());
        SPHStyle s = GetStyle();
        BoundingBox bb = GetBoundingBox();

        bool is_hs = false;
        if(!raw_value)
        {
            is_hs = s.m_IsHS;
        }


        //if there is a streaming map
        if(!(m_RDP->IsMemoryMap()))
        {

            RasterStreamBuffer * rsb_r = nullptr;
            RasterStreamBuffer * rsb_g = nullptr;
            RasterStreamBuffer * rsb_b = nullptr;


            if(m_RDP->GetBandCount() > 1 && !m_RDP->IsDuoMap())
            {
                int index1 = s.m_IndexB1;
                int index2 = s.m_IndexB2;
                int index3 = s.m_IndexB3;

                rsb_r = Fill_RasterStreamBuffers(m,state,tm, m_Buffersr,index1);
                rsb_g = Fill_RasterStreamBuffers(m,state,tm, m_Buffersg,index2);
                rsb_b = Fill_RasterStreamBuffers(m,state,tm, m_Buffersb,index3);
            }else if(m_RDP->IsDuoMap())
            {
                rsb_r = Fill_RasterStreamBuffers(m,state,tm, m_Buffersr,0);
                rsb_g = Fill_RasterStreamBuffers(m,state,tm, m_Buffersg,1);
            }else
            {
                rsb_r = Fill_RasterStreamBuffers(m,state,tm, m_Buffersr,0);
            }


            BoundingBox b;
            if(!gltransform->IsGeneric())
            {
                b = gltransform->GetBoundingBox();
            }else
            {
                b.Set(state.tlx,state.brx,state.bry,state.tly);
            }

            if(!b.AllFinite())
            {
                b = GetBoundingBox();
            }

            //check if we are out of sight
            if(GetBoundingBox().Overlaps(b))
            {

                //now draw this rasterstreambuffer
                if(m_RDP->GetBandCount() == 1 && rsb_r != nullptr)
                {

                    float hmin = m_RDP->GetBandStats(rsb_r->band).min;
                    float hmax = m_RDP->GetBandStats(rsb_r->band).max;


                    if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                    {
                        hmax = s.m_Intervalb1.GetMax() ;
                        hmin = s.m_Intervalb1.GetMin() ;
                    }

                    //layer geometry
                    float l_width = ((float)(rsb_r->Map->nrCols()))*rsb_r->cellsize_x;
                    float l_height = ((float)(rsb_r->Map->nrRows()))*rsb_r->cellsize_y;
                    float l_cx = rsb_r->tlx+ 0.5f * l_width;
                    float l_cy = rsb_r->tly+ 0.5f * l_height;

                    //set shader uniform values
                    OpenGLProgram * program = m->m_GLProgram_uimap;
                    SPHStyle s = GetStyle();

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
                    int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                    int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                    int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");

                    glad_glUniform1f(hsa1_loc,s.angle_hor);
                    glad_glUniform1f(hsa2_loc,s.angle_vert);
                    glad_glUniform1i(ishs_loc,is_hs? 1:0);
                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_RDP->IsLDD()? 1:0);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(rsb_r->Map->nrCols()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(rsb_r->Map->nrRows()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(rsb_r->cellsize_x));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(rsb_r->cellsize_y));
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
                    glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);

                    if(gltransform != nullptr)
                    {
                        if(!gltransform->IsGeneric())
                        {
                            BoundingBox btrf = gltransform->GetBoundingBox();

                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),btrf.GetSizeX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),btrf.GetSizeY());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),btrf.GetCenterX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),btrf.GetCenterY());

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
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"zscale"),raw_value?zscale:s.DemScale);

                    if(raw_value)
                    {
                        glad_glUniform1i(tex_loc,0);
                        glad_glActiveTexture(GL_TEXTURE0);
                        glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


                        glad_glUniform1i(israw_loc,1);
                    }else {
                        glad_glUniform1i(israw_loc,0);
                    }


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

                    if(raw_value)
                    {
                        glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                    }


                }else if(m_RDP->IsDuoMap() == false && m_RDP->GetBandCount() > 1 && rsb_r != nullptr  && rsb_g != nullptr  && rsb_b != nullptr)
                {

                    if(IsDraw())
                    {
                        WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                        //Get style
                        SPHStyle s = GetStyle();
                        BoundingBox bb = GetBoundingBox();


                        int index1 = s.m_IndexB1;
                        int index2 = s.m_IndexB2;
                        int index3 = s.m_IndexB3;



                        OpenGLCLTexture * m_Texture1 = rsb_r->GLTexture;
                        OpenGLCLTexture * m_Texture2 = rsb_g->GLTexture;
                        OpenGLCLTexture * m_Texture3 = rsb_b->GLTexture;
                        float hmax1 = -1e31f;
                        float hmin1 = 1e31f;
                        float hmax2 = -1e31f;
                        float hmin2 = 1e31f;
                        float hmax3 = -1e31f;
                        float hmin3 = 1e31f;

                        hmax1 = m_RDP->GetBandStats(index1).max;
                        hmin1 = m_RDP->GetBandStats(index1).min;
                        hmax2 = m_RDP->GetBandStats(index2).max;
                        hmin2 = m_RDP->GetBandStats(index2).min;
                        hmax3 = m_RDP->GetBandStats(index3).max;
                        hmin3 = m_RDP->GetBandStats(index3).min;

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
                        float l_width = ((float)(rsb_r->Map->nrCols()))*rsb_r->cellsize_x;
                        float l_height = ((float)(rsb_r->Map->nrRows()))*rsb_r->cellsize_y;
                        float l_cx = rsb_r->tlx+ 0.5f * l_width;
                        float l_cy = rsb_r->tly+ 0.5f * l_height;

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
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(rsb_r->Map->nrCols()));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(rsb_r->Map->nrRows()));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(rsb_r->Map->cellSizeX()));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(rsb_r->Map->cellSizeY()));
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


                }else if(m_RDP->IsDuoMap())
                {


                    float hmin1 = m_RDP->GetBandStats(rsb_r->band).min;
                    float hmax1 = m_RDP->GetBandStats(rsb_r->band).max;

                    float hmin2 = m_RDP->GetBandStats(rsb_g->band).min;
                    float hmax2 = m_RDP->GetBandStats(rsb_g->band).max;

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

                    //layer geometry
                    float l_width = ((float)(rsb_r->Map->nrCols()))*rsb_r->cellsize_x;
                    float l_height = ((float)(rsb_r->Map->nrRows()))*rsb_r->cellsize_y;
                    float l_cx = rsb_r->tlx+ 0.5f * l_width;
                    float l_cy = rsb_r->tly+ 0.5f * l_height;

                    //set shader uniform values
                    OpenGLProgram * program = m->m_GLProgram_uiduomap;
                    SPHStyle s = GetStyle();

                    // bind shader
                    glad_glUseProgram(program->m_program);
                    // get uniform locations

                    int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                    int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                    int h_max2_loc = glad_glGetUniformLocation(program->m_program,"h_max2");
                    int h_min2_loc = glad_glGetUniformLocation(program->m_program,"h_min2");
                    int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                    int tex_loc = glad_glGetUniformLocation(program->m_program,"tex1");
                    int tex2_loc = glad_glGetUniformLocation(program->m_program,"tex2");
                    int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                    int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                    int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                    int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                    int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                    int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
                    int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");
                    int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                    int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                    int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");
                    int vel1_loc = glad_glGetUniformLocation(program->m_program,"is_vel");
                    int vel2_loc = glad_glGetUniformLocation(program->m_program,"vel_spacing");


                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_fractional"),s.m_IsFractional? 1:0);

                    glad_glUniform1i(vel1_loc,s.m_IsVelocity? 1:0);
                    glad_glUniform1f(vel2_loc,s.m_VelSpacing);

                    glad_glUniform1f(hsa1_loc,s.angle_hor);
                    glad_glUniform1f(hsa2_loc,s.angle_vert);
                    glad_glUniform1i(ishs_loc,is_hs? 1:0);
                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_RDP->IsLDD()? 1:0);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(rsb_r->Map->nrCols()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(rsb_r->Map->nrRows()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(rsb_r->cellsize_x));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(rsb_r->cellsize_y));
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

                    for(int i = 0; i <SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
                    {
                        QString is = QString::number(i);
                        int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                        int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                        if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                        {
                            glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb2.m_Gradient_Stops.at(i));
                            ColorF c = s.m_ColorGradientb2.m_Gradient_Colors.at(i);
                            glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                        }else {
                            glad_glUniform1f(colorstop_i_loc,1e30f);
                            glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                        }
                    }



                    // bind texture
                    glad_glUniform1i(tex_loc,0);
                    glad_glActiveTexture(GL_TEXTURE0);
                    glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);

                    glad_glUniform1i(tex2_loc,1);
                    glad_glActiveTexture(GL_TEXTURE1);
                    glad_glBindTexture(GL_TEXTURE_2D,rsb_g->GLTexture->m_texgl);


                    if(gltransform != nullptr)
                    {
                        if(!gltransform->IsGeneric())
                        {
                            BoundingBox btrf = gltransform->GetBoundingBox();

                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),btrf.GetSizeX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),btrf.GetSizeY());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),btrf.GetCenterX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),btrf.GetCenterY());

                            glad_glUniform1i(istransformed_loc,1);
                            glad_glUniform1i(istransformedf_loc,1);

                            glad_glUniform1i(tex_x_loc,2);
                            glad_glActiveTexture(GL_TEXTURE2);
                            glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                            glad_glUniform1i(tex_y_loc,3);
                            glad_glActiveTexture(GL_TEXTURE3);
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


                    glad_glUniform1f(h_max_loc,hmax1);
                    glad_glUniform1f(h_min_loc,hmin1);

                    glad_glUniform1f(h_max2_loc,hmax2);
                    glad_glUniform1f(h_min2_loc,hmin2);

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



        //if there is a memory map
        }else {


            //get the relevant GL Textures for the required bands
            if(m_RDP->IsDuoMap() == false)
            {

                //1 band visualization
                if(m_RDP->GetBandCount() == 1)
                {
                    if(m_CreatedTextures)
                    {
                        if(IsDraw() && m_Textures.at(0) != nullptr)
                        {

                            OpenGLCLTexture * Texture = m_Textures.at(0);
                            cTMap * Map = m_RDP->GetMemoryMap(0,0);

                            WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                            //Get style
                            SPHStyle s = GetStyle();
                            BoundingBox bb = GetBoundingBox();

                            float hmax = -1e31f;
                            float hmin = 1e31f;
                            float havg = 0.0f;
                            float n = 0.0;

                            UpdateGLData();

                            hmax = m_RDP->GetBandStats(0).max;
                            hmin = m_RDP->GetBandStats(0).min;


                            if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                            {
                                hmax = s.m_Intervalb1.GetMax() ;
                                hmin = s.m_Intervalb1.GetMin() ;
                            }

                            //layer geometry
                            float l_width = ((float)(Map->nrCols()))*Map->data.cell_sizeX();
                            float l_height = ((float)(Map->nrRows()))*Map->data.cell_sizeY();
                            float l_cx = Map->data.west() + 0.5f * l_width;
                            float l_cy = Map->data.north()+ 0.5f * l_height;

                            //set shader uniform values
                            OpenGLProgram * program = m->m_GLProgram_uimap;

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
                            int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                            int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                            int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");

                            glad_glUniform1f(hsa1_loc,s.angle_hor);
                            glad_glUniform1f(hsa2_loc,s.angle_vert);
                            glad_glUniform1i(ishs_loc,is_hs? 1:0);
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),Map->AS_IsLDD? 1:0);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(Map->nrCols()));
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(Map->nrRows()));
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(Map->cellSizeX()));
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(Map->cellSizeY()));
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
                            glad_glBindTexture(GL_TEXTURE_2D,Texture->m_texgl);

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

                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"zscale"),raw_value?zscale:s.DemScale);

                            if(raw_value)
                            {
                                glad_glUniform1i(tex_loc,0);
                                glad_glActiveTexture(GL_TEXTURE0);
                                glad_glBindTexture(GL_TEXTURE_2D,Texture->m_texgl);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



                                glad_glUniform1i(israw_loc,1);
                            }else {
                                glad_glUniform1i(israw_loc,0);
                            }


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

                            if(raw_value)
                            {
                                glad_glBindTexture(GL_TEXTURE_2D,Texture->m_texgl);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                            }
                        }
                    }






                //three band visualization
                }else
                {
                    int index1 = s.m_IndexB1;
                    int index2 = s.m_IndexB2;
                    int index3 = s.m_IndexB3;

                    if(m_CreatedTextures)
                    {
                            if(IsDraw())
                            {
                                WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                                //Get style
                                SPHStyle s = GetStyle();
                                BoundingBox bb = GetBoundingBox();


                                int index1 = s.m_IndexB1;
                                int index2 = s.m_IndexB2;
                                int index3 = s.m_IndexB3;

                                cTMap * m_Map1 = m_RDP->GetMemoryMap(index1,0);
                                cTMap * m_Map2 = m_RDP->GetMemoryMap(index2,0);
                                cTMap * m_Map3 = m_RDP->GetMemoryMap(index3,0);
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

                                hmax1 = m_RDP->GetBandStats(index1).max;
                                hmin1 = m_RDP->GetBandStats(index1).min;
                                hmax2 = m_RDP->GetBandStats(index2).max;
                                hmin2 = m_RDP->GetBandStats(index2).min;
                                hmax3 = m_RDP->GetBandStats(index3).max;
                                hmin3 = m_RDP->GetBandStats(index3).min;

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


            }else {

                //duo band visualization

                int index1 = s.m_IndexB1;
                int index2 = s.m_IndexB2;

                OpenGLCLTexture * Texture1 = m_Textures.at(index1);
                cTMap * Map1 = m_RDP->GetMemoryMap(index1,0);

                OpenGLCLTexture * Texture2 = m_Textures.at(index2);
                cTMap * Map2 = m_RDP->GetMemoryMap(index2,0);

                float hmin1 = m_RDP->GetBandStats(index1).min;
                float hmax1 = m_RDP->GetBandStats(index1).max;

                float hmin2 = m_RDP->GetBandStats(index2).min;
                float hmax2 = m_RDP->GetBandStats(index2).max;





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


                //layer geometry
                float l_width = ((float)(Map1->nrCols()))*Map1->data.cell_sizeX();
                float l_height = ((float)(Map1->nrRows()))*Map1->data.cell_sizeY();
                float l_cx = Map1->data.west() + 0.5f * l_width;
                float l_cy = Map1->data.north()+ 0.5f * l_height;


                //set shader uniform values
                OpenGLProgram * program = m->m_GLProgram_uiduomap;
                SPHStyle s = GetStyle();

                // bind shader
                glad_glUseProgram(program->m_program);
                // get uniform locations

                int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                int h_max2_loc = glad_glGetUniformLocation(program->m_program,"h_max2");
                int h_min2_loc = glad_glGetUniformLocation(program->m_program,"h_min2");
                int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                int tex_loc = glad_glGetUniformLocation(program->m_program,"tex1");
                int tex2_loc = glad_glGetUniformLocation(program->m_program,"tex2");
                int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
                int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");
                int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");
                int vel1_loc = glad_glGetUniformLocation(program->m_program,"is_vel");
                int vel2_loc = glad_glGetUniformLocation(program->m_program,"vel_spacing");

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_fractional"),s.m_IsFractional? 1:0);

                glad_glUniform1i(vel1_loc,s.m_IsVelocity? 1:0);
                glad_glUniform1f(vel2_loc,s.m_VelSpacing);
                glad_glUniform1f(hsa1_loc,s.angle_hor);
                glad_glUniform1f(hsa2_loc,s.angle_vert);
                glad_glUniform1i(ishs_loc,is_hs? 1:0);
                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_RDP->IsLDD()? 1:0);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(Map1->nrCols()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(Map1->nrRows()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(Map1->cellSizeX()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(Map1->cellSizeY()));
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

                for(int i = 0; i <SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
                {
                    QString is = QString::number(i);
                    int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                    int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                    if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                    {
                        glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb2.m_Gradient_Stops.at(i));
                        ColorF c = s.m_ColorGradientb2.m_Gradient_Colors.at(i);
                        glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                    }else {
                        glad_glUniform1f(colorstop_i_loc,1e30f);
                        glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                    }
                }



                // bind texture
                glad_glUniform1i(tex_loc,0);
                glad_glActiveTexture(GL_TEXTURE0);
                glad_glBindTexture(GL_TEXTURE_2D,Texture1->m_texgl);

                glad_glUniform1i(tex2_loc,1);
                glad_glActiveTexture(GL_TEXTURE1);
                glad_glBindTexture(GL_TEXTURE_2D,Texture2->m_texgl);


                if(gltransform != nullptr)
                {
                    if(!gltransform->IsGeneric())
                    {
                        BoundingBox btrf = gltransform->GetBoundingBox();

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),btrf.GetSizeX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),btrf.GetSizeY());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),btrf.GetCenterX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),btrf.GetCenterY());

                        glad_glUniform1i(istransformed_loc,1);
                        glad_glUniform1i(istransformedf_loc,1);

                        glad_glUniform1i(tex_x_loc,2);
                        glad_glActiveTexture(GL_TEXTURE2);
                        glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                        glad_glUniform1i(tex_y_loc,3);
                        glad_glActiveTexture(GL_TEXTURE3);
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


                glad_glUniform1f(h_max_loc,hmax1);
                glad_glUniform1f(h_min_loc,hmin1);

                glad_glUniform1f(h_max2_loc,hmax2);
                glad_glUniform1f(h_min2_loc,hmin2);

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

    virtual void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
        SPHStyle style = GetStyle();

        if(style.m_IsSurface && this->CouldBeDEM())
        {
            return;
        }
        Draw_Raster(m,s,tm,true,s.projection.GetUnitZMultiplier() * GetStyle().DemScale);
    }

    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

        //is this a raster that acts as a seperate surface layer (value indicates density of objects)
        //in case of transparancy, we need a double pass that provides us with an indication of the depth of the closest surface fragment
        //we only render for the area contained both within the layer extent and the view extent
        //if available, we only render surface for upper two levels of detail

        //we get the height and coloring of the layer just like the normal mapdraw shader

        SPHStyle style = GetStyle();

        if(style.m_IsSurface && this->CouldBeDEM())
        {
            float scalebase = s.m_2D3DRenderTargetScales.at(0);

            for(int i = s.GL_FrameBuffer3DWindow.length()-1; i > -1; i--)
            {



                OpenGLCLMSAARenderTarget * TargetC = s.GL_FrameBuffer3DLayerColor;
                OpenGLCLMSAARenderTarget * TargetD = s.GL_FrameBuffer3DLayerElevation;


                {
                    std::cout << "draw lod level " << i << std::endl;

                    //draw color

                    GeoWindowState sn = s;

                    BoundingBox bn = s.m_2D3DRenderTargetBoundingBox.at(i);
                    std::cout << "bounding box " << bn.GetMinX() << " " << bn.GetMaxX() << " " << bn.GetMinY() << " " << bn.GetMaxY() << std::endl;

                    sn.scale = bn.GetSizeX();
                    sn.width = bn.GetSizeX();
                    sn.height = bn.GetSizeY();
                    sn.tlx = bn.GetCenterX() - 0.5 * bn.GetSizeX();
                    sn.brx = bn.GetCenterX() + 0.5 * bn.GetSizeX();
                    sn.tly = bn.GetCenterY() - 0.5 * bn.GetSizeY();
                    sn.bry = bn.GetCenterY() + 0.5 * bn.GetSizeY();
                    sn.scr_pixwidth = TargetC->GetWidth();
                    sn.scr_pixheight = TargetC->GetHeight();
                    sn.scr_width = TargetC->GetWidth();
                    sn.scr_height = TargetC->GetHeight();
                    sn.draw_ui = false;
                    sn.draw_legends = false;
                    sn.GL_FrameBuffer = TargetC;
                    sn.ui_scale2d3d = scalebase/s.m_2D3DRenderTargetScales.at(i);

                    GLuint FB = TargetC->GetFrameBuffer();

                    m->m_ShapePainter->UpdateRenderTargetProperties(FB,sn.scr_pixwidth,sn.scr_pixheight);
                    m->m_TextPainter->UpdateRenderTargetProperties(FB,sn.scr_pixwidth,sn.scr_pixheight);
                    m->m_TexturePainter->UpdateRenderTargetProperties(FB,sn.scr_pixwidth,sn.scr_pixheight);

                    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
                    glad_glViewport(0,0,sn.scr_pixwidth,sn.scr_pixheight);
                    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glad_glDisable(GL_DEPTH_TEST);
                    glad_glClearColor(0.1,0.1,0.1,1.0);

                    //request raster draw

                    Draw_Raster(m,sn,tm,false);

                    TargetC->BlitToTexture();
                    m->m_ShapePainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m->GL_GLOBAL.Width,m->m_height);
                    m->m_TextPainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m->GL_GLOBAL.Width,m->m_height);
                    m->m_TexturePainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m->GL_GLOBAL.Width,m->m_height);


                    //draw elevation
                    FB = TargetD->GetFrameBuffer();
                    sn.GL_FrameBuffer = TargetD;
                    sn.scr_pixwidth = TargetD->GetWidth();
                    sn.scr_pixheight = TargetD->GetHeight();
                    sn.scr_width = TargetD->GetWidth();
                    sn.scr_height = TargetD->GetHeight();


                    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
                    glad_glDisable(GL_DEPTH_TEST);
                    glad_glViewport(0,0,sn.scr_pixwidth,sn.scr_pixheight);
                    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                    static GLfloat mv[] = {-1e30f, 0.0f, 0.0f, 1.0f};
                    glad_glClearBufferfv(GL_COLOR, 0, mv);


                    //request raster draw
                    Draw_Raster(m,sn,tm,true,s.projection.GetUnitZMultiplier() * GetStyle().DemScale);



                    TargetD->BlitToTexture();
                }



                s.GL_FrameBuffer->SetAsTarget();

                double zscale = 1.0f;//s.projection.GetUnitZMultiplier();

                glad_glDepthMask(GL_TRUE);
                glad_glEnable(GL_DEPTH_TEST);
                glad_glViewport(0,0,s.scr_pixwidth,s.scr_pixheight);
                //set shader uniform values
                OpenGLProgram * program = m->m_GLProgram_uiterrainlayer;

                // bind shader
                glad_glUseProgram(program->m_program);

                glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"viewportSize"),s.scr_pixwidth,s.scr_pixheight);
                glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(i).GetCenterX(),0.0,s.GL_FrameBuffer3DWindow.at(i).GetCenterY());
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSize"),s.GL_FrameBuffer3DWindow.at(i).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i).GetSizeY());

                if(i > 0)
                {
                    glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeY());
                }else
                {
                    glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),0,0);
                }

                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ZScale"),zscale);

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureC"),0);
                glad_glActiveTexture(GL_TEXTURE0);
                glad_glBindTexture(GL_TEXTURE_2D,TargetC->GetTexture());

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureD"),1);
                glad_glActiveTexture(GL_TEXTURE1);
                glad_glBindTexture(GL_TEXTURE_2D,s.GL_FrameBuffer3DElevation.at(i)->GetTexture());

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureDL"),2);
                glad_glActiveTexture(GL_TEXTURE2);
                glad_glBindTexture(GL_TEXTURE_2D,TargetD->GetTexture());

                // now render stuff
                glad_glBindVertexArray(m->m_GeometryPlane->m_vao);
                glad_glPatchParameteri(GL_PATCH_VERTICES,3);
                glad_glDrawElements(GL_PATCHES,m->m_GeometryPlane->m_IndexLength,GL_UNSIGNED_INT,0);
                glad_glBindVertexArray(0);

                glad_glBindTexture(GL_TEXTURE_2D,0);
            }

        }

    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        SPHStyle style = GetStyle();

        if(style.m_IsSurface && this->CouldBeDEM())
        {
            return;
        }
        Draw_Raster(m,s,tm,false);
    }

    inline void CheckDeleteFromBuffers(OpenGLCLManager * m,QList<RasterStreamBuffer *> &list)
    {
        for(int i = list.length()-1; i >-1 ; i--)
        {
            RasterStreamBuffer * rsb_i =  list.at(i);

            //std::cout << "check usage" << i <<  std::endl;

            //this buffer has not been used, and we have repeatedly used another, so this must be obsolete
            if(rsb_i->bufferused == false)
            {
                std::cout << 1 <<  std::endl;
                //destroy and remove buffer
                if(rsb_i->write_done == true)
                {
                    //std::cout << "destroy buffer " << std::endl;
                    rsb_i->m_MapMutex->lock();
                    rsb_i->m_MapMutex->unlock();
                    rsb_i->Destroy(m);
                    list.removeAt(i);
                    delete rsb_i;
                }
            }else {
                rsb_i->bufferused = false;
            }

        }
    }

    inline void DestroyBuffers(OpenGLCLManager * m,QList<RasterStreamBuffer *> &list)
    {
        for(int i =  list.length()-1; i >-1 ; i--)
        {
            RasterStreamBuffer * rsb_i =  list.at(i);

            //std::cout << "check usage" << i <<  std::endl;

            //this buffer has not been used, and we have repeatedly used another, so this must be obsolete

            {
                //destroy and remove buffer
                {
                    //std::cout << "destroy buffer " << std::endl;
                    rsb_i->m_MapMutex->lock();
                    rsb_i->m_MapMutex->unlock();
                    rsb_i->Destroy(m);
                    list.removeAt(i);
                    delete rsb_i;
                }
            }

        }
    }

    inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
    {
        UpdateGLData();

        m_DataHasChanged = false;
        int time = GetStyle().m_CurrentTime;

        if(m_CurrentTime != time)
        {
            m_CurrentTime = time;
            m_CurrentTimeIndex = m_RDP->GetClosestTimeIndex(time);
            CreateGLTextures(m,s);
            m_DataHasChanged = true;

        }
        CheckDeleteFromBuffers(m,m_Buffersr);
        CheckDeleteFromBuffers(m,m_Buffersg);
        CheckDeleteFromBuffers(m,m_Buffersb);

        if(m_RDP->IsNative())
        {
            m_RDP->UpdateBandStats();
        }
    }

    inline void CreateGLTextures(OpenGLCLManager * m,GeoWindowState s)
    {

        if(m_RDP->IsMemoryMap())
        {
            if(!m_RDP->IsNative())
            {
                if(!m_CreatedTextures)
                {
                    for(int i = 0; i < m_RDP->GetBandCount(); i++)
                    {
                        cTMap * map = m_RDP->GetMemoryMap(i,m_CurrentTimeIndex);
                        OpenGLCLTexture * _T = m->GetMCLGLTexture(map,false,false,false);
                        this->m_Textures.append(_T);
                    }
                    m_CreatedTextures = true;
                }else
                {
                    for(int i = 0; i < m_RDP->GetBandCount(); i++)
                    {
                        cTMap * map = m_RDP->GetMemoryMap(i,m_CurrentTimeIndex);

                        glad_glBindTexture(GL_TEXTURE_2D,m_Textures.at(i)->m_texgl);
                        glad_glTexSubImage2D(GL_TEXTURE_2D,0,0,0,map->nrCols(),map->nrRows(),GL_RED,GL_FLOAT,map->data[0]);
                        glad_glBindTexture(GL_TEXTURE_2D,0);
                    }
                    m_CreatedTextures = true;
                }
            }else {
                std::cout << "create native textures " << std::endl;
                    m_Textures.clear();

                    for(int i = 0; i < m_RDP->GetBandCount(); i++)
                    {
                        cTMap * map = m_RDP->GetMemoryMap(i,m_CurrentTimeIndex);
                        OpenGLCLTexture * _T = m_RDP->GetTexture(i,m_CurrentTimeIndex);
                        this->m_Textures.append(_T);
                    }
                    m_CreatedTextures = true;
             }
        }
    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        CreateGLTextures(m,s);

        m_IsPrepared = true;
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState s)
    {
        if(s.legendindex < s.legendtotal)
        {
            if((m_RDP->IsDuoMap()))
            {

            }else {

                float width = s.scr_pixwidth;
                float height = s.scr_pixheight;

                SPHVector4 LineColor = SPHVector4(0.4,0.4,0.4,1.0);
                float linethickness = std::max(1.0f,1.0f*s.ui_scale);
                float framewidth = 0.65*s.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
                float ui_ticktextscale = s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

                float legendwidth =s.ui_scalem*0.085 * width ;
                float legendspacing = s.ui_scalem*0.015* width;

                float legendsq_top = framewidth + (float(s.legendindex)/float(s.legendtotal)) * (height - 2.0 * framewidth) +  (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.025;
                float legendsq_bottom = legendsq_top + (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.975 - 15*s.ui_scale;
                float legendsq_left = width - framewidth - 0.65 * legendwidth + legendspacing;
                float legendsq_right = width - legendspacing;

                float space_text = std::max(50.0f,(legendsq_right  - legendsq_left) * 0.60f);

                float legendcsq_right = std::max(legendsq_left + 10.0f,legendsq_left + (legendsq_right - legendsq_left)-space_text);

                m->m_ShapePainter->DrawSquareLine(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,linethickness,LineColor);

                //Gradient
                m->m_ShapePainter->DrawSquare(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,SPHVector4(0.1,0.1,0.1,1));

                m->m_ShapePainter->DrawSquareGradient(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,&(GetStyleRef()->m_ColorGradientb1));

                //title

                m->m_TextPainter->DrawString(this->m_Name,NULL,legendsq_left - legendwidth * 0.2 ,legendsq_bottom + 3 +  2 * s.ui_scale,SPHVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);


                //ticks

                SPHVector4 TickColor = SPHVector4(0.2,0.2,0.2,1.0);
                SPHVector4 TickSmallColor = SPHVector4(0.5,0.5,0.5,1.0);
                float tickwidth = 2.0f*s.ui_scale;
                float ticksmallwidth = std::max(2.0f,1.5f * ui_ticktextscale);
                float ticklength = 7.0f*s.ui_scale;
                float ticksmalllength = 4.0f* s.ui_scale;
                float tickdist = 2.0f *s.ui_scale;

                int n_ticks  = 10;

                n_ticks = std::min(10,std::max(2,int(0.2 * std::fabs(legendsq_top-legendsq_bottom)/9.0 * ui_ticktextscale)));

                double hmax = 0.0;
                double hmin = 0.0;

                hmax = m_RDP->GetBandStats(0).max;
                hmin = m_RDP->GetBandStats(0).min;

                if(!(GetStyle().m_Intervalb1.GetMax()  == 0.0f && GetStyle().m_Intervalb1.GetMin()  == 0.0f) && GetStyle().m_Intervalb1.GetMax() > GetStyle().m_Intervalb1.GetMin() )
                {
                    hmax = GetStyle().m_Intervalb1.GetMax() ;
                    hmin = GetStyle().m_Intervalb1.GetMin() ;
                }

                for(int i = 0; i < n_ticks + 1; i++)
                {
                    double val = hmax + float(i) * (hmin -hmax)/float(n_ticks);
                    double y = legendsq_bottom +float(i) * float(legendsq_top -legendsq_bottom)/float(n_ticks);
                    m->m_ShapePainter->DrawLine(legendcsq_right,y,legendcsq_right+ticklength + tickdist,y,tickwidth,TickColor);
                    m->m_TextPainter->DrawString(QString::number(val),NULL,legendcsq_right + tickdist+ticklength+1,y + (i == n_ticks? 0.0:-9.0f* ui_ticktextscale),SPHVector4(0.0,0.0,0.0,1.0),9 * ui_ticktextscale);

                }



            }


        }

    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        m_IsCRSChanged = false;
    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {


        //destroy all rasterstreambuffers
        DestroyBuffers(m,m_Buffersr);
        DestroyBuffers(m,m_Buffersg);
        DestroyBuffers(m,m_Buffersb);

        if(!m_RDP->IsNative())
        {
            //destroy all openGL textures for maps
            for(int i = 0; i < m_Textures.length(); i++)
            {
                m_Textures.at(i)->Destroy();
            }
        }

        m_Textures.clear();
        m_CreatedTextures = false;

        //destroy rasterdataprovider
        m_RDP->Destroy();
        delete m_RDP;

        m_IsPrepared = false;
    }

    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline double GetMinimumValue(int i) override
    {
        return m_RDP->GetBandStats(i).min;
    }

    inline double GetAverageValue(int i) override
    {
        return m_RDP->GetBandStats(i).mean;
    }

    inline double GetMaximumValue(int i) override
    {
        return m_RDP->GetBandStats(i).max;
    }

    inline virtual LayerProbeResult Probe(SPHVector2 Pos, GeoProjection proj, double tolerence)
    {
        LayerProbeResult p;
        p.hit = false;
        //convert location to local coordinate system
        GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        SPHVector2 locn = transformer->Transform(Pos);
        //then if it is within the bounding box of the raster
        BoundingBox b = GetBoundingBox();

        if(b.Contains(locn.x,locn.y))
        {
            p.hit = true;

            float valraw = m_RDP->GetValueAtLocation(locn,SPH_RASTER_SAMPLE_NEAREST,0,m_CurrentTimeIndex);

            QString value = QString::number(valraw);
            if(pcr::isMV(valraw))
            {
                value = "Missing Value";
            }

            //finally get a cell location
            p.AttrNames.append(QString("Raster Value"));
            p.AttrValues.append(value);
        }

        return p;
    }

    inline LayerInfo GetInfo()
    {
        LayerInfo l;
        l.Add("Type","Raster Layer");
        l.Add("FileName",m_RDP->GetFilePathAtTime(0));
        l.Add("Name",m_Name);
        l.Add(UIGeoLayer::GetInfo());
        l.Add("Nr of Bands", QString::number(m_RDP->GetBandCount()));
        l.Add("Nr of Rows", QString::number(m_RDP->GetTotalSizeY()));
        l.Add("Nr of Cols", QString::number(m_RDP->GetTotalSizeX()));
        l.Add("CellSize x", QString::number(m_RDP->GetCellSizeX()));
        l.Add("CellSize y", QString::number(m_RDP->GetCellSizeY()));
        return l;
    }

    inline cTMap * GetMap()
    {
        if(m_RDP->GetBandCount()==1 && m_RDP->HasTime() == false)
        {
            if(m_RDP->IsMemoryMap())
            {
                return m_RDP->GetMemoryMap(0,0);
            }
        }

        return nullptr;//m_Map;
    }
    inline OpenGLCLTexture * GetMainTexture()
    {
        if(m_RDP->GetBandCount()==1 && m_RDP->HasTime() == false)
        {
            if(m_RDP->IsMemoryMap())
            {
                return m_Textures.at(0);
            }
        }

        return nullptr;//m_Texture;
    }


    inline virtual MatrixTable * GetTimeSeries(QList<SPHVector2> Pos, GeoProjection proj)
    {
        //allocate matrixtable
        MatrixTable * m = new MatrixTable();

        if(m_RDP->HasTime())
        {
            //get coordinate transformer

            GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

            //get all time values
            QList<int> times = m_RDP->GetTimes();

            //get the number of position (nr of cols), add 1 for time
            //get the number of timesteps (nr of rows)
            m->SetSize(times.length(),Pos.length()+1);

            for(int i = 0; i < times.length(); i++)
            {
                m->SetValue(i,0,times.at(i));
            }

            for(int j = 0; j < Pos.length(); j++)
            {
                SPHVector2 pos = Pos.at(j);

                if(!transformer->IsGeneric())
                {
                    pos = transformer->Transform(pos);
                }

                QList<double> ts = m_RDP->GetTimeSeries(pos);

                for(int i = 0; i < times.length(); i++)
                {
                    m->SetValue(i,j+1,ts.at(i));
                }
            }

        }
        return m;
    }



    inline void NotifyDataChange(BoundingBox b)
    {
        //b should contain row and column bounds to replace
        m_DataEdits.append(b);

    }

    UILayerEditor* GetEditor();



};


inline UIStreamRasterLayer * GetUIModelRasterLayer(cTMap * map, OpenGLCLTexture * m, QString name,QString file, bool native)
{

    QList<QList<cTMap *>> lm;
    QList<QList<OpenGLCLTexture *>> lt;

    QList<cTMap*> lm1;
    lm1.append(map);
    lm.append(lm1);

    QList<OpenGLCLTexture*> lt1;
    lt1.append(m);
    lt.append(lt1);

    return new UIStreamRasterLayer(new RasterDataProvider(lm,lt,false,false),name,false,file,true);

}

inline UIStreamRasterLayer * GetUIModelDuoRasterLayer(cTMap * map, OpenGLCLTexture * m,cTMap * map2, OpenGLCLTexture * m2, QString name,QString file, bool native)
{

    QList<QList<cTMap *>> lm;
    QList<QList<OpenGLCLTexture *>> lt;

    QList<cTMap*> lm1;
    lm1.append(map);
    lm1.append(map2);
    lm.append(lm1);
    QList<OpenGLCLTexture*> lt1;
    lt1.append(m);
    lt1.append(m2);
    lt.append(lt1);
    return new UIStreamRasterLayer(new RasterDataProvider(lm,lt,true,false),name,false,file,true);

}

