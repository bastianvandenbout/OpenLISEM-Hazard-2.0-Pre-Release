#ifndef SHAPEOFFSET_H
#define SHAPEOFFSET_H

#include "QList"
#include "shape.h"
#include "shapefile.h"
#include "clipper.hpp"
#include "shapealgorithms.h"


#define SPH_OFFSET_ROUND 0
#define SPH_OFFSET_SQUARE 1
#define SPH_OFFSET_MITER 2



static QList<SPHShape * >ShapeOffset(QList<SPHShape *> shapes,float offset, int offset_type);
static ShapeFile * ShapeFileOffset(ShapeLayer * sf1, float offset, int offset_type);

inline static ShapeFile *AS_VectorOffset(ShapeFile * f,float offset)
{
    return ShapeFileOffset(f->GetLayer(0),offset,SPH_OFFSET_ROUND);
}

//returns a list of shapes that are given by the operator applied to shape1 and shape2. Thus result = s1 *s2 (with * some operator)
inline static ShapeFile * ShapeFileOffset(ShapeLayer * sf1, float offset, int offset_type)
{
    std::cout << "doing shape offset " << std::endl;

    if(sf1 == nullptr)
    {
        return new ShapeFile();
    }
    ShapeFile * ret_sf = new ShapeFile();
    ShapeLayer * ret_layer = new ShapeLayer();
    ret_sf->AddLayer(ret_layer);
    sf1 = sf1->Copy();

    //Brute force n2, add all intersections betwee sf1 and sf2
    for(int i = 0; i < sf1->GetFeatureCount(); i++)
    {
        //prepare input
        Feature * f1 = sf1->GetFeature(i);
        QList<SPHShape *> in_A;
        for(int j = 0; j < f1->GetShapeCount(); j++)
        {
            in_A.append(f1->GetShape(j));
        }

        QList<SPHShape *> ret_A = ShapeOffset(in_A,offset,offset_type);
        for(int j = 0; j < ret_A.length(); j++)
        {
            Feature * fnew = new Feature();
            fnew->AddShape(ret_A.at(j));
            ret_layer->AddFeature(fnew);
        }
    }
    sf1->Destroy();
    delete sf1;

    return ret_sf;
}

inline static QList<SPHShape * >ShapeOffset(QList<SPHShape *> shapes,float offset, int offset_type)
{

    //run clipperlib offset code

    BoundingBox extent;
    for(int i = 0; i < shapes.length(); i++)
    {
        SPHShape * s1 = shapes.at(i);
        if(i == 0)
        {
            extent = s1->GetBoundingBox();
        }else {
            extent.MergeWith(s1->GetBoundingBox());
        }
    }

    extent.Scale(3);

    if(extent.GetSizeX() <offset * 4)
    {
        extent.Set(extent.GetMinX() - offset * 1.5,extent.GetMaxX() + offset * 1.5,extent.GetMinY(),extent.GetMaxY());
    }
    if(extent.GetSizeY() <offset * 4)
    {
        extent.Set(extent.GetMinX(),extent.GetMaxX(),extent.GetMinY() - offset * 1.5,extent.GetMaxY() + offset * 1.5);
    }
    if(extent.GetSizeX() > extent.GetSizeY())
    {
        extent.Set(extent.GetMinX(),extent.GetMaxX(),extent.GetCenterY() - extent.GetSizeX() *0.5,extent.GetCenterY() +  extent.GetSizeX() *0.5);
    }


    ClipperLib::ClipperOffset C;

    //fill the paths

    int n = 0;

    //find out how many paths we need to allocate
    for(int i = 0; i < shapes.length(); i++)
    {
        SPHShape * s1 = shapes.at(i);
        int type1 = s1->GetType();

        if(type1 == SPH_SHAPE_UNKNOWN)
        {
            continue;
        }else if(type1 == SPH_SHAPE_MULTIPOINT)
        {
            if(offset > 0.0f)
            {
                n++;
                SPHMultiPoint *msp = (SPHMultiPoint*)s1;
                for(int j = 0; j < msp->GetPointCount(); j++)
                {
                    SPHPoint * point = msp->GetPoint(j);
                    ClipperLib::Paths Path_A(1);
                    PointToClipperPath(point,&Path_A,extent,0);
                    C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
                }
            }

        }else if(type1 == SPH_SHAPE_MULTILINE)
        {
            if(offset > 0.0f)
            {
                n++;
                SPHMultiLine *msp = (SPHMultiLine*)s1;
                for(int j = 0; j < msp->GetLineCount(); j++)
                {
                    SPHLine * line = msp->GetLine(j);
                    if(line->GetVertexCount() > 1)
                    {
                        ClipperLib::Paths Path_A(1);
                        LineToClipperPath(line,&Path_A,extent,0);
                        C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
                    }
                }
            }

        }else if(type1 == SPH_SHAPE_MULTIPOLYGON)
        {
            SPHMultiPolygon *msp = (SPHMultiPolygon*)s1;
            for(int j = 0; j < msp->GetPolygonCount(); j++)
            {
                SPHPolygon * polygon = msp->GetPolygon(j);
                if(polygon->GetExtRing() != nullptr)
                {
                    n++;
                    ClipperLib::Paths Path_A(polygon->GetIntRingCount() + 1);
                    PolygonToClipperPath(polygon,&Path_A,extent,0);
                    C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etClosedPolygon);
                }
            }
        }else if(type1 == SPH_SHAPE_POINT)
        {
            if(offset > 0.0f)
            {
                n++;
                SPHPoint * point = (SPHPoint *) s1;
                ClipperLib::Paths Path_A(1);
                PointToClipperPath(point,&Path_A,extent,0);
                C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
            }

        }else if(type1 == SPH_SHAPE_POLYGON)
        {
            SPHPolygon * polygon = (SPHPolygon *) s1;
            if(polygon->GetExtRing() != nullptr)
            {
                n++;
                ClipperLib::Paths Path_A(polygon->GetIntRingCount() + 1);
                PolygonToClipperPath(polygon,&Path_A,extent,0);
                C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etClosedPolygon);
            }
        }else if(type1 == SPH_SHAPE_LINE)
        {
            if(offset > 0.0f)
            {
                n++;
                SPHLine * line = (SPHLine *) s1;
                if(line->GetVertexCount() > 1)
                {
                    ClipperLib::Paths Path_A(1);
                    LineToClipperPath(line,&Path_A,extent,0);
                    C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
                }
            }
        }
    }

    //recalculate offset value

    double offsetn = 2.0*9e18*(offset/extent.GetSizeX());

    //ClipperLib::Paths Polygon1(n_path_1);
    ClipperLib::PolyTree Solution;
    C.ArcTolerance = offsetn * 0.01;
    QList<SPHShape*> ret_pol;


    if(n > 0)
    {
        std::cout << offsetn << " " << C.ArcTolerance << " " << extent.GetMinX() << " " << extent.GetMaxX() << " " << extent.GetMinY() << " " << extent.GetMaxY() << std::endl;
        std::cout << "execute offset " << std::endl;
        C.Execute(Solution,offsetn);
        std::cout << "end execute offset " << std::endl;

        for(int i = 0; i < Solution.Childs.size(); i++)
        {
            PolygonTreeToPolygonList(&ret_pol,Solution.Childs.at(i),extent,nullptr);
        }

    }


    return ret_pol;

}






#endif // SHAPEOFFSET_H
