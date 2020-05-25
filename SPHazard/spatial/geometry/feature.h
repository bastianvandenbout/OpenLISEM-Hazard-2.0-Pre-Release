#pragma once


#include "shape.h"
#include "polygon.h"
#include "point.h"
#include "line.h"
#include "multiline.h"
#include "multipoint.h"
#include "multipolygon.h"
#include <iostream>
#include "spherror.h"
#include <QList>
#include "glgeometrylist.h"

#include "functional"

class Feature
{
private:

    QList<SPHShape*> m_ShapeList;
    int m_index = -1;

public:



    inline int GetIndex()
    {
        return m_index;
    }

    inline void RemoveShape(int i)
    {

        m_ShapeList.removeAt(i);
        GetBoundingBox();
    }

    inline void RemoveShape(SPHShape*s)
    {
        m_ShapeList.removeOne(s);
        GetBoundingBox();
    }

    inline void ReplaceShape(SPHShape*s, SPHShape * s2)
    {
        int i = m_ShapeList.indexOf(s);
        if( i > -1)
        {
            m_ShapeList.replace(i,s2);
            GetBoundingBox();
        }
    }



    inline int GetShapeCount()
    {
        return m_ShapeList.length();
    }

    inline void AddShape(SPHShape*s)
    {
        m_ShapeList.append(s);
    }
    inline SPHShape * GetShape(int i)
    {

        return m_ShapeList.at(i);
    }

    inline Feature * Copy()
    {
        Feature * f = new Feature();

        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            f->AddShape(m_ShapeList.at(i)->Copy());
        }
        return f;
    }

    inline void Destroy()
    {
        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            m_ShapeList.at(i)->Destroy();
            delete m_ShapeList.at(i);
        }
        m_ShapeList.clear();

    }

    inline BoundingBox GetBoundingBox()
    {
        BoundingBox b;
        if(m_ShapeList.length() > 0)
        {
            b = m_ShapeList.at(0)->GetBoundingBox();
            for(int i = 1; i < m_ShapeList.length(); i++)
            {
                b.MergeWith(m_ShapeList.at(i)->GetBoundingBox());


            }
        }

        return b;
    }

    inline bool Contains(SPHVector2 pos, double tolerence)
    {

        //check for all shapes
        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            SPHShape * s = m_ShapeList.at(i);
            if(s->GetBoundingBox().Contains(pos.x,pos.y,tolerence))
            {
                if(s->Contains(pos, tolerence))
                {
                    return true;
                }
            }
        }


        return false;

    }


    SPHVector2 GetClosestEdgeLocation(SPHVector2 pos, SPHShape ** s_ret = nullptr)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }

        for(int j = 0; j < m_ShapeList.length(); j++)
        {
            SPHVector2 p = m_ShapeList.at(j)->GetClosestEdgeLocation(pos);
            float distance = SPHVector2(pos - p).length();
            if(!std::isnan(p.x) && !std::isnan(p.y) && std::isfinite(p.x) && std::isfinite(p.y))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr)
                    {
                        *s_ret = m_ShapeList.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr)
                    {
                        *s_ret = m_ShapeList.at(j);
                    }
                }
            }

        }

        return closest;
    }

    SPHVector2 GetClosestVertex(SPHVector2 pos, SPHShape ** s_ret = nullptr)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }
        for(int j = 0; j < m_ShapeList.length(); j++)
        {
            SPHVector2 p = m_ShapeList.at(j)->GetClosestVertex(pos);
            float distance = SPHVector2(pos - p).length();
            if(j == 0)
            {
                mindist = distance;
                closest = p;
                if(s_ret != nullptr)
                {
                    *s_ret = m_ShapeList.at(j);
                }
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = p;
                if(s_ret != nullptr)
                {
                    *s_ret = m_ShapeList.at(j);
                }
            }
        }
        return closest;
    }

    inline void AddToGlGeometryList(GLGeometryList * gl_list,int index)
    {
        for(int k = 0; k < m_ShapeList.length(); k++)
        {
            SPHShape * s = m_ShapeList.at(k);

            AddShapeToGlGeometryList(gl_list,index,s);
        }
    }

    static inline void AddShapeToGlGeometryList(GLGeometryList * gl_list,int index,SPHShape * s)
    {
        if(s == nullptr)
        {
            return;
        }

        int type = s->GetType();
        if(type == SPH_SHAPE_UNKNOWN)
        {

        }else if(type == SPH_SHAPE_POINT)
        {
            SPHPoint * sp = (SPHPoint*) s;

            float x = sp->GetX();
            float y = sp->GetY();

            gl_list->AddPoint_Point(x,y);
            gl_list->AddVertexPoint(index,x,y);

            for(int i = 0; i < 10 ; i++)
            {
                float thetai = float(i) * 2.0f * 3.14159f/10.0f;
                float thetaip = float(i+1) * 2.0f * 3.14159f/10.0f;

                float x_i = x + sin(thetai);
                float y_i = y + cos(thetai);

                float x_ip = x + sin(thetaip);
                float y_ip = y + cos(thetaip);

                bool connect = (i > 0);

                gl_list->AddPoint_Line(index,x_i,y_i,x_ip,y_ip,x,y, connect);
                gl_list->AddPoint_Triangle(index,x_i,y_i,x_ip,y_ip,x,y,x,y);
            }

        }else if(type == SPH_SHAPE_LINE)
        {
            SPHLine * sp = (SPHLine*) s;

            int nvertex = sp->GetVertexCount();
            double * vertices = sp->GetVertexData();

            for(int i = 0; i < nvertex; i++)
            {
                gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
            }

            for(int i = 0; i < nvertex - 1; i++)
            {
                bool connect = (i > 0);

                gl_list->AddLine_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
            }

        }else if(type == SPH_SHAPE_POLYGON)
        {
            SPHPolygon * sp = (SPHPolygon*) s;

            SPHMultiPolygon * mp = sp->Triangulate();

            Ring * ext_ring = sp->GetExtRing();
            if(ext_ring != nullptr){
                double * vertices = ext_ring->GetVertexData();
                int nvertex = ext_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                }

                for(int i = 0; i < nvertex; i++)
                {
                    bool connect = (i > 0);
                    if(i == nvertex-1)
                    {
                        gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1],connect);
                    }else {
                        gl_list->AddPolygon_Line(index,vertices[2*i],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                    }
                }
            }

            int int_ring_count = sp->GetIntRingCount();
            for(int j = 0; j < int_ring_count; j++)
            {
                Ring * int_ring = sp->GetIntRing(j);
                double * vertices = int_ring->GetVertexData();
                int nvertex = int_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                }

                for(int i = 0; i < nvertex; i++)
                {
                    bool connect = i > 0;

                    if(i == nvertex-1)
                    {
                        gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1],connect);
                    }else {
                        gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                    }
                }
            }


            if(mp != nullptr)
            {
                for(int j = 0; j < mp->GetPolygonCount(); j++)
                {
                    SPHPolygon * pj = mp->GetPolygon(j);



                    if(pj != nullptr)
                    {
                        Ring * ring = pj->GetExtRing();
                        if(ring != nullptr)
                        {
                            if(ring->GetVertexCount() == 3)
                            {
                                double * data = ring->GetVertexData();
                                gl_list->AddPolygon_Triangle(index,data[0],data[1],data[2],data[3],data[4],data[5]);
                            }
                        }
                    }
                }

                mp->Destroy();
                delete mp;

            }


        }else if(type == SPH_SHAPE_MULTIPOINT)
        {

            SPHMultiPoint *msp = (SPHMultiPoint*)s;

            for(int j = 0; j < msp->GetPointCount(); j++)
            {
                SPHPoint *sp = msp->GetPoint(j);

                float x = sp->GetX();
                float y = sp->GetY();

                gl_list->AddPoint_Point(x,y);
                gl_list->AddVertexPoint(index,x,y);

                for(int i = 0; i < 10 ; i++)
                {
                    float thetai = float(i) * 2.0f * 3.14159f/10.0f;
                    float thetaip = float(i+1) * 2.0f * 3.14159f/10.0f;

                    float x_i = x + sin(thetai);
                    float y_i = y + cos(thetai);

                    float x_ip = x + sin(thetaip);
                    float y_ip = y + cos(thetaip);

                    bool connect = (i > 0);


                    gl_list->AddPoint_Line(index,x_i,y_i,x_ip,y_ip,x,y, connect);
                    gl_list->AddPoint_Triangle(index,x_i,y_i,x_ip,y_ip,x,y,x,y);
                }

            }

        }else if(type == SPH_SHAPE_MULTILINE)
        {
            SPHMultiLine *msp = (SPHMultiLine*)s;

            for(int i = 0; i < msp->GetLineCount(); i++)
            {
                SPHLine *sp = msp->GetLine(i);

                int nvertex = sp->GetVertexCount();
                double * vertices = sp->GetVertexData();

                for(int i = 0; i < nvertex; i++)
                {
                    gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                }

                for(int i = 0; i < nvertex; i++)
                {
                    bool connect = i > 0;
                    gl_list->AddLine_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                }

            }

        }else if(type == SPH_SHAPE_MULTIPOLYGON)
        {
            SPHMultiPolygon *msp = (SPHMultiPolygon*)s;

            for(int i = 0; i < msp->GetPolygonCount(); i++)
            {
                SPHPolygon *sp = msp->GetPolygon(i);

                SPHMultiPolygon * mp = sp->Triangulate();

                Ring * ext_ring = sp->GetExtRing();
                if(ext_ring != nullptr){
                    double * vertices = ext_ring->GetVertexData();
                    int nvertex = ext_ring->GetVertexCount();

                    for(int i = 0; i < nvertex; i++)
                    {
                        gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                    }

                    for(int i = 0; i < nvertex; i++)
                    {
                        bool connect = (i > 0);
                        if(i == nvertex-1)
                        {
                            gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1],connect);
                        }else {
                            gl_list->AddPolygon_Line(index,vertices[2*i],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                        }
                    }
                }

                int int_ring_count = sp->GetIntRingCount();
                for(int j = 0; j < int_ring_count; j++)
                {
                    Ring * int_ring = sp->GetIntRing(j);
                    double * vertices = int_ring->GetVertexData();
                    int nvertex = int_ring->GetVertexCount();

                    for(int i = 0; i < nvertex; i++)
                    {
                        gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                    }

                    for(int i = 0; i < nvertex; i++)
                    {
                        bool connect = (i > 0);
                        if(i == nvertex-1)
                        {
                            gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1], connect);
                        }else {
                            gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                        }
                    }
                }


                if(mp != nullptr)
                {
                    for(int j = 0; j < mp->GetPolygonCount(); j++)
                    {
                        SPHPolygon * pj = mp->GetPolygon(j);



                        if(pj != nullptr)
                        {
                            Ring * ring = pj->GetExtRing();
                            if(ring != nullptr)
                            {
                                if(ring->GetVertexCount() == 3)
                                {
                                    double * data = ring->GetVertexData();
                                    gl_list->AddPolygon_Triangle(index,data[0],data[1],data[2],data[3],data[4],data[5]);
                                }
                            }
                        }
                    }

                    mp->Destroy();
                    delete mp;
                }


            }
        }
    }

};


inline SPHShape * FromGDALShape(OGRGeometry * g)
{
    //the gdal formats we recognize for now

    //wkbPoint = 1,           /**< 0-dimensional geometric object, standard WKB */
    //wkbLineString = 2,      /**< 1-dimensional geometric object with linear
    //                         *   interpolation between Points, standard WKB */
    //wkbPolygon = 3,         /**< planar 2-dimensional geometric object defined
    //                         *   by 1 exterior boundary and 0 or more interior
    //                         *   boundaries, standard WKB */
    //wkbMultiPoint = 4,      /**< GeometryCollection of Points, standard WKB */
    //wkbMultiLineString = 5, /**< GeometryCollection of LineStrings, standard WKB */
    //wkbMultiPolygon = 6,    /**< GeometryCollection of Polygons, standard WKB */

    SPHShape * s;

    OGRwkbGeometryType type = g->getGeometryType();
    OGRwkbGeometryType isotype = g->getIsoGeometryType();
    OGRPoint ptTemp;

    if ( wkbFlatten (type) == wkbPoint )
    {
        SPHPoint * p = new SPHPoint();
        OGRPoint *poPoint = g->toPoint();
        p->SetPos(poPoint->getX(),poPoint->getY(),poPoint->getZ());

        s = p;
    }else if ( wkbFlatten (type) == wkbLineString )
    {
        SPHLine * p = new SPHLine();

        OGRLineString * ls = g->toLineString();
        int NumberOfVertices = ls->getNumPoints();
        for ( int k = 0; k < NumberOfVertices; k++ )
        {
           ls->getPoint(k,&ptTemp);
            p->AddVertex(ptTemp.getX(),ptTemp.getY());
        }
        s = p;
    }else if(wkbFlatten(type) == wkbPolygon )
    {
        SPHPolygon * p = new SPHPolygon();

        OGRPolygon *po = g->toPolygon();

        OGRLinearRing *ext_ring = po->getExteriorRing();
        if(ext_ring != NULL)
        {
            int exterior_ring_point_count = ext_ring->getNumPoints();
            int interior_ring_count = po->getNumInteriorRings();

            Ring * r = new Ring();
            for(int k = 0; k <  exterior_ring_point_count; k++)
            {
                ext_ring->getPoint(k,&ptTemp);
                r->AddVertex(ptTemp.getX(),ptTemp.getY());
            }
            p->SetExtRing(r);

        }
        int numintrings = po->getNumInteriorRings();
        for(int i =0; i < numintrings; i++)
        {
            OGRLinearRing *int_ring = po->getInteriorRing(i);
            if(int_ring != NULL)
            {
                int interior_ring_point_count = int_ring->getNumPoints();
                Ring * r = new Ring();
                for(int k = 0; k <  interior_ring_point_count; k++)
                {
                    int_ring->getPoint(k,&ptTemp);
                    r->AddVertex(ptTemp.getX(),ptTemp.getY());
                }
                p->AddIntRing(r);

            }
        }

        s = p;
    }else if ( wkbFlatten (type) == wkbMultiPoint )
    {
        SPHMultiPoint * p = new SPHMultiPoint();

        OGRMultiPoint *poMultipoint = g->toMultiPoint();
        int NumberOfGeometries = poMultipoint->getNumGeometries();

        for ( int j = 0; j < NumberOfGeometries; j++ )
        {
           OGRPoint *poPoint = poMultipoint ->getGeometryRef(j)->toPoint();
           if(poPoint != NULL)
           {
               SPHPoint * point = new SPHPoint();
               point->SetPos(poPoint->getX(),poPoint->getY(),poPoint->getZ());
               p->Add(point);
           }
        }

        s = p;
    }else if ( wkbFlatten (type) == wkbMultiLineString )
    {
        SPHMultiLine * p = new SPHMultiLine();
        OGRMultiLineString *mls = g->toMultiLineString();
        int NumberOfGeometries = mls->getNumGeometries();
        for ( int j = 0; j < NumberOfGeometries; j++ )
        {
            SPHLine * pl = new SPHLine();
            OGRLineString * ls = mls->getGeometryRef(j)->toLineString();
            if(ls != NULL)
            {
                int NumberOfVertices = ls->getNumPoints();
                for ( int k = 0; k < NumberOfVertices; k++ )
                {
                   ls->getPoint(k,&ptTemp);
                    pl->AddVertex(ptTemp.getX(),ptTemp.getY());
                }
                p->Add(pl);
            }
        }
        s = p;

    }else if(wkbFlatten(type) == wkbMultiPolygon )
    {
        SPHMultiPolygon * mp = new SPHMultiPolygon();

        OGRMultiPolygon *mpo = g->toMultiPolygon();
        int po_count = mpo->getNumGeometries();

        for(int l = 0; l < po_count;l++)
        {
            SPHPolygon * p = new SPHPolygon();

            OGRPolygon *po = mpo->getGeometryRef(l)->toPolygon();

            OGRLinearRing *ext_ring = po->getExteriorRing();
            if(ext_ring != NULL)
            {
                int exterior_ring_point_count = ext_ring->getNumPoints();

                Ring * r = new Ring();
                for(int k = 0; k <  exterior_ring_point_count; k++)
                {
                    ext_ring->getPoint(k,&ptTemp);
                    r->AddVertex(ptTemp.getX(),ptTemp.getY());
                }
                p->SetExtRing(r);

            }
            int numintrings = po->getNumInteriorRings();
            for(int i =0; i < numintrings; i++)
            {
                OGRLinearRing *int_ring = po->getInteriorRing(i);
                if(int_ring != NULL)
                {
                    int interior_ring_point_count = int_ring->getNumPoints();
                    Ring * r = new Ring();
                    for(int k = 0; k <  interior_ring_point_count; k++)
                    {
                        int_ring->getPoint(k,&ptTemp);
                        r->AddVertex(ptTemp.getX(),ptTemp.getY());
                    }
                    p->AddIntRing(r);
                }
            }
            mp->Add(p);
        }
        s = mp;
    }

    return s;
}

inline OGRGeometry * ToGDALShape(SPHShape *s)
{

    //#define SPH_SHAPE_UNKNOWN 0
    //#define SPH_SHAPE_POINT 1
    //#define SPH_SHAPE_LINE 2
    //#define SPH_SHAPE_POLYGON 3
    //#define SPH_SHAPE_MULTIPOINT 4
    //#define SPH_SHAPE_MULTILINE 5
    //#define SPH_SHAPE_MULTIPOLYGON 6

    int type = s->GetType();
    if(type == SPH_SHAPE_UNKNOWN)
    {

    }else if(type == SPH_SHAPE_POINT)
    {
        SPHPoint * sp = (SPHPoint*) s;

        OGRPoint *pt = new OGRPoint();

        pt->setX( sp->GetX());
        pt->setY( sp->GetY() );
        return pt;

    }else if(type == SPH_SHAPE_LINE)
    {
        SPHLine * sp = (SPHLine*) s;

        OGRLineString *pt = new OGRLineString();

        int nvertex = sp->GetVertexCount();
        double * vertices = sp->GetVertexData();

        for(int i = 0; i < nvertex; i++)
        {
            pt->addPoint(vertices[2*i+0],vertices[2*i+1]);
        }
        return pt;

    }else if(type == SPH_SHAPE_POLYGON)
    {
        SPHPolygon * sp = (SPHPolygon*) s;
        OGRPolygon *pt = new OGRPolygon();

        OGRLinearRing * ogr_ext_ring = new OGRLinearRing();
        Ring * ext_ring = sp->GetExtRing();
        if(ext_ring != nullptr){
            double * vertices = ext_ring->GetVertexData();
            int nvertex = ext_ring->GetVertexCount();

            for(int i = 0; i < nvertex; i++)
            {
                ogr_ext_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
            }
        }
        pt->addRing(ogr_ext_ring);

        int int_ring_count = sp->GetIntRingCount();
        for(int j = 0; j < int_ring_count; j++)
        {
            OGRLinearRing * ogr_int_ring = new OGRLinearRing();
            Ring * int_ring = sp->GetIntRing(j);
            double * vertices = int_ring->GetVertexData();
            int nvertex = int_ring->GetVertexCount();

            for(int i = 0; i < nvertex; i++)
            {
                ogr_int_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
            }
            pt->addRing(ogr_int_ring);
        }

        return pt;

    }else if(type == SPH_SHAPE_MULTIPOINT)
    {

        OGRMultiPoint *mp = new OGRMultiPoint();
        SPHMultiPoint *msp = (SPHMultiPoint*)s;

        for(int i = 0; i < msp->GetPointCount(); i++)
        {
            SPHPoint *sp = msp->GetPoint(i);
            OGRPoint *pt = new OGRPoint();

            pt->setX( sp->GetX());
            pt->setY( sp->GetY());
            mp->addGeometry(pt);
        }
        return mp;

    }else if(type == SPH_SHAPE_MULTILINE)
    {

        OGRMultiLineString *mp = new OGRMultiLineString();
        SPHMultiLine *msp = (SPHMultiLine*)s;

        for(int i = 0; i < msp->GetLineCount(); i++)
        {
            SPHLine *sp = msp->GetLine(i);
            OGRLineString *pt = new OGRLineString();

            int nvertex = sp->GetVertexCount();
            double * vertices = sp->GetVertexData();

            for(int i = 0; i < nvertex; i++)
            {
                pt->addPoint(vertices[2*i+0],vertices[2*i+1]);
            }
            mp->addGeometry(pt);

        }
        return mp;

    }else if(type == SPH_SHAPE_MULTIPOLYGON)
    {
        OGRMultiPolygon *mp = new OGRMultiPolygon();
        SPHMultiPolygon *msp = (SPHMultiPolygon*)s;

        for(int i = 0; i < msp->GetPolygonCount(); i++)
        {
            SPHPolygon *sp = msp->GetPolygon(i);
            OGRPolygon *pt = new OGRPolygon();

            OGRLinearRing * ogr_ext_ring = new OGRLinearRing();
            Ring * ext_ring = sp->GetExtRing();
            if(ext_ring != nullptr){
                double * vertices = ext_ring->GetVertexData();
                int nvertex = ext_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    ogr_ext_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
                }
            }
            pt->addRing(ogr_ext_ring);

            int int_ring_count = sp->GetIntRingCount();
            for(int j = 0; j < int_ring_count; j++)
            {
                OGRLinearRing * ogr_int_ring = new OGRLinearRing();
                Ring * int_ring = sp->GetIntRing(j);
                double * vertices = int_ring->GetVertexData();
                int nvertex = int_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    ogr_int_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
                }
                pt->addRing(ogr_int_ring);
            }

            mp->addGeometry(pt);

        }
        return mp;
    }

    return nullptr;

}


inline Feature * FromGDALFeature(OGRFeature *d)
{
    Feature * f = new Feature();

    int gcount = d->GetGeomFieldCount();
    for(int k = 0; k < gcount; k++)
    {

        OGRGeometry *poGeometry = d->GetGeomFieldRef(k);
        if( poGeometry != NULL )
        {
            if(poGeometry->IsValid())
            {
                SPHShape* s = FromGDALShape(poGeometry);

                f->AddShape(s);
            }
        }
    }
    return f;
}

inline OGRFeature * ToGDALFeature(OGRFeature * to,Feature * f)
{
    for(int i = 0; i < f->GetShapeCount();i++)
    {

        std::cout << "add shape to gdal feature " << f->GetShape(i)->GetType() << std::endl;

        OGRGeometry * g = ToGDALShape(f->GetShape(i));
        if(g == nullptr)
        {
            SPH_ERROR("Could not create gdal geometry from shape");
            continue;
        }

        to->SetGeometryDirectly(g);

        //so far we only do one shape per feature
        //this is sufficient for most types of shapefiles
        break;
    }
    return to;
}



