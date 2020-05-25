#ifndef GEOALGORITHMSCRIPTING_H
#define GEOALGORITHMSCRIPTING_H


#include <angelscript.h>
#include "scriptmanager.h"
#include "shapefile.h"
#include "rasterize.h"
#include "vectorize.h"
#include "rastercontour.h"
#include "rasterwarp.h"
#include "simplify.h"
#include "segmentize.h"
#include "rastersegmentize.h"



inline static void RegisterGeoAlgorithmsToScriptEngine(asIScriptEngine *engine)
{
    //this is a non-executed test piece of code to force linking to the relevant dll's
    //this allows debugging the setup of all dependencies
    if(engine == nullptr)
    {



    }


    //register conversion functions
    int r = engine->RegisterGlobalFunction("Map @Rasterize(Map &in s, Shapes &in s,string attribute = "", bool burn = false, float burn_value = 1.0, bool invert = false, bool all_touched = false, bool add = false)", asFUNCTION( AS_Rasterize),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Shapes @Vectorize(Map &in s,string attributename, bool diagonal)", asFUNCTION( AS_Vectorize),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Shapes @RasterContour(Map &in s,float interval)", asFUNCTIONPR( AS_RasterContour,(cTMap *,float),ShapeFile*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterWarp(Map &in target,array<Map> &in sources, string interpolation = \"linear\")", asFUNCTIONPR( AS_RasterWarp,(cTMap *,CScriptArray*,QString),cTMap *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterWarp(Map &in target,Map &in sources, string interpolation = \"linear\")", asFUNCTIONPR( AS_RasterWarp,(cTMap *,cTMap*,QString),cTMap *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Shapes @VectorWarp(Shapes &in source,GeoProjection &in CRS)", asFUNCTIONPR( AS_VectorWarp,(ShapeFile*,GeoProjection *),ShapeFile *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Shapes @Segmentize(Shapes &in source,float distancemax)", asFUNCTIONPR( AS_Segmentize,(ShapeFile *,float),ShapeFile *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Shapes @Simplify(Shapes &in source,float distancethreshold)", asFUNCTIONPR( AS_Simplify,(ShapeFile *,float),ShapeFile *),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @Segmentize(Map &in source,float spatial_radius, float range_radius, float min_region_size, float max_iter, float in_thres)", asFUNCTIONPR( AS_RasterSegmentize,(cTMap *,float,float,float,float,float),cTMap *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Segmentize(array<Map> &in multi_band_source,float spatial_radius, float range_radius, float min_region_size, float max_iter, float in_thres)", asFUNCTIONPR( AS_RasterSegmentize,(CScriptArray*,float,float,float,float,float),cTMap *),  asCALL_CDECL); assert( r >= 0 );




}


#endif // GEOALGORITHMSCRIPTING_H
