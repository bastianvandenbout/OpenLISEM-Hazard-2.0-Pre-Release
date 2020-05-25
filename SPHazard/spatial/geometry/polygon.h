#pragma once
#include "QList"
#include "shape.h"
#include "ring.h"
#include "multipolygon.h"
#include "iostream"

class SPHMultiPolygon;

class SPHPolygon: public SPHShape
{

    Ring * m_Ext_Ring = nullptr;
    QList<Ring*> m_Int_Ring;

    BoundingBox m_BoundingBox;

public:
    inline SPHPolygon() : SPHShape(SPH_SHAPE_POLYGON)
    {
    }

    inline SPHPolygon(double x1, double y1, double x2, double y2, double x3, double y3) : SPHShape(SPH_SHAPE_POLYGON)
    {
        m_Ext_Ring = new Ring();
        m_Ext_Ring->AddVertex(x1,y1);
        m_Ext_Ring->AddVertex(x2,y2);
        m_Ext_Ring->AddVertex(x3,y3);

        m_BoundingBox.SetAs(SPHVector2(x1,y1));
        m_BoundingBox.MergeWith(SPHVector2(x2,y2));
        m_BoundingBox.MergeWith(SPHVector2(x3,y3));

    }


    inline void SetExtRing(Ring * r)
    {
        m_Ext_Ring = r;

        if(r->GetVertexCount() > 0)
        {
            m_BoundingBox.SetAs(r->GetVertex(0));
            for(int i = 1; i < r->GetVertexCount(); i++)
            {
                m_BoundingBox.MergeWith(r->GetVertex(i));
            }
        }

    }


    inline void SetAsRegularNGon(SPHVector2 center, double radius, int n_points = 25)
    {

        Ring * r = new Ring();
        for(int i = 0; i < n_points; i++)
        {
            double thetai = float(i) * 2.0 * 3.14159/((float)(n_points));
            double thetaip = float(i+1) * 2.0 * 3.14159/((float)(n_points));

            double x_i = center.x + radius*sin(thetai);
            double y_i = center.y + radius*cos(thetai);

            double x_ip = center.x + radius*sin(thetaip);
            double y_ip = center.y + radius*cos(thetaip);

            r->AddVertex(x_i,y_i);
        }

        this->SetExtRing(r);
    }

    inline void SetAsSquare(SPHVector2 tl, SPHVector2 br)
    {
        Ring * r = new Ring();
        r->AddVertex(tl.x,tl.Y());
        r->AddVertex(tl.x,br.Y());
        r->AddVertex(br.x,br.Y());
        r->AddVertex(br.x,tl.Y());
        r->AddVertex(tl.x,tl.Y());
        this->SetExtRing(r);
    }

    inline void AddIntRing(Ring * r)
    {

        m_Int_Ring.append(r);
    }

    inline int GetIntRingCount()
    {
        return m_Int_Ring.length();

    }

    inline Ring * GetIntRing(int i)
    {
        return m_Int_Ring.at(i);
    }

    inline Ring * GetExtRing()
    {
        return m_Ext_Ring;
    }

    inline SPHShape * Copy() override
    {

        SPHPolygon * p = new SPHPolygon();

        if(m_Ext_Ring != nullptr)
        {
            p->SetExtRing(m_Ext_Ring->Copy());
        }

        for(int i = 0; i < GetIntRingCount(); i++)
        {
            if(GetIntRing(i) != nullptr)
            {
                p->AddIntRing(GetIntRing(i)->Copy());
            }
        }

        return p;
    }

    inline void Destroy() override
    {
        if(m_Ext_Ring != nullptr)
        {
            delete m_Ext_Ring;
        }

        for(int i = 0; i < GetIntRingCount(); i++)
        {
            if(GetIntRing(i) != nullptr)
            {
                delete GetIntRing(i);
            }
        }
        m_Int_Ring.clear();

    }

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline float direction(SPHVector2 pi, SPHVector2 pj, SPHVector2 pk) {
        return (pk.x - pi.x) * (pj.y - pi.y) - (pj.x - pi.x) * (pk.y - pi.y);
    }

    inline bool onSegment(SPHVector2 pi, SPHVector2 pj, SPHVector2 pk) {
        if (std::min(pi.x, pj.x) <= pk.x && pk.x <= std::max(pi.x, pj.x)
            && std::min(pi.y, pj.y) <= pk.y && pk.y <= std::max(pi.y, pj.y)) {
            return true;
        } else {
            return false;
        }
    }

    inline bool segmentIntersect(SPHVector2 p1, SPHVector2 p2, SPHVector2 p3, SPHVector2 p4) {
        float d1 = direction(p3, p4, p1);
        float d2 = direction(p3, p4, p2);
        float d3 = direction(p1, p2, p3);
        float d4 = direction(p1, p2, p4);

        if (((d1 > 0.0 && d2 <  0.0) || (d1 <  0.0 && d2 >  0.0)) && ((d3 >  0.0 && d4 <  0.0) || (d3 <  0.0 && d4 >  0.0))) {
            return true;
        } else if (d1 ==  0.0 && onSegment(p3, p4, p1)) {
            return true;
        } else if (d2 ==  0.0 && onSegment(p3, p4, p2)) {
            return true;
        } else if (d3 ==  0.0 && onSegment(p1, p2, p3)) {
            return true;
        } else if (d4 ==  0.0 && onSegment(p1, p2, p4)) {
            return true;
        } else {
            return false;
        }
    }

    virtual bool Contains(SPHVector2 point, double tolerence)
    {
        BoundingBox bbox = GetBoundingBox();

        //create a ray (segment) starting from the given point,
       //and to the point outside of polygon.
       SPHVector2 outside(bbox.GetMinX() - 1.0, bbox.GetMinY());
       int intersections = 0;
       //check intersections between the ray and every side of the polygon.
       if(m_Ext_Ring != nullptr)
       {
           //check for intersections with internal ring
           for (int i = 0; i < m_Ext_Ring->GetVertexCount() - 1; ++i) {
           if (segmentIntersect(point, outside,m_Ext_Ring->GetVertex(i), m_Ext_Ring->GetVertex(i + 1))) {
                   intersections++;
               }
           }
           //check the last line
           if (segmentIntersect(point, outside,m_Ext_Ring->GetVertex(m_Ext_Ring->GetVertexCount() - 1), m_Ext_Ring->GetVertex(0))) {
               intersections++;
           }

           //check for intersections with internal rings
           for(int j = 0; j < m_Int_Ring.length(); j++)
           {
               Ring * rint = m_Int_Ring.at(j);


               for (int i = 0; i < rint->GetVertexCount() - 1; ++i) {
               if (segmentIntersect(point, outside,rint->GetVertex(i), rint->GetVertex(i + 1))) {
                       intersections++;
                   }
               }
               //check the last line
               if (segmentIntersect(point, outside,rint->GetVertex(rint->GetVertexCount() - 1), rint->GetVertex(0))) {
                   intersections++;
               }

           }
           return (intersections % 2 != 0);
       }

        return false;
    }


    SPHVector2 GetClosestEdgeLocationP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        double mindist = 1e31;

        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(x2 != nullptr)
        {
            *x2 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }
        if(y2 != nullptr)
        {
            *y2 = nullptr;
        }

        if(m_Ext_Ring != nullptr)
        {
            double ** x1t = new (double*);
            double ** y1t = new (double*);
            double ** x2t = new (double*);
            double ** y2t = new (double*);

            double ** x1t2 = new (double*);
            double ** y1t2 = new (double*);
            double ** x2t2 = new (double*);
            double ** y2t2 = new (double*);

            SPHVector2 p = m_Ext_Ring->GetClosestEdgeLocationP(pos,x1t,y1t,x2t,y2t);

            mindist = SPHVector2(pos - p).length();
            closest = p;


            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                SPHVector2 p = rint->GetClosestEdgeLocationP(pos,x1t2,y1t2,x2t2,y2t2);
                float distance = SPHVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;

                    *x1t = *x1t2;*y1t = *y1t2;*x2t = *x2t2;*y2t = *y2t2;
                }
            }

            if(x1 != nullptr)
            {
                *x1 = *x1t;
            }
            if(x2 != nullptr)
            {
                *x2 = *x2t;
            }
            if(y1 != nullptr)
            {
                *y1 = *y1t;
            }
            if(y2 != nullptr)
            {
                *y2 = *y2t;
            }
            delete x1t;delete y1t; delete x2t; delete y2t;
            delete x1t2;delete y1t2; delete x2t2; delete y2t2;

        }
        return closest;
    }

    SPHVector2 GetClosestVertexP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        double mindist = 1e31;

        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }

        if(m_Ext_Ring != nullptr)
        {

            double ** x1t = new (double*);
            double ** y1t = new (double*);

            double ** x1t2 = new (double*);
            double ** y1t2 = new (double*);

            SPHVector2 p = m_Ext_Ring->GetClosestVertexP(pos,x1t,y1t);


            mindist = SPHVector2(pos - p).length();
            closest = p;

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                SPHVector2 p = rint->GetClosestVertexP(pos,x1t2,y1t2);
                float distance = SPHVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;

                    *x1t = *x1t2;*y1t = *y1t2;
                }
            }

            if(x1 != nullptr)
            {
                *x1 = *x1t;
            }
            if(y1 != nullptr)
            {
                *y1 = *y1t;
            }
            delete x1t;delete y1t;
            delete x1t2;delete y1t2;

        }

        return closest;
    }

    SPHVector2 GetClosestEdgeLocation(SPHVector2 pos) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        double mindist = 1e31;
        if(m_Ext_Ring != nullptr)
        {

            SPHVector2 p = m_Ext_Ring->GetClosestEdgeLocation(pos);

            mindist = SPHVector2(pos - p).length();
            closest = p;


            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                SPHVector2 p = rint->GetClosestEdgeLocation(pos);
                float distance = SPHVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;
                }
            }
        }
        return closest;
    }

    SPHVector2 GetClosestVertex(SPHVector2 pos) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        double mindist = 1e31;
        if(m_Ext_Ring != nullptr)
        {
            SPHVector2 p = m_Ext_Ring->GetClosestVertex(pos);

            mindist = SPHVector2(pos - p).length();
            closest = p;

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                SPHVector2 p = rint->GetClosestVertex(pos);
                float distance = SPHVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;
                }
            }
        }

        return closest;
    }

    virtual void MoveVertex(double *x, double *y, SPHVector2 d) override
    {
        if(m_Ext_Ring != nullptr)
        {
            m_Ext_Ring->MoveVertex(x,y,d);
            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                m_Int_Ring.at(j)->MoveVertex(x,y,d);
            }
            Ring * r = m_Ext_Ring;
            if(r->GetVertexCount() > 0)
            {
                m_BoundingBox.SetAs(r->GetVertex(0));
                for(int i = 1; i < r->GetVertexCount(); i++)
                {
                    m_BoundingBox.MergeWith(r->GetVertex(i));
                }
            }
        }
    }

    inline void Move(SPHVector2 d) override
    {

        m_BoundingBox.Move(d);
        if(m_Ext_Ring != nullptr)
        {
            m_Ext_Ring->Move(d);
            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                m_Int_Ring.at(j)->Move(d);
            }

        }

    }

    SPHMultiPolygon * Triangulate();


};
