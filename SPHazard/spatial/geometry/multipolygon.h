#pragma once

#include "shape.h"
#include "polygon.h"
#include "QList"

class SPHPolygon;

class SPHMultiPolygon: public SPHShape
{

    QList<SPHPolygon*> m_List;


    BoundingBox m_BoundingBox;

public:
    inline SPHMultiPolygon() : SPHShape(SPH_SHAPE_MULTIPOLYGON)
    {
    }

    void Add(SPHPolygon * p);

    inline int GetPolygonCount()
    {
        return m_List.length();
    }

    inline SPHPolygon * GetPolygon(int i)
    {
        return m_List.at(i);
    }

    SPHShape * Copy() override;

    SPHVector2 GetClosestEdgeLocationP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override;

    SPHVector2 GetClosestVertexP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override;


    SPHVector2 GetClosestEdgeLocation(SPHVector2 pos) override;

    SPHVector2 GetClosestVertex(SPHVector2 pos) override;

    void Destroy() override;

    bool Contains(SPHVector2 pos, double tolerence);


    virtual void MoveVertex(double *x, double *y, SPHVector2 d) override;

    inline void Move(SPHVector2 d) override;

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }
};

