#ifndef VECTORIZE_H
#define VECTORIZE_H

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



inline ShapeFile * Vectorize(cTMap * m,QString attributename = "value", bool connect_diagonal = false)
{

    CPLStringList sl;
    sl.AddString("8");
    //sl.AddNameValue("burn","1.0");

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
    OGRLayer * l = poDS->CreateLayer("Vectorize",ref,wkbPolygon,nullptr);

    if(d->GetRasterBand(1) != nullptr)
    {
        GDALFPolygonize(d->GetRasterBand(1),nullptr,l,0,sl.List(),nullptr,0);
    }else {
        SPHS_ERROR("No rasterband to vectorize");
    }

    ShapeFile * res = FromGDALDataset(poDS);

    delete ref;
    GDALClose( (GDALDataset*) d );
    GDALClose( (GDALDataset*) poDS);

    return res;
}


inline ShapeFile* AS_Vectorize(cTMap * m,std::string attributename = "value", bool connect_diagonal = false)
{

    return Vectorize(m,attributename.c_str(),connect_diagonal);


}


#endif // VECTORIZE_H
