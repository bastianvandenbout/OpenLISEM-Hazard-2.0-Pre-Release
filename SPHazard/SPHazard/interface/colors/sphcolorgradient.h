#ifndef SPHCOLORGRADIENT_H
#define SPHCOLORGRADIENT_H

#include "iostream"
#include "colorf.h"
#include "QList"
#include "math.h"
#include "QJsonObject"
#include "serialization.h"
#include "iostream"

class SPHColorGradient
{
public:

    QList<float> m_Gradient_Stops;
    QList<ColorF> m_Gradient_Colors;

    inline SPHColorGradient(QList<float> stops, QList<ColorF> colors)
    {
        m_Gradient_Stops = stops;
        m_Gradient_Colors = colors;
    }
    inline SPHColorGradient()
    {

    }
    inline void MakeValid()
    {

        if(m_Gradient_Stops.length() < 2)
        {

            m_Gradient_Stops.clear();
            m_Gradient_Colors.clear();
            m_Gradient_Stops.append(0.0);
            m_Gradient_Stops.append(1.0);
            m_Gradient_Colors.append(ColorF(0.0,0.0,0.0,1.0));
            m_Gradient_Colors.append(ColorF(1.0,1.0,1.0,1.0));
        }

        if(m_Gradient_Colors.length() < m_Gradient_Stops.length())
        {

        }
    }

    inline ColorF MixColors(ColorF c1, ColorF c2, float fac)
    {
        ColorF ret;
        ret.r = fac * c2.r + (1.0-fac) * c1.r;
        ret.g = fac * c2.g + (1.0-fac) * c1.g;
        ret.b = fac * c2.b + (1.0-fac) * c1.b;
        ret.a = fac * c2.a + (1.0-fac) * c1.a;

                return ret;
    }

    inline ColorF ColorAt(float stop)
    {
        if(m_Gradient_Stops.length() > 0)
        {
            for(int i = 0; i < m_Gradient_Stops.length(); i++)
            {
                if(m_Gradient_Stops.at(i) > stop)
                {
                    if(i == 0)
                    {
                        return m_Gradient_Colors.at(0);
                    }else {
                        return MixColors(m_Gradient_Colors.at(i-1),m_Gradient_Colors.at(i),(stop -m_Gradient_Stops.at(i-1))/std::max(0.000001f,m_Gradient_Stops.at(i)-m_Gradient_Stops.at(i-1)));
                    }
                }
            }

            return m_Gradient_Colors.at(m_Gradient_Colors.length()-1);
        }
        return ColorF(0,0,0,1);

    }

    inline SPHColorGradient Invert()
    {
        SPHColorGradient gradient;
        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;

        int length = m_Gradient_Colors.length();
        for(int i = 0; i < m_Gradient_Colors.length(); i++)
        {
            if(m_Gradient_Stops.at(i) > 1.0)
            {
                length = i+1;
                break;
            }
        }

        for(int k = 0; k < (length/2); k++) gradient.m_Gradient_Colors.swap(k,length-(1+k));

        return gradient;

    }

    inline SPHColorGradient ApplyLog()
    {
        SPHColorGradient gradient;

        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;

        for(int i = 0; i < gradient.m_Gradient_Colors.length(); i++)
        {
            if(gradient.m_Gradient_Stops.at(i) < 1.0 && gradient.m_Gradient_Stops.at(i) > 0.0)
            {
                float stop_new = std::min(1.0,std::max(0.0,std::log((gradient.m_Gradient_Stops.at(i)*100.0))/100.0));
                gradient.m_Gradient_Stops.replace(i,stop_new);
            }
        }

        return gradient;

    }
    inline SPHColorGradient ApplyInvLog()
    {
        SPHColorGradient gradient;


        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;

        for(int i = 0; i < gradient.m_Gradient_Colors.length(); i++)
        {
            if(gradient.m_Gradient_Stops.at(i) < 1.0 && gradient.m_Gradient_Stops.at(i) > 0.0)
            {
                float stop_new = std::exp((gradient.m_Gradient_Stops.at(i)*100.0))/100.0;
                gradient.m_Gradient_Stops.replace(i,stop_new);
            }
        }

        return gradient;

    }

    inline SPHColorGradient ToggleTransparancy()
    {
        SPHColorGradient gradient;
        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;

        return gradient;


    }

    inline void Save(QJsonObject * obj)
    {
        SPHSerialize::FromQListFloat(obj,"ColorStops",m_Gradient_Stops);

        QList<QString> colors;
        for(int i = 0; i < m_Gradient_Colors.length(); i++)
        {
            colors.append(m_Gradient_Colors.at(i).toString());
        }

        SPHSerialize::FromQListString(obj,"Colors",colors);


    }

    inline void Restore(QJsonObject * obj)
    {
        std::cout << "restore color gradient" << std::endl;
        m_Gradient_Stops.clear();
        m_Gradient_Colors.clear();
        SPHSerialize::ToQListFloat(obj,"ColorStops",m_Gradient_Stops);

        QList<QString> colors;
        SPHSerialize::ToQListString(obj,"Colors",colors);

        for(int i = 0;i < colors.length(); i++)
        {
            ColorF c;
            c.fromString(colors.at(i));
            m_Gradient_Colors.append(c);
        }

        std::cout << "count " << m_Gradient_Stops.length() << " " << m_Gradient_Colors.length() << std::endl;

    }
};


#define SPHAZARD_SHADER_GRADIENT_NCOLORS 10

const static float COLORGRADIENT_DEFEAULT_WH_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,0.001,0.01,0.025,0.05,0.1,0.25,0.5,0.75,1.0};

const static ColorF COLORGRADIENT_DEFEAULT_WH_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(0.9019,0.9764,1.0,0.0),
    ColorF(0.9019,0.9764,1.0,0.25),
    ColorF(0.0,1.0,1.0,0.5),
    ColorF(0.0,0.8,1.0,0.75),
    ColorF(0.0,0.6,1.0,1.0),
    ColorF(0.0,0.2,0.8,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(0.5,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.0,1.0)

};

const static float COLORGRADIENT_DEFEAULT_UI_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.725,0.85,1.0};

const static ColorF COLORGRADIENT_DEFEAULT_UI_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(53,3,355,255),
    ColorF(33,101,112,255),
    ColorF(115,238,255,255),
    ColorF(11,255,3,255),
    ColorF(171,255,3,255),
    ColorF(255,221,3,255),
    ColorF(255,150,3,255),
    ColorF(255,11,3,255),
    ColorF(255,3,255,255),
    ColorF(50,50,50,255)

};

const static float COLORGRADIENT_DEFEAULT_GRAYSCALE_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_GRAYSCALE_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};

const static float COLORGRADIENT_DEFEAULT_RYG_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,0.5,1.0,2.0,2.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_RYG_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,0.0,1.0),
    ColorF(0.0,1.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_ELEVATION_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_ELEVATION_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_RBGB_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,0.33,0.66,1.0,2.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_RBGB_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(0.8,0.2,0.2,1.0),
    ColorF(0.0,1.0,0.0,1.0),
    ColorF(0.0,0.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_BLUES_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_BLUES_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_REDS_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_REDS_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(0.0,0.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_MAGMA_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,0.2,0.4,0.6,0.8,1.0,2.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_MAGMA_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.2,0.1,0.4,1.0),
    ColorF(0.65,0.12,0.3,1.0),
    ColorF(0.95,0.4,0.12,1.0),
    ColorF(0.96,0.97,0.75,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};

const static float COLORGRADIENT_DEFEAULT_FLUIDS_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,0.001,0.05,0.15,0.3,0.6,1.0,2.0,2.0,2.0};

const static ColorF COLORGRADIENT_DEFEAULT_FLUIDS_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(0.9019,0.9764,1.0,0.0),
    ColorF(0.9019,0.9764,1.0,0.25),
    ColorF(0.0,1.0,1.0,0.5),
    ColorF(0.0,0.8,1.0,0.75),
    ColorF(0.0,0.6,1.0,1.0),
    ColorF(0.0,0.2,0.8,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.5,1.0)

};


const static float COLORGRADIENT_DEFEAULT_SOLIDS_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,0.1,0.3,0.5,0.75,1.0,2.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_SOLIDS_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,0.0),
    ColorF(1.0,1.0,0.0,1.0),
    ColorF(1.0,0.6,0.0,1.0),
    ColorF(0.65,0.4,0.0,1.0),
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};


const static float COLORGRADIENT_DEFEAULT_VEL_WHITE_STOPS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.6,0.8,1.0,2.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_VEL_WHITE_COLORS[SPHAZARD_SHADER_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,0.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};





#define SPH_GRADIENT_NUM 12

#define SPH_GRADIENT_WH 0
#define SPH_GRADIENT_HS 1
#define SPH_GRADIENT_UI 2
#define SPH_GRADIENT_RYG 3
#define SPH_GRADIENT_ELEVATION 4
#define SPH_GRADIENT_RBGB 5
#define SPH_GRADIENT_BLUES 6
#define SPH_GRADIENT_REDS 7
#define SPH_GRADIENT_MAGMA 8
#define SPH_GRADIENT_WH_FLUIDS 9
#define SPH_GRADIENT_WH_SOLIDS 10
#define SPH_GRADIENT_VEL_WHITE 11

static inline SPHColorGradient GetDefaultGradient(int g)
{

    SPHColorGradient grad;

    if(g == SPH_GRADIENT_WH)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_WH_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_WH_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_HS)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_GRAYSCALE_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_GRAYSCALE_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_UI)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_UI_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_UI_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_RYG)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_RYG_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_RYG_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_ELEVATION)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_ELEVATION_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_ELEVATION_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_RBGB)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_RBGB_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_RBGB_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_BLUES)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_BLUES_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_BLUES_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_REDS)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_REDS_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_REDS_COLORS[i]);
        }
    }else if(g == SPH_GRADIENT_MAGMA)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_MAGMA_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_MAGMA_COLORS[i]);
        }

    }else if(g == SPH_GRADIENT_WH_FLUIDS)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_FLUIDS_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_FLUIDS_COLORS[i]);
        }

    }else if(g == SPH_GRADIENT_WH_SOLIDS)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_SOLIDS_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_SOLIDS_COLORS[i]);
        }

    }else if(g == SPH_GRADIENT_VEL_WHITE)
    {
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_VEL_WHITE_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_VEL_WHITE_COLORS[i]);
        }

    }else {//default grayscale
        for(int i = 0; i < SPHAZARD_SHADER_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_UI_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_UI_COLORS[i]);
        }
    }

    return grad;

}
#endif // SPHCOLORGRADIENT_H
