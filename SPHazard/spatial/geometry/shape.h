#pragma once
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "ogr_api.h"

#include "boundingbox.h"

#include "sph_vector.h"
#include "sph_vector2.h"
#include "sph_vector3.h"
#include "sph_vector4.h"

#include "functional"

#define SPH_SHAPE_UNKNOWN 0
#define SPH_SHAPE_POINT 1
#define SPH_SHAPE_LINE 2
#define SPH_SHAPE_TRIANGLE 3
#define SPH_SHAPE_POLYGON 4
#define SPH_SHAPE_MULTIPOINT 5
#define SPH_SHAPE_MULTILINE 6
#define SPH_SHAPE_MULTIPOLYGON 7
#define SPH_SHAPE_MULTITRIANGLE 8

class SPHShape
{

private:

    int m_Type;

    //virtual double GetArea();
    //virtual double GetCentroid();
    //virtual int GetType();

public:
    inline SPHShape(int type)
    {
        m_Type = type;
    }

    inline int GetType()
    {
        return m_Type;
    }

    virtual SPHShape * Copy()
    {
        return new SPHShape(SPH_SHAPE_UNKNOWN);
    }

    virtual BoundingBox GetBoundingBox()
    {

        return BoundingBox(0,0,0,0);
    }

    virtual bool Contains(SPHVector2 pos, double tolerence)
    {
        return false;
    }

    virtual SPHVector2 GetClosestEdgeLocationP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr)
    {



        return SPHVector2();
    }

    virtual SPHVector2 GetClosestVertexP(SPHVector2 pos, double **x = nullptr, double **y = nullptr)
    {
        return SPHVector2();
    }

    virtual SPHVector2 GetClosestEdgeLocation(SPHVector2 pos)
    {



        return SPHVector2();
    }

    virtual SPHVector2 GetClosestVertex(SPHVector2 pos)
    {
        return SPHVector2();
    }

    virtual void MoveVertex(double *x, double *y, SPHVector2 d)
    {

    }

    virtual void MoveVertex(SPHVector2 d)
    {

    }

    virtual void Move(SPHVector2 pos)
    {

    }

    virtual SPHShape *Buffer(double buf_dist)
    {

        return nullptr;
    }


    virtual void Destroy()
    {


    }

};

