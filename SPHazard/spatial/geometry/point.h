#ifndef POINT_H
#define POINT_H
#include "shape.h"

class SPHPoint: public SPHShape
{
    double m_x = 0.0;
    double m_y = 0.0;
    double m_z = 0.0;
    double m_radius = 0.0;




public:
    inline SPHPoint() : SPHShape(SPH_SHAPE_POINT)
    {
    }

    inline SPHPoint(double x, double y) : SPHShape(SPH_SHAPE_POINT)
    {
        m_x = x;
        m_y = y;
    }


    inline void SetPos(double x, double y, double z = 0.0f)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    inline double GetX()
    {
        return m_x;
    }

    inline double GetY()
    {
        return m_y;
    }

    inline double GetZ()
    {
        return m_z;
    }


    inline SPHShape * Copy() override
    {
        SPHPoint * p = new SPHPoint();
        p->SetPos(GetX(),GetY(),GetZ());

        return p;
    }

    bool Contains(SPHVector2 pos, double tolerence)
    {
        float xdifsq = (pos.x-m_x);
        float ydifsq = (pos.y-m_y);
        if(std::sqrt((xdifsq * xdifsq + ydifsq * ydifsq)) <= tolerence)
        {
            return true;
        }
        return false;
    }



    inline SPHVector2 GetClosestEdgeLocation(SPHVector2 pos) override
    {
        return SPHVector2(m_x,m_y);
    }

    inline SPHVector2 GetClosestVertex(SPHVector2 pos) override
    {
        return SPHVector2(m_x,m_y);
    }


    inline SPHVector2 GetClosestEdgeLocationP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
    {
        if(x1 != nullptr)
        {
            *x1 = &m_x;
        }
        if(x2 != nullptr)
        {
            *x2 = &m_x;
        }
        if(y1 != nullptr)
        {
            *y1 = &m_y;
        }
        if(y2 != nullptr)
        {
            *y2 = &m_y;
        }

        return SPHVector2(m_x,m_y);
    }

    inline SPHVector2 GetClosestVertexP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
    {
        if(x1 != nullptr)
        {
            *x1 = &m_x;
        }

        if(y1 != nullptr)
        {
            *y1 = &m_y;
        }
        return SPHVector2(m_x,m_y);
    }


    virtual void MoveVertex(double *x, double *y, SPHVector2 d) override
    {
        m_x += d.X();
        m_y += d.Y();
    }

    inline void Move(SPHVector2 d) override
    {
        m_x += d.X();
        m_y += d.Y();
    }

    inline BoundingBox GetBoundingBox()
    {
        BoundingBox b;
        b.Set(m_x - m_radius,m_x + m_radius,m_y - m_radius,m_y + m_radius);

        return b;
    }


};

#endif // POINT_H
