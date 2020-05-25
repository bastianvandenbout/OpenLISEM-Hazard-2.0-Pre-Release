#pragma once

#include <QString>

#include "spherror.h"

#include "angelscript.h"
#include "scriptstdstring.h"
#include "scripthelper.h"
#include "OpenCLUtil.h"
#include "mapscripting.h"
#include "QThread"
#include <QWaitCondition>
#include "scriptbuilder.h"
#include <QRegularExpression>
#include "scriptpreprocessor.h"
#include "sphscriptengine.h"



void MessageCallback(const asSMessageInfo *msg, void *param);
void ExecString(asIScriptEngine *engine, std::string &arg);
void AddVariable(asIScriptEngine *engine, std::string &arg);
void DeleteVariable(asIScriptEngine *engine, std::string &arg);
void AddFunction(asIScriptEngine *engine, std::string &arg);
void DeleteFunction(asIScriptEngine *engine, std::string &arg);
void ListVariables(asIScriptEngine *engine);
void ListFunctions(asIScriptEngine *engine);
void PrintHelp();
void grab(int v);
void grab(asUINT v);
void grab(bool v);
void grab(float v);
void grab(double v);
void grab(const QString &v);
void grab();
void print(const QString &s);
QString ToString(float x);
QString ToString(double x);
QString ToString(int x);
QString ToString(bool x);

typedef struct SPHScript
{
    QString file;
    QString name;
    QString code;
    QString HomeDir;
    bool is_singleline = false;
    bool is_preprocess = false;
    bool is_allowinclude = false;
    bool compiled = false;

    CScriptBuilder * scriptbuilder;

    std::function<void(bool)> m_CallBackDone;
    std::function<void(asIScriptContext *)> m_CallBackLine;
    std::function<void(const asSMessageInfo *)> m_CallBackCompileError;
    std::function<void(asIScriptContext *)> m_CallBackException;
    std::function<void(QString)> m_CallBackPrint;

    bool m_CallBackDoneSet = false;
    bool m_CallBackLineSet = false;
    bool m_CallBackCompileErrorSet = false;
    bool m_CallBackExceptionSet = false;
    bool m_CallBackPrintSet = false;

    inline void SetCode(QString code_in)
    {
        code= code_in;
    }

    inline void SetFileOrigin(QString path)
    {
        file = path;
    }

    inline void SetSingleLine(bool x)
    {
        is_singleline = x;
    }

    inline void SetHomeDir(QString dir)
    {
        HomeDir = dir;
    }

    inline void SetAllowInclude(bool x)
    {
        is_allowinclude = x;
    }

    inline void SetPreProcess(bool x)
    {
        is_preprocess = x;
    }
    inline bool IsCompiled()
    {
        return compiled;

    }


    //example usage, either class member function, bound to this and placeholder, or lambda function

    //s->SetCallBackPrint(&DatabaseTool::OnScriptCallBackPrint,this,std::placeholders::_1);
    //s->SetCallBackPrint(std::function<void(SPHScript*,QString)>([=](SPHScript*,QString) -> void{  ;}),s,std::placeholders::_1);

    template<typename _Callable, typename... _Args>
    inline void SetCallBackPrint(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackPrint = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackPrintSet = true;
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackDone(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackDone = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackDoneSet = true;

    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackLine(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackLine = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackLineSet = true;
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackCompilerError(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackCompileError = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackCompileErrorSet = true;
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackException(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackException = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackExceptionSet = true;
    }


} SPHScript;


struct SPHCodeJob
{
    SPHScript * program;


    inline void OnCallBackLine(asIScriptContext *ctx)
    {
        if(program->m_CallBackLineSet)
        {
            program->m_CallBackLine(ctx);

        }
    }

    inline void OnCallBackException(asIScriptContext *ctx)
    {
        if(program->m_CallBackExceptionSet)
        {
            program->m_CallBackException(ctx);
        }
    }


    inline void OnCallBackCompileError(const asSMessageInfo *msg)
    {
        if(program->m_CallBackCompileErrorSet)
        {
            program->m_CallBackCompileError(msg);
        }

    }

};




class ScriptManager : public QThread
{
    Q_OBJECT

public:
    ScriptManager();

    QMutex m_StartWorkMutex;
    QWaitCondition m_StartWorkWaitCondition;

    QMutex m_WorkMutex;
    QList<SPHCodeJob> m_Work;

    QMutex m_CompileMutex;
    SPHScript * m_CurrentCompilation = nullptr;

    QMutex m_StopMutex;
    bool m_StopRequested = false;

    SPHScriptEngine *m_Engine;

    void Initialize();
    void run() override;

    inline static int PragmaCallback(const std::string &pragmaText, CScriptBuilder &builder, void *userParam)
    {
        return 0;
    }

    inline static int IncludeCallback(const char *include, const char *from, CScriptBuilder *builder, void *userParam)
    {

        return 0;
    }

    inline static int IncludeIgnoreCallback(const char *include, const char *from, CScriptBuilder *builder, void *userParam)
    {

        return 0;
    }

    inline QStringList GetGlobalFunctionNames()
    {
        QStringList ret;

        QList<ScriptFunctionInfo> list = m_Engine->GetGlobalFunctionList();
        for(int i =0; i < list.length(); i++)
        {
            ret.append(list.at(i).Function->GetName());
        }
        return ret;
    }

    inline SPHScript * CompileScriptTable(QString script)
    {
        SPHScript * s = new SPHScript();
        s->file= "";

            QString funcCode = "Table TableEdit(Table &t) {\n";
            funcCode += script;
            funcCode += ";\nreturn t;\n}";
            script = funcCode;

        s->code = script;
        s->is_singleline = true;


        s->scriptbuilder = new CScriptBuilder();
        int r = s->scriptbuilder->StartNewModule(this->m_Engine,"SPHTableScript");
        if(r < 0)
        {
            SPHS_DEBUG("Could not create new module for script!");
            SPH_DEBUG("Could not create new module for script!");
            s->compiled = false;
            return s;
        }
        s->scriptbuilder->SetPragmaCallback(ScriptManager::PragmaCallback,(void*)s);
        s->scriptbuilder->SetIncludeCallback(ScriptManager::IncludeCallback,(void*)s);

        r = s->scriptbuilder->AddSectionFromMemory("main",script.toStdString().c_str());

        SPH_DEBUG(script);

        if(r < 0)
        {
            SPH_DEBUG("Script compilation error!");
            SPHS_DEBUG("Script compilation error!");
            s->compiled = false;
            return s;
        }

        r = s->scriptbuilder->BuildModule();
        if(r < 0)
        {
            SPH_DEBUG("Script compilation error!");
            SPHS_DEBUG("Script compilation error!");
            s->compiled = false;
            return s;
        }

        asIScriptFunction *func = s->scriptbuilder->GetModule()->GetFunctionByDecl("Table TableEdit(Table &t) ");
        if(func == 0)
        {
            SPH_DEBUG("Could not find entry point: Table TableEdit(Table &t)");
            SPHS_DEBUG("Could not find entry point: Table TableEdit(Table &t)");
            s->compiled = false;
            return s;
        }

        s->compiled = true;

        return s;

    }


    inline SPHScript * CompileScript(QString script, bool is_singleline = false, bool preprocess = false, QString HomeDir = "")
    {
        SPHScript * s = new SPHScript();
        s->SetFileOrigin("");
        s->SetHomeDir(HomeDir);
        s->SetSingleLine(is_singleline);
        s->SetPreProcess(preprocess);
        s->SetCode(script);


        return CompileScript_Generic(s);

    }

    inline SPHScript * CompileScript_Generic(SPHScript * s)
    {
        if(s->code.isEmpty())
        {
            s->compiled = false;
            return s;
        }

        if(s->is_singleline)
        {
            QString funcCode = " void main() {\n";
            funcCode += s->code;
            funcCode += "\n;}";
            s->code = funcCode;

        }

        if(s->is_preprocess)
        {
            s->code = PreProcessScript(s->code);

        }


        s->scriptbuilder = new CScriptBuilder();
        int r = s->scriptbuilder->StartNewModule(this->m_Engine,"SPHScript");
        if(r < 0)
        {
            SPHS_DEBUG("Could not create new module for script!");
            SPH_DEBUG("Could not create new module for script!");
            s->compiled = false;
            return s;
        }


        s->scriptbuilder->SetPragmaCallback(ScriptManager::PragmaCallback,(void*)s);
        if(s->is_allowinclude)
        {
            s->scriptbuilder->SetIncludeCallback(ScriptManager::IncludeCallback,(void*)s);
        }else {
            s->scriptbuilder->SetIncludeCallback(ScriptManager::IncludeIgnoreCallback,(void*)s);
        }

        m_CompileMutex.lock();
        m_CurrentCompilation = s;

        r = s->scriptbuilder->AddSectionFromMemory("main",s->code.toStdString().c_str());

        SPH_DEBUG(s->code);

        if(r < 0)
        {
            m_CurrentCompilation = nullptr;
            m_CompileMutex.unlock();
            s->compiled = false;
            return s;
        }

        r = s->scriptbuilder->BuildModule();

        m_CurrentCompilation = nullptr;
        m_CompileMutex.unlock();


        if(r < 0)
        {
            s->compiled = false;
            return s;
        }

        asIScriptFunction *func = s->scriptbuilder->GetModule()->GetFunctionByDecl("void main()");
        if(func == 0)
        {
            SPH_DEBUG("Could not find entry point: void main()");
            SPHS_DEBUG("Could not find entry point: void main()");
            s->compiled = false;
            return s;
        }

        s->compiled = true;

        return s;

    }

    template<typename _Callable, typename... _Args>
    inline void RunScript(SPHScript * s,_Callable&& __f, _Args&&... __args)
    {
        SPHCodeJob cj;

        cj.program = s;

        cj.program->m_CallBackDone = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        cj.program->m_CallBackDoneSet = true;



        m_StartWorkMutex.lock();

        m_Work.append(cj);
        m_StartWorkMutex.unlock();
        m_StartWorkWaitCondition.notify_all();
    }

    inline void RunScript(SPHScript * s)
    {

        SPHCodeJob cj;
        cj.program = s;

        m_StartWorkMutex.lock();

        m_Work.append(cj);
        m_StartWorkMutex.unlock();
        m_StartWorkWaitCondition.notify_all();
    }

    inline void RequestStop()
    {
        m_StopMutex.lock();
        m_StopRequested = true;
        m_StopMutex.unlock();
    }


    inline void RunCommand(QString name)
    {
        std::string cmd, arg;
        std::string input = name.toStdString();

        size_t pos;
        if( (pos = input.find(" ")) != std::string::npos )
        {
            cmd = input.substr(0, pos);
            arg = input.substr(pos+1);
        }
        else
        {
            cmd = input;
            arg = "";
        }

        // Interpret the command
        if( cmd == "exec" )
        {
            ExecString(m_Engine, arg);
        }
        else if( cmd == "addfunc" )
        {
            AddFunction(m_Engine, arg);
        }
        else if( cmd == "delfunc" )
        {
            DeleteFunction(m_Engine, arg);
        }
        else if( cmd == "addvar" )
        {
            AddVariable(m_Engine, arg);
        }
        else if( cmd == "delvar" )
        {
            DeleteVariable(m_Engine, arg);
        }
        else if( cmd == "help" )
        {
            PrintHelp();
        }
        else if( cmd == "listfuncs" )
        {
            ListFunctions(m_Engine);
        }
        else if( cmd == "listvars" )
        {
            ListVariables(m_Engine);
        }
        else
        {
            SPH_ERROR("Unknown command.");
        }

    }

    inline void Destroy()
    {
        if(this->isRunning())
        {
            this->RequestStop();
            this->m_StartWorkWaitCondition.notify_all();

            if(!this->wait(100))
            {
                this->terminate();
            }
        }

        m_Engine->ShutDownAndRelease();
        m_Engine = NULL;
    }


};


extern ScriptManager * CScriptManager;

inline static void InitScriptManager(ScriptManager * m)
{
   CScriptManager = m;
}

inline static ScriptManager * GetScriptManager()
{
    return CScriptManager;

}

inline static void DestroyScriptManager()
{
    if(CScriptManager != nullptr)
    {
        CScriptManager->Destroy();
        delete CScriptManager;
        CScriptManager = nullptr;

    };
}
