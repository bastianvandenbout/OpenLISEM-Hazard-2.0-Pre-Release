#ifndef VECTOR2_H
#define VECTOR2_H

#include  "math.h"

class SPHVector2
{
public:

    float x;
    float y;

    inline SPHVector2()
    {
        x = 0.0f;
        y = 0.0f;
    }

    inline SPHVector2(float in_x, float in_y)
    {
        x = in_x;
        y = in_y;
    }

    float dot(SPHVector2 other)
    {
        return x * other.x + y * other.y;
    }

    inline float X()
    {
        return x;
    }

    inline float Y()
    {
        return y;
    }

    inline float length()
    {
        double lsq = (x*x +y*y);
        return lsq > 0.0f? sqrt(lsq):0.0f;
    }

    SPHVector2 operator+(const SPHVector2 & in)
    {
        SPHVector2 out;
        out.x = x + in.x;
        out.y = y + in.y;
        return out;
    }
    SPHVector2 operator-(const SPHVector2 & in)
    {
        SPHVector2 out;
        out.x = x - in.x;
        out.y = y - in.y;
        return out;
    }

    SPHVector2 operator*(const float & in)
    {
        SPHVector2 out;
        out.x = x * in;
        out.y = y * in;
        return out;
    }
    SPHVector2 operator/(const float & in)
    {
        SPHVector2 out;
        out.x = x / in;
        out.y = y / in;
        return out;
    }

};

#endif // VECTOR2_H
