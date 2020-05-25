#pragma once
#include "openglclmanager.h"
#include "qcolor.h"
#include "model.h"
#include "map.h"
#include "openglcltexture.h"
#include "openglprogram.h"
#include "iomanager.h"
#include "sphazardmath.h"
#include "QObject"
#include "openglcldatabuffer.h"
#include "colorf.h"
#include "uilayer.h"
#include "boundingbox.h"



////
/// \brief The UIGeoLayer class
///
/// This subclass of UILayer provides geo-referencing,
/// and deals with having a projection, a CRS based bounding
/// box and other CRS related stuff
///
/// @see UILayer
///
class UIGeoLayer : public UILayer
{
private:


protected:


    GeoCoordTransformer * m_Transformer = nullptr;
    GeoCoordTransformer * m_Transformerinv = nullptr;


    GeoProjection m_Projection = GeoProjection();
    BoundingBox m_BoundingBox = BoundingBox();

    OpenGLGeometry * m_LineGeometry;
    OpenGLGeometry * m_PointInnerGeometry;
    OpenGLGeometry * m_PointOuterGeometry;
    OpenGLGeometry * m_PolygonInnerGeometry;
    OpenGLGeometry * m_PolygonOuterGeometry;

public:

    inline UIGeoLayer()
    {
        m_IsGeo = true;
    }
    inline UIGeoLayer(GeoProjection p,BoundingBox b, QString name, bool file = false, QString filepath = "", bool dynamic = false) : UILayer(name, file,filepath,dynamic)
    {
        m_IsGeo = true;
        Initialize(p,b,name,file,filepath,dynamic);
    }

    inline void Initialize(GeoProjection p,BoundingBox b, QString name, bool file = false, QString filepath = "", bool dynamic = false)
    {
        UILayer::Initialize(name,file,filepath,dynamic);

        m_Projection = p;
        m_BoundingBox = b;

        if(m_Transformer != nullptr)
        {
            delete m_Transformer;
        }
        if(m_Transformerinv != nullptr)
        {
            delete m_Transformerinv;
        }
        m_Transformer = GeoCoordTransformer::GetCoordTransformerGeneric();
        m_Transformerinv = GeoCoordTransformer::GetCoordTransformerGeneric();


    }
    inline ~UIGeoLayer()
    {

    }
    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {

        m_IsPrepared = true;
    }
    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_IsPrepared = false;
    }

    inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
    {

    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        if(m_Transformer != nullptr)
        {
            delete m_Transformer;
        }
        if(m_Transformerinv != nullptr)
        {
            delete m_Transformerinv;
        }
        m_Transformer = GeoCoordTransformer::GetCoordTransformer(s.projection,m_Projection);
        m_Transformerinv = GeoCoordTransformer::GetCoordTransformer(m_Projection,s.projection);


        m_IsCRSChanged = false;
    }


    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline void  SetProjection(GeoProjection p)
    {
        m_Projection = p;
        m_IsCRSChanged = true;
    }
    inline GeoProjection GetProjection()
    {
        return m_Projection;
    }
    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline BoundingBox GetGeoExtentInProjection(GeoProjection * p)
    {

        return BoundingBox();
    }

    inline LayerInfo GetInfo() override
    {
        LayerInfo l;
        l.Add("CRS EPSG",QString::number(m_Projection.GetEPSG()));
        l.Add("CRS Name",m_Projection.GetName());
        l.Add("CRS WKT",m_Projection.GetWKT());
        l.Add(UILayer::GetInfo());
        return l;
    }

};

