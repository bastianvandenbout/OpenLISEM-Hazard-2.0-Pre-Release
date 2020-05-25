#ifndef SPHSTYLE_H
#define SPHSTYLE_H

#include "colorf.h"
#include "QList"
#include "sph_interval.h"
#include "sphlinestyle.h"
#include "sphfillstyle.h"
#include "sphcolorgradient.h"
#include "sph_vector3.h"
#include "QImage"
#include "QMutex"
#include "mutex"
#include "openglobject.h"
#include "serialization.h"

#define SPH_STYLE_DEFAULT_RASTERHS 1
#define SPH_STYLE_DEFAULT_RASTERWH 2
#define SPH_STYLE_DEFAULT_RASTERUI 3
#define SPH_STYLE_DEFAULT_VECTORUI 4
#define SPH_STYLE_DEFAULT_MULTIRASTER 5
#define SPH_STYLE_DEFAULT_POINTCLOUDUI 6
#define SPH_STYLE_DEFAULT_DUORASTER 7
#define SPH_STYLE_DEFAULT_DUORASTER_VEL 8

class SPHStyle
{
public:

    inline SPHStyle()
    {

    }

    inline ~SPHStyle()
    {

    }

    inline void Save(QJsonObject * obj)
    {
        SPHSerialize::From(obj,"Transparancy",m_Transparancy);
        SPHSerialize::FromObject<SPHColorGradient>(obj,"ColorGradient1",m_ColorGradientb1);
        SPHSerialize::FromObject<SPHColorGradient>(obj,"ColorGradient2",m_ColorGradientb2);
        SPHSerialize::From<SPHInterval>(obj,"IntervalB1",m_Intervalb1);
        SPHSerialize::From<SPHInterval>(obj,"IntervalB2",m_Intervalb2);
        SPHSerialize::From<SPHInterval>(obj,"IntervalB2",m_Intervalb3);


        SPHSerialize::From(obj,"PointSize",m_PointSize);
        SPHSerialize::FromObject<SPHFillStyle>(obj,"PointFillStyle",m_PointFillStyle);
        SPHSerialize::FromObject<SPHFillStyle>(obj,"PolygonFillStyle",m_PolygonFillStyle);
        SPHSerialize::FromObject<SPHLineStyle>(obj,"PointLineStyle",m_PointLineStyle);
        SPHSerialize::FromObject<SPHLineStyle>(obj,"PolygonLineStyle",m_PolygonLineStyle);
        SPHSerialize::FromObject<SPHLineStyle>(obj,"LineLineStyle",m_LineLineStyle);

        SPHSerialize::From(obj,"Band1",m_IndexB1);
        SPHSerialize::From(obj,"Band2",m_IndexB2);
        SPHSerialize::From(obj,"Band3",m_IndexB3);

        SPHSerialize::From(obj,"CurrentTime",m_CurrentTime);
        SPHSerialize::From(obj,"PlaySpeed",m_PlaySpeed);
        SPHSerialize::From(obj,"Play",m_Play);

        SPHSerialize::From(obj,"CurrentTime",m_CurrentTime);
        SPHSerialize::From(obj,"PlaySpeed",m_PlaySpeed);
        SPHSerialize::From(obj,"Play",m_Play);

        SPHSerialize::From<ColorF>(obj,"SunColor",SunColor);
        SPHSerialize::From(obj,"AngleVertical", angle_vert);
        SPHSerialize::From(obj,"AngleHorizontal", angle_hor);
        SPHSerialize::From(obj,"IsElevation",m_IsDEM);
        SPHSerialize::From(obj,"IsHillShade",m_IsHS);
        SPHSerialize::From(obj,"IsVelocity",m_IsVelocity);
        SPHSerialize::From(obj,"IsDuoFractional",m_IsFractional);
        SPHSerialize::From(obj,"ArrowSpacing",m_VelSpacing);
    }

    inline void Restore(QJsonObject * obj)
    {
        std::cout << "style restore " << m_ColorGradientb1.m_Gradient_Stops.length() << std::endl;

        SPHSerialize::To(obj,"Transparancy",m_Transparancy);
        SPHSerialize::ToObject<SPHColorGradient>(obj,"ColorGradient1",m_ColorGradientb1);
        SPHSerialize::ToObject<SPHColorGradient>(obj,"ColorGradient2",m_ColorGradientb2);
        SPHSerialize::To<SPHInterval>(obj,"IntervalB1",m_Intervalb1);
        SPHSerialize::To<SPHInterval>(obj,"IntervalB2",m_Intervalb2);
        SPHSerialize::To<SPHInterval>(obj,"IntervalB2",m_Intervalb3);

        std::cout << "style restore2 " << m_ColorGradientb1.m_Gradient_Stops.length() <<m_Transparancy << std::endl;

        SPHSerialize::To(obj,"PointSize",m_PointSize);
        SPHSerialize::ToObject<SPHFillStyle>(obj,"PointFillStyle",m_PointFillStyle);
        SPHSerialize::ToObject<SPHFillStyle>(obj,"PolygonFillStyle",m_PolygonFillStyle);
        SPHSerialize::ToObject<SPHLineStyle>(obj,"PointLineStyle",m_PointLineStyle);
        SPHSerialize::ToObject<SPHLineStyle>(obj,"PolygonLineStyle",m_PolygonLineStyle);
        SPHSerialize::ToObject<SPHLineStyle>(obj,"LineLineStyle",m_LineLineStyle);

        SPHSerialize::To(obj,"Band1",m_IndexB1);
        SPHSerialize::To(obj,"Band2",m_IndexB2);
        SPHSerialize::To(obj,"Band3",m_IndexB3);

        SPHSerialize::To(obj,"CurrentTime",m_CurrentTime);
        SPHSerialize::To(obj,"PlaySpeed",m_PlaySpeed);
        SPHSerialize::To(obj,"Play",m_Play);

        SPHSerialize::To(obj,"CurrentTime",m_CurrentTime);
        SPHSerialize::To(obj,"PlaySpeed",m_PlaySpeed);
        SPHSerialize::To(obj,"Play",m_Play);

        SPHSerialize::To<ColorF>(obj,"SunColor",SunColor);
        SPHSerialize::To(obj,"AngleVertical", angle_vert);
        SPHSerialize::To(obj,"AngleHorizontal", angle_hor);
        SPHSerialize::To(obj,"IsElevation",m_IsDEM);
        SPHSerialize::To(obj,"IsHillShade",m_IsHS);
        SPHSerialize::To(obj,"IsVelocity",m_IsVelocity);
        SPHSerialize::To(obj,"IsDuoFractional",m_IsFractional);
        SPHSerialize::To(obj,"ArrowSpacing",m_VelSpacing);
    }

    //these options are available to determine what a user can choose
    float m_Transparancy = 0.0;

    //these values are indications of what the layer needs
    //when setting the style, these values are maintainted from the original
    bool m_HasSingleBand = false;
    bool m_HasDuoBand = false;
    bool m_HasTime = false;
    bool m_HasTripleBand = false;
    bool m_HasVectorPointLine = false;
    bool m_HasVectorPointFill = false;
    bool m_HasVectorLineLine = false;
    bool m_HasVectorPolygonLine = false;
    bool m_HasVectorPolygonFill = false;

    //these are meant for simplified settings within the layer widget
    bool m_StyleSimpleGradient = false; //adds a simple gradient
    bool m_StyleSimpleFillColor = false; //adds a color selection
    bool m_StyleSimpleLineColor = false; //adds a color selection
    bool m_StyleSimpleRange = false; //adds a simple min/max spin box
    bool m_StyleSimpleSize = false; //adds a simple line size selector

    int m_TimeMax = 0;
    int m_TimeMin = 0;

    int m_Bands = 0;




    //the following structures carry the actual drawing information

    //single or multi-band coloring
    //single band means color gradient
    //triple band means rgb
    SPHColorGradient m_ColorGradientb1;
    SPHColorGradient m_ColorGradientb2;
    SPHInterval m_Intervalb1;
    SPHInterval m_Intervalb2;
    SPHInterval m_Intervalb3;
    int m_IndexB1 = 0;
    int m_IndexB2 = 0;
    int m_IndexB3 = 0;

    //time properties
    int m_CurrentTime = 0;
    float m_PlaySpeed = 0.1;
    float m_TimePassed = 0.0;
    bool m_Play = false;

    //DEM properties
    ColorF SunColor;
    float angle_vert = 70.0;
    float angle_hor = 240.0;
    float DemScale = 1.0;
    bool m_IsDEM = false;
    bool m_IsHS = false;

    //surface properties
    bool m_IsSurface = false;
    bool m_IsSurfaceFlow = false;
    bool m_UseSecondDuoBandAsHeight = false;

    //rendering properties
    bool m_RenderTextured = false;

    QMutex * m_RenderTextureMutex = nullptr;

    //textures: bump, normal, specular
    QString m_RenderTextureDiffuseLoc;
    QString m_RenderTextureBumpLoc;
    QString m_RenderTextureNormalLoc;
    QString m_RenderTextureSpecularLoc;

    QString m_RenderTextureDiffuseCLoc;
    QString m_RenderTextureBumpCLoc;
    QString m_RenderTextureNormalCLoc;
    QString m_RenderTextureSpecularCLoc;

    QImage m_RenderTextureDiffuse;
    QImage m_RenderTextureBump;
    QImage m_RenderTextureNormal;
    QImage m_RenderTextureSpecular;

    bool m_RenderInstanced = false;

    //full textured object to instance

    QMutex * m_RenderInstancedMutex = nullptr;

    QString m_RenderInstancedLoc;
    QString m_RenderInstancedCLoc;

    OpenGLCLObject* m_RenderInstancedObject = nullptr;

    //velocity properties
    bool m_IsVelocity = false;
    int m_VelSpacing = 36;

    bool m_IsFractional = false;

    //for each type of element in shapefile
    //there is a fill or linestyle struct
    float m_PointSize = 5;
    SPHFillStyle m_PointFillStyle;
    SPHLineStyle m_PointLineStyle;
    SPHLineStyle m_LineLineStyle;
    SPHFillStyle m_PolygonFillStyle;
    SPHLineStyle m_PolygonLineStyle;

    bool IncreaseTimeBy(float sec)
    {
        m_TimePassed += sec;
        if(m_TimePassed/m_PlaySpeed > 1.0)
        {
            m_CurrentTime += std::max(0.0f,std::floor(m_TimePassed/m_PlaySpeed));
            m_TimePassed = 0;
            if(m_CurrentTime > m_TimeMax)
            {
                m_CurrentTime = m_TimeMin;
            }
            return true;
        }

        return false;

    }

    float GetTransparancy()
    {
        return m_Transparancy;
    }
    void SetTransparancy(float trans)
    {
        m_Transparancy = trans;
    }

    inline void CopyFrom(SPHStyle s)
    {
        m_Transparancy = s.m_Transparancy;
        m_ColorGradientb1 = s.m_ColorGradientb1;
        m_ColorGradientb2 = s.m_ColorGradientb2;
        m_Intervalb1 = s.m_Intervalb1;
        m_Intervalb2 = s.m_Intervalb2;
        m_Intervalb3 = s.m_Intervalb3;

        m_PointSize = s.m_PointSize;
        m_PointFillStyle = s.m_PointFillStyle;
        m_PointLineStyle = s.m_PointLineStyle;
        m_LineLineStyle = s.m_LineLineStyle;
        m_PolygonFillStyle = s.m_PolygonFillStyle;
        m_PolygonLineStyle = s.m_PolygonLineStyle;

        m_IndexB1 = s.m_IndexB1;
        m_IndexB2 = s.m_IndexB2;
        m_IndexB3 = s.m_IndexB3;

        m_CurrentTime = s.m_CurrentTime;
        m_PlaySpeed = s.m_PlaySpeed;
        m_Play = s.m_Play;

        //DEM properties
        SunColor = s.SunColor;
        DemScale = s.DemScale;
        angle_hor = s.angle_hor;
        angle_vert = s.angle_vert;
        m_IsDEM = s.m_IsDEM;
        m_IsHS = s.m_IsHS;
        m_IsVelocity = s.m_IsVelocity;
        m_IsFractional = s.m_IsFractional;
    }


    inline bool IsSimpleStyleGradient()
    {
        return m_StyleSimpleGradient;
    }

    inline bool IsSimpleStyleFill()
    {
        return m_StyleSimpleFillColor;
    }

    inline bool IsSimpleStyleLine()
    {
        return m_StyleSimpleLineColor;
    }

    inline bool IsSimpleStyleSize()
    {
        return m_StyleSimpleSize;
    }

    inline bool IsSimpleStyleRange()
    {
        return m_StyleSimpleRange;
    }


};

inline static SPHStyle GetStyleDefault(int style)
{


    SPHStyle s;
    if(style == SPH_STYLE_DEFAULT_RASTERHS)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(SPH_GRADIENT_HS);

    }else if(style == SPH_STYLE_DEFAULT_RASTERWH)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(SPH_GRADIENT_WH);


    }else if(style == SPH_STYLE_DEFAULT_RASTERUI)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(SPH_GRADIENT_UI);
    }else if(style == SPH_STYLE_DEFAULT_VECTORUI)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = false;

        s.m_HasVectorLineLine = true;
        s.m_HasVectorPointLine = true;
        s.m_HasVectorPolygonLine = true;
        s.m_HasVectorPointFill = true;
        s.m_HasVectorPolygonFill = true;

        SPHLineStyle ls = GetDefaultLineStyle(SPH_LINESTYLE_BLACK);
        SPHFillStyle fs = GetDefaultFillStyle(SPH_FILLSTYLE_BLACK);

        s.m_LineLineStyle = ls;
        s.m_PointLineStyle = ls;
        s.m_PolygonLineStyle = ls;
        s.m_PointFillStyle = fs;
        s.m_PolygonFillStyle = fs;
    }else if(style == SPH_STYLE_DEFAULT_MULTIRASTER)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = true;

        s.m_HasVectorLineLine = false;
        s.m_HasVectorPointLine = false;
        s.m_HasVectorPolygonLine = false;
        s.m_HasVectorPointFill = false;
        s.m_HasVectorPolygonFill = false;

        s.m_IndexB1 = 0;
        s.m_IndexB2 = 1;
        s.m_IndexB3 = 2;
    }if(style == SPH_STYLE_DEFAULT_POINTCLOUDUI)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(SPH_GRADIENT_UI);
    }else if(style == SPH_STYLE_DEFAULT_DUORASTER)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasDuoBand = true;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = false;
        s.m_IndexB1 = 0;
        s.m_IndexB2 = 1;
        s.m_IndexB3 = 2;
        s.m_ColorGradientb1 = GetDefaultGradient(SPH_GRADIENT_WH_FLUIDS);
        s.m_ColorGradientb2 = GetDefaultGradient(SPH_GRADIENT_WH_SOLIDS);
    }else if(style == SPH_STYLE_DEFAULT_DUORASTER_VEL)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasDuoBand = true;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = false;
        s.m_IsVelocity = true;
        s.m_IndexB1 = 0;
        s.m_IndexB2 = 1;
        s.m_IndexB3 = 2;
        s.m_ColorGradientb1 = GetDefaultGradient(SPH_GRADIENT_VEL_WHITE);
        s.m_ColorGradientb2 = GetDefaultGradient(SPH_GRADIENT_WH);
    }




    return s;
}

#endif // SPHSTYLE_H
