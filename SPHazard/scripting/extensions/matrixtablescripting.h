#ifndef MATRIXTABLESCRIPTING_H
#define MATRIXTABLESCRIPTING_H


#include <angelscript.h>
#include "scriptmanager.h"
#include "matrixtable.h"
#include "tableanalysis.h"
#include "correlations.h"

inline void RegisterTableToScriptEngine(asIScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("Table",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Table",asBEHAVE_ADDREF,"void f()",asMETHOD(MatrixTable,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Table",asBEHAVE_RELEASE,"void f()",asMETHOD(MatrixTable,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(Table &in m)", asMETHODPR(MatrixTable,AS_Assign,(MatrixTable*),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Table",asBEHAVE_FACTORY,"Table@ CSF0()",asFUNCTIONPR(TableFactory,(),MatrixTable *),asCALL_CDECL); assert( r >= 0 );

    //regiter member functions

    r = engine->RegisterObjectMethod("Table","Table &opIndex(int index)",asMETHODPR(MatrixTable,AS_OpIndex,(int),MatrixTable *), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","string &opIndex(int index, int index2)",asMETHODPR(MatrixTable,AS_OpIndex,(int,int),QString *),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","void SetSize(int r, int c)",asMETHODPR(MatrixTable,SetSize,(int,int),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","void SetSize(int r)",asMETHODPR(MatrixTable,SetSize,(int),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","int Rows()",asMETHODPR(MatrixTable,GetSizeR,(void),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","int Cols()",asMETHODPR(MatrixTable,GetSizeC,(void),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table","string &GetValue(int r,int c)",asMETHODPR(MatrixTable,GetValueString,(int,int),std::string*), asCALL_THISCALL); assert( r >= 0 );


    ////operators with tables
    r = engine->RegisterObjectMethod("Table", "Table& opNeg()", asMETHODPR(MatrixTable,AS_OpNeg,(void),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opCom()", asMETHODPR(MatrixTable,AS_OpCom,(void),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("Table", "Table@ opSub(Table &in s)", asMETHODPR(MatrixTable,AS_OpSub,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(Table &in s)", asMETHODPR(MatrixTable,AS_OpAdd,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul(Table &in m)", asMETHODPR(MatrixTable,AS_OpMul,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv(Table &in m)", asMETHODPR(MatrixTable,AS_OpDiv,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow(Table &in m)", asMETHODPR(MatrixTable,AS_OpPow,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod(Table &in m)", asMETHODPR(MatrixTable,AS_OpMod,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(Table &in m)", asMETHODPR(MatrixTable,AS_OpAddAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMulAssign(Table &in m)", asMETHODPR(MatrixTable,AS_OpMulAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opSubAssign(Table &in m)", asMETHODPR(MatrixTable,AS_OpSubAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDivAssign(Table &in m)", asMETHODPR(MatrixTable,AS_OpDivAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPowAssign(Table &in m)", asMETHODPR(MatrixTable,AS_OpPowAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opModAssign(Table &in m)", asMETHODPR(MatrixTable,AS_OpModAssign,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd(Table &in m)", asMETHODPR(MatrixTable,AS_And,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr(Table &in m)", asMETHODPR(MatrixTable,AS_Or,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor(Table &in m)", asMETHODPR(MatrixTable,AS_Xor,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(Table &in m)", asMETHODPR(MatrixTable,AS_EqualTo,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(Table &in m)", asMETHODPR(MatrixTable,AS_NotEqualTo,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl(Table &in m)", asMETHODPR(MatrixTable,AS_SmallerThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr(Table &in m)", asMETHODPR(MatrixTable,AS_LargerThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShlAssign(Table &in m)", asMETHODPR(MatrixTable,AS_SmallerEqualThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShrAssign(Table &in m)", asMETHODPR(MatrixTable,AS_LargerEqualThen,(MatrixTable *),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );


    //operators with doubles
    r = engine->RegisterObjectMethod("Table", "Table@ opSub(double s)", asMETHODPR(MatrixTable,AS_OpSub,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(double s)", asMETHODPR(MatrixTable,AS_OpAdd,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul(double m)", asMETHODPR(MatrixTable,AS_OpMul,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv(double m)", asMETHODPR(MatrixTable,AS_OpDiv,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow(double m)", asMETHODPR(MatrixTable,AS_OpPow,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod(double m)", asMETHODPR(MatrixTable,AS_OpMod,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(double m)", asMETHODPR(MatrixTable,AS_OpAddAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMulAssign(double m)", asMETHODPR(MatrixTable,AS_OpMulAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opSubAssign(double m)", asMETHODPR(MatrixTable,AS_OpSubAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDivAssign(double m)", asMETHODPR(MatrixTable,AS_OpDivAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPowAssign(double m)", asMETHODPR(MatrixTable,AS_OpPowAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opModAssign(double m)", asMETHODPR(MatrixTable,AS_OpModAssign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd(double m)", asMETHODPR(MatrixTable,AS_And,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr(double m)", asMETHODPR(MatrixTable,AS_Or,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor(double m)", asMETHODPR(MatrixTable,AS_Xor,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(double m)", asMETHODPR(MatrixTable,AS_EqualTo,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(double m)", asMETHODPR(MatrixTable,AS_NotEqualTo,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl(double m)", asMETHODPR(MatrixTable,AS_SmallerThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr(double m)", asMETHODPR(MatrixTable,AS_LargerThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShlAssign(double m)", asMETHODPR(MatrixTable,AS_SmallerEqualThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShrAssign(double m)", asMETHODPR(MatrixTable,AS_LargerEqualThen,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table@ opSub_r(double s)", asMETHODPR(MatrixTable,AS_OpSub_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd_r(double s)", asMETHODPR(MatrixTable,AS_OpAdd_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul_r(double m)", asMETHODPR(MatrixTable,AS_OpMul_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv_r(double m)", asMETHODPR(MatrixTable,AS_OpDiv_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow_r(double m)", asMETHODPR(MatrixTable,AS_OpPow_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod_r(double m)", asMETHODPR(MatrixTable,AS_OpMod_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd_r(double m)", asMETHODPR(MatrixTable,AS_And_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr_r(double m)", asMETHODPR(MatrixTable,AS_Or_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor_r(double m)", asMETHODPR(MatrixTable,AS_Xor_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq_r(double m)", asMETHODPR(MatrixTable,AS_EqualTo_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq_r(double m)", asMETHODPR(MatrixTable,AS_NotEqualTo_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl_r(double m)", asMETHODPR(MatrixTable,AS_SmallerThen_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr_r(double m)", asMETHODPR(MatrixTable,AS_LargerThen_r,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //operators with int
    r = engine->RegisterObjectMethod("Table", "Table@ opSub(int s)", asMETHODPR(MatrixTable,AS_OpSub,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(int s)", asMETHODPR(MatrixTable,AS_OpAdd,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul(int m)", asMETHODPR(MatrixTable,AS_OpMul,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv(int m)", asMETHODPR(MatrixTable,AS_OpDiv,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow(int m)", asMETHODPR(MatrixTable,AS_OpPow,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod(int m)", asMETHODPR(MatrixTable,AS_OpMod,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(int m)", asMETHODPR(MatrixTable,AS_OpAddAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMulAssign(int m)", asMETHODPR(MatrixTable,AS_OpMulAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opSubAssign(int m)", asMETHODPR(MatrixTable,AS_OpSubAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDivAssign(int m)", asMETHODPR(MatrixTable,AS_OpDivAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPowAssign(int m)", asMETHODPR(MatrixTable,AS_OpPowAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opModAssign(int m)", asMETHODPR(MatrixTable,AS_OpModAssign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd(int m)", asMETHODPR(MatrixTable,AS_And,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr(int m)", asMETHODPR(MatrixTable,AS_Or,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor(int m)", asMETHODPR(MatrixTable,AS_Xor,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(int m)", asMETHODPR(MatrixTable,AS_EqualTo,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(int m)", asMETHODPR(MatrixTable,AS_NotEqualTo,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl(int m)", asMETHODPR(MatrixTable,AS_SmallerThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr(int m)", asMETHODPR(MatrixTable,AS_LargerThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShlAssign(int m)", asMETHODPR(MatrixTable,AS_SmallerEqualThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShrAssign(int m)", asMETHODPR(MatrixTable,AS_LargerEqualThen,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //operators with int
    r = engine->RegisterObjectMethod("Table", "Table@ opSub_r(int s)", asMETHODPR(MatrixTable,AS_OpSub_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table@ opAdd_r(int s)", asMETHODPR(MatrixTable,AS_OpAdd_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMul_r(int m)", asMETHODPR(MatrixTable,AS_OpMul_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opDiv_r(int m)", asMETHODPR(MatrixTable,AS_OpDiv_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opPow_r(int m)", asMETHODPR(MatrixTable,AS_OpPow_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opMod_r(int m)", asMETHODPR(MatrixTable,AS_OpMod_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& opAnd_r(int m)", asMETHODPR(MatrixTable,AS_And_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opOr_r(int m)", asMETHODPR(MatrixTable,AS_Or_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opXor_r(int m)", asMETHODPR(MatrixTable,AS_Xor_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq_r(int m)", asMETHODPR(MatrixTable,AS_EqualTo_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq_r(int m)", asMETHODPR(MatrixTable,AS_NotEqualTo_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShl_r(int m)", asMETHODPR(MatrixTable,AS_SmallerThen_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opShr_r(int m)", asMETHODPR(MatrixTable,AS_LargerThen_r,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    //operators with string

    r = engine->RegisterObjectMethod("Table", "Table& opAssign(string m)", asMETHODPR(MatrixTable,AS_Assign,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(double m)", asMETHODPR(MatrixTable,AS_Assign,(double),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAssign(int m)", asMETHODPR(MatrixTable,AS_Assign,(int),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table@ opAdd(string s)", asMETHODPR(MatrixTable,AS_OpAdd,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& opAddAssign(string m)", asMETHODPR(MatrixTable,AS_OpAddAssign,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table@ opAdd_r(string s)", asMETHODPR(MatrixTable,AS_OpAdd_r,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Table", "Table& eq(string m)", asMETHODPR(MatrixTable,AS_EqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq(string m)", asMETHODPR(MatrixTable,AS_NotEqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& eq_r(string m)", asMETHODPR(MatrixTable,AS_EqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Table", "Table& neq_r(string m)", asMETHODPR(MatrixTable,AS_NotEqualTo,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );



    //adding and removing columns and rows

    r = engine->RegisterObjectMethod("Table", "void AppendColumns(Table &in s)", asMETHODPR(MatrixTable,AS_AppendColumns,(MatrixTable *),void), asCALL_THISCALL); assert( r >= 0 );

    //conversions
    r = engine->RegisterGlobalFunction("Table @ TToInt(Table& in table)",asFUNCTIONPR(AS_TToInt,(MatrixTable*),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ TToDouble(Table& in table)",asFUNCTIONPR(AS_TToDouble,(MatrixTable*),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ TToString(Table& in table)",asFUNCTIONPR(AS_TToString,(MatrixTable*),MatrixTable*),asCALL_CDECL); assert( r >= 0 );

    //sorting
    r = engine->RegisterGlobalFunction("Table @ SortByColumn(Table& in table, int column = 0, bool descending = false)",asFUNCTIONPR(SortByColumn,(MatrixTable*,int,bool),MatrixTable*),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @ SortColumn(Table& in table, int column = 0, bool descending = false)",asFUNCTIONPR(SortColumn,(MatrixTable*,int,bool),MatrixTable*),asCALL_CDECL); assert( r >= 0 );

    //selecting
    r = engine->RegisterGlobalFunction("Table @ GetColumn(Table& in table, int column = 0)",asFUNCTIONPR(AS_TableGetColumn,(MatrixTable*, int column),MatrixTable*),asCALL_CDECL); assert( r >= 0 );


    //correlations
    r = engine->RegisterGlobalFunction("float PearsonCorrelation(Table& in table)",asFUNCTIONPR(AS_TableCorrelationPearson,(MatrixTable*),float),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float SpearmanCorrelation(Table& in table)",asFUNCTIONPR(AS_TableCorrelationSpearman,(MatrixTable*),float),asCALL_CDECL); assert( r >= 0 );

}


#endif // MATRIXTABLESCRIPTING_H
