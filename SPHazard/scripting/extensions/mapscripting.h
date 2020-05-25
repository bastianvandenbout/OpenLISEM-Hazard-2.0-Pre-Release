#pragma once

#include <angelscript.h>
#include "map.h"
#include "rastermath.h"
#include "rasterconditional.h"
#include "rasterspread.h"
#include "rasterreduce.h"
#include "rastertrigonometric.h"
#include "rasterlimit.h"
#include "rasternetwork.h"
#include "iogdal.h"
#include "scriptmanager.h"
#include "rasterderivative.h"
#include "rasterclassified.h"
#include "rasterinterpolation.h"
#include "rasteralgorithms.h"
#include "rasterconvolve.h"
#include "rasteralgebra.h"
#include "rasterconstructors.h"
#include "matrixtable.h"
#include "rastertable.h"
#include "rasterflow.h"
#include "rastershading.h"

inline void RegisterMapToScriptEngine(asIScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("Map",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()
    if(r < 0){SPH_DEBUG("error in registering scripting: class Map");};

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C0()",asFUNCTIONPR(MapFactory,(),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C1(int rows, int columns)",asFUNCTIONPR(MapFactory,(int,int),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C2(int rows, int columns, float dx)",asFUNCTIONPR(MapFactory,(int,int,float),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C3(int rows, int columns, float dx, float value)",asFUNCTIONPR(MapFactory,(int, int, float,float),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C4(int rows, int columns, float dx, float value, float north, float west)",asFUNCTIONPR(MapFactory,(int,int,float,float,float,float),cTMap *),asCALL_CDECL);  assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C5(int)",asFUNCTIONPR(MapFactory,(int),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C6(float)",asFUNCTIONPR(MapFactory,(float),cTMap *),asCALL_CDECL); assert( r >= 0 );
    //r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C7(int rows, int columns, BoundingBox &in extent)",asFUNCTIONPR(MapFactory,(float),cTMap *),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_ADDREF,"void f()",asMETHOD(cTMap,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_RELEASE,"void f()",asMETHOD(cTMap,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );

    //register member functions
    r = engine->RegisterObjectMethod("Map", "int rows()", asMETHOD(cTMap,AS_Rows), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "int cols()", asMETHOD(cTMap,AS_Cols), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Map", "float north()", asMETHOD(cTMap,AS_North), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "float west()", asMETHOD(cTMap,AS_West), asCALL_THISCALL); assert( r >= 0 );

    //referencing options
    r = engine->RegisterGlobalFunction("Map @SetCRS(Map &in s1,GeoProjection &in p)", asFUNCTIONPR( AS_SetCRS,(cTMap*,GeoProjection*),cTMap*),  asCALL_CDECL); assert( r >= 0 );




    //special operators

    r = engine->RegisterObjectMethod("Map", "float& opIndex(int, int)", asMETHOD(cTMap,AS_Value), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAssign(Map &in m)", asMETHODPR(cTMap,AS_Assign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAssign(float v)", asMETHODPR(cTMap,AS_Assign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opNeg()", asMETHODPR(cTMap,AS_OpNeg,(void),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //mathematical operators

    r = engine->RegisterObjectMethod("Map", "Map& opAdd(Map &in m)", asMETHODPR(cTMap,AS_OpAdd,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMul(Map &in m)", asMETHODPR(cTMap,AS_OpMul,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opSub(Map &in m)", asMETHODPR(cTMap,AS_OpSub,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opDiv(Map &in m)", asMETHODPR(cTMap,AS_OpDiv,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opPow(Map &in m)", asMETHODPR(cTMap,AS_OpPow,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMod(Map &in m)", asMETHODPR(cTMap,AS_OpMod,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAdd(float v)", asMETHODPR(cTMap,AS_OpAdd,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMul(float v)", asMETHODPR(cTMap,AS_OpMul,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opSub(float v)", asMETHODPR(cTMap,AS_OpSub,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opDiv(float v)", asMETHODPR(cTMap,AS_OpDiv,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opPow(float v)", asMETHODPR(cTMap,AS_OpPow,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMod(float v)", asMETHODPR(cTMap,AS_OpMod,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //assignment operators

    r = engine->RegisterObjectMethod("Map", "Map& opAddAssign(Map &in m)", asMETHODPR(cTMap,AS_OpAddAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMulAssign(Map &in m)", asMETHODPR(cTMap,AS_OpMulAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opSubAssign(Map &in m)", asMETHODPR(cTMap,AS_OpSubAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opDivAssign(Map &in m)", asMETHODPR(cTMap,AS_OpDivAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opPowAssign(Map &in m)", asMETHODPR(cTMap,AS_OpPowAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opModAssign(Map &in m)", asMETHODPR(cTMap,AS_OpModAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Map", "Map& opAddAssign(float v)", asMETHODPR(cTMap,AS_OpAddAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMulAssign(float v)", asMETHODPR(cTMap,AS_OpMulAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opSubAssign(float v)", asMETHODPR(cTMap,AS_OpSubAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opDivAssign(float v)", asMETHODPR(cTMap,AS_OpDivAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opPowAssign(float v)", asMETHODPR(cTMap,AS_OpPowAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opModAssign(float v)", asMETHODPR(cTMap,AS_OpModAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //register comparison operators

    r = engine->RegisterObjectMethod("Map", "Map& eq(Map &in m)", asMETHODPR(cTMap,AS_EqualTo,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& neq(Map &in m)", asMETHODPR(cTMap,AS_NotEqualTo,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShl(Map &in m)", asMETHODPR(cTMap,AS_SmallerThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShr(Map &in m)", asMETHODPR(cTMap,AS_LargerThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShlAssign(Map &in m)", asMETHODPR(cTMap,AS_SmallerEqualThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShrAssign(Map &in m)", asMETHODPR(cTMap,AS_LargerEqualThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& eq(float v)", asMETHODPR(cTMap,AS_EqualTo,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& neq(float v)", asMETHODPR(cTMap,AS_NotEqualTo,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShl(float v)", asMETHODPR(cTMap,AS_SmallerThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShr(float v)", asMETHODPR(cTMap,AS_LargerThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShlAssign(float v)", asMETHODPR(cTMap,AS_SmallerEqualThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShrAssign(float v)", asMETHODPR(cTMap,AS_LargerEqualThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //register logical operators
    r = engine->RegisterObjectMethod("Map", "Map& opCom()", asMETHODPR(cTMap,AS_Negate,(),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAnd(Map &in m)", asMETHODPR(cTMap,AS_And,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opOr(Map &in m)", asMETHODPR(cTMap,AS_Or,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opXor(Map &in m)", asMETHODPR(cTMap,AS_Xor,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAnd(float v)", asMETHODPR(cTMap,AS_And,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opOr(float v)", asMETHODPR(cTMap,AS_Or,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opXor(float v)", asMETHODPR(cTMap,AS_Xor,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    //register logical assignment operators
    r = engine->RegisterObjectMethod("Map", "Map& opAndAssign(Map &in m)", asMETHODPR(cTMap,AS_AndAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opOrAssign(Map &in m)", asMETHODPR(cTMap,AS_OrAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opXorAssign(Map &in m)", asMETHODPR(cTMap,AS_XorAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAndAssign(float v)", asMETHODPR(cTMap,AS_AndAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opOrAssign(float v)", asMETHODPR(cTMap,AS_OrAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opXorAssign(float v)", asMETHODPR(cTMap,AS_XorAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //reverse mathematical operators
    r = engine->RegisterObjectMethod("Map", "Map& opAdd_r(float v)", asMETHODPR(cTMap,AS_OpAdd_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMul_r(float v)", asMETHODPR(cTMap,AS_OpMul_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opSub_r(float v)", asMETHODPR(cTMap,AS_OpSub_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opDiv_r(float v)", asMETHODPR(cTMap,AS_OpDiv_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opPow_r(float v)", asMETHODPR(cTMap,AS_OpPow_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMod_r(float v)", asMETHODPR(cTMap,AS_OpMod_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAnd_r(float v)", asMETHODPR(cTMap,AS_And_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opOr_r(float v)", asMETHODPR(cTMap,AS_Or_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opXor_r(float v)", asMETHODPR(cTMap,AS_Xor_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Map", "Map& eq_r(float v)", asMETHODPR(cTMap,AS_EqualTo_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& neq_r(float v)", asMETHODPR(cTMap,AS_NotEqualTo_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShl_r(float v)", asMETHODPR(cTMap,AS_SmallerThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShr_r(float v)", asMETHODPR(cTMap,AS_LargerThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShlAssign_r(float v)", asMETHODPR(cTMap,AS_SmallerEqualThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShrAssign_r(float v)", asMETHODPR(cTMap,AS_LargerEqualThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );


}

inline void RegisterMapMathToScriptEngine(asIScriptEngine *engine)
{

    //register mathematical functions for maps

    int r = engine->RegisterGlobalFunction("Map @sin(Map &in s)", asFUNCTION( AS_Mapsin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @cos(Map &in s)", asFUNCTION( AS_Mapcos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @tan(Map &in s)", asFUNCTION( AS_Maptan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @sinh(Map &in s)", asFUNCTION( AS_Mapsinh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @cosh(Map &in s)", asFUNCTION( AS_Mapcosh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @tanh(Map &in s)", asFUNCTION( AS_Maptanh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @asin(Map &in s)", asFUNCTION( AS_Mapasin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @acos(Map &in s)", asFUNCTION( AS_Mapacos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @atan(Map &in s)", asFUNCTION( AS_Mapatan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @atan2(Map &in s1,Map &in s2)", asFUNCTION( AS_Mapatan2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @abs(Map &in s)", asFUNCTION( AS_Mapabs),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @max(Map &in s1,Map &in s2)", asFUNCTION( AS_Mapmax),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @min(Map &in s1,Map &in s2)", asFUNCTION( AS_Mapmin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @exp(Map &in s)", asFUNCTION( AS_Mapexp),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @pow(Map &in s1,Map &in s2)", asFUNCTION( AS_Mappow),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @sqrt(Map &in s)", asFUNCTION( AS_Mapsqrt),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @log(Map &in s)", asFUNCTION( AS_Maplog),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @log10(Map &in s)", asFUNCTION( AS_Maplog10),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @ceil(Map &in s)", asFUNCTION( AS_Mapceil),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @floor(Map &in s)", asFUNCTION( AS_Mapfloor),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @fraction(Map &in s)", asFUNCTION( AS_Mapfraction),  asCALL_CDECL); assert( r >= 0 );

    //register functions related to missing values

    r = engine->RegisterGlobalFunction("Map @Cover(Map &in s,Map &in s2)", asFUNCTIONPR( AS_Cover,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Cover(Map &in s,float s2)", asFUNCTIONPR( AS_Cover,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Cover(float s,Map &in s2)", asFUNCTIONPR( AS_Cover,(float,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Cover(float s, float s2)", asFUNCTIONPR( AS_Cover,(float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SetMV(Map &in s,Map &in s2)", asFUNCTIONPR( AS_SetMV,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SetMV(Map &in s,float s2)", asFUNCTIONPR( AS_SetMV,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @IsMV(Map &in s)", asFUNCTIONPR( AS_IsMV,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float GetMVVal()", asFUNCTION( AS_GetMVVal),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float IsValMV(float s)", asFUNCTION( AS_IsValMV),  asCALL_CDECL); assert( r >= 0 );

    //register map averages (normal and reductive->meaning not a map but single float is returned)
    r = engine->RegisterGlobalFunction("Map @MapTotal(Map &in s)", asFUNCTION( AS_MapTotal),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapTotalRed(Map &in s)", asFUNCTION( AS_MapTotalRed),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapAverage(Map &in s)", asFUNCTION( AS_MapAverage),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapAverageRed(Map &in s)", asFUNCTION( AS_MapAverageRed),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapMinimum(Map &in s)", asFUNCTION( AS_MapMinimum),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapMinimumRed(Map &in s)", asFUNCTION( AS_MapMinimumRed),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapMaximum(Map &in s)", asFUNCTION( AS_MapMaximum),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapMaximumRed(Map &in s)", asFUNCTION( AS_MapMaximumRed),  asCALL_CDECL); assert( r >= 0 );


    //register conditional functions
    r = engine->RegisterGlobalFunction("Map @MapIf(Map &in m1,Map &in m2,Map &in m3)", asFUNCTIONPR( AS_MapIf,(cTMap *,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(Map &in m1,Map &in m2,float)", asFUNCTIONPR( AS_MapIf,(cTMap *,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(Map &in m1,float,Map &in m3)", asFUNCTIONPR( AS_MapIf,(cTMap *,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,Map &in m2,Map &in m3)", asFUNCTIONPR( AS_MapIf,(float,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,float,Map &in m3)", asFUNCTIONPR( AS_MapIf,(float,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(Map &in m1,float,float)", asFUNCTIONPR( AS_MapIf,(cTMap *,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,float,float)", asFUNCTIONPR( AS_MapIf,(float,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(Map &in m1,Map &in m2)", asFUNCTIONPR( AS_MapIf,(cTMap *,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float ,Map &in m2)", asFUNCTIONPR( AS_MapIf,(float,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(Map &in m1,float)", asFUNCTIONPR( AS_MapIf,(cTMap *,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,float)", asFUNCTIONPR( AS_MapIf,(float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );




}

inline void RegisterMapAlgorithmsToScriptEngine(asIScriptEngine *engine)
{

    //register algorithm for maps

    //spread algorithms
    int r = engine->RegisterGlobalFunction("Map @Spread(Map &in s1,Map &in s2,Map &in s3)", asFUNCTIONPR( AS_Spread,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Spread(Map &in s1,float s2,Map &in s3)", asFUNCTIONPR( AS_Spread,(cTMap*,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Spread(Map &in s1,Map &in s2,float s3)", asFUNCTIONPR( AS_Spread,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Spread(Map &in s1,float s2,float s3)", asFUNCTIONPR( AS_Spread,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(Map &in s1,Map &in s2,Map &in s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(Map &in s1,float s2,Map &in s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(Map &in s1,Map &in s2,float s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(Map &in s1,float s2,float s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //register raster derivative functions
    r = engine->RegisterGlobalFunction("Map @Slope(Map &in s1)", asFUNCTION( AS_Slope),  asCALL_CDECL); assert( r >= 0 );


    //register ldd related functions


    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,Map &in s2,Map &in s3,Map &in s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,cTMap *,cTMap *,cTMap *,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,float s3,float s4,float s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,float,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,Map &in s2,float s3,float s4,float s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,float,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,Map &in s3,float s4,float s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,float,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,float s3,Map &in s4,float s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,cTMap*,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,float s3,float s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,float,cTMap*,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,float s3,Map &in s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,cTMap*,cTMap*,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,Map &in s3,float s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,float,cTMap*,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,Map &in s2,float s3,float s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,float,cTMap*,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,Map &in s3,Map &in s4,float s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,cTMap*,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,Map &in s2,float s3,Map &in s4,float s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,cTMap*,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,Map &in s2,Map &in s3,Map &in s4,float s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,cTMap*,cTMap*,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,float s2,Map &in s3,Map &in s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,cTMap*,cTMap*,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,Map &in s2,float s3,Map &in s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,cTMap*,cTMap*,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1,Map &in s2,Map &in s3,float s4,Map &in s5,bool lddin = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,cTMap*,float,cTMap*,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(Map &in s1)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //register accuflux function

    r = engine->RegisterGlobalFunction("Map @Accuflux(Map &in s1)", asFUNCTIONPR( AS_Accuflux,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Accuflux(Map &in s1, float)", asFUNCTIONPR( AS_Accuflux,(cTMap*, float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Accuflux(Map &in s1, Map &in s2)", asFUNCTIONPR( AS_Accuflux,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //Outlet and Catchments stuff

    r = engine->RegisterGlobalFunction("Map @Outlets(Map &in s1)", asFUNCTIONPR( AS_Outlets,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Catchments(Map &in s1)", asFUNCTIONPR( AS_Catchments,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Catchments(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_Catchments,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //streamorder
    r = engine->RegisterGlobalFunction("Map @StreamOrder(Map &in s1)", asFUNCTIONPR( AS_StreamOrder,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //area reduction
    r = engine->RegisterGlobalFunction("Map @AreaArea(Map &in s1)", asFUNCTIONPR( AS_MapAreaArea,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaAverage(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_MapAreaAverage,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaTotal(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_MapAreaTotal,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaMinimum(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_MapAreaMinimum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaMaximum(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_MapAreaMaximum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    //inversedistance

    r = engine->RegisterGlobalFunction("Map @InverseDistance(Map &in s1,Map &in s2,Map &in s3)", asFUNCTIONPR( AS_InverseDistance,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @InverseDistance(Map &in s1,Map &in s2,float)", asFUNCTIONPR( AS_InverseDistance,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //coordinate related functions


    r = engine->RegisterGlobalFunction("Map @Shift(Map &in s1,int,int,float)", asFUNCTIONPR( AS_Shift,(cTMap*,int,int,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Shift(Map &in s1,int,int)", asFUNCTIONPR( AS_Shift,(cTMap*,int,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @XIndex(Map &in s1)", asFUNCTIONPR( AS_XIndex,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @YIndex(Map &in s1)", asFUNCTIONPR( AS_YIndex,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @XCoord(Map &in s1)", asFUNCTIONPR( AS_XCoord,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @YCoord(Map &in s1)", asFUNCTIONPR( AS_YCoord,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Clone(Map &in s1,float)", asFUNCTIONPR( AS_Clone,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    //register random number generator


    //gaussian influence
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(Map &in s1,Map &in s2,Map &in s3)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(Map &in s1,float,Map &in s3)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,float,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(Map &in s1,Map &in s2,float)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(Map &in s1,float s2,float s3)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //register clump, uniqueID etc..
    r = engine->RegisterGlobalFunction("Map @Clump(Map &in s1, float threshold = 0.0, bool adaptive = false)", asFUNCTIONPR( AS_Clump,(cTMap*, float, bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @UniqueCellID(Map &in s1)", asFUNCTIONPR( AS_UniqueID,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //register windowaverage, windowminimum, window ...


    r = engine->RegisterGlobalFunction("Map @WindowAverage(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_WindowAverage,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowAverage(Map &in s1,float)", asFUNCTIONPR( AS_WindowAverage,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowTotal(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_WindowTotal,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowTotal(Map &in s1,float)", asFUNCTIONPR( AS_WindowTotal,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowMaximum(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_WindowMaximum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowMaximum(Map &in s1,float)", asFUNCTIONPR( AS_WindowMaximum,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowMinimum(Map &in s1,Map &in s2)", asFUNCTIONPR( AS_WindowMinimum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowMinimum(Map &in s1,float)", asFUNCTIONPR( AS_WindowMinimum,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //table interactions
    r = engine->RegisterGlobalFunction("Map @RasterFromTableValues(Table &table,Map &RowValue, Map &ColumnValue)", asFUNCTIONPR( AS_RasterFromTableHeader,(MatrixTable *,cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterFromTable(Table &table,Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterFromTable,(MatrixTable *,cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterClasses(Map &m)", asFUNCTIONPR( AS_RasterTableClasses,(cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableAverage(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableAverage,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableMinimum(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableMinimum,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableTotal(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableTotal,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableMaximum(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableMaximum,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );


    //flow algorithms
    r = engine->RegisterGlobalFunction("Map @FlowDiffusiveMax(Map &in DEM,Map &in H,int iter = 10,float courant = 0.1)", asFUNCTIONPR( AS_DiffusiveMaxWave,(cTMap*,cTMap*,int,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @FlowDiffusiveMaxCG(Map &in DEM,Map &in H, Map &in DEMSlope, float slopefactor, int iter = 10,float courant = 0.1)", asFUNCTIONPR( AS_DiffusiveMaxWaveCG,(cTMap*,cTMap*,cTMap*,float, int,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );



    r = engine->RegisterGlobalFunction("Map @HillShade(Map &in elevation, float scale = 1.0)", asFUNCTIONPR( AS_HillShade,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

}



