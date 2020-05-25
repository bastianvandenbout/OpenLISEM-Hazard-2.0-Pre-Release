#ifndef SPHLINESTYLE_H
#define SPHLINESTYLE_H
#include "colorf.h"
#include "sph_interval.h"
#include "QJsonObject"
#include "serialization.h"
#include "sphcolorgradient.h"

struct SPHLineStyle
{
    bool m_DoDraw = false;

    bool m_HasBackGroundColor = false;
    ColorF m_Color1;
    ColorF m_Color2;
    QString m_Color1Parameter;
    QString m_Color2Parameter;
    SPHInterval m_Color1Interval;
    SPHInterval m_Color2Interval;
    SPHColorGradient m_ColorGradient1;
    SPHColorGradient m_ColorGradient2;


    QString m_LineWParameter;
    SPHInterval m_LineWInterval;
    float m_LineWMax = 2.0;
    bool m_HasPattern = false;
    QList<float> pattern;
    int m_ShapeType =0;
    float m_ShapeSize = 1;
    float m_ShapeSeperation = 1;
    bool m_ScreenUnits = false;

    inline void Save(QJsonObject * obj)
    {
        SPHSerialize::From(obj,"Draw",m_DoDraw);
        SPHSerialize::From(obj,"HasBackGroundColor",m_HasBackGroundColor);
        SPHSerialize::From(obj,"Color1",m_Color1);
        SPHSerialize::From(obj,"Color2",m_Color2);
        SPHSerialize::From(obj,"Color1Parameter",m_Color1Parameter);
        SPHSerialize::From(obj,"Color2Parameter",m_Color2Parameter);
        SPHSerialize::From(obj,"Color1Interval",m_Color1Interval);
        SPHSerialize::From(obj,"Color2Interval",m_Color2Interval);
        SPHSerialize::FromObject<SPHColorGradient>(obj,"ColorGradient1",m_ColorGradient1);
        SPHSerialize::FromObject<SPHColorGradient>(obj,"ColorGradient2",m_ColorGradient2);

        SPHSerialize::From(obj,"LineWParameter",m_LineWParameter);
        SPHSerialize::From(obj,"LineWInterval",m_LineWInterval);
        SPHSerialize::From(obj,"LineWMax",m_LineWMax);
        SPHSerialize::From(obj,"HasPattern",m_HasPattern);
        SPHSerialize::From(obj,"Pattern",pattern);
        SPHSerialize::From(obj,"ShapeType",m_ShapeType);
        SPHSerialize::From(obj,"ShapeSize",m_ShapeSize);
        SPHSerialize::From(obj,"ShapeSeperation",m_ShapeSeperation);
        SPHSerialize::From(obj,"UseScreenUnits",m_ScreenUnits);
    }

    inline void Restore(QJsonObject * obj)
    {
        SPHSerialize::To(obj,"Draw",m_DoDraw);
        SPHSerialize::To(obj,"HasBackGroundColor",m_HasBackGroundColor);
        SPHSerialize::To(obj,"Color1",m_Color1);
        SPHSerialize::To(obj,"Color2",m_Color2);
        SPHSerialize::To(obj,"Color1Parameter",m_Color1Parameter);
        SPHSerialize::To(obj,"Color2Parameter",m_Color2Parameter);
        SPHSerialize::To(obj,"Color1Interval",m_Color1Interval);
        SPHSerialize::To(obj,"Color2Interval",m_Color2Interval);
        SPHSerialize::ToObject<SPHColorGradient>(obj,"ColorGradient1",m_ColorGradient1);
        SPHSerialize::ToObject<SPHColorGradient>(obj,"ColorGradient2",m_ColorGradient2);

        SPHSerialize::To(obj,"LineWParameter",m_LineWParameter);
        SPHSerialize::To(obj,"LineWInterval",m_LineWInterval);
        SPHSerialize::To(obj,"LineWMax",m_LineWMax);
        SPHSerialize::To(obj,"HasPattern",m_HasPattern);
        SPHSerialize::To(obj,"Pattern",pattern);
        SPHSerialize::To(obj,"ShapeType",m_ShapeType);
        SPHSerialize::To(obj,"ShapeSize",m_ShapeSize);
        SPHSerialize::To(obj,"ShapeSeperation",m_ShapeSeperation);
        SPHSerialize::To(obj,"UseScreenUnits",m_ScreenUnits);
    }

};

#define SPH_LINESHAPE_NUM 11

#define SPH_LINESHAPE_SQUARE 0
#define SPH_LINESHAPE_CIRCLE 1
#define SPH_LINESHAPE_DIAMOND 2

#define SPH_LINESTYLE_NUM 12

#define SPH_LINESTYLE_BLACK 0
#define SPH_LINESTYLE_RED 1
#define SPH_LINESTYLE_BLUE 2
#define SPH_LINESTYLE_BLACKDASHED 3
#define SPH_LINESTYLE_REDDASHED 4
#define SPH_LINESTYLE_BLUEDASHED 5
#define SPH_LINESTYLE_BLACKDASHEDBACKGROUND 6
#define SPH_LINESTYLE_BLACKDOTTED 7
#define SPH_LINESTYLE_REDDOTTED 8
#define SPH_LINESTYLE_BLUEDOTTED 9
#define SPH_LINESTYLE_BLACKDOTBACKGROUND 10

static inline SPHLineStyle GetDefaultLineStyle(int g)
{

    SPHLineStyle s;


    if(g == SPH_LINESTYLE_BLACK)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = false;
        return s;
    }else if(g == SPH_LINESTYLE_RED)
    {

        s.m_Color1 = ColorF(1.0,0.0,0.0,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = false;
        return s;
    }else if(g == SPH_LINESTYLE_BLUE)
    {

        s.m_Color1 = ColorF(0.0,0.0,1.0,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = false;
        return s;
    }else if(g == SPH_LINESTYLE_BLACKDASHED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_SQUARE;
        return s;
    }else if(g == SPH_LINESTYLE_BLUEDASHED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_SQUARE;
        return s;
    }else if(g == SPH_LINESTYLE_REDDASHED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_SQUARE;
        return s;
    }else if(g == SPH_LINESTYLE_BLACKDASHEDBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_SQUARE;
        return s;
    }else if(g == SPH_LINESTYLE_BLACKDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_CIRCLE;
        return s;
    }else if(g == SPH_LINESTYLE_BLUEDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_CIRCLE;
        return s;
    }else if(g == SPH_LINESTYLE_REDDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_CIRCLE;
        return s;
    }else if(g == SPH_LINESTYLE_BLACKDOTBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = SPH_LINESHAPE_CIRCLE;        return s;
    }

    return GetDefaultLineStyle(SPH_LINESTYLE_BLACK);




}

#endif // SPHLINESTYLE_H
