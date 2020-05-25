/*************************************************************************
**  openLISEM: a spatial surface water balance and soil erosion model
**  Copyright (C) 2010,2011  Victor Jetten
**  contact:
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  Author: Victor Jetten
**  Developed in: MingW/Qt/
**  website, information and code: http://lisem.sourceforge.net
**
*************************************************************************/
#ifndef CsfMapH
#define CsfMapH
#include <QString>
#include "masked_raster.h"
#include <angelscript.h>
#include <functional>
#include <iostream>
#include "spherror.h"
#include "geoobject.h"

#define FOR_ROW_COL_MV(map) for(int r = 0; r < map->nrRows(); r++)\
    for (int c = 0; c < map->nrCols(); c++)\
    if(!pcr::isMV(map->data[r][c]))

#define FOR_ROW_COL(map) for(int r = 0; r < map->nrRows(); r++)\
    for (int c = 0; c < map->nrCols(); c++)\

#define MAPMATH_PI 3.14159f

#define Drc data[r][c]

#define SPH_ASMAP_BOOLFROMFLOAT(x) (x>0.5f?true:false)


typedef struct RasterBandStats
{
    double min = 0.0;
    double max = 0.0;
    double stdev = 0.0;
    double mean = 0.0;
    int band = 0;
} RasterBandStats;


typedef struct cTMapProps
{
    bool can_be_read = false;
    QString file_path = "";
    int sizex = 0.0;
    int sizey = 0.0;
    int bands = 0;
    int band = 0;
    bool is_ldd = false;
    double cellsizex = 0.0;
    double cellsizey = 0.0;

    QString projection;

    double uly = 0.0;
    double ulx = 0.0;

    inline bool equals(cTMapProps p)
    {
        if(p.can_be_read != can_be_read)
        {
            return false;
        }
        if(sizex != p.sizex)
        {
            return false;
        }
        if(sizey != p.sizey)
        {
            return false;
        }
        if(bands != bands)
        {
            return false;
        }
        if(is_ldd != p.is_ldd)
        {
            return false;
        }
        if(cellsizex != p.cellsizex)
        {
            return false;
        }
        if(cellsizey != p.cellsizey)
        {
            return false;
        }
        if(projection != p.projection)
        {
            return false;
        }
        if(ulx != p.ulx)
        {
            return false;
        }
        if(uly != p.uly)
        {
            return false;
        }
        return true;
    }

} cTMapProps;

/*!
    @brief      A cTMap contains all relevant information about a raster.
    @todo       The data member must be made private.

    cTMap instances contain raster data, projection information and a map name.
    I/O of cTMap instances is handles by functions defined in the io module.
*/
class cTMap : public GeoObject
{

public:

    //! The actual raster.
    MaskedRaster<float> data;

                   cTMap               ()=default;

                   cTMap               (MaskedRaster<float>&& data,
                                        QString const& projection,
                                        QString const& mapName,
                                        bool is_ldd = false);

                   cTMap               (cTMap const& other)=delete;

                   cTMap               (cTMap&& other)=default;

                   ~cTMap              ()=default;

    cTMap&         operator=           (cTMap const& other)=delete;

    cTMap&         operator=           (cTMap&& other)=default;

    int            nrRows              () const;

    int            nrCols              () const;

    double         north               () const;

    double         west                () const;

    double         cellSize            () const;

    double         cellSizeX            () const;
    double         cellSizeY            () const;

    float          val_min             ();
    float          val_max             ();

    QString const& projection          () const;

    QString const& mapName             () const;

    void           setAllMV            ();

    void           MakeMap             (cTMap *dup,
                                        REAL4 value);

    cTMap*         GetCopy             ();

    cTMapProps     GetRasterProps      ();
    RasterBandStats GetRasterBandStats  (bool stdev);

    //functions related to the Angelscript Scripting Access for maps
    //NOTE: maps can have a single numerical value, as indicated by member AS_IsSingleValue
    //the value itself is contained in the 1x1 raster data accasable by map->data[0][0]
public:

    QString    AS_FileName          = "";
    bool           AS_writeonassign     = false;
    std::function<void(cTMap *,QString)> AS_writefunc;
    std::function<cTMap *(QString)> AS_readfunc;
    bool           AS_Created           = false;
    bool           AS_IsSingleValue     = false;
    bool           AS_IsLDD             = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    float&         AS_Value             (int r, int c);
    int            AS_Rows              ();
    int            AS_Cols              ();
    float          AS_North             ();
    float          AS_West              ();
    float          AS_CellSize          ();

    cTMap *        AS_Assign            (cTMap *other);
    cTMap *        AS_OpAdd             (cTMap *other);
    cTMap *        AS_OpMul             (cTMap *other);

    cTMap *        AS_OpNeg             ();
    cTMap *        AS_OpSub             (cTMap *other);
    cTMap *        AS_OpDiv             (cTMap *other);
    cTMap *        AS_OpPow             (cTMap *other);
    cTMap *        AS_OpAddAssign       (cTMap *other);
    cTMap *        AS_OpSubAssign       (cTMap *other);
    cTMap *        AS_OpMulAssign       (cTMap *other);
    cTMap *        AS_OpDivAssign       (cTMap *other);
    cTMap *        AS_OpPowAssign       (cTMap *other);

    cTMap *        AS_Assign            (float other);
    cTMap *        AS_OpAdd             (float other);
    cTMap *        AS_OpMul             (float other);

    cTMap *        AS_OpSub             (float other);
    cTMap *        AS_OpDiv             (float other);
    cTMap *        AS_OpPow             (float other);
    cTMap *        AS_OpAddAssign       (float other);
    cTMap *        AS_OpSubAssign       (float other);
    cTMap *        AS_OpMulAssign       (float other);
    cTMap *        AS_OpDivAssign       (float other);
    cTMap *        AS_OpPowAssign       (float other);

    cTMap *        AS_OpModAssign       (cTMap *other);
    cTMap *        AS_OpModAssign       (float other);
    cTMap *        AS_OpMod             (cTMap *other);
    cTMap *        AS_OpMod             (float other);

    cTMap *        AS_EqualTo           (cTMap *other);
    cTMap *        AS_LargerThen        (cTMap *other);
    cTMap *        AS_SmallerThen       (cTMap *other);
    cTMap *        AS_NotEqualTo        (cTMap *other);
    cTMap *        AS_LargerEqualThen   (cTMap *other);
    cTMap *        AS_SmallerEqualThen  (cTMap *other);

    cTMap *        AS_EqualTo           (float other);
    cTMap *        AS_LargerThen        (float other);
    cTMap *        AS_SmallerThen       (float other);
    cTMap *        AS_NotEqualTo        (float other);
    cTMap *        AS_LargerEqualThen   (float other);
    cTMap *        AS_SmallerEqualThen  (float other);

    cTMap *        AS_And               (cTMap *other);
    cTMap *        AS_Or                (cTMap *other);
    cTMap *        AS_Xor               (cTMap *other);

    cTMap *        AS_And               (float other);
    cTMap *        AS_Or                (float other);
    cTMap *        AS_Xor               (float other);

    cTMap *        AS_AndAssign         (cTMap *other);
    cTMap *        AS_OrAssign          (cTMap *other);
    cTMap *        AS_XorAssign         (cTMap *other);

    cTMap *        AS_AndAssign         (float other);
    cTMap *        AS_OrAssign          (float other);
    cTMap *        AS_XorAssign         (float other);

    cTMap *        AS_Negate            ();

    cTMap *        AS_OpMod_r           (float other);
    cTMap *        AS_OpAdd_r           (float other);
    cTMap *        AS_OpMul_r           (float other);
    cTMap *        AS_OpSub_r           (float other);
    cTMap *        AS_OpDiv_r           (float other);
    cTMap *        AS_OpPow_r           (float other);
    cTMap *        AS_LargerThen_r      (float other);
    cTMap *        AS_SmallerThen_r     (float other);
    cTMap *        AS_NotEqualTo_r      (float other);
    cTMap *        AS_LargerEqualThen_r (float other);
    cTMap *        AS_SmallerEqualThen_r(float other);
    cTMap *        AS_EqualTo_r         (float other);
    cTMap *        AS_And_r             (float other);
    cTMap *        AS_Or_r              (float other);
    cTMap *        AS_Xor_r             (float other);


private:

    //! Projection string as WKT string. Possibly empty.
    QString        _projection;

    QString        _mapName;

};


inline cTMap::cTMap(
    MaskedRaster<float>&& data,
    QString const& projection,
    QString const& mapName,
        bool is_ldd)

    : data(std::forward<MaskedRaster<float>>(data)),
      _projection(projection),
      _mapName(mapName)

{

    m_Projection = GeoProjection::FromString(projection);
    m_BoundingBox = BoundingBox(data.west(),data.west() + data.cell_sizeX() * ((double)(data.nr_cols())),data.north(),data.north() + data.cell_sizeY() * ((double)(data.nr_rows())));

    AS_IsLDD = is_ldd;
}


inline int cTMap::nrRows() const
{
    return static_cast<int>(data.nr_rows());
}


inline int cTMap::nrCols() const
{
    return static_cast<int>(data.nr_cols());
}




inline double cTMap::north() const
{
    return data.north();
}


inline double cTMap::west() const
{
    return data.west();
}


inline double cTMap::cellSize() const
{
    return data.cell_size();
}

inline double cTMap::cellSizeX() const
{
    return data.cell_sizeX();
}


inline double cTMap::cellSizeY() const
{
    return data.cell_sizeY();
}


inline QString const& cTMap::projection() const
{
    return _projection;
}


inline QString const& cTMap::mapName() const
{
    return _mapName;
}


inline void cTMap::setAllMV()
{
    data.set_all_mv();
}


// make a new map according to dup as a mask and filled with value
inline void cTMap::MakeMap(
    cTMap *dup,
    REAL4 value)
{
  if (dup == nullptr)
    return;

  data = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
      dup->west(), dup->cellSizeX(),dup->cellSizeY());

  data.set_all_mv();

  for(int r=0; r < nrRows(); r++)
    for(int c=0; c < nrCols(); c++)
      if (!pcr::isMV(dup->data[r][c]))
        {
          data[r][c] = value;
        }

    SetProjection(dup->GetProjection());
    m_BoundingBox = dup->GetBoundingBox();
}

inline cTMap * cTMap::GetCopy()
{
    cTMap * dup = this;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
        dup->west(), dup->cellSizeX(),dup->cellSizeY());

    cTMap * m = new cTMap(std::move(datac),_projection,_mapName,AS_IsLDD);
    m->AS_IsSingleValue = AS_IsSingleValue;
    m->AS_FileName = AS_FileName;
    m->AS_IsLDD = AS_IsLDD;
    m->m_Projection = m_Projection;
    m->m_BoundingBox = m_BoundingBox;

    m->setAllMV();

    for(int r=0; r < m->nrRows(); r++)
    {
      for(int c=0; c < m->nrCols(); c++)
      {
        if (!pcr::isMV(data[r][c]))
          {
            m->data[r][c] = data[r][c];
          }
      }
    }

    return m;
}


//Angelscript related functionality

inline void cTMap::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void cTMap::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {

        delete this;
    }

}



inline float& cTMap::AS_Value(int r, int c)
{
    return this->data[r][c];
}


inline int cTMap::AS_Rows()
{
    return this->nrRows();
}

inline int cTMap::AS_Cols()
{
    return this->nrCols();
}

inline float cTMap::AS_North()
{
    return this->north();

}

inline float cTMap::AS_West()
{
    return this->west();

}

inline float cTMap::AS_CellSize()
{
    return this->cellSize();

}

inline static cTMap * AS_SetCRS(cTMap *m ,GeoProjection * p)
{
    m = m->GetCopy();
    if(p != nullptr)
    {
        m->SetProjection(*p);
    }
    return m;

}


inline cTMap* FromGDALDatasetR(GDALDataset * dataset)
{
    cTMap * ret;

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        return new cTMap();
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};

    QString datatype(dataset->GetMetadataItem("PCRASTER_VALUESCALE"));

    // Read the first raster band.
    GDALRasterBand* band{dataset->GetRasterBand(1)};
    assert(band);

    int const nr_rows{band->GetYSize()};
    int const nr_cols{band->GetXSize()};
    double const west{transformation[0]};
    double const north{transformation[3]};
    double const cell_size{transformation[1]};
    double const cell_sizeY{transformation[5]};

    MaskedRaster<float> raster_data(nr_rows, nr_cols, north, west, cell_size, cell_sizeY);

    // All raster values are read into float. PCRaster value scales are not
    // taken into account.
    if(band->RasterIO(GF_Read, 0, 0, nr_cols, nr_rows, raster_data[0],
            nr_cols, nr_rows, GDT_Float32, 0, 0) != CE_None) {
        SPH_ERROR(QString("Raster band cannot be read."));
        return new cTMap();
    }

    int hasNoDataValue{false};
    double noDataValue{band->GetNoDataValue(&hasNoDataValue)};
    if(hasNoDataValue) {
        raster_data.replace_with_mv(noDataValue);
    }


    return new cTMap(std::move(raster_data), projection, "",datatype.compare("VS_LDD") == 0);

}

inline GDALDataset * ToGDALDataset(GDALDataset * to, cTMap * raster)
{
    //return new ShapeFile();

    int const nrRows{raster->nrRows()};
    int const nrCols{raster->nrCols()};
    int const nrBands{1};

    MaskedRaster<float> const& raster_data{raster->data};

    double transformation[]{
        raster_data.west(),
        raster_data.cell_sizeX(),
        0.0,
        raster_data.north(),
        0.0,
        raster_data.cell_sizeY()};

    to->SetGeoTransform(transformation);
    to->SetProjection(raster->GetProjection().GetWKT().toStdString().c_str());

    auto band = to->GetRasterBand(1);
    band->SetNoDataValue(-FLT_MAX);
    if(band->RasterIO(GF_Write, 0, 0, nrCols, nrRows,
            const_cast<float*>(&raster_data.cell(0)),
            nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
        SPH_ERROR(QString("Raster band cannot be written to memory file."));
        return to;
    }




    return to;
}

inline cTMapProps     cTMap::GetRasterProps      ()
{
    cTMapProps p;

    p.ulx = west();
    p.uly = north();
    p.band = 0;
    p.bands = 1;
    p.sizex = nrCols();
    p.sizey = nrRows();
    p.is_ldd = AS_IsLDD;
    p.projection = projection();
    p.cellsizex = cellSizeX();
    p.cellsizey = cellSizeY();
    p.can_be_read = AS_FileName.isEmpty()?false:true;
    p.file_path = AS_FileName;

    return p;
}

inline RasterBandStats cTMap::GetRasterBandStats(bool do_stdev = false)
{
    RasterBandStats r;

    float hmax = -1e31f;
    float hmin = 1e31f;
    float havg = 0.0f;
    float n = 0.0;
    float stdev = 0.0;

    FOR_ROW_COL_MV(this)
    {
        if(std::isfinite(this->data[r][c]) && !std::isnan((this->data[r][c])))
        {
            n++;
            havg += (this->data[r][c]);
            hmax = std::max(hmax,this->data[r][c]);
            hmin = std::min(hmin,this->data[r][c]);
        }
    }

    n = std::max(1.0f,n);

    if(do_stdev)
    {
        FOR_ROW_COL_MV(this)
        {
            if(std::isfinite(this->data[r][c]) && !std::isnan((this->data[r][c])))
            {
                stdev += (this->data[r][c] -havg)*(this->data[r][c] -havg);
            }
        }
        stdev = stdev/n;
    }else {
        stdev = 0.35 * (hmax-hmin);
    }

    r.mean = havg/n;
    r.min = hmin;
    r.max = hmax;
    r.stdev = std::sqrt(stdev);

    return r;

}



#endif
