#ifndef GL2DCAMERA_H
#define GL2DCAMERA_H

#include "boundingbox.h"
#include "sph_vector2.h"
#include <algorithm>
class GL2DCamera
{

private:

    inline void UpdateCorners()
    {
        tlx = cx - 0.5f *sizex;
        brx = cx + 0.5f *sizex;
        tly = cy - 0.5f *sizey;
        bry = cy + 0.5f *sizey;
    }
public:

    float tlx;
    float tly;
    float brx;
    float bry;

    float cx;
    float cy;
    float sizex;
    float sizey;


    int screensizex;
    int screensizey;



    inline GL2DCamera()
    {
        cx = 0.0f;
        cy = 0.0f;
        sizex = 1.0f;
        sizey = 1.0f;
        screensizex = 1;
        screensizey = 1;
        UpdateCorners();
    }

    inline void LookAt(float in_x, float in_y, float in_sizex, float in_sizey)
    {

        cx = in_x;
        cy = in_y;
        sizex = in_sizex;
        sizey = in_sizey;

        UpdateCorners();
    }

    inline BoundingBox GetBoundingBox()
    {
        return BoundingBox(tlx,brx,tly,bry);
    }

    inline void LookAt(BoundingBox b)
    {
        cx = b.GetCenterX();
        cy = b.GetCenterY();
        sizex = b.GetSizeX();
        sizey = b.GetSizeY();

        float ratio = float(screensizey)/std::max(1.0f,float(screensizex));
        sizey = sizex *ratio;

        UpdateCorners();
    }

    inline void Move(float dx, float dy)
    {
        cx += dx;
        cy += dy;

        UpdateCorners();
    }

    inline void Resize(float r)
    {
        sizex = std::min(1000000000000.0f,std::max(0.0000000001f,sizex * r));
        sizey = std::min(1000000000000.0f,std::max(0.0000000001f,sizey * r));

        float ratio = float(screensizey)/std::max(1.0f,float(screensizex));
        sizey = sizex *ratio;

        UpdateCorners();
    }
    inline void SetScreenSize(int x, int y)
    {
        float ratio = float(y)/std::max(1.0f,float(x));
        screensizex = x;
        screensizey = y;
        sizey = sizex *ratio;

        UpdateCorners();

    }
    inline SPHVector2 GetScreenSize()
    {
        return SPHVector2(screensizex,screensizey);

    }
    inline SPHVector2 GetPosition()
    {
        return SPHVector2(cx,cy);
    }
    inline SPHVector2 GetSize()
    {
        return SPHVector2(sizex,sizey);
    }

};


#endif // GL3DCAMERA_H
