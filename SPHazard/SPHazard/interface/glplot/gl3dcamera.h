#ifndef GL3DCAMERA_H
#define GL3DCAMERA_H

#include "sph_vector.h"
#include "sph_vector2.h"
#include "sph_vector3.h"
#include "sph_matrix4x4.h"
#include "boundingbox.h"
#include "gl3delevationprovider.h"

class GL3DCamera
{
private:

    double m_ZNear;
    double m_ZFar;
    double m_FoV;


    SPHMatrix4x4 m_Projection;
    SPHMatrix4x4 m_ProjectionNormalZ;
    SPHMatrix4x4 m_CLookAtNoTranslationNormalZ;
    SPHMatrix4x4 m_CLookAtNoTranslation;
    SPHMatrix4x4 m_CLookAtNoTranslationXZ;
    SPHMatrix4x4 m_CLookAtNoTranslationInverse;
    SPHMatrix4x4 m_CameraMatrix;

    SPHVector3 m_Position;
    SPHVector3 m_ViewDir;
    SPHVector3 m_Up;
    SPHVector3 m_Right;

    double RotatedX;
    double RotatedY;
    double RotatedZ;
    double RotatedRight;

    double aspect;

    SPHVector2 m_viewportSize;


public:
    inline GL3DCamera()
    {
        m_ZNear = 0.1f;
        m_ZFar = 100000.0f;

        m_FoV = 75;

        m_Projection.SetIdentity();
        m_ProjectionNormalZ.SetIdentity();
        m_CLookAtNoTranslationNormalZ.SetIdentity();
        m_CLookAtNoTranslation.SetIdentity();
        m_CLookAtNoTranslationInverse.SetIdentity();
        m_CameraMatrix.SetIdentity();

        m_Position.setX(0);
        m_Position.setY(0);
        m_Position.setZ(0);
        m_ViewDir.setX(0);
        m_ViewDir.setY(0);
        m_ViewDir.setZ(1);
        m_Up.setX(0);
        m_Up.setY(1);
        m_Up.setZ(0);
        m_Right.setX(1);
        m_Right.setY(0);
        m_Right.setZ(0);

        RotatedX = 0;
        RotatedY = 0;
        RotatedZ = 0;
        RotatedRight = 0;
    }

    inline ~GL3DCamera()
    {

    }

    inline void SetZNear(float x)
    {
        m_ZNear = x;
    }
    inline void SetZFar(float x)
    {
        m_ZFar = x;
    }


    void ResizeViewPort(int w, int h);

    void SetCurrentMatrix();

    void SetRotation(double a, double b, double c);
    void SetPosition(double a, double b, double c);
    void Rotate(double a, double b, double c);
    void Move(double a, double b, double c);
    void RotateX(double Angle);
    void RotateAroundRight(double Angle);
    void RotateY(double Angle);
    void RotateZ(double Angle);
    void MoveForward(double Distance );
    void StrafeRight ( double Distance );
    void MoveUpward( double Distance );

    void LookAt(float x, float y, float z);
    void PlaceAndLookAtAuto(BoundingBox b);
    void Zoom(double dAngle);

    inline SPHVector3 GetPosition()
    {
        return m_Position;
    }

    inline SPHVector3 GetViewDir()
    {
        return m_ViewDir;
    }
    inline SPHVector3 GetUpDir()
    {
        return m_Up;
    }
    inline SPHVector3 GetRightDir()
    {
        return m_Right;
    }

    inline SPHMatrix4x4 GetProjectionMatrixNoTranslationXZ()
    {
        return m_CLookAtNoTranslationXZ;
    }

    inline SPHMatrix4x4 GetProjectionMatrix()
    {
        return m_CameraMatrix;
    }

    inline SPHMatrix4x4 GetProjectionMatrixNoTranslation()
    {
        return m_CLookAtNoTranslation;
    }
    inline SPHMatrix4x4 GetProjectionMatrixNoTranslationNormalZ()
    {
        return m_CLookAtNoTranslationNormalZ;
    }

    inline BoundingBox Get2DViewEquivalent(GL3DElevationProvider * p)
    {

        float cx = GetPosition().x;
        float cy = GetPosition().z;

        float elevation = p->GetElevation(cx,cy);
        if(elevation < -1e25 || !std::isfinite(elevation))
        {
            elevation  = 0.0;
        }
        float sizex = std::max(elevation + 0.01f,(float)GetPosition().y)- elevation;
        float sizey = std::max(elevation + 0.01f,(float)GetPosition().y) -elevation;

        BoundingBox a;
        a.Set(cx - 0.5 * sizex,cx + 0.5 * sizex,cy - 0.5 * sizey, cy + 0.5 * sizey);

        return a;
    }


    inline SPHVector4 GetRayFromWindow(float rx, float ry)
    {

        SPHVector2 normalizedCoords = GetNormalizedDeviceCoordinates(rx,ry);
        SPHVector4 clipCoords = SPHVector4(normalizedCoords.x,normalizedCoords.y,-1.0f,1.0f);
        SPHVector4 eyeCoords = GetEyeCoords(clipCoords);
        return eyeCoords.normalized();
    }

    inline SPHVector4 GetEyeCoords(SPHVector4 clipcoords)
    {
        SPHMatrix4x4 invproj = m_CLookAtNoTranslation;
        invproj.Invert();
        return invproj*clipcoords;
    }

    inline SPHVector2 GetNormalizedDeviceCoordinates(float mouse_x, float mouse_y)
    {
        return SPHVector2(2.0f * mouse_x/m_viewportSize.x - 1.0f,2.0f * (m_viewportSize.y-mouse_y)/m_viewportSize.y - 1.0f);
    }

};

#endif // GL3DCAMERA_H
