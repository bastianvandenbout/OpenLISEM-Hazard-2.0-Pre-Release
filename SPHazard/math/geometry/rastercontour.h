#ifndef RASTERCONTOUR_H
#define RASTERCONTOUR_H

#include "gdal.h"
#include "ogr_spatialref.h"
#include "ogr_api.h"
#include "gdal_utils.h"
#include "shapefile.h"
#include "shapelayer.h"
#include <QString>
#include "spherror.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "ogr_spatialref.h"
#include "gdal_utils.h"
#include "gdal.h"
#include "gdal_alg.h"


#include "map.h"


inline ShapeFile* RasterContour(cTMap * m,float interval,float start = 0.0, QString attributename = "value")
{
    GDALDriver *poDriver;
    const char *pszDriverName = "Memory";

    poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )
    {
        SPH_ERROR("Could not get vector file driver for memory");
        return new ShapeFile();
    }

    GDALDataset *poDS;

    poDS = poDriver->Create("temp1", 0, 0, 0, GDT_Unknown, NULL );
    if( poDS == NULL )
    {
        SPH_ERROR("Could not write to file: temp1 using driver: memory");
        return new ShapeFile();
    }


    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        SPH_ERROR("Could not get raster file driver for memory");
        return new ShapeFile();
    }

    int const nrRows{m->nrRows()};
    int const nrCols{m->nrCols()};
    int const nrBands{1};
    GDALDataset * d = poDriver->Create("temp2",
                  nrCols, nrRows, nrBands, GDT_Float32, nullptr);

    ToGDALDataset(d,m);

    OGRSpatialReference * ref = new OGRSpatialReference(m->GetProjection().ToGDALRef());
    OGRLayer * l = poDS->CreateLayer("Vectorize",ref,wkbLineString,nullptr);

    if(d->GetRasterBand(1) != nullptr)
    {
        GDALContourGenerate(d->GetRasterBand(1)->ToHandle(d->GetRasterBand(1)),interval,0,0,nullptr,0,0,l->ToHandle(l),0,1,nullptr,nullptr);
    }else {
        SPHS_ERROR("No rasterband to create contours from");
    }

    ShapeFile * res = FromGDALDataset(poDS);

    delete ref;
    GDALClose( (GDALDataset*) d );
    GDALClose( (GDALDataset*) poDS);

    return res;

}

inline ShapeFile * AS_RasterContour(cTMap * m, float interval)
{

    return RasterContour(m,interval,0.0,QString("value"));

}

inline ShapeFile * AS_RasterContour(cTMap * m, float interval, std::string attributename = "value")
{

    return RasterContour(m,interval,0.0,QString(attributename.c_str()));

}

inline ShapeFile * AS_RasterContour(cTMap * m, float interval, float start, std::string attributename = "value")
{

    return RasterContour(m,interval,0.0,QString(attributename.c_str()));

}


#endif // RASTERCONTOUR_H
