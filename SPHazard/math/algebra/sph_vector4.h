#ifndef VECTOR4_H
#define VECTOR4_H

#include "math.h"

#include "sph_vector3.h"

class SPHVector4
{
public:

    float x;
    float y;
    float z;
    float w;

    inline SPHVector4()
    {


    }
    inline SPHVector4(float in_x, float in_y, float in_z, float in_w)
    {
        x = in_x;
        y = in_y;
        z = in_z;
        w = in_w;

    }

    inline SPHVector4 normalized()
    {
        double length = sqrt(x*x + y*y + z*z + w*w);
        SPHVector4 out;
        if(length > 0.0)
        {

            out.x = x/length;
            out.y = y/length;
            out.z = z/length;
            out.w = w/length;
        }
        return out;
    }

    inline SPHVector3 xyz()
    {
        SPHVector3 out;
        out.x = x;
        out.y = y;
        out.z = z;
        return out;
    }
};

#endif // VECTOR4_H
