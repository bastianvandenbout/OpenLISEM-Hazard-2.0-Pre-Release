#ifndef SPH_INTERVAL_H
#define SPH_INTERVAL_H

#include "QString"
#include "QStringList"
#include "serialization.h"

class SPHInterval : public SPHSerialize::SPHStringable
{
    float m_min = 0.0f;
    float m_max = 0.0f;
public:
    void Set(float min, float max)
    {
        m_min = min;
        m_max= max;
    }
    void SetMax(float x)
    {
        m_max = x;
    }
    void SetMin(float x)
    {
        m_min = x;
    }
    float GetMin()
    {
        return m_min;
    }
    float GetMax()
    {
        return m_max;
    }

    inline QString toString() const override
    {
        return QString(QString::number(m_min) + ";" + QString::number(m_max));
    }

    inline void fromString(QString x) override
    {
        QStringList l = x.split(";");
        if(l.length() == 2)
        {
            m_min = l.at(0).toDouble();
            m_max = l.at(1).toDouble();
        }else {
            m_min =0;
            m_max = 0;
        }
    }

};

class SPHInterval2D
{
    float m_minx;
    float m_maxx;
    float m_miny;
    float m_maxy;

};

class SPHInterval3D
{
    float m_minx;
    float m_maxx;
    float m_miny;
    float m_maxy;
    float m_minz;
    float m_maxz;

};
#endif // SPH_INTERVAL_H
