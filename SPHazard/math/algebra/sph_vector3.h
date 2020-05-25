#ifndef VECTOR3_H
#define VECTOR3_H

#include "math.h"
#include "QString"
#include "QStringList"

class SPHVector3
{
    public:

    float x = 0;
    float y = 0;
    float z = 0;


    inline SPHVector3()
    {


    }
    inline SPHVector3(float in_x, float in_y, float in_z)
    {
        x = in_x;
        y = in_y;
        z = in_z;

    }

    inline float X()
    {
        return x;
    }
    inline float Y()
    {
        return y;
    }
    inline float Z()
    {
        return z;
    }

    inline void setX(float in_x)
    {
        x = in_x;
    }
    inline void setY(float in_x)
    {
        y = in_x;
    }
    inline void setZ(float in_x)
    {
        z = in_x;
    }

    inline void Substract(SPHVector3 vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
    }
    inline float length()
    {
        double lsq = (x*x +y*y +z*z);
        return lsq > 0.0f? sqrt(lsq):0.0f;
    }

    inline SPHVector3 Normalize()
    {
        SPHVector3 out;
        out.x = x;
        out.y = y;
        out.z = z;
        return out/length();

    }

    inline float Angle(SPHVector3 other)
    {
        float x1 = x;
        float x2 = other.x;
        float y1 = y;
        float y2 = other.y;
        float z1 = z;
        float z2 = other.z;


        float dot = x1*x2 + y1*y2 + z1*z2;
        float lenSq1 = x1*x1 + y1*y1 + z1*z1;
        float lenSq2 = x2*x2 + y2*y2 + z2*z2;
        if(lenSq1 * lenSq2 > 0.0)
        {
            return acos(dot/sqrt(lenSq1 * lenSq2));
        }else {
            return 0.0;
        }
    }

    inline static SPHVector3 CrossProduct(SPHVector3 v1, SPHVector3 v2)
    {
        SPHVector3 out;
        out.x = v1.y * v2.z - v1.z * v2.y;
        out.y = v1.z * v2.x - v1.x * v2.z;
        out.z = v1.x * v2.y - v1.y * v2.x;

        return out;
    }

    SPHVector3 operator+(const SPHVector3 & in)
    {
        SPHVector3 out;
        out.x = x + in.x;
        out.y = y + in.y;
        out.z = z + in.z;
        return out;
    }
    SPHVector3 operator-(const SPHVector3 & in)
    {
        SPHVector3 out;
        out.x = x - in.x;
        out.y = y - in.y;
        out.z = z - in.z;
        return out;
    }

    SPHVector3 operator*(const float & in)
    {
        SPHVector3 out;
        out.x = x * in;
        out.y = y * in;
        out.z = z * in;
        return out;
    }
    SPHVector3 operator/(const float & in)
    {
        SPHVector3 out;
        out.x = x / in;
        out.y = y / in;
        out.z = z / in;
        return out;
    }

    inline QString toString() const
    {
        return QString(QString::number(x) + ";" + QString::number(y) + ";" + QString::number(z));
    }

    inline void fromString(QString in)
    {
        QStringList l = in.split(";");
        if(l.length() == 3)
        {
            x = l.at(0).toDouble();
            y = l.at(1).toDouble();
            z = l.at(2).toDouble();
        }else {
            x = 0.0;
            y = 0.0;
            z = 0.0;
        }
    }
};

#endif // VECTOR3_H
