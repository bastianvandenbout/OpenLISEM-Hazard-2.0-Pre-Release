#ifndef IOMANAGER_H
#define IOMANAGER_H

#define SPH_FOLDER_KERNELS "/kernels/"
#define SPH_FOLDER_ASSETS "/assets/"

#include "shapefile.h"
#include "map.h"
#include "scriptmanager.h"
#include <QString>
#include <QDir>
#include "iogdalshape.h"
#include "iogdal.h"
#include "QFileInfo"
#include "matrixtable.h"
#include "scriptarray.h"
#include "iopdal.h"
#include "pointcloud.h"

#define SPH_IO_ERROR_CANNOT_OPEN -1
#define SPH_IO_SUCCESS 1

#define SPH_FILE_EXT_SHP ".shp"
#define SPH_FILE_EXT_MAP ".map"
#define SPH_FILE_EXT_TIF ".tif"
#define SPH_FILE_EXT_ASC ".asc"
#define SPH_FILE_EXT_RUN ".run"
#define SPH_FILE_EXT_SCRIPT ".script"
#define SPH_FILE_EXT_SETTING ".ini"

#define SPH_FILE_TYPE_UNKNOWN 0
#define SPH_FILE_TYPE_MAP 1
#define SPH_FILE_TYPE_RUN 2
#define SPH_FILE_TYPE_SCRIPT 3
#define SPH_FILE_TYPE_VECTOR 4
#define SPH_FILE_TYPE_TABLE 5
#define SPH_FILE_TYPE_POINTCLOUD 6

extern QString AS_DIR_Org;
extern QString AS_DIR;
inline void SetASDefaultPath(QString path, bool org = true)
{
    AS_DIR = path;
    AS_DIR_Org = path;
}

inline void AS_SetASDefaultPath(QString path)
{
    SetASDefaultPath(QString(path),false);
}

inline void AS_RestoreASDefaultPath()
{
    SetASDefaultPath(AS_DIR_Org,false);
}

inline QString AS_GetASDefaultPathOrg()
{
    return AS_DIR_Org;
}

inline QString AS_GetASDefaultPath()
{
    return AS_DIR;
}

inline QString GetASDefaultPathOrg()
{
    return AS_DIR_Org;
}

inline QString GetASDefaultPath()
{
    return AS_DIR;
}
inline MatrixTable * AS_LoadTableFromFileSep(const QString &path, const QString &sep)
{
    MatrixTable * t = new MatrixTable();

    t->LoadAs(AS_DIR + QString(path),QString(sep),false);
    return t;
}


inline MatrixTable * AS_LoadTableFromFile(const QString &path)
{
    return AS_LoadTableFromFileSep(path,",");
}

inline void AS_SaveTableToFileSep(MatrixTable * table,const QString &path, const QString &sep)
{
    table->SaveAs(AS_DIR + QString(path),QString(sep));
}

inline void AS_SaveTableToFile(MatrixTable * table,const QString &path)
{
    std::cout << "write table " << path.toStdString()  << std::endl;

    return AS_SaveTableToFileSep(table,path,",");
}

inline MatrixTable * AS_SaveThisTableToFile(const QString &path)
{
    std::cout << "savethistabletofile " << std::endl;
    MatrixTable * _M = new MatrixTable();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    std::function<void(MatrixTable *,QString)> writecall;
    writecall = [](MatrixTable * m, QString p){AS_SaveTableToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<MatrixTable *(QString)> readcall;
    readcall = [](QString p){return AS_LoadTableFromFile(p);};
    _M->AS_readfunc = readcall;
    std::cout << "savethistabletofile 2" << std::endl;
    return _M;
}


inline ShapeFile * AS_LoadVectorFromFile(const QString &path)
{
    QString qpath = QString(AS_DIR + path);
    return LoadVector(qpath);
}
inline ShapeFile * AS_LoadVectorFromFileAbsPath(const QString &path)
{
    QString qpath = QString( path);
    return LoadVector(qpath);
}





inline cTMap * AS_LoadMapFromFileAbsPath(const QString &path)
{
    QString qpath = QString(path);
    cTMap * _M = new cTMap(readRaster(qpath));
    return _M;
}


inline cTMap * AS_LoadMapFromFile(const QString &path)
{
    QString qpath = QString(AS_DIR + path);
    cTMap * _M = new cTMap(readRaster(qpath));

    return _M;
}


inline void CopyMapByPath(QString path1, QString path2)
{
    QFileInfo f1((path1));
    QFileInfo f2((path2));


    if(!QDir(QString(f2.absolutePath())).exists())
    {
        if(!QDir(QString(f2.absolutePath())).mkdir(QString(f2.absolutePath())))
        {
            SPHS_DEBUG("Could not create target directory for copy: from " + QString(path1) + " to " + QString(path2));
        }
    }

    if(f2.exists())
    {
        QFile(path2).remove();
    }
    if(!QFile::copy(QString(path1),QString(path2)))
    {
        SPHS_DEBUG("Could not copy: from " + QString(path1) + " to " + QString(path2));

    }

}

inline void CopyMap(QString dir, QString file, QString dir2, QString file2)
{
    QFileInfo f1(QString(dir) + "/" + QString(file));
    QFileInfo f2(QString(dir2) + "/" + QString(file2));

    if(!QDir(QString(dir2)).exists())
    {
        if(!QDir(QString(dir2)).mkdir(QString(dir2)))
        {
            SPHS_DEBUG("Could not create target directory for copy: from " + QString(dir) + "/" + QString(file) + "  to: " + QString(dir2) + "/" + QString(file2))
        }
    }
    if(f2.exists())
    {
        QFile(QString(dir2) + "/" + QString(file2)).remove();
    }
    if(!QFile::copy(QString(dir) + "/" + QString(file),QString(dir2) + "/" + QString(file2)))
    {
            SPHS_DEBUG("Could not copy: from " + QString(dir) + "/" + QString(file) + "  to: " + QString(dir2) + "/" + QString(file2))
    }

}

inline void AS_SaveMapToFileAbsolute(cTMap *m, const QString &path)
{

    QString qp = QString(AS_DIR +path);

    QString format = "GEOTIFF";


    if(path.length() > 3)
    {
        //try to automatically guess format based on extension
        // .map -> PCRaster
        // .tif -> GEOTIFF
        // .asc -> ASCII Grid

        //else use .tif format


        QString ext = qp.right(4);
        if(ext.compare(".map")== 0)
        {
            format = "PCRaster";
        }else if(ext.compare(".tif")== 0)
        {
            format = "GTIFF";
        }else if(ext.compare(".asc")== 0)
        {
            format = "AAIGRID";
        }


    }

    writeRaster(*m,qp,format);


}


inline void AS_SaveMapToFile(cTMap *m, const QString &in_path)
{

    QString path = QString(in_path).trimmed();

    AS_SaveMapToFileAbsolute(m,path);

}

inline void AS_SaveVectorToFileAbsolute(ShapeFile *m, const QString &path)
{
    QString qp = QString(path);

    QString format = "ESRI Shapefile";


    if(path.length() > 3)
    {
        //try to automatically guess format based on extension
        // .shp -> ESRI Shapefile
        // .kml -> KML

        //else use .shp format


        QString ext = qp.right(4);
        if(ext.compare(".shp")== 0)
        {
            format = "ESRI ShapeFile";
        }else if(ext.compare(".kml")== 0)
        {
            format = "LIBKML";
        }

    }

    SaveVector(m,qp,format);

}

inline void AS_SaveVectorToFile(ShapeFile *m, const QString &path)
{
    AS_SaveVectorToFileAbsolute(m,QString(AS_DIR +path));
}

inline ShapeFile * AS_SaveThisVectorToFile(const QString &path)
{
    ShapeFile * _M = new ShapeFile();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    std::function<void(ShapeFile *,QString)> writecall;
    writecall = [](ShapeFile * m, QString p){AS_SaveVectorToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<ShapeFile *(QString)> readcall;
    readcall = [](QString p){return AS_LoadVectorFromFile(p);};
    _M->AS_readfunc = readcall;
    return _M;
}


inline QString AS_GetFileDir(QString full_path)
{
    return QFileInfo(QString(full_path)).absolutePath();
}

inline QString AS_GetFileName(QString full_path)
{
    return QFileInfo(QString(full_path)).baseName();
}

inline QString AS_GetFileExt(QString full_path)
{
return ("." + QFileInfo(QString(full_path)).completeSuffix());
}

inline cTMap * AS_SaveThisMapToFile(const QString &path)
{
    cTMap * _M = new cTMap();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;
    _M->AS_refcount = 0;

    std::function<void(cTMap *,QString)> writecall;
    writecall = [](cTMap * m, QString p){AS_SaveMapToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<cTMap *(QString)> readcall;
    readcall = [](QString p){return AS_LoadMapFromFile(p);};
    _M->AS_readfunc = readcall;
    return _M;
}


inline static CScriptArray *LoadMapBandList(const QString &name)
{
    // Obtain a pointer to the engine
    asIScriptContext *ctx = asGetActiveContext();
    asIScriptEngine *engine = ctx->GetEngine();

    // TODO: This should only be done once
    // TODO: This assumes that CScriptArray was already registered
    asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<Map>");

    // Create the array object
    CScriptArray *array = CScriptArray::Create(arrayType);

    QList<cTMap*> bands = readRasterList(AS_DIR + name);

    array->Resize(bands.length());

    for(int i = 0; i < bands.length(); i++)
    {
        array->SetValue(i,bands.at(i));
    }

    return array;
}

inline static CScriptArray * LoadMapBandListFromStrings(CScriptArray * names)
{

    // Obtain a pointer to the engine
    asIScriptContext *ctx = asGetActiveContext();
    asIScriptEngine *engine = ctx->GetEngine();

    // TODO: This should only be done once
    // TODO: This assumes that CScriptArray was already registered
    asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<Map>");

    // Create the array object
    CScriptArray *array = CScriptArray::Create(arrayType);
    array->Resize(names->GetSize());

    for(int i = 0; i < names->GetSize(); i++)
    {
        QString name = *((QString*)names->At(i));
        std::cout << "get map " << name.toStdString()  << std::endl;
        cTMap* band = new cTMap(readRaster(AS_DIR + name));
        if(band == nullptr)
        {

            SPHS_ERROR("Could not load map " + name);
            throw 1;
        }
        array->SetValue(i,band);
    }

    return array;
}

inline static CScriptArray * GetMapBandListStrings(const QString & name, int count, int start, QString between)
{

    // Obtain a pointer to the engine
    asIScriptContext *ctx = asGetActiveContext();
    asIScriptEngine *engine = ctx->GetEngine();

    // TODO: This should only be done once
    // TODO: This assumes that CScriptArray was already registered
    asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<string>");

    // Create the array object
    CScriptArray *array = CScriptArray::Create(arrayType);

    int indexdot =name.lastIndexOf(".");
    if(indexdot < 0)
    {
        indexdot = name.length();
    }
    QString firstpart = name.left(indexdot);
    QString lastpart = name.right(name.length() - indexdot);

    array->Resize(count);

    for(int i = 0; i < count; i++)
    {
        array->SetValue(i,new QString(firstpart + between + QString::number(start+i)+lastpart));
    }

    return array;
}


inline static PointCloud * AS_LoadPointCloudFromFileAbsPath(const QString & name)
{
    PointCloud * ret = ReadPointCloud(name);
    if(ret == nullptr)
    {

        SPHS_ERROR("Could not read Point Cloud from file: " + name);
        SPH_ERROR("Could not read Point Cloud from file: " + name);
        throw 1;
    }else {
        return ret;
    }
}

inline static PointCloud * AS_LoadPointCloudFromFile(const QString & name)
{
    return AS_LoadPointCloudFromFileAbsPath(AS_DIR + name);
}

inline static void AS_SavePointCloudToFileAbsPath(PointCloud * pc, const QString & name)
{

    //call writing function for point cloud

}
inline static void AS_SavePointCloudToFile(PointCloud * pc,const QString & name)
{
    AS_SavePointCloudToFileAbsPath(pc,AS_DIR + name);
}

inline static PointCloud * AS_SaveThisPointCloudToFile(const QString & path)
{
    PointCloud * _M = new PointCloud();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    std::function<void(PointCloud *,QString)> writecall;
    writecall = [](PointCloud * m, QString p){AS_SavePointCloudToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<PointCloud *(QString)> readcall;
    readcall = [](QString p){return AS_LoadPointCloudFromFile(p);};
    _M->AS_readfunc = readcall;
    return _M;

}


inline QStringList GetFileStringList(QString path)
{
    QStringList res;

    QFile fin(path);
    if (!fin.open(QFile::ReadOnly | QFile::Text)) {

        return  res;
    }


    while (!fin.atEnd())
    {
        QString S = fin.readLine().trimmed();
        res.append(S);
    }

    return res;


}

class IOManager
{
public:
    IOManager();

    QString ASDir = "";

    inline ~IOManager()
    {


    }

    inline void Destroy()
    {


    }

    inline static QStringList GetSupportedFileTypeExtensionsFilter()
    {
        QStringList ret;

        ret.append("*.shp");
        ret.append("*.map");
        ret.append("*.tif");
        ret.append("*.tbl");
        ret.append("*.csv");
        ret.append("*.las");

        return ret;

    }

    inline static QStringList GetSupportedFileTypeExtensions()
    {
        QStringList ret;

        ret.append(".shp");
        ret.append(".map");
        ret.append(".tif");
        ret.append(".tbl");
        ret.append(".csv");
        ret.append(".las");

        return ret;

    }

    inline void SetScriptFunctions(ScriptManager * sm)
    {
        int r;

        r = sm->m_Engine->RegisterGlobalFunction("Table @LoadTable(const string &in s)", asFUNCTION( AS_LoadTableFromFile), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SaveTable(Table &in sh, const string &in s)", asFUNCTION( AS_SaveTableToFile), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Table @LoadTable(const string &in s,const string &in s2)", asFUNCTION( AS_LoadTableFromFileSep), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SaveTables(Table &in sh, const string &in s,const string &in s2)", asFUNCTION( AS_SaveTableToFileSep), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Table @SaveThisTable(const string &in s)", asFUNCTION( AS_SaveThisTableToFile),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Shapes @LoadVectorAbsPath(const string &in s)", asFUNCTION( AS_LoadVectorFromFileAbsPath), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Shapes @LoadVector(const string &in s)", asFUNCTION( AS_LoadVectorFromFile), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SaveVector(Shapes &in sh, const string &in s)", asFUNCTION( AS_SaveVectorToFile), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SaveVectorAbsPath(Shapes &in sh, const string &in s)", asFUNCTION( AS_SaveVectorToFileAbsolute), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Shapes @SaveThisVector(const string &in s)", asFUNCTION( AS_SaveThisVectorToFile),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Map @LoadMapAbsPath(const string &in s)", asFUNCTION( AS_LoadMapFromFileAbsPath), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Map @LoadMap(const string &in s)", asFUNCTION( AS_LoadMapFromFile), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SaveMap(Map &in m, const string &in s)", asFUNCTION(AS_SaveMapToFile),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SaveMapAbsPath(Map &in m, const string &in s)", asFUNCTION(AS_SaveMapToFileAbsolute),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Map &SaveThisMap(const string &in s)", asFUNCTION( AS_SaveThisMapToFile),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SetWorkDir(const string &in dir)", asFUNCTION( AS_SetASDefaultPath),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void RestoreWorkDir()", asFUNCTION( AS_RestoreASDefaultPath),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void CopyFile(string dir1, string file1, string dir2, string file2)", asFUNCTION( CopyMap),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void CopyFile(string path1, string path2)", asFUNCTION( CopyMapByPath),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("string GetDir()", asFUNCTION( AS_GetASDefaultPath),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("string GetDirOrg()", asFUNCTION( AS_GetASDefaultPathOrg),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("string GetFileDir(string s)", asFUNCTION( AS_GetFileDir),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("string GetFileName(string s)", asFUNCTION( AS_GetFileName),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("string GetFileExt(string s)", asFUNCTION( AS_GetFileExt),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("array<Map>@ LoadMapBandList(string s)", asFUNCTION( LoadMapBandList),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("array<Map>@ LoadMapBandList(array<string> &in s)", asFUNCTION( LoadMapBandListFromStrings),  asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("array<string>@ GetMapListNames(string & in name, int count, int start = 0, string between = \"_\")", asFUNCTION( GetMapBandListStrings),  asCALL_CDECL); assert( r >= 0 );

        r = sm->m_Engine->RegisterGlobalFunction("PointCloud @LoadPointCloudAbsPath(const string &in s)", asFUNCTION( AS_LoadPointCloudFromFileAbsPath), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("PointCloud @LoadPointCloud(const string &in s)", asFUNCTION( AS_LoadPointCloudFromFile), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SavePointCloud(Shapes &in sh, const string &in s)", asFUNCTION( AS_SavePointCloudToFile), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void SavePointCloudAbsPath(Shapes &in sh, const string &in s)", asFUNCTION( AS_SavePointCloudToFileAbsPath), asCALL_CDECL); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Shapes @SaveThisPointCloud(const string &in s)", asFUNCTION( AS_SaveThisPointCloudToFile),  asCALL_CDECL); assert( r >= 0 );

    }

    inline static QStringList ReadFileAsText(QString path, bool trim = true, int * error = nullptr)
    {

        QStringList sl;

        QFile fin(path);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

            if(!(error == nullptr))
            {
                *error = SPH_IO_ERROR_CANNOT_OPEN;
            }
            return sl;
        }

        while (!fin.atEnd())
        {
            QString S;
            if(trim)
            {
                S = fin.readLine().trimmed();
            }else {
                S = fin.readLine();
                }

            sl.append(S);
        }

        if(!(error == nullptr))
        {
            *error = SPH_IO_SUCCESS;
        }

        return sl;

    }

};

#endif // IOMANAGER_H
