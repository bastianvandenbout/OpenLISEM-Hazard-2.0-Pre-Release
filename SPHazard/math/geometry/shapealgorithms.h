#pragma once

#include "QList"
#include "shape.h"
#include "clipper.hpp"
#include "point.h"
#include "polygon.h"
#include "line.h"
#include "multipolygon.h"
#include "multiline.h"
#include "multipoint.h"
#include "shapefile.h"
#include "iostream"

#define SPH_OFFSET_ROUND 0
#define SPH_OFFSET_SQUARE 1
#define SPH_OFFSET_MITER 2

#define SPH_SHAPE_NONE 0
#define SPH_SHAPE_INTERSECT 1   //A && B
#define SPH_SHAPE_UNION 2       //A || B
#define SPH_SHAPE_DIFFERENCE 3  //A && !B
#define SPH_SHAPE_XOR 4         //A || B  && !(A&&B)
#define SPH_SHAPE_ADD 5         //A + B






//returns a list of shapes that are given by the operator applied to shape1 and shape2. Thus result = s1 *s2 (with * some operator)
//this function can do a large variety of gis vector operations for two shapes.
//for example, combining two lines with intersect finds the interscect location for points
//or, using the union operator on two polygons creates a single unified polygon
//note: there are 9 unique combinations for the shapes we support
//then for each combination, 5 operators.
//we only support 6 of them, with partial support for operators.
//in the function defined below, numerous combinations are not defined and simply return an empty list
//Taking a point out of a polygon doesnt seem to usefull (a polygon with a infinitely small hole might be better represented as a polygon and a seperate point anyway)
//similarly, point and point comparisons show no direct use, except to check wether points exists on identical locations.

static void ShapeAlgebra(SPHShape * s1, SPHShape * s2, QList<SPHShape *> * ret_A,  QList<SPHShape *> * ret_B, QList<SPHShape *> * ret_C);
static void ShapeAlgebra(QList<SPHShape *> s1, QList<SPHShape *> s2, QList<SPHShape *> * ret_A,  QList<SPHShape *> * ret_B, QList<SPHShape *> * ret_C);
static void PolygonPolygonDisect(QList<SPHPolygon *> p1, QList<SPHPolygon *> p2,  QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C);
static void LinePolygonDisect(QList<SPHLine *> p1, QList<SPHPolygon *> p2, QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C);
static void PointPolygonDisect(QList<SPHPoint *> p1, QList<SPHPolygon *> p2, QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C);
static void LineLineDisect(QList<SPHLine *> p1, QList<SPHLine *> p2, QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C);

inline static ShapeFile * ShapeLayerAlgebra(ShapeLayer * sf1, ShapeLayer * sf2,int Operator)
{

    std::cout << "doing shape algebra " << std::endl;

    if(sf1 == nullptr || sf2 == nullptr)
    {
        return new ShapeFile();
    }

    ShapeFile * ret_sf = new ShapeFile();
    ShapeLayer * ret_layer = new ShapeLayer();
    ret_sf->AddLayer(ret_layer);

    //make a copy and flatten in (this means we put all the shapes in induvidual features
    //this way, later on, we can make things easier when calling the functions to provide the new shapes
    sf1 = sf1->Copy();
    sf2 = sf2->Copy();

    sf1->Flatten();
    sf2->Flatten();


    std::cout << "shape counts " << sf1->GetShapeCount() << "  " << sf2->GetShapeCount() << std::endl;

    //while we distribute the actual work to a shape-shape based operating function
    //we must apply the logic here also!

    //the shape operator function returns three parts: A, B and C
    //A: the difference A NOT B
    //B: the difference B NOT A
    //C: the intersection of both A AND B
    //together these three make up the union (but fragmented into regions for which we can set attribute values

    //these are the operator types
    //#define SPH_SHAPE_UNKNOWN 0
    //#define SPH_SHAPE_POINT 1
    //#define SPH_SHAPE_LINE 2
    //#define SPH_SHAPE_POLYGON 3
    //#define SPH_SHAPE_MULTIPOINT 4
    //#define SPH_SHAPE_MULTILINE 5
    //#define SPH_SHAPE_MULTIPOLYGON 6

    if(Operator == SPH_SHAPE_INTERSECT)
    {
        //Brute force n2, add all intersections betwee sf1 and sf2
        for(int i = 0; i < sf1->GetFeatureCount(); i++)
        {
            for(int j = 0; j < sf2->GetFeatureCount(); j++)
            {
                //prepare input
                Feature * f1 = sf1->GetFeature(i);
                Feature * f2 = sf2->GetFeature(j);
                if(!(f1->GetShapeCount() > 0 && f2->GetShapeCount() > 0))
                {
                    continue;
                }else {

                    QList<SPHShape *> ret_A;
                    QList<SPHShape *> ret_B;
                    QList<SPHShape *> ret_C;

                    //do calculation
                    ShapeAlgebra(f1->GetShape(0),f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                    //now add the relevant part

                    if(ret_B.length() > 0)
                    {

                        for(int k = 0; k < ret_B.length(); k++)
                        {
                            Feature * fnew = new Feature();
                            fnew->AddShape(ret_B.at(k));
                            ret_layer->AddFeature(fnew);
                        }
                    }

                    for(int k = 0; k < ret_A.length(); k++)
                    {
                        ret_A.at(k)->Destroy();
                        delete ret_A.at(k);
                    }
                    for(int k = 0; k < ret_C.length(); k++)
                    {
                        ret_C.at(k)->Destroy();
                        delete ret_C.at(k);
                    }

                    //set the attributes of this part

                }
            }
        }


    }else if(Operator == SPH_SHAPE_UNION)
    {

        //Bruter force, n3??
        //first, add all sf2 shapes to the new shapefile
        //then keep adding sf1 shapes to the new shapefile
            //union with all shapes in results
            //only add new parts

        while((sf1->GetFeatureCount() + sf2->GetFeatureCount()) > 0)
        {
            bool take_l2 = false;
            Feature * f1;

            if(sf1->GetFeatureCount() != 0)
            {
                f1 = sf1->GetFeature(0);
                sf1->RemoveFeature(0);
            }else {
                f1 = sf2->GetFeature(0);
                sf2->RemoveFeature(0);
            }

            QList<SPHShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                f1->Destroy();
                delete f1;
                continue;
            }else
            {
                shape_parts.append(f1->GetShape(0));
                shape_parts_startindex.append(0);
            }

            while(shape_parts.length() > 0)
            {



                QList<Feature *> f_replace_existing;
                QList<Feature *> f_remove_existing;
                QList<Feature *> f_add_existing;

                SPHShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < ret_layer->GetFeatureCount(); i++)
                {

                    Feature * f2 = ret_layer->GetFeature(i);

                    if(!((f1->GetShapeCount() > 0) && (f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {

                        QList<SPHShape *> ret_A;
                        QList<SPHShape *> ret_B;
                        QList<SPHShape *> ret_C;

                        //do calculation
                        ShapeAlgebra(shape_current,f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                        //now add the intersections
                        if(ret_B.length() > 0)
                        {
                            for(int k = 0; k < ret_B.length(); k++)
                            {
                                Feature * fnew = new Feature();
                                fnew->AddShape(ret_B.at(k));
                                f_add_existing.append(fnew);
                            }
                        }
                        bool do_break_nothingleft = false;

                        //this is the part we continue with
                        if(ret_A.length() > 0)
                        {
                            did_interact = true;
                            shape_current = ret_A.at(0);

                            for(int k = 1; k < ret_A.length(); k++)
                            {
                                shape_parts.append(ret_A.at(k));
                                shape_parts_startindex.append(i+1);
                            }
                        }else { //if there is nothing left of the original shape, continue
                            do_break_nothingleft = true;
                        }

                        //replace the one that was already in
                        if(ret_C.length() > 0)
                        {

                            if(!f_remove_existing.contains(f2))
                            {
                                f_remove_existing.append(f2);
                                for(int k = 0; k < ret_C.length(); k++)
                                {
                                    Feature * fnew = new Feature();
                                    fnew->AddShape(ret_C.at(k));
                                    f_replace_existing.append(fnew);
                                }
                            }
                        }

                        if(do_break_nothingleft)
                        {
                            shape_current = nullptr;
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {
                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    f_add_existing.append(fnew);
                }

                for(int i = 0; i < f_remove_existing.length(); i++)
                {
                    ret_layer->RemoveFeature(f_remove_existing.at(i));
                }
                for(int i= 0; i < f_replace_existing.length(); i++)
                {
                    ret_layer->AddFeature(f_replace_existing.at(i));
                }
                for(int i= 0; i < f_add_existing.length(); i++)
                {
                    ret_layer->AddFeature(f_add_existing.at(i));
                }

            }


            f1->Destroy();
            delete f1;
        }



    }else if(Operator == SPH_SHAPE_DIFFERENCE)
    {
        //brute force, n2
        //for each shape in sf1, do a difference with all the shapes in sf2
        //then add to results

        for(int j = 0; j < sf1->GetFeatureCount(); j++)
        {
            bool take_l2 = false;
            Feature * f1;

            f1 = sf1->GetFeature(j);

            QList<SPHShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                continue;
            }else
            {
                shape_parts.append(f1->GetShape(0));
                shape_parts_startindex.append(0);
            }

            while(shape_parts.length() > 0)
            {

                SPHShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < sf2->GetFeatureCount(); i++)
                {

                    Feature * f2 = sf2->GetFeature(i);

                    if(!((f1->GetShapeCount() > 0) && (f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {

                        QList<SPHShape *> ret_A;
                        QList<SPHShape *> ret_B;
                        QList<SPHShape *> ret_C;

                        //do calculation
                        ShapeAlgebra(shape_current,f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                        bool do_break_nothingleft = false;

                        //this is the part we continue with
                        if(ret_A.length() > 0)
                        {
                            did_interact = true;
                            shape_current = ret_A.at(0);

                            for(int k = 1; k < ret_A.length(); k++)
                            {
                                shape_parts.append(ret_A.at(k));
                                shape_parts_startindex.append(i+1);
                            }
                        }else { //if there is nothing left of the original shape, continue
                            do_break_nothingleft = true;
                        }

                        for(int k = 0; k < ret_B.length(); k++)
                        {
                            ret_B.at(k)->Destroy();
                            delete ret_B.at(k);
                        }
                        for(int k = 0; k < ret_C.length(); k++)
                        {
                            ret_C.at(k)->Destroy();
                            delete ret_C.at(k);
                        }

                        if(do_break_nothingleft)
                        {
                            shape_current = nullptr;
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {
                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    ret_layer->AddFeature(fnew);
                }

            }

        }


    }else if(Operator == SPH_SHAPE_XOR)
    {
        //same as in difference, but for each difference calculation, add the C part
        for(int j = 0; j < sf1->GetFeatureCount(); j++)
        {
            Feature * f1;

            f1 = sf1->GetFeature(j);

            QList<SPHShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                continue;
            }else
            {
                shape_parts.append(f1->GetShape(0));
                shape_parts_startindex.append(0);
            }

            while(shape_parts.length() > 0)
            {

                SPHShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < sf2->GetFeatureCount(); i++)
                {

                    Feature * f2 = sf2->GetFeature(i);

                    if(!((f1->GetShapeCount() > 0) && (f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {

                        QList<SPHShape *> ret_A;
                        QList<SPHShape *> ret_B;
                        QList<SPHShape *> ret_C;

                        //do calculation
                        ShapeAlgebra(shape_current,f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                        bool do_break_nothingleft = false;

                        //this is the part we continue with
                        if(ret_A.length() > 0)
                        {
                            did_interact = true;
                            shape_current = ret_A.at(0);

                            for(int k = 1; k < ret_A.length(); k++)
                            {
                                shape_parts.append(ret_A.at(k));
                                shape_parts_startindex.append(i+1);
                            }
                        }else { //if there is nothing left of the original shape, continue
                            do_break_nothingleft = true;
                        }

                        for(int k = 0; k < ret_B.length(); k++)
                        {
                            ret_B.at(k)->Destroy();
                            delete ret_B.at(k);
                        }
                        for(int k = 0; k < ret_C.length(); k++)
                        {
                            ret_C.at(k)->Destroy();
                            delete ret_C.at(k);
                        }

                        if(do_break_nothingleft)
                        {
                            shape_current = nullptr;
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {
                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    ret_layer->AddFeature(fnew);
                }

            }

        }


        for(int j = 0; j < sf2->GetFeatureCount(); j++)
        {
            Feature * f1;

            f1 = sf2->GetFeature(j);

            QList<SPHShape *> shape_parts;
            QList<int> shape_parts_startindex;
            if(!(f1->GetShapeCount() > 0))
            {
                continue;
            }else
            {
                shape_parts.append(f1->GetShape(0));
                shape_parts_startindex.append(0);
            }

            while(shape_parts.length() > 0)
            {

                SPHShape * shape_current = shape_parts.at(0);
                int startindex =shape_parts_startindex.at(0);
                shape_parts_startindex.removeAt(0);
                shape_parts.removeAt(0);


                bool did_interact = false;

                //do all the interactions
                for(int i = startindex; i < sf1->GetFeatureCount(); i++)
                {

                    Feature * f2 = sf1->GetFeature(i);

                    if(!((f1->GetShapeCount() > 0) && (f2->GetShapeCount() > 0)))
                    {
                        continue;
                    }else {

                        QList<SPHShape *> ret_A;
                        QList<SPHShape *> ret_B;
                        QList<SPHShape *> ret_C;

                        //do calculation
                        ShapeAlgebra(shape_current,f2->GetShape(0),&ret_A,&ret_B,&ret_C);

                        bool do_break_nothingleft = false;

                        //this is the part we continue with
                        if(ret_A.length() > 0)
                        {
                            did_interact = true;
                            shape_current = ret_A.at(0);

                            for(int k = 1; k < ret_A.length(); k++)
                            {
                                shape_parts.append(ret_A.at(k));
                                shape_parts_startindex.append(i+1);
                            }
                        }else { //if there is nothing left of the original shape, continue
                            do_break_nothingleft = true;
                        }

                        for(int k = 0; k < ret_B.length(); k++)
                        {
                            ret_B.at(k)->Destroy();
                            delete ret_B.at(k);
                        }
                        for(int k = 0; k < ret_C.length(); k++)
                        {
                            ret_C.at(k)->Destroy();
                            delete ret_C.at(k);
                        }

                        if(do_break_nothingleft)
                        {
                            shape_current = nullptr;
                            break;
                        }
                    }

                }

                if(shape_current != nullptr)
                {
                    //add the remainder
                    Feature * fnew = new Feature();
                    fnew->AddShape(shape_current->Copy());
                    ret_layer->AddFeature(fnew);
                }

            }

        }

    }else if(Operator == SPH_SHAPE_ADD)
    {
        //simply copy and add all of the features to a single new shapefile

        for(int i = 0; i < sf1->GetFeatureCount(); i++)
        {
            ret_layer->AddFeature(sf1->GetFeature(i)->Copy());
        }
        for(int i = 0; i < sf2->GetFeatureCount(); i++)
        {
            ret_layer->AddFeature(sf2->GetFeature(i)->Copy());
        }


    }else//assume that if no other valid operation is given, we do noting: if(Operator == SPH_SHAPE_NONE)
    {
        //return nothing
        return new ShapeFile();
    }

    //since we made a copy of the shapefiles, delete these copies
    sf1->Destroy();
    delete sf1;
    sf2->Destroy();
    delete sf2;

    return ret_sf;
}

inline static void ShapeAlgebra(SPHShape * s1, SPHShape * s2, QList<SPHShape *> * ret_A,  QList<SPHShape *> * ret_B, QList<SPHShape *> * ret_C)
{


    //first do a simple bound check to see if there is potential interaction
    bool interaction_possible = false;

    if(!(s1->GetBoundingBox().Overlaps(s2->GetBoundingBox())))
    {
        ret_A->append(s1->Copy());
        ret_C->append(s2->Copy());
        return;
    }

    //check for null pointer
    if(s1 == nullptr || s2 == nullptr)
    {
        return;
    }

    int type1 = s2->GetType();
    int type2 = s1->GetType();

    QList<SPHShape*> s1_shapelist;
    QList<SPHShape*> s2_shapelist;

    //check if multi-shape
    //if so, do recursive call and add restuls to return list
    if(type1 == SPH_SHAPE_UNKNOWN || type2 == SPH_SHAPE_UNKNOWN)
    {
        return;
    }else if(type1 == SPH_SHAPE_MULTIPOINT)
    {
        SPHMultiPoint *msp = (SPHMultiPoint*)s1;
        for(int i = 0; i < msp->GetPointCount(); i++)
        {
            s1_shapelist.append((msp->GetPoint(i)));
        }

    }else if(type1 == SPH_SHAPE_MULTILINE)
    {
        SPHMultiLine *msp = (SPHMultiLine*)s1;
        for(int i = 0; i < msp->GetLineCount(); i++)
        {
            s1_shapelist.append((msp->GetLine(i)));
        }

    }else if(type1 == SPH_SHAPE_MULTIPOLYGON)
    {
        SPHMultiPolygon *msp = (SPHMultiPolygon*)s1;
        for(int i = 0; i < msp->GetPolygonCount(); i++)
        {
            s1_shapelist.append((msp->GetPolygon(i)));
        }
    }else
    {
        s1_shapelist.append(s1);
    }

    if(type2 == SPH_SHAPE_MULTIPOINT)
    {
        SPHMultiPoint *msp = (SPHMultiPoint*)s2;
        for(int i = 0; i < msp->GetPointCount(); i++)
        {
            s2_shapelist.append((s1,msp->GetPoint(i)));
        }
    }else if(type2 == SPH_SHAPE_MULTILINE)
    {
        SPHMultiLine *msp = (SPHMultiLine*)s2;
        for(int i = 0; i < msp->GetLineCount(); i++)
        {
            s2_shapelist.append((s1,msp->GetLine(i)));
        }
    }else if(type2 == SPH_SHAPE_MULTIPOLYGON)
    {
        SPHMultiPolygon *msp = (SPHMultiPolygon*)s2;
        for(int i = 0; i < msp->GetPolygonCount(); i++)
        {
            s2_shapelist.append((s1,msp->GetPolygon(i)));
        }
    }else
    {
        s2_shapelist.append(s2);
    }

    ShapeAlgebra(s1_shapelist,s2_shapelist,ret_A,ret_B,ret_C);

    return;
}

//this functions must get as arguments two lists of single-instance shapes (so no multi-point, multi-polygon or multi-line)
//additionaly, the shapes in the list must be all from the same type
inline static void ShapeAlgebra(QList<SPHShape *> s1, QList<SPHShape *> s2, QList<SPHShape *> * ret_A,  QList<SPHShape *> * ret_B, QList<SPHShape *> * ret_C)
{

    if(!(s1.length() > 0 && s2.length() > 0))
    {
        return;
    }

    int type1 = s1.at(0)->GetType();
    int type2 = s2.at(0)->GetType();

    if(type1 == SPH_SHAPE_POINT && type2 == SPH_SHAPE_POLYGON)
    {
        QList<SPHPoint *> sl1;
        QList<SPHPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((SPHPoint*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((SPHPolygon*)s2.at(i));
        }
        PointPolygonDisect(sl1,sl2,ret_A,ret_B,ret_C);

    }else if(type1 == SPH_SHAPE_LINE && type2 == SPH_SHAPE_POLYGON)
    {
        QList<SPHLine *> sl1;
        QList<SPHPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((SPHLine*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((SPHPolygon*)s2.at(i));
        }
        LinePolygonDisect(sl1,sl2,ret_A,ret_B,ret_C);
    }else if(type1 == SPH_SHAPE_POLYGON && type2 == SPH_SHAPE_POINT)
    {
        QList<SPHPoint *> sl1;
        QList<SPHPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((SPHPoint*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((SPHPolygon*)s2.at(i));
        }
        PointPolygonDisect(sl1,sl2,ret_C,ret_B,ret_A);

    }else if(type1 == SPH_SHAPE_POLYGON && type2 == SPH_SHAPE_LINE)
    {
        QList<SPHLine *> sl1;
        QList<SPHPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((SPHLine*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((SPHPolygon*)s2.at(i));
        }
        LinePolygonDisect(sl1,sl2,ret_C,ret_B,ret_A);
    }else if(type1 == SPH_SHAPE_LINE && type2 == SPH_SHAPE_LINE)
    {
        QList<SPHLine *> sl1;
        QList<SPHLine *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((SPHLine*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((SPHLine*)s2.at(i));
        }
        LineLineDisect(sl1,sl2,ret_A,ret_B,ret_C);
    }else if(type1 == SPH_SHAPE_POLYGON && type2 == SPH_SHAPE_POLYGON)
    {
        QList<SPHPolygon *> sl1;
        QList<SPHPolygon *> sl2;
        for(int i = 0; i < s1.length(); i++)
        {
            sl1.append((SPHPolygon*)s1.at(i));
        }
        for(int i = 0; i < s2.length(); i++)
        {
            sl2.append((SPHPolygon*)s2.at(i));
        }
        PolygonPolygonDisect(sl1,sl2,ret_A,ret_B,ret_C);

    }else if(type1 == SPH_SHAPE_POINT && type2 == SPH_SHAPE_LINE)
    {
        //we dont do point-line interactions
        return;

    }else if(type1 == SPH_SHAPE_POINT && type2 == SPH_SHAPE_LINE)
    {
        //we dont do line-point interactions
        return;

    }else if(type1 == SPH_SHAPE_POINT && type2 == SPH_SHAPE_POINT)
    {
        //we dont do point-point interactions
        return;

    }else {
        //else not supported

        return;
    }
}



//specific functions


inline static void PolygonTreeToPolygonList(QList<SPHShape*> * ret_pols,ClipperLib::PolyNode * node, BoundingBox &extent,SPHPolygon * parent)
{

    if(node != nullptr)
    {
        if(node->IsOpen() == true)
        {



        }else {
            SPHPolygon * current_parent = nullptr;
            if(!(node->IsHole()))
            {
                SPHPolygon * p = new SPHPolygon();
                Ring * r = new Ring();
                for(int i = 0; i < node->Contour.size(); i++)
                {
                    double x = (((double)(node->Contour.at(i).X))+(9.0e18))/(2.0*9e18);
                    double y = (((double)(node->Contour.at(i).Y))+(9.0e18))/(2.0*9e18);
                    SPHVector2 point = extent.GetCoordinateAbsoluteFromRelative(SPHVector2(x,y));
                    r->AddVertex(point.x,point.y);
                }
                if(r->GetVertexCount() > 0)
                {
                    r->AddVertex(r->GetVertex(0).x,r->GetVertex(0).y);
                }
                p->SetExtRing(r);
                ret_pols->append((SPHShape*)p);

                for(int i = 0; i < node->Childs.size();i++)
                {
                    PolygonTreeToPolygonList(ret_pols,node->Childs.at(i),extent,p);
                }
                current_parent = p;
            }else if(node->IsHole())
            {

                if(parent != nullptr)
                {
                    Ring * r = new Ring();
                    for(int i = 0; i < node->Contour.size(); i++)
                    {
                        double x = (((double)(node->Contour.at(i).X))+(9.0e18))/(2.0*9e18);
                        double y = (((double)(node->Contour.at(i).Y))+(9.0e18))/(2.0*9e18);
                        SPHVector2 point = extent.GetCoordinateAbsoluteFromRelative(SPHVector2(x,y));
                        r->AddVertex(point.x,point.y);

                    }
                    if(r->GetVertexCount() > 0)
                    {
                        r->AddVertex(r->GetVertex(0).x,r->GetVertex(0).y);
                    }
                    parent->AddIntRing(r);
                }
                for(int i = 0; i < node->Childs.size();i++)
                {
                    PolygonTreeToPolygonList(ret_pols,node->Childs.at(i),extent,nullptr);
                }

            }
        }
    }

}

inline static int PointToClipperPath(SPHPoint * p1,ClipperLib::Paths *Polygon1, BoundingBox &extent, int k)
{
    SPHVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(SPHVector2(p1->GetX(),p1->GetY()));

    ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
    ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));
    (*Polygon1)[k] << ClipperLib::IntPoint(x,y);
    (*Polygon1)[k] << ClipperLib::IntPoint(x+1,y);
    return k + 1;

}


inline static int LineToClipperPath(SPHLine * p1,ClipperLib::Paths *Polygon1, BoundingBox &extent, int k)
{
    {
        for(int i = 0; i < p1->GetVertexCount();i++)
        {
            SPHVector2 p1r_p = p1->GetVertex(i);
            //convert to clipper coordinates
            SPHVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));
            (*Polygon1)[k] << ClipperLib::IntPoint(x,y);
        }
    }

    return k + 1;


}


inline static int PolygonToClipperPath(SPHPolygon * p1,ClipperLib::Paths *Polygon1, BoundingBox &extent, int k)
{
    {
        Ring * p1r = p1->GetExtRing();
        for(int i = 0; i < p1r->GetVertexCount();i++)
        {
            SPHVector2 p1r_p = p1r->GetVertex(i);
            //convert to clipper coordinates
            SPHVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9.0e18) + (double)(p1r_p_i.y) * (2.0*9e18));
            (*Polygon1)[k] << ClipperLib::IntPoint(x,y);
        }
    }

    for(int j = 0; j < p1->GetIntRingCount(); j++)
    {
        Ring * p1r = p1->GetIntRing(j);
        for(int i = 0; i < p1r->GetVertexCount();i++)
        {
            SPHVector2 p1r_p = p1r->GetVertex(i);
            //convert to clipper coordinates
            SPHVector2 p1r_p_i = extent.GetCoordinateRelativeFromAbsolute(p1r_p);

            ClipperLib::cInt x = (ClipperLib::cInt)((-9e18) + (double)(p1r_p_i.x) * (2.0*9e18));
            ClipperLib::cInt y = (ClipperLib::cInt)((-9e18) + (double)(p1r_p_i.y) * (2.0*9e18));
            (*Polygon1)[k+1+j] << ClipperLib::IntPoint(x,y);
        }
    }

    return k + 2 + p1->GetIntRingCount();


}

//line-polygon intersection (returns multiple polygons and a list of lines that are inside or outside the polygon but end when they meet a polygon boundary)
inline static void PolygonPolygonDisect(QList<SPHPolygon *> p1l, QList<SPHPolygon *> p2l, QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C)
{
    int n_path_1 = 0;
    int n_path_2 = 0;

    if(!p1l.length() > 0 && p2l.length() > 0)
    {

    }

    BoundingBox extent;

    for(int i = 0; i < p1l.length(); i++)
    {
        SPHPolygon * p1 = p1l.at(i);
        if(i== 0)
        {
            extent = p1->GetBoundingBox();
        }else {
            extent.MergeWith(p1->GetBoundingBox());
        }
        if(!(p1->GetExtRing() == nullptr))
        {
            n_path_1 += 1;
            n_path_1 += p1->GetIntRingCount();
        }
    }
    for(int i = 0; i < p2l.length(); i++)
    {
        SPHPolygon * p2 = p2l.at(i);

        extent.MergeWith(p2->GetBoundingBox());

        if(!(p2->GetExtRing() == nullptr))
        {
            n_path_2 += 1;
            n_path_2 += p2->GetIntRingCount();
        }
    }

    extent.Scale(3.0);

    //to use the clipper library, we have to rescale the coordinates to 64 bit integers within a certain bounding box
    //to do this, we define the bounding box as the total bounding box of both shapes with a 100 percent margin on both sides


    ClipperLib::Paths Polygon1(n_path_1);
    ClipperLib::Paths Polygon2(n_path_2);

    int k = 0;
    for(int i = 0; i < p1l.length(); i++)
    {
        SPHPolygon * p1 = p1l.at(i);
        k = PolygonToClipperPath(p1,&Polygon1,extent,k);
    }
    k = 0;
    for(int i = 0; i < p2l.length(); i++)
    {
        SPHPolygon * p2 = p2l.at(i);
        k = PolygonToClipperPath(p2,&Polygon2,extent,k);
    }


    //now run the desired operators

    int solution_count = 3;
    ClipperLib::ClipType OperatorClipLib[3] = {ClipperLib::ctIntersection,ClipperLib::ctDifference,ClipperLib::ctDifference}; // default value
    ClipperLib::Paths * Path_A[3] = {&Polygon1,&Polygon1,&Polygon2};
    ClipperLib::Paths * Path_B[3] = {&Polygon2,&Polygon2,&Polygon1};

    QList<SPHShape*> * Path_Ret[3] = {ret_pol_B,ret_pol_A,ret_pol_C};

    for(int s = 0; s < solution_count; s++)
    {
        ClipperLib::PolyTree Solution;
        ClipperLib::Clipper C;
        C.AddPaths(*Path_A[s],ClipperLib::ptSubject,true);
        C.AddPaths(*Path_B[s],ClipperLib::ptClip,true);

        C.Execute(OperatorClipLib[s],Solution,ClipperLib::pftEvenOdd);

        //now convert back the Solution path to a list of polygons

        for(int i = 0; i < Solution.Childs.size(); i++)
        {
            PolygonTreeToPolygonList(Path_Ret[s],Solution.Childs.at(i),extent,nullptr);
        }


    }

}


//line-polygon intersection (returns multiple polygons and a list of lines that are inside or outside the polygon but end when they meet a polygon boundary)
inline static void LinePolygonDisect(QList<SPHLine *> p1, QList<SPHPolygon *> p2, QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C)
{




}

//line-polygon intersection (returns multiple polygons and a list of lines that are inside or outside the polygon but end when they meet a polygon boundary)
inline static void PointPolygonDisect(QList<SPHPoint *> p1, QList<SPHPolygon *> p2, QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C)
{


}

//line-polygon intersection (returns multiple polygons and a list of lines that are inside or outside the polygon but end when they meet a polygon boundary)
inline static void LineLineDisect(QList<SPHLine *> p1, QList<SPHLine *> p2, QList<SPHShape*> * ret_pol_A,QList<SPHShape*> * ret_pol_B,QList<SPHShape*> * ret_pol_C)
{




}





