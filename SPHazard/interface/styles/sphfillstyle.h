#ifndef SPHFILLSTYLE_H
#define SPHFILLSTYLE_H
#include "colorf.h"
#include "sphcolorgradient.h"

#include "sph_interval.h"


struct SPHFillStyle
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

    bool m_IsLines = false;
    float m_LineSize = 1.0;
    float m_LineSeperation = 1.0;
    float m_LineAngle = 0.0;

    bool m_IsShapes = false;
    int m_ShapeType =0;
    float m_ShapeSize = 1;
    float m_ShapeSeperation = 1;

    bool  m_ScreenUnits;

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

        SPHSerialize::From(obj,"IsLines",m_IsLines);
        SPHSerialize::From(obj,"LineSize",m_LineSize);
        SPHSerialize::From(obj,"LineSeperation",m_LineSeperation);
        SPHSerialize::From(obj,"LineAngle",m_LineAngle);
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

        SPHSerialize::To(obj,"IsLines",m_IsLines);
        SPHSerialize::To(obj,"LineSize",m_LineSize);
        SPHSerialize::To(obj,"LineSeperation",m_LineSeperation);
        SPHSerialize::To(obj,"LineAngle",m_LineAngle);
        SPHSerialize::To(obj,"ShapeType",m_ShapeType);
        SPHSerialize::To(obj,"ShapeSize",m_ShapeSize);
        SPHSerialize::To(obj,"ShapeSeperation",m_ShapeSeperation);
        SPHSerialize::To(obj,"UseScreenUnits",m_ScreenUnits);
    }

};


#define SPH_FILLSHAPE_NUM 3

#define SPH_FILLSHAPE_SQUARE 0
#define SPH_FILLSHAPE_CIRCLE 1
#define SPH_FILLSHAPE_DIAMOND 2

#define SPH_FILLSTYLE_NUM 14

#define SPH_FILLSTYLE_BLACK 0
#define SPH_FILLSTYLE_RED 1
#define SPH_FILLSTYLE_BLUE 2
#define SPH_FILLSTYLE_BLACKLINES 3
#define SPH_FILLSTYLE_REDLINES 4
#define SPH_FILLSTYLE_BLUELINES 5
#define SPH_FILLSTYLE_BLACKLINESBACKGROUND 6
#define SPH_FILLSTYLE_BLACKDOTTED 7
#define SPH_FILLSTYLE_REDDOTTED 8
#define SPH_FILLSTYLE_BLUEDOTTED 9
#define SPH_FILLSTYLE_BLACKDOTBACKGROUND 10
#define SPH_FILLSTYLE_BLACKLINESANGLE1 11
#define SPH_FILLSTYLE_BLACKLINESANGLE2 12
#define SPH_FILLSTYLE_BLACKLINESANGLE3 13

static inline SPHFillStyle GetDefaultFillStyle(int g)
{

    SPHFillStyle s;


    if(g == SPH_FILLSTYLE_BLACK)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.5);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == SPH_FILLSTYLE_RED)
    {

        s.m_Color1 = ColorF(1.0,0.0,0.0,0.5);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == SPH_FILLSTYLE_BLUE)
    {

        s.m_Color1 = ColorF(0.0,0.0,1.0,0.5);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == SPH_FILLSTYLE_BLACKLINES)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.67);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == SPH_FILLSTYLE_BLUELINES)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,0.67);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == SPH_FILLSTYLE_REDLINES)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,0.67);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == SPH_FILLSTYLE_BLACKLINESBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == SPH_FILLSTYLE_BLACKDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = SPH_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == SPH_FILLSTYLE_REDDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = SPH_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == SPH_FILLSTYLE_BLUEDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = SPH_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == SPH_FILLSTYLE_BLACKDOTBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = SPH_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == SPH_FILLSTYLE_BLACKLINESANGLE1)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 90;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }
    else if(g == SPH_FILLSTYLE_BLACKLINESANGLE2)
        {

            s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
            s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
            s.m_IsLines = true;
            s.m_LineSize = 1;
            s.m_LineAngle = 135;
            s.m_LineSeperation = 3;
            s.m_HasBackGroundColor = false;
            return s;
        }
    else if(g == SPH_FILLSTYLE_BLACKLINESANGLE3)
        {

            s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
            s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
            s.m_IsLines = true;
            s.m_LineSize = 1;
            s.m_LineAngle = 45;
            s.m_LineSeperation = 3;
            s.m_HasBackGroundColor = false;
            return s;
        }



    return GetDefaultFillStyle(SPH_FILLSTYLE_BLACK);




}


#endif // SPHFILLSTYLE_H
