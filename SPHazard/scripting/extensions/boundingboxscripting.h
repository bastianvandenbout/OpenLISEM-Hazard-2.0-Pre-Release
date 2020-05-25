#ifndef BOUNDINGBOXSCRIPTING_H
#define BOUNDINGBOXSCRIPTING_H

#include "boundingbox.h"
#include "sphscriptengine.h"
#include "point.h"

inline static void RegionFactory(void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) BoundingBox();

}

inline static void RegionFactory(double x1,double x2,double y1,double y2,void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) BoundingBox(x1,x2,y1,y2);

}


inline void RegisterGeoElementScripting(asIScriptEngine *engine)
{
    int r = engine->RegisterObjectType("Region", sizeof(BoundingBox), asOBJ_VALUE | asOBJ_POD| asGetTypeTraits<BoundingBox>()); assert( r >= 0 );

    // Register the behaviours
    r = engine->RegisterObjectBehaviour("Region", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(RegionFactory,(void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Region", asBEHAVE_CONSTRUCT, "void CSF0(double,double,double,double)", asFUNCTIONPR(RegionFactory,(double,double,double,double,void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Region", "double XMin()", asMETHOD(BoundingBox,GetMinX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Region", "double XMax()", asMETHOD(BoundingBox,GetMaxX), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Region", "double YMin()", asMETHOD(BoundingBox,GetMinX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Region", "double YMax()", asMETHOD(BoundingBox,GetMaxX), asCALL_THISCALL); assert( r >= 0 );

}

inline static void PointFactory(void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) SPHPoint();

}

inline static void PointFactory(double x1,double x2,void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) SPHPoint(x1,x2);

}


inline void RegisterGeometryScripting(asIScriptEngine *engine)
{

    int r = engine->RegisterObjectType("Point", sizeof(SPHPoint), asOBJ_VALUE | asOBJ_POD| asGetTypeTraits<SPHPoint>()); assert( r >= 0 );

    // Register the behaviours
    r = engine->RegisterObjectBehaviour("Point", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(PointFactory,(void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Point", asBEHAVE_CONSTRUCT, "void CSF0(double,double,double,double)", asFUNCTIONPR(PointFactory,(double,double,void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Point", "double X()", asMETHOD(SPHPoint,GetX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Point", "double Y()", asMETHOD(SPHPoint,GetY), asCALL_THISCALL); assert( r >= 0 );

    //point

    //line

    //polygon

    //circle



    //algorithms



}



#endif // BOUNDINGBOXSCRIPTING_H
