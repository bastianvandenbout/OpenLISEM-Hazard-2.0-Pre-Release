#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "QList"
#include "shape.h"

class SPHTriangle: public SPHShape
{

private:

    SPHVector2 m_P1;
    SPHVector2 m_P2;
    SPHVector2 m_P3;


    BoundingBox m_BoundingBox;

public:

    inline SPHTriangle() : SPHShape(SPH_SHAPE_TRIANGLE)
    {

    }

    inline SPHTriangle(SPHVector2 p1, SPHVector2 p2, SPHVector2 p3) : SPHShape(SPH_SHAPE_TRIANGLE)
    {
        Set(p1,p2,p3);
    }

    inline SPHTriangle(float x1, float y1, float x2, float y2, float x3, float y3) : SPHShape(SPH_SHAPE_TRIANGLE)
    {
        Set(SPHVector2(x1,y1),SPHVector2(x2,y2),SPHVector2(x3,y3));
    }


    inline void Set(SPHVector2 p1, SPHVector2 p2, SPHVector2 p3)
    {
        m_P1 = p1;
        m_P2 = p2;
        m_P3 = p3;


        m_BoundingBox.SetAs(p1);
        m_BoundingBox.MergeWith(p2);
        m_BoundingBox.MergeWith(p3);
    }

    inline void Set(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        Set(SPHVector2(x1,y1),SPHVector2(x2,y2),SPHVector2(x3,y3));
    }



    inline SPHShape * Copy() override
    {

        SPHTriangle * p = new SPHTriangle();
        p->Set(m_P1,m_P2,m_P3);
        return p;
    }

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    float sign (SPHVector2 p1, SPHVector2 p2, SPHVector2 p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool PointInTriangle (SPHVector2 pt, SPHVector2 v1, SPHVector2 v2, SPHVector2 v3)
    {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(pt, v1, v2);
        d2 = sign(pt, v2, v3);
        d3 = sign(pt, v3, v1);

        has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }

    virtual bool Contains(SPHVector2 pos, double tolerence)
    {
        return PointInTriangle(pos,m_P1,m_P2,m_P3);
    }


};

#endif // TRIANGLE_H
