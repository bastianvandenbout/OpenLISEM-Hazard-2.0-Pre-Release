#ifndef OPENGLCLMANAGER_H
#define OPENGLCLMANAGER_H

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"

#include <random>
#include <iostream>
#include <memory>
#include <functional>

#include <QApplication>
#include <QTimer>
#include <QMutex>
#include <QList>
#include <QWaitCondition>
#include <QPixmap>

#include "generalsettingsmanager.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>

#include "openglprogram.h"
#include "openglcltexture.h"
#include "openglgeometry.h"
#include "openclprogram.h"
#include "spherror.h"
#include "openglshapepainter.h"
#include "opengltextpainter.h"
#include "opengltexturepainter.h"
#include "openglmsaarendertarget.h"
#include "openglrendertarget.h"

#include "scriptmanager.h"
#include "iomanager.h"

#include "layerinfowidget.h"
#include "gl3dgeometry.h"

#include "QMessageBox"

typedef std::vector<cl::Platform>::iterator PlatformIter;

typedef struct {
    cl::Device d;
    cl::CommandQueue q;
    cl::Program p;
    cl::Kernel k;
    cl::Buffer i;
    cl::size_t<3> dims;
} process_params;

typedef struct {
    GLuint prg;
    GLuint vao;
    GLuint vbo;
    cl::BufferGL tmp;
} render_params;

static int const wind_width = 1600;
static int const wind_height= 800;

static const float matrix[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f};

static void glfw_error_callback(int error, const char* desc)
{
    std::cout << "glfw error callback " << error << desc << std::endl;
}

typedef struct UI_STATE
{
    float zoom;
    float Scroll;
    float Drag_x;
    float Drag_y;
    float Drag_zoomcomp_x;
    float Drag_zoomcomp_y;
    float Pos_x;
    float Pos_y;
    bool Dragging;
    int Width;
    int Height;
} UI_STATE;

static void glfw_mouse_scroll_callback(GLFWwindow* wind, double scrollx, double scrolly);
static void glfw_key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods);
static void glfw_mouse_callback(GLFWwindow* wind, int key, int action, int mods);
static void glfw_mouse_inout_callback(GLFWwindow* wind, int inout);
static void glfw_mouse_move_callback(GLFWwindow* wind, double x, double y);
static void glfw_framebuffer_size_callback(GLFWwindow* wind, int width, int height);


class GLListener
{
public:
    virtual void OnMouseScroll(double,double) = 0;
    virtual void OnMouseMove(double,double) = 0;
    virtual void OnMouseInOut(bool) = 0;
    virtual void OnMouseKey(int,int,int) = 0;
    virtual void OnFrameBufferSize(int,int) = 0;
    virtual void OnKey(int,int,int) = 0;
};

class OpenGLCLManager : public QObject
{
    Q_OBJECT

public:
    OpenGLCLManager();

    cl::Context context;
    GLFWwindow* window;
    cl::CommandQueue q;
    cl::Device m_device;
    GLFWmonitor* monitor;

    QMutex m_GLMutex;

    QMutex m_GLRestartMutex;
    bool m_RequestGLRestart = false;

    QMutex m_GLOutputUIMutex;

    QList<GLListener*> m_ListenerList;
    QList<std::function<void(void)>> m_CallBackFrameList;
    std::function<void(void)> m_CallBackFrame;

    QString m_KernelDir;
    QString m_AssetDir;

    OpenGLProgram * m_GLProgram_CopyText;
    OpenGLGeometry * m_Quad;
    GL3DGeometry * m_GeometryPlane;



    OpenGLTextPainter * m_TextPainter;
    OpenGLShapePainter * m_ShapePainter;
    OpenGLTexturePainter * m_TexturePainter;

    OpenGLProgram * m_GLProgram_uimap;
    OpenGLProgram * m_GLProgram_uiduomap;
    OpenGLProgram * m_GLProgram_uimapshape;
    OpenGLProgram * m_GLProgram_uimultimap;
    OpenGLProgram * m_GLProgram_uivectorl;
    OpenGLProgram * m_GLProgram_uivectorp;
    OpenGLProgram * m_GLProgram_uipointsimple;
    OpenGLProgram * m_GLProgram_particles;
    OpenGLProgram * m_GLProgram_pointcloud;
    OpenGLProgram * m_GLProgram_pointcloud3d;
    OpenGLProgram * m_GLProgram_uiterrain;
    OpenGLProgram * m_GLProgram_uiterrainlayer;
    OpenGLProgram * m_GLProgram_uiocean;
    OpenGLProgram * m_GLProgram_uiobject;
    OpenGLProgram * m_GLProgram_uiinstanced;


    OpenGLCLMSAARenderTarget * m_MSAATarget = 0;

    int m_width = wind_width;
    int m_height = wind_height;

    UI_STATE GL_GLOBAL;


    inline int CreateGLWindow(QPixmap pixmap)
    {

        GL_GLOBAL.zoom = 1.0;
        GL_GLOBAL.Drag_zoomcomp_x = 0.0;
        GL_GLOBAL.Drag_zoomcomp_y = 0.0;
        GL_GLOBAL.Scroll = 0;
        GL_GLOBAL.Drag_x = 0;
        GL_GLOBAL.Drag_y = 0;
        GL_GLOBAL.Pos_x = 0;
        GL_GLOBAL.Pos_y = 0;
        GL_GLOBAL.Dragging = false;
        GL_GLOBAL.Width = wind_width;
        GL_GLOBAL.Height = wind_height;

        m_KernelDir = QCoreApplication::applicationDirPath() + SPH_FOLDER_KERNELS;
        m_AssetDir = QCoreApplication::applicationDirPath() + SPH_FOLDER_ASSETS;
        SPH_DEBUG("init glcl manager");
        if (!glfwInit())
        {
            SPH_ERROR("glfwInit failed!");

            return 255;
        }

        glfwSetErrorCallback(glfw_error_callback);

        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode    = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS    , mode->redBits    );
        glfwWindowHint(GLFW_GREEN_BITS  , mode->greenBits  );
        glfwWindowHint(GLFW_BLUE_BITS   , mode->blueBits   );
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        SPH_DEBUG("Creating Window");

        m_width = wind_width;
        m_height = wind_height;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        window = glfwCreateWindow(wind_width,wind_height,"OpenLISEM Hazard",NULL,NULL);
        if (!window) {


            glfwTerminate();
            return 254;
        }
        glfwSetWindowUserPointer(window, this);

        GLFWimage image;
        image.width = pixmap.width();
        image.height = pixmap.height();
        image.pixels = pixmap.toImage().bits();
        glfwSetWindowIcon(window,1,&image);

        return 0;
    }

    inline void Restart()
    {
        m_GLRestartMutex.lock();
        m_RequestGLRestart = true;
        m_GLRestartMutex.unlock();
    }

    inline void SetScriptFunctions(ScriptManager * sm)
    {

        sm->m_Engine->RegisterGlobalFunction("void Restart()", asMETHOD(OpenGLCLManager, Restart), asCALL_THISCALL_ASGLOBAL, this);
    }

    inline int InitGLCL()
    {
        m_GLProgramList.clear();
        m_CLProgramList.clear();
        m_GLCLTextureList.clear();

        glfwMakeContextCurrent(window);


        int suc = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        if(!suc) {
            SPH_ERROR("gladLoadGL failed!");
            SPH_ERROR("Error Code " + QString::number(suc) + " " + QString::number(WIN32));
            return 253;
        }

        glad_glEnable(GL_BLEND);
        glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glad_glEnable(GL_MULTISAMPLE);

        glfwSetErrorCallback(glfw_error_callback);
        glfwSetKeyCallback(window,glfw_key_callback);
        glfwSetMouseButtonCallback(window,glfw_mouse_callback);
        glfwSetCursorPosCallback(window,glfw_mouse_move_callback);
        glfwSetScrollCallback(window,glfw_mouse_scroll_callback);
        glfwSetCursorEnterCallback(window,glfw_mouse_inout_callback);

        glfwSetFramebufferSizeCallback(window,glfw_framebuffer_size_callback);

        LoadPreferredCLContext();

        //init basic shader programs

        m_GLProgram_CopyText = new OpenGLProgram();
        m_GLProgram_CopyText->LoadProgram(m_KernelDir+ "CopyTexture.vert", m_KernelDir+ "CopyTexture.frag");
        m_Quad = new OpenGLGeometry();
        m_Quad->CreateUnitQuad();

        m_GeometryPlane = new GL3DGeometry();
        m_GeometryPlane->CreateFromRegularUnitGrid(25);

        m_TextPainter = new OpenGLTextPainter();
        m_TextPainter->LoadDefaultFontsAndShaders();
        m_ShapePainter = new OpenGLShapePainter();
        m_ShapePainter->LoadShaders();
        m_TexturePainter = new OpenGLTexturePainter();
        m_TexturePainter->LoadShaders();

        m_GLProgram_uimap = GetMGLProgram(m_KernelDir+ "UIMapDraw.vert", m_KernelDir+ "UIMapDraw.frag");
        m_GLProgram_uiduomap = GetMGLProgram(m_KernelDir+ "UIDuoMapDraw.vert", m_KernelDir+ "UIDuoMapDraw.frag");
        m_GLProgram_uimapshape = GetMGLProgram(m_KernelDir+ "UIMapDrawShape.vert", m_KernelDir+ "UIMapDrawShape.frag");
        m_GLProgram_uimultimap = GetMGLProgram(m_KernelDir+ "UIMultiMapDraw.vert", m_KernelDir+ "UIMultiMapDraw.frag");
        m_GLProgram_uivectorl = GetMGLProgram(m_KernelDir+ "UIGeoLineDraw.vert", m_KernelDir+ "UIGeoLineDraw.frag");
        m_GLProgram_uivectorp = GetMGLProgram(m_KernelDir+ "UIGeoPolygonDraw.vert", m_KernelDir+ "UIGeoPolygonDraw.frag");
        m_GLProgram_particles = GetMGLProgram(m_KernelDir + "partsim.vert", m_KernelDir + "partsim.frag");
        m_GLProgram_pointcloud = GetMGLProgram(m_KernelDir + "UIGeoPointCloud.vert", m_KernelDir + "UIGeoPointCloud.frag");
        m_GLProgram_pointcloud3d = GetMGLProgram(m_KernelDir + "UIGeo3DPointCloud.vert", m_KernelDir + "UIGeo3DPointCloud.frag");
        m_GLProgram_uipointsimple =GetMGLProgram(m_KernelDir+ "UIGeoSimplePointDraw.vert", m_KernelDir+ "UIGeoSimplePointDraw.frag");
        m_GLProgram_uiterrain = GetMGLProgram(m_KernelDir + "terrain_vs.glsl", m_KernelDir + "terrain_fs.glsl", m_KernelDir + "terrain_gs.glsl", m_KernelDir + "terrain_tc.glsl", m_KernelDir + "terrain_te.glsl");
        m_GLProgram_uiterrainlayer = GetMGLProgram(m_KernelDir + "terrain_vs_layer.glsl", m_KernelDir + "terrain_fs_layer.glsl", m_KernelDir + "terrain_gs_layer.glsl", m_KernelDir + "terrain_tc_layer.glsl", m_KernelDir + "terrain_te_layer.glsl");
        m_GLProgram_uiocean = GetMGLProgram(m_KernelDir + "terrain_vs_ocean.glsl", m_KernelDir + "terrain_fs_ocean.glsl", m_KernelDir + "terrain_gs_ocean.glsl", m_KernelDir + "terrain_tc_ocean.glsl", m_KernelDir + "terrain_te_ocean.glsl");
        m_MSAATarget = new OpenGLCLMSAARenderTarget();

        return 0;

    }



    inline void LoadPreferredCLContext()
    {


        SPH_DEBUG("Get Context");

        SPH_STATUS(QString("OpenGL %1.%2\n").arg(GLVersion.major).arg(GLVersion.minor));

        SPH_DEBUG("Get Preferred Platform setting");

        QString preferred = GetSettingsManager()->GetSetting("PreferredGPUDevice");


        SPH_DEBUG("Get Platforms");

        float max_score = 0.0;

        cl::Platform okPlatform;

        bool is_okdeviceset = false;
        QList<cl::Device> ok_cldevices;
        QList<QString> ok_devices;
        QList<float> ok_devicescores;

        cl_int errCode;
        try {
            ListPlatforms();

            //get preferred platform from settings


            //now iterate over all platforms and devices and see what we can get
            cl_int error = 0;
            try {
                // Get available platforms
                std::vector<cl::Platform> platforms;
                cl::Platform::get(&platforms);

                for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {

                    cl::Platform lPlatform = *it;

                    std::string platformname = it->getInfo<CL_PLATFORM_NAME>();

                    std::vector<cl::Device> devices;
                    lPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
                    for (unsigned d=0; d<devices.size(); ++d) {

                        QString devicename = QString(devices[d].getInfo<CL_DEVICE_NAME>().c_str());

                        {
                            int computeunits = devices[d].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
                            int computeclock = devices[d].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
                            int computepars = devices[d].getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>();
                            int computetype = devices[d].getInfo<CL_DEVICE_TYPE>();
                            QString clversion = QString(devices[d].getInfo<CL_DEVICE_VERSION>().c_str());
                            unsigned long long computemem = devices[d].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
                            QString clcversion = QString(devices[d].getInfo<CL_DEVICE_OPENCL_C_VERSION>().c_str());

                            size_t maxwidth = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>();
                            size_t maxheight = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>();

                            const char *typestr;

                            switch(computetype) {
                            case CL_DEVICE_TYPE_CPU:
                                typestr = "CPU";
                                break;
                            case CL_DEVICE_TYPE_ACCELERATOR:
                                typestr = "accelerator";
                                break;
                            case CL_DEVICE_TYPE_GPU:
                                typestr = "GPU";
                                break;
                            default:
                                typestr = "<unknown>";
                            }
                            bool interop = checkExtnAvailability(devices[d],CL_GL_SHARING_EXT);

                            if(interop)
                            {
                                bool is_preferred = false;
                                if((QString(platformname.c_str()) + "  " + devicename).compare(preferred) == 0)
                                {
                                    is_preferred = true;
                                    SPH_DEBUG("Found Preferred Device " + QString(platformname.c_str()) + "  " + devicename);
                                }

                                ok_cldevices.append(devices[d]);
                                ok_devices.append(QString(platformname.c_str()) + "  " + devicename);

                                float score =(computetype == CL_DEVICE_TYPE_GPU? 5.0:0.0);
                                score += (QString(platformname.c_str()).startsWith("NVIDIA"))? 5 : 0.0;
                                score += (QString(platformname.c_str()).compare("AMD") == 0)? 3 : 0.0;
                                score += (is_preferred? 10.0:0.0);

                                ok_devicescores.append(score);

                                if(score > max_score)
                                {
                                    max_score= score;
                                    m_device = devices[d];
                                    is_okdeviceset = true;
                                    okPlatform = lPlatform;
                                }
                            }else
                            {
                                if((QString(platformname.c_str()) + "  " + devicename).compare(preferred) == 0)
                                {
                                    SPH_DEBUG("Preferred device does not have gl/cl interoperability");

                                }
                            }

                            break;
                        }


                    }
                }
            } catch(cl::Error err) {
            }

            if(is_okdeviceset)
            {
                SPH_DEBUG("Platform Found");
            }else
            {
                SPH_ERROR("Can not found suitible platform for OpenGL/OpenCL interoperability");
                throw 1;
            }

            // Select the default platform and create a context using this platform and the GPU
    #ifdef OS_LNX
            cl_context_properties cps[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetGLXContext(window),
                CL_GLX_DISPLAY_KHR, (cl_context_properties)glfwGetX11Display(),
                CL_CONTEXT_PLATFORM, (cl_context_properties)okPlatform(),
                0
            };
    #endif
    #ifdef OS_WIN
            cl_context_properties cps[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
                CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
                CL_CONTEXT_PLATFORM, (cl_context_properties)okPlatform(),
                0
            };
    #endif


            SPH_DEBUG("Create CL Context");

            context = cl::Context(m_device, cps);

        } catch(cl::Error error) {
            std::cout << error.what() << "(" << error.err() << ")" << std::endl;
            std::string val = cl::Program().getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_device);
            std::cout<<"Log:\n"<<val<<std::endl;
            throw 249;
        }

        // Create a command queue and use the first device
        q = cl::CommandQueue(context, m_device);


    }

    inline void AddListener(GLListener * l)
    {
        m_ListenerList.append(l);
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackFrame(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackFrame = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackFrameList.append(m_CallBackFrame);
    }

    inline OpenGLCLMSAARenderTarget * GetFrameBuffer()
    {
        return m_MSAATarget;
    }
    inline void CreateMSAABuffer()
    {

        m_MSAATarget->Resize(m_width,m_height,1);
    }

    inline void GLCLLoop()
    {

        SPH_STATUS("Done initializing OpenGL and OpenCL, Starting Loop..");

        bool restart = false;

        while (!glfwWindowShouldClose(window)) {

            m_GLOutputUIMutex.lock();
            m_GLMutex.lock();

            //is there a call to recreate the window using a different context?
            //we can only do this if the model is not running, since its resources will be invalidated
            //this is guaranteed by the user interface
            //if a restart is requested, we throw a specific error code through the qt app to the sphazard execute function

            m_GLRestartMutex.lock();
            if(m_RequestGLRestart)
            {
                m_RequestGLRestart = false;
                glfwSetWindowShouldClose(window,GL_TRUE);
                restart = true;
            }
            m_GLRestartMutex.unlock();

            //make this thread current for opengl
            glfwMakeContextCurrent(window);


            CreateMSAABuffer();


            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_MSAATarget->GetFrameBuffer());
            glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glad_glDisable(GL_DEPTH_TEST);
            glad_glClearColor(0.5,0.5,0.5,1.0);

            m_TexturePainter->UpdateRenderTargetProperties(m_MSAATarget->GetFrameBuffer(),GL_GLOBAL.Width,GL_GLOBAL.Height);
            m_TextPainter->UpdateRenderTargetProperties(m_MSAATarget->GetFrameBuffer(),GL_GLOBAL.Width,GL_GLOBAL.Height);
            m_ShapePainter->UpdateRenderTargetProperties(m_MSAATarget->GetFrameBuffer(),GL_GLOBAL.Width,GL_GLOBAL.Height);


            for(int i = 0; i < m_CallBackFrameList.length(); i++)
            {
                m_CallBackFrameList.at(i)();
            }

            // render msaa  render buffer to normal texture

            m_MSAATarget->BlitToTexture();


            //render texture to screen
            glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glad_glDrawBuffers(1, DrawBuffers);

            glad_glViewport(0,0,m_width,m_height);

            glad_glDisable(GL_DEPTH_TEST);
            // bind shader
            glad_glUseProgram(m_GLProgram_CopyText->m_program);
            // get uniform locations
            int mat_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"matrix");
            int tex_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"tex");
            // bind texture
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glUniform1i(tex_loc,0);
            glad_glBindTexture(GL_TEXTURE_2D,m_MSAATarget->GetTexture());
            // set project matrix
            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
            // now render stuff
            glad_glBindVertexArray(m_Quad->m_vao);
            glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);


            //swap opengl front and back buffer
            glfwSwapBuffers(window);

            //reset the current thread for opengl
            glfwMakeContextCurrent(NULL);

            m_GLMutex.unlock();
            m_GLOutputUIMutex.unlock();

            if(restart)
            {
                break;
            }
        }


        if(restart)
        {

            QCoreApplication::exit(SPH_CODE_RETURN_RESTART);
        }else
        {
            //now, since we quit this display, we also quit the other qt window

            QCoreApplication::exit(SPH_CODE_RETURN_NORMAL);

        }




    }
    inline int CopyTextureToFrameBuffer(GLuint Source, GLuint Target)
    {


        return 0.0;
    }


    inline int CopyTextureUsingShaders(OpenGLCLTexture * m_Texture_source, OpenGLCLTexture * m_Texture_Target)
    {
        if(!(m_Texture_Target->m_IsFrambuffer))
        {
            return 1;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_Texture_Target->m_FramebufferName);

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glad_glDrawBuffers(1, DrawBuffers);

        glad_glViewport(0,0,m_Texture_Target->m_dims[0],m_Texture_Target->m_dims[1]);

        glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glad_glClearColor(0.0,0.0,0.0,0.0);
        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_CopyText->m_program);
        // get uniform locations
        int mat_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"matrix");
        int tex_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"tex");
        // bind texture
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glUniform1i(tex_loc,0);
        glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source->m_texgl);
        // set project matrix
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
        // now render stuff
        glad_glBindVertexArray(m_Quad->m_vao);
        glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);

        return 0;

    }
    inline int CopyTextureToMap(GLuint m_Texture_source, cTMap * m_Target)
    {

        if(m_Target != 0 && m_Texture_source != 0)
        {
            //void * data = new float(m_Texture_source->m_dims[0] * m_Texture_source->m_dims[1]);//m_Target->data[0];
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source);
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,m_Target->data[0]);
        }

        return 0;
    }

    inline int CopyTextureToMap(OpenGLCLTexture * m_Texture_source, cTMap * m_Target)
    {

        if(m_Target != 0 && m_Texture_source != 0)
        {
            //void * data = new float(m_Texture_source->m_dims[0] * m_Texture_source->m_dims[1]);//m_Target->data[0];
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source->m_texgl);
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,m_Target->data[0]);
        }

        return 0;
    }
    inline int CopyTextureToMap(OpenGLCLMSAARenderTarget* m_Texture_source, cTMap * m_Target)
    {

        if(m_Target != 0 && m_Texture_source != 0)
        {
            //void * data = new float(m_Texture_source->m_dims[0] * m_Texture_source->m_dims[1]);//m_Target->data[0];
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source->GetTexture());
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,m_Target->data[0]);
        }

        return 0;
    }


    QList<OpenGLProgram*> m_GLProgramList;
    QList<OpenCLProgram*> m_CLProgramList;
    QList<OpenGLCLTexture*> m_GLCLTextureList;
    QList<OpenGLProgram*> m_GLProgramModelList;
    QList<OpenCLProgram*> m_CLProgramModelList;
    QList<OpenGLCLTexture*> m_GLCLTextureModelList;

    inline OpenGLProgram* ModelGetMGLProgram(QString file_vs, QString file_fs, bool add = true)
    {
        OpenGLProgram* Program = new OpenGLProgram();
        Program->LoadProgram(file_vs, file_fs);
        if(add)
        {
            m_GLProgramModelList.append(Program);
        }
        return Program;

    }

    inline OpenCLProgram* ModelGetMCLProgram(QString file, QString function, bool add = true)
    {
        OpenCLProgram* Program = new OpenCLProgram();
        Program->LoadProgram(this->context,this->m_device,file,function);
        if(add)
        {
            m_CLProgramModelList.append(Program);
        }
        return Program;
    }

    inline OpenGLCLTexture * ModelGetMCLGLTexture(int width, int height, float value = 0.0, bool rendertarget = false, bool add =  true )
    {
        static int i = 0;
        std::cout << "Get Texture in OpenCL " << i++ << std::endl;
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,value,rendertarget);
        if(add)
        {
            m_GLCLTextureModelList.append(_T);
        }
        return _T;

    }

    inline OpenGLProgram* GetMGLProgram(QString file_vs, QString file_fs, bool add = false)
    {
        OpenGLProgram* Program = new OpenGLProgram();
        Program->LoadProgram(file_vs, file_fs);
        m_GLProgramList.append(Program);
        return Program;

    }

    inline OpenGLProgram* GetMGLProgram(QString file_vs, QString file_fs, QString file_gs, QString file_tcs, QString file_tes, bool add = false)
    {
        OpenGLProgram* Program = new OpenGLProgram();
        Program->LoadProgram(file_vs, file_fs, file_gs, file_tcs,file_tes);
        m_GLProgramList.append(Program);
        return Program;

    }

    inline OpenCLProgram* GetMCLProgram(QString file, QString function, bool add = false)
    {
        OpenCLProgram* Program = new OpenCLProgram();
        Program->LoadProgram(this->context,this->m_device,file,function);
        if(add)
        {
            m_CLProgramList.append(Program);
        }
        return Program;
    }

    inline OpenGLCLTexture * GetMCLGLTexture(int width, int height, float value = 0.0, bool rendertarget = false, bool add = false)
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,value,rendertarget);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;

    }

    inline OpenGLCLTexture * GetMGLTexture(int width, int height, float * data, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,0.0,rendertarget,false);
        _T->FillData2DRF32(data);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;
    }

    inline OpenGLCLTexture * GetMCLGLTexture(int width, int height, float * data, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,0.0,rendertarget);
        _T->FillData2DRF32(data);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;
    }

    inline OpenGLCLTexture * GetMCLGLLinearTexture(int width, int height, float value = 0.0, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32Linear(context,width,height,value,rendertarget);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;

    }

    inline OpenGLCLTexture * GetMCLGLLinearTexture(int width, int height, float * data, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32Linear(context,width,height,0.0,rendertarget);
        _T->FillData2DRF32(data);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;

    }


    inline void DestroyMCLGLTexture(OpenGLCLTexture * t)
    {
        for(int i = m_GLCLTextureList.length()-1; i > -1; i--)
        {
            if(t ==m_GLCLTextureList.at(i))
            {
                m_GLCLTextureList.at(i)->Destroy();
                delete m_GLCLTextureList.at(i);
                m_GLCLTextureList.removeAt(i);
                break;
            }

        }
    }


    inline OpenGLCLTexture * ModelGetMCLGLTexture(cTMap * map, bool rendertarget =false, bool flipv = false, bool add = true)
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DFromMap(context,map,rendertarget, flipv);
        if(add)
        {
            m_GLCLTextureModelList.append(_T);
        }
        return _T;
    }

    inline OpenGLCLTexture * GetMCLGLTexture(cTMap * map, bool rendertarget =false, bool flipv = false, bool add = false)
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DFromMap(context,map,rendertarget, flipv);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;
    }

    void DeleteMGLCLModelObjects()
    {
        for(int i = m_GLProgramModelList.length()-1; i > -1; i--)
        {
            m_GLProgramModelList.at(i)->Destroy();
            delete m_GLProgramModelList.at(i);
        }
        m_GLProgramModelList.clear();

        for(int i = m_CLProgramModelList.length()-1; i > -1; i--)
        {
            m_CLProgramModelList.at(i)->Destroy();
            delete m_CLProgramModelList.at(i);
        }
        m_CLProgramModelList.clear();

        for(int i = m_GLCLTextureModelList.length()-1; i > -1; i--)
        {
            m_GLCLTextureModelList.at(i)->Destroy();
            delete m_GLCLTextureModelList.at(i);
        }
        m_GLCLTextureModelList.clear();
    }

    void DeleteMGLCLObjects()
    {
        for(int i = m_GLProgramList.length()-1; i > -1; i--)
        {
            m_GLProgramList.at(i)->Destroy();
            delete m_GLProgramList.at(i);
        }
        m_GLProgramList.clear();

        for(int i = m_CLProgramList.length()-1; i > -1; i--)
        {
            m_CLProgramList.at(i)->Destroy();
            delete m_CLProgramList.at(i);
        }
        m_CLProgramList.clear();

        for(int i = m_GLCLTextureList.length()-1; i > -1; i--)
        {
            m_GLCLTextureList.at(i)->Destroy();
            delete m_GLCLTextureList.at(i);
        }
        m_GLCLTextureList.clear();
    }

    void Destroy()
    {
        m_GLProgram_CopyText->Destroy();
        SAFE_DELETE(m_GLProgram_CopyText);

        m_Quad->Destroy();
        SAFE_DELETE(m_Quad);

        DeleteMGLCLModelObjects();
        DeleteMGLCLObjects();

        m_TexturePainter->Destroy();
        m_TextPainter->Destroy();
        m_ShapePainter->Destroy();

        SAFE_DELETE(m_TexturePainter);
        SAFE_DELETE(m_TextPainter);
        SAFE_DELETE(m_ShapePainter);


        m_MSAATarget->Destroy();

        glfwDestroyWindow(window);


        glfwTerminate();

        m_CallBackFrameList.clear();

    }

    //let objects call a dialog
public:
    MatrixTable *m_DialogInput;
    MatrixTable *m_DialogResult;
    int m_DialogReturn = 0;
    bool m_DoDialog = false;
    QMutex m_DialogMutex;
    QWaitCondition m_DialogWaitCondition;

    inline MatrixTable * DoTableDialog(MatrixTable * t,bool can_edit = false, bool can_save = false, bool can_script = false, bool has_types_columns = false, bool has_type_horizontal = false, bool is_dialog = false)
    {
        m_DialogInput = t;

        m_DialogMutex.lock();

        m_DoDialog = true;

        m_DialogWaitCondition.wait(&m_DialogMutex);

        m_DialogMutex.unlock();
        return GetResultingMatrixTable();
    }

    //return code (success, failure)
    inline int GetResultingMatrixTableCode()
    {
        return  m_DialogReturn;
    }

    inline MatrixTable * GetResultingMatrixTable()
    {
        return m_DialogResult;
    }


private:
    inline void DoMatrixTableDialogPrivate()
    {

        m_DialogMutex.lock();
        m_DoDialog = false;

        if(m_DialogInput != nullptr)
        {
            LayerInfoWidget *w = new LayerInfoWidget(m_DialogInput,false,false,false,false,false,true);
            if(QDialog::Accepted == w->exec())
            {
                m_DialogReturn = 1;
            }else
            {
                m_DialogReturn = 0;
            }
            m_DialogResult = w->GetMatrixTable();

        }

        m_DialogMutex.unlock();
        m_DialogWaitCondition.notify_all();

    }
public slots:

    //this function has to be called from main thread
    inline void ProcessEvents()
    {
        //glfwPollEvents();

        if(!glfwWindowShouldClose(window))
        {


            bool dodialog = false;
            m_DialogMutex.lock();


            if(m_DoDialog)
            {
                dodialog = true;
                m_DoDialog = false;
            }

            m_DialogMutex.unlock();


            if(dodialog)
            {
                DoMatrixTableDialogPrivate();
            }

            QTimer::singleShot(0,this,&OpenGLCLManager::ProcessEvents);

        }
    }

};


inline static void glfw_mouse_scroll_callback(GLFWwindow* wind, double scrollx, double scrolly)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);

    if(man == NULL)
    {
        return;
    }
    man->GL_GLOBAL.Scroll = std::min(100000000.0,std::max(-10000000.0,man->GL_GLOBAL.Scroll +scrolly));

    man->GL_GLOBAL.zoom = std::min(10000000.0,std::max(0.00000001,man->GL_GLOBAL.zoom * std::pow(0.95,scrolly)));

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseScroll(scrollx,scrolly);
    }
}


inline static void glfw_key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        //glfwSetWindowShouldClose(wind, GL_TRUE);
    }

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnKey(key,action,mods);
    }
}

inline static void glfw_mouse_callback(GLFWwindow* wind, int key, int action, int mods)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    if (key == GLFW_MOUSE_BUTTON_1  && action == GLFW_PRESS) {
        man->GL_GLOBAL.Dragging = true;
    }else if(key == GLFW_MOUSE_BUTTON_1  && action == GLFW_RELEASE) {
        man->GL_GLOBAL.Dragging = false;
    }
    if (key == GLFW_MOUSE_BUTTON_2  && action == GLFW_PRESS) {
    }

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseKey(key,action,mods);
    }
}

inline static void glfw_mouse_inout_callback(GLFWwindow* wind, int inout)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseInOut(inout? true:false);
    }

}

inline static void glfw_mouse_move_callback(GLFWwindow* wind, double x, double y)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    if(man->GL_GLOBAL.Dragging)
    {
        double dx = x - man->GL_GLOBAL.Pos_x;
        double dy = y - man->GL_GLOBAL.Pos_y;

        man->GL_GLOBAL.Drag_x += dx;
        man->GL_GLOBAL.Drag_y += dy;

        man->GL_GLOBAL.Drag_zoomcomp_x += man->GL_GLOBAL.zoom * dx;
        man->GL_GLOBAL.Drag_zoomcomp_y += man->GL_GLOBAL.zoom * dy;
    }
    man->GL_GLOBAL.Pos_x = x;
    man->GL_GLOBAL.Pos_y = y;

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseMove(x,y);
    }
}

inline static void glfw_framebuffer_size_callback(GLFWwindow* wind, int width, int height)
{
    glad_glViewport(0,0,width,height);
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    man->m_width = width;
    man->m_height = height;
    man->GL_GLOBAL.Width = width;
    man->GL_GLOBAL.Height = height;

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnFrameBufferSize(width,height);
    }
}


#endif // OPENGLCLMANAGER_H
