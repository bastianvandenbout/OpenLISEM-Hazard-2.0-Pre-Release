#ifndef GEOPROJECTIONSCRIPTING_H
#define GEOPROJECTIONSCRIPTING_H


#include <angelscript.h>
#include "scriptmanager.h"
#include "geoprojection.h"

inline void RegisterGeoProjectionToScriptEngine(asIScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("GeoProjection",0,asOBJ_REF);//Shapes
    if(r < 0){SPH_DEBUG("error in registering scripting: class GeoProjection");};

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("GeoProjection",asBEHAVE_ADDREF,"void f()",asMETHOD(GeoProjection,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting GeoProjection@ AR()");};

    r = engine->RegisterObjectBehaviour("GeoProjection",asBEHAVE_RELEASE,"void f()",asMETHOD(GeoProjection,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting GeoProjection@ RR()");};

    r = engine->RegisterObjectMethod("GeoProjection", "GeoProjection& opAssign(GeoProjection &in m)", asMETHOD(GeoProjection,AS_Assign), asCALL_THISCALL); assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting GeoProjection& opAssign(GeoProjection &in m)");};

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("GeoProjection",asBEHAVE_FACTORY,"GeoProjection@ CSF0()",asFUNCTIONPR(GeoProjectionFactory,(),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting GeoProjection@ C0()");};


    r = engine->RegisterGlobalFunction("GeoProjection &GeoProjectionFromEPSG(int)", asFUNCTIONPR(AS_GetGeoProjectionFromEPSG,(int),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting GeoProjection &GeoProjectionFromEPSG(int)");};

    r = engine->RegisterGlobalFunction("GeoProjection &GeoProjectionFromWKT(string)", asFUNCTIONPR(AS_GetGeoProjectionFromWKT,(std::string),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting GeoProjection &GeoProjectionFromWKT(string)");};

    r = engine->RegisterGlobalFunction("GeoProjection &GeoProjectionFromWGS84UTM(int,bool)", asFUNCTIONPR(AS_GetGeoProjectionFromWGS84UTM,(int, bool),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting GeoProjection &GeoProjectionFromWGS84UTM(int,bool)");};




}
#endif // GEOPROJECTIONSCRIPTING_H
