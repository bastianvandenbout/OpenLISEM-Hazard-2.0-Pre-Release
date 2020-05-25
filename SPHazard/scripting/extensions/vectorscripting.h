#ifndef VECTORSCRIPTING_H
#define VECTORSCRIPTING_H

#include <angelscript.h>
#include "scriptmanager.h"
#include "shapefile.h"
#include "shapeoffset.h"

inline void RegisterVectorToScriptEngine(asIScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("Shapes",0,asOBJ_REF);//Shapes
    if(r < 0){SPH_DEBUG("error in registering scripting: class Shapes");};

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Shapes",asBEHAVE_ADDREF,"void f()",asMETHOD(ShapeFile,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Shapes",asBEHAVE_RELEASE,"void f()",asMETHOD(ShapeFile,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes& opAssign(Shapes &in m)", asMETHOD(ShapeFile,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Shapes",asBEHAVE_FACTORY,"Shapes@ CSF0()",asFUNCTIONPR(ShapesFactory,(),ShapeFile *),asCALL_CDECL); assert( r >= 0 );


    //register binary operators
    r = engine->RegisterObjectMethod("Shapes", "Shapes& opAnd(Shapes &in s)", asMETHODPR(ShapeFile,AS_And,(ShapeFile *),ShapeFile*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes& opOr(Shapes &in s)", asMETHODPR(ShapeFile,AS_Or,(ShapeFile *),ShapeFile*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes& opXor(Shapes &in s)", asMETHODPR(ShapeFile,AS_Xor,(ShapeFile *),ShapeFile*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes& opSub(Shapes &in s)", asMETHODPR(ShapeFile,AS_Sub,(ShapeFile *),ShapeFile*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes& opAdd(Shapes &in s)", asMETHODPR(ShapeFile,AS_Add,(ShapeFile *),ShapeFile*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Shapes& VectorOffset(Shapes &in s, float offset)", asFUNCTIONPR(AS_VectorOffset,(ShapeFile *,float),ShapeFile*), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Table& GetTable()", asMETHODPR(ShapeFile,AS_GetTable,(),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Shapes", "Table& opIndex(string name)", asMETHODPR(ShapeFile,AS_GetTable,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
}

#endif // VECTORSCRIPTING_H
