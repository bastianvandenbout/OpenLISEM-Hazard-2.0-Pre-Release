#ifndef WORLD2DWINDOW_H
#define WORLD2DWINDOW_H

#include "openglclmanager.h"
#include "qcolor.h"
#include "model.h"
#include "map.h"
#include "openglcltexture.h"
#include "openglprogram.h"
#include "iomanager.h"
#include "sphazardmath.h"
#include "QObject"
#include "openglcldatabuffer.h"
#include "colorf.h"
#include "uigeolayer.h"
#include "uirasterlayer.h"
#include "uivectorlayer.h"
#include "uipointcloudlayer.h"
#include "uimodelrasterlayer.h"
#include "uigeolayer.h"
#include "uimapedge.h"
#include "worldgltransforms.h"
#include "uimultirasterlayer.h"
#include "uiwebtilelayer.h"
#include "uimodelpointlayer.h"
#include "uilayereditor.h"
#include "gl3dcamera.h"
#include "gl3dcameracontroller.h"
#include "gl2dcamera.h"
#include "gl2dcameracontroller.h"
#include "QTime"
#include "QList"
#include "gl3dskybox.h"
#include "gl3dterrain.h"
#include "openglmsaarendertarget.h"
#include "gl3delevationprovider.h"
#include "QList"
#include "uilayereditor.h"
#include "layerregistry.h"

typedef struct World2DMouseState
{
    float scrollold = 0.0f;
    float scroll = 0.0f;
    bool dragging = false;
    bool in = true;

    float Drag_x = 0.0f;
    float Drag_y = 0.0f;
    float PosOld_x = 0.0f;
    float PosOld_y = 0.0f;
    float PosDO_x = 0.0f;
    float PosDO_y = 0.0f;
    float PosDOO_x = 0.0f;
    float PosDOO_y = 0.0f;
    float Pos_x = 0.0f;
    float Pos_y = 0.0f;

    bool move_first = true;
    float Move_X = 0.0f;
    float Move_Y = 0.0f;

    float Scroll_x = 0.0f;
    float Scroll_y = 0.0f;
    float ScrollOld_x = 0.0f;
    float ScrollOld_y = 0.0f;

    bool movescroll_first = true;
    float MoveScroll_X = 0.0f;
    float MoveScroll_Y = 0.0f;

    QList<bool> KeyEventShift;
    QList<int> KeyEvents;
    QList<int> KeyAction;
    QList<int> KeyMods;

    QList<bool> MouseButtonEventsShift;
    QList<int> MouseButtonEvents;
    QList<int> MouseButtonKeyAction;

} World2DMouseState;


//probe the world to find values
typedef struct WorldGeoProbeResult
{
    QList<UILayer * > Layers;
    QList<QString> Values;

}  WorldGeoProbeResult;


class WorldWindow : public QObject, public GLListener
{
    Q_OBJECT;

public:


private:
    //new
    //layers

    QList<UILayer *> m_UILayerList;
    UILayer * m_UIFocusLayer = nullptr;

    //internal signaling that
    bool m_UILayersChanged;
    bool m_CRSChanged =false;
    //
    GeoWindowState m_CurrentWindowState;
    GeoWindowState m_CurrentDrawWindowState;

    QMutex MouseStateMutex;
    World2DMouseState m_MouseState;

    //general draw settings:
    bool m_DrawUI = true;
    bool m_DrawLines = false;
    double m_DrawUIScale = 1.0;
    bool m_DrawLegends = true;
    bool m_Draw3D = false;
    bool m_Draw3DGlobe = false;

    SPHVector3 m_SunDir = SPHVector3(0.0,0.4,1.0);//default solar position
    bool m_SunDrag = false;
    GL3DCamera * m_Camera3D;
    GL3DCameraController *m_Camera3DController;
    GL2DCamera * m_Camera2D;
    GL2DCameraController *m_Camera2DController;

    GL3DElevationProvider * m_ElevationProvider;

    QElapsedTimer m_time;
    qint64 m_timeold;
    float m_GLDT;
    float m_GLT = 0;
    bool dt_first = true;
    bool draw_first = true;

    SPHVector3 m_Prev_GeoLoc;

    QList<OpenGLCLMSAARenderTarget *> m_2D3DRenderCTargets;
    QList<OpenGLCLMSAARenderTarget *> m_2D3DRenderDTargets;
    QList<float> m_2D3DRenderTargetScales;
    QList<BoundingBox> m_2D3DRenderTargetBoundingBox;
    QList<WorldGLTransformManager  *> m_2D3DTransformManager;

    OpenGLCLMSAARenderTarget * m_TDRenderTarget;

    OpenGLCLMSAARenderTarget * m_3DRenderTarget;
    OpenGLCLMSAARenderTarget * m_3DLayerRenderTargetC;
    OpenGLCLMSAARenderTarget * m_3DLayerRenderTargetD;


    cTMap * m_3DScreenPosX = nullptr;
    cTMap * m_3DScreenPosY = nullptr;
    cTMap * m_3DScreenPosZ = nullptr;

    UILayerEditor *m_LayerEditor = nullptr;

    QMutex m_FocusMutex;
    QList<SPHVector2> m_FocusLocations;
    bool m_DraggingFocusSquare = false;
    SPHVector2 m_FocusSquareStart;
    QList<BoundingBox> m_FocusSquare;

public:
    QMutex m_UILayerMutex;
    QMutex m_CRSMutex;
    //default drawing programs


    WorldGLTransformManager * m_TransformManager;
    OpenGLCLManager * m_OpenGLCLManager;
    SPHModel * m_Model;
    QString m_KernelDir;

    GeoProjection m_CurrentProjection;

    inline WorldWindow(OpenGLCLManager * glclm) : QObject()
    {

        m_OpenGLCLManager = glclm;
        m_OpenGLCLManager->AddListener(this);

        m_TransformManager = new WorldGLTransformManager(glclm);
        AddUILayer(new UIMapEdgeLayer());
        //AddUILayer(new UISkyBoxLayer());
        AddUILayer(new UITerrainLayer());
        GeoProjection p;
        p.SetGeneric();
        SetCurrentProjection(p,true);

        m_ElevationProvider = new GL3DElevationProvider();

        m_Camera3D = new GL3DCamera();
        m_Camera3DController = new GL3DCameraController(m_Camera3D,m_ElevationProvider);
        m_Camera2D = new GL2DCamera();
        m_Camera2DController = new GL2DCameraController(m_Camera2D);



        RegisterLayerTypes();


        m_time.start();
        m_timeold = m_time.nsecsElapsed();
        m_GLDT = 0.0f;
    }

    void SetDraw3D(bool d);
    void SetDraw3DGlobe(bool d);
    void SetUIDraw(bool d);
    void SetLinesDraw(bool d);
    void SetUIScale(double d);
    void SetLegendDraw(bool d);
    GeoProjection GetCurrentProjection();
    void SetCurrentProjection(GeoProjection p, bool forceupdate = false);
    GeoWindowState GetCurrentWindowState();

    QImage *DoExternalDraw(GeoWindowState s);

    void SignalClose();
    void SetModel( SPHModel * m);


    void AddOceanLayer();
    void AddSkyBoxLayer();
    void AddCloudLayer();

    int AddUILayer(UILayer *ML, bool emitsignal = true);
    UILayer * GetUILayerFromName(QString name);
    UILayer * GetUILayerFromID(int id);
    UILayer * GetUILayerFromFile(QString path);
    UILayer * GetUIStreamLayerFromFile(QString path);
    UILayer * GetUIDuoLayerFromFile(QList<QString> path);
    UILayer * GetUITimeSeriesLayerFromFile(QList<QString> path);
    UILayer * GetUILayerFromMap(cTMap * map, QString name = "", bool native = false);
    UILayer * GetUIVectorLayerFromFile(QString path);
    UILayer * GetUIPointCloudLayerFromFile(QString path);
    UILayer * GetUILayerFromShapeFile(ShapeFile * map, QString name = "", bool native = false);
    UILayer * GetGoogleLayer();
    UILayer * GetGoogleDEMLayer();
    void RemoveUILayer(UILayer *ML, bool emitsignal = true);
    void ReplaceUILayer(UILayer *ML, UILayer *ML2);
    void SetUILayerAs(UILayer *ML,  bool emitsignal = false);
    QList<UILayer *> GetUILayers();
    void SetLayerOrder(QList<UILayer *> newlist, bool emitsignal = false);
    int GetUILayerCount();
    UILayer* GetUILayer(int i);
    void AddNativeUILayers();
    void RemoveNativeUILayers();

    inline void SetFocusLayer(UILayer *fl)
    {
        m_UILayerMutex.lock();

        m_UIFocusLayer = fl;

        m_UILayerMutex.unlock();
    }
    //control of the display geolocation
    BoundingBox GetLook();
    void LookAt(UIGeoLayer* geolayer);
    void LookAt(BoundingBox b);


    inline void SetEditor(UILayerEditor *e)
    {
        m_UILayerMutex.lock();
        m_LayerEditor = e;
        m_UILayerMutex.unlock();

        m_FocusMutex.lock();
        m_FocusSquare.clear();
        m_FocusLocations.clear();
        m_FocusMutex.unlock();

        emit OnFocusLocationChanged();
    }

    inline void RemoveEditor()
    {
        m_LayerEditor = nullptr;
    }

    inline void RemoveAndDeleteEditor()
    {
        if(m_LayerEditor != nullptr)
        {
            delete m_LayerEditor;
        }
        m_LayerEditor = nullptr;
    }


    inline QList<SPHVector2> GetFocusLocations()
    {
        QList<SPHVector2> temp;
        m_FocusMutex.lock();

        for(int i = 0; i < m_FocusLocations.length(); i++)
        {
            temp.append(m_FocusLocations.at(i));
        }
        m_FocusMutex.unlock();

        return temp;
    }




    //input listener functions as called by OpenGLCLManager
    //here we also call all the ui functions for the layers:
    //inline void OnMouseMove(SPHVector2 pos, SPHVector2 mov)
    //inline void OnMouseScroll(SPHVector2 pos, SPHVector2 mov)
    //inline void OnMousePressed(int button, int pressed)
    //inline void OnMouseLeave(bool leave)
    //inline void OnKeyPressed(int key, bool pressed)
    //inline void OnGeoMousePressed(int button, GeoWindowState s, SPHVector2 pos)
    //inline void OnGeoMouseMove(GeoWindowState s, SPHVector2 pos, SPHVector2 mov)

    //furthermore, we have to call the ui functions for the currently active editor

    void OnMouseScroll(double scrollx ,double scrolly) override;
    void OnMouseMove(double posx,double posy) override;
    void OnMouseInOut(bool enter) override;
    void OnMouseKey( int key, int action, int mods) override;
    void OnFrameBufferSize(int w,int h) override;
    void OnKey(int key, int action, int mods) override;

    void InputToLayers();
    /////
    /// \brief UpdateCurrentWindowState
    /// This function calculates, based on the MouseState
    /// the current window state (zoom, extent, location etc..)
    ///
    /// \return true if the user has altered the windowstate
    ///
    bool UpdateCurrentWindowState();
    bool UpdateCamera();
    bool UpdateTransformers();

    WorldGeoProbeResult ProbeAll();
    WorldGeoProbeResult ProbeLayer(UILayer * l);

    void Draw();

    void DrawToFrameBuffer2D(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawToFrameBuffer2DElevation(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void Draw2DPlotDataToBuffer(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawToFrameBuffer3D(GeoWindowState S);
    void DrawGlobeToBuffer(GeoWindowState S);
    void Draw3DPlotDataToBuffer(GeoWindowState S);
    void DrawBaseLayers3D(GeoWindowState S);
    void DrawColoredTerrain3D(GeoWindowState *S);
    void Draw3DObjects(GeoWindowState S);
    void Draw3DPost(GeoWindowState s);
    void DrawGeoLayers2DColor(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawGeoLayers2DElevation(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawUILayers(GeoWindowState S, WorldGLTransformManager * glt = nullptr);

    //serialization

    UILayerTypeRegistry * m_LayerRegistry;

    void RegisterLayerTypes();
    void SaveTo(QJsonObject*);
    void LoadFrom(QJsonObject*);



signals:
    void OnMapsChanged();
    void OnFocusLocationChanged();

};

#endif // WORLD2DWINDOW_H
