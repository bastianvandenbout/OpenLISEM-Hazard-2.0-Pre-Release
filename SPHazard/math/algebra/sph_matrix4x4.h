#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "math.h"
#include "sph_vector3.h"
#include "iostream"
#include "sph_vector4.h"

class SPHMatrix4x4
{

    float data[4][4];


public:
    inline SPHMatrix4x4()
    {
        data[0][0] = 1.0f;
        data[0][1] = 0.0f;
        data[0][2] = 0.0f;
        data[0][3] = 0.0f;

        data[1][0] = 0.0f;
        data[1][1] = 1.0f;
        data[1][2] = 0.0f;
        data[1][3] = 0.0f;

        data[2][0] = 0.0f;
        data[2][1] = 0.0f;
        data[2][2] = 1.0f;
        data[2][3] = 0.0;

        data[3][0] = 0.0f;
        data[3][1] = 0.0f;
        data[3][2] = 0.0f;
        data[3][3] = 1.0f;


    }

    inline void SetIdentity()
    {
        data[0][0] = 1.0f;
        data[0][1] = 0.0f;
        data[0][2] = 0.0f;
        data[0][3] = 0.0f;

        data[1][0] = 0.0f;
        data[1][1] = 1.0f;
        data[1][2] = 0.0f;
        data[1][3] = 0.0f;

        data[2][0] = 0.0f;
        data[2][1] = 0.0f;
        data[2][2] = 1.0f;
        data[2][3] = 0.0;

        data[3][0] = 0.0f;
        data[3][1] = 0.0f;
        data[3][2] = 0.0f;
        data[3][3] = 1.0f;
    }

    inline void MultiplyBy(const SPHMatrix4x4 &other)
    {
        float temp[4][4];

        for (int row = 0; row < 4; ++row) {
                 for (int col = 0; col < 4; ++col) {
                     float sum(0.0f);
                     for (int j = 0; j < 4; ++j)
                     {
                         sum += data[j][row] * other.data[col][j];
                     }
                    temp[col][row] = sum;
                }
            }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                data[i][j] = temp[i][j];
            }
        }


    }

    inline SPHVector4 MultipliedBy(const SPHVector4 &other)
    {
        float temp[4];

        for (int row = 0; row < 4; ++row) {
                     float sum(0.0f);

                         sum += data[0][row] * other.x;
                         sum += data[1][row] * other.y;
                         sum += data[2][row] * other.z;
                         sum += data[3][row] * other.w;

                    temp[row] = sum;

            }

        SPHVector4 out;
        out.x = temp[0];
        out.y = temp[1];
        out.z = temp[2];
        out.w = temp[3];
        return out;
    }


    inline void Invert()
    {
        double m[16];

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[4*j+i] = data[i][j];
            }
        }
        double invOut[16];
        double inv[16], det;
            int i;

            inv[0] = m[5]  * m[10] * m[15] -
                     m[5]  * m[11] * m[14] -
                     m[9]  * m[6]  * m[15] +
                     m[9]  * m[7]  * m[14] +
                     m[13] * m[6]  * m[11] -
                     m[13] * m[7]  * m[10];

            inv[4] = -m[4]  * m[10] * m[15] +
                      m[4]  * m[11] * m[14] +
                      m[8]  * m[6]  * m[15] -
                      m[8]  * m[7]  * m[14] -
                      m[12] * m[6]  * m[11] +
                      m[12] * m[7]  * m[10];

            inv[8] = m[4]  * m[9] * m[15] -
                     m[4]  * m[11] * m[13] -
                     m[8]  * m[5] * m[15] +
                     m[8]  * m[7] * m[13] +
                     m[12] * m[5] * m[11] -
                     m[12] * m[7] * m[9];

            inv[12] = -m[4]  * m[9] * m[14] +
                       m[4]  * m[10] * m[13] +
                       m[8]  * m[5] * m[14] -
                       m[8]  * m[6] * m[13] -
                       m[12] * m[5] * m[10] +
                       m[12] * m[6] * m[9];

            inv[1] = -m[1]  * m[10] * m[15] +
                      m[1]  * m[11] * m[14] +
                      m[9]  * m[2] * m[15] -
                      m[9]  * m[3] * m[14] -
                      m[13] * m[2] * m[11] +
                      m[13] * m[3] * m[10];

            inv[5] = m[0]  * m[10] * m[15] -
                     m[0]  * m[11] * m[14] -
                     m[8]  * m[2] * m[15] +
                     m[8]  * m[3] * m[14] +
                     m[12] * m[2] * m[11] -
                     m[12] * m[3] * m[10];

            inv[9] = -m[0]  * m[9] * m[15] +
                      m[0]  * m[11] * m[13] +
                      m[8]  * m[1] * m[15] -
                      m[8]  * m[3] * m[13] -
                      m[12] * m[1] * m[11] +
                      m[12] * m[3] * m[9];

            inv[13] = m[0]  * m[9] * m[14] -
                      m[0]  * m[10] * m[13] -
                      m[8]  * m[1] * m[14] +
                      m[8]  * m[2] * m[13] +
                      m[12] * m[1] * m[10] -
                      m[12] * m[2] * m[9];

            inv[2] = m[1]  * m[6] * m[15] -
                     m[1]  * m[7] * m[14] -
                     m[5]  * m[2] * m[15] +
                     m[5]  * m[3] * m[14] +
                     m[13] * m[2] * m[7] -
                     m[13] * m[3] * m[6];

            inv[6] = -m[0]  * m[6] * m[15] +
                      m[0]  * m[7] * m[14] +
                      m[4]  * m[2] * m[15] -
                      m[4]  * m[3] * m[14] -
                      m[12] * m[2] * m[7] +
                      m[12] * m[3] * m[6];

            inv[10] = m[0]  * m[5] * m[15] -
                      m[0]  * m[7] * m[13] -
                      m[4]  * m[1] * m[15] +
                      m[4]  * m[3] * m[13] +
                      m[12] * m[1] * m[7] -
                      m[12] * m[3] * m[5];

            inv[14] = -m[0]  * m[5] * m[14] +
                       m[0]  * m[6] * m[13] +
                       m[4]  * m[1] * m[14] -
                       m[4]  * m[2] * m[13] -
                       m[12] * m[1] * m[6] +
                       m[12] * m[2] * m[5];

            inv[3] = -m[1] * m[6] * m[11] +
                      m[1] * m[7] * m[10] +
                      m[5] * m[2] * m[11] -
                      m[5] * m[3] * m[10] -
                      m[9] * m[2] * m[7] +
                      m[9] * m[3] * m[6];

            inv[7] = m[0] * m[6] * m[11] -
                     m[0] * m[7] * m[10] -
                     m[4] * m[2] * m[11] +
                     m[4] * m[3] * m[10] +
                     m[8] * m[2] * m[7] -
                     m[8] * m[3] * m[6];

            inv[11] = -m[0] * m[5] * m[11] +
                       m[0] * m[7] * m[9] +
                       m[4] * m[1] * m[11] -
                       m[4] * m[3] * m[9] -
                       m[8] * m[1] * m[7] +
                       m[8] * m[3] * m[5];

            inv[15] = m[0] * m[5] * m[10] -
                      m[0] * m[6] * m[9] -
                      m[4] * m[1] * m[10] +
                      m[4] * m[2] * m[9] +
                      m[8] * m[1] * m[6] -
                      m[8] * m[2] * m[5];

            det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

            if (!(det == 0.0f))
            {

                det = 1.0 / det;
            }

            for (i = 0; i < 16; i++)
            {
                invOut[i] = inv[i] * det;
            }

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    data[i][j] = invOut[4*j+i];
                }
            }

    }

    inline void SetTranslation(float transx, float transy, float transz)
    {
        data[0][0] = 1.0f;
        data[0][1] = 0.0f;
        data[0][2] = 0.0f;
        data[0][3] = transx;

        data[1][0] = 0.0f;
        data[1][1] = 1.0f;
        data[1][2] = 0.0f;
        data[1][3] = transy;

        data[2][0] = 0.0f;
        data[2][1] = 0.0f;
        data[2][2] = 1.0f;
        data[2][3] = transz;

        data[3][0] = 0.0f;
        data[3][1] = 0.0f;
        data[3][2] = 0.0f;
        data[3][3] = 1.0f;


    }

    inline void SetScaling(float scalex, float scaley, float scalez)
    {
        data[0][0] = scalex;
        data[0][1] = 0.0f;
        data[0][2] = 0.0f;
        data[0][3] = 0.0f;

        data[1][0] = 0.0f;
        data[1][1] = scaley;
        data[1][2] = 0.0f;
        data[1][3] = 0.0f;

        data[2][0] = 0.0f;
        data[2][1] = 0.0f;
        data[2][2] = scalez;
        data[2][3] = 0.0f;

        data[3][0] = 0.0f;
        data[3][1] = 0.0f;
        data[3][2] = 0.0f;
        data[3][3] = 1.0f;


    }

    inline void SetRotation()
    {

    }

    inline void SetOrtho(    float left,
                              float right,
                              float bottom,
                              float top,
                              float zNear,
                              float zFar)
    {        



        data[0][0] = 2.0f / (right - left);
        data[0][1] = 0.0f;
        data[0][2] = 0.0f;
        data[0][3] = 0.0f;

        data[1][0] = 0.0f;
        data[1][1] = 2.0f / (top - bottom);
        data[1][2] = 0.0f;
        data[1][3] = 0.0f;

        data[2][0] = 0.0f;
        data[2][1] = 0.0f;
        data[2][2] = - 2.0f / (zFar - zNear);
        data[2][3] = 0.0f;

        data[3][0] = - (right + left) / (right - left);
        data[3][1] = - (top + bottom) / (top - bottom);
        data[3][2] = - (zFar + zNear) / (zFar - zNear);
        data[3][3] = 1.0f;

    }

    inline void SetPerspective( float verticalAngle, float aspect, float nearplane, float farplane)
    {

        double radians = (verticalAngle / 2.0f) * 3.14159f/180.0f;
        float sine = sin(radians);
        float cotan = cos(radians) / sine;
        float clip = farplane - nearplane;

        data[0][0] = cotan / aspect;
        data[1][0] = 0.0f;
        data[2][0] = 0.0f;
        data[3][0] = 0.0f;

        data[0][1] = 0.0f;
        data[1][1] = cotan;
        data[2][1] = 0.0f;
        data[3][1] = 0.0f;

        data[0][2] = 0.0f;
        data[1][2] = 0.0f;
        data[2][2] = -(nearplane + farplane) / clip;
        data[3][2] = -(2.0f * nearplane * farplane) / clip;

        data[0][3] = 0.0f;
        data[1][3] = 0.0f;
        data[2][3] = -1.0f;
        data[3][3] = 0.0f;


    }

    inline void SetLookAt( SPHVector3 from, SPHVector3 to, SPHVector3 up)
    {
        SPHVector3 dir = (to-from).Normalize();

        SPHVector3 side = SPHVector3::CrossProduct(dir, up).Normalize()*-1.0;
        SPHVector3 upVector = SPHVector3::CrossProduct(dir,side).Normalize();

        data[0][0] = side.X();
        data[1][0] = side.Y();
        data[2][0] = side.Z();
        data[3][0] = 0.0f;
        data[0][1] = upVector.X();
        data[1][1] = upVector.Y();
        data[2][1] = upVector.Z();
        data[3][1] = 0.0f;
        data[0][2] = -dir.X();
        data[1][2] = -dir.Y();
        data[2][2] = -dir.Z();
        data[3][2] = 0.0f;
        data[0][3] = 0.0f;
        data[1][3] = 0.0f;
        data[2][3] = 0.0f;
        data[3][3] = 1.0f;

        Translate(from * -1.0f);

    }

    inline void Translate(SPHVector3 v)
    {
        float vx = v.X();
        float vy = v.Y();
        float vz = v.Z();

        data[3][0] += data[0][0] * vx + data[1][0] * vy + data[2][0] * vz;
        data[3][1] += data[0][1] * vx + data[1][1] * vy + data[2][1] * vz;
        data[3][2] += data[0][2] * vx + data[1][2] * vy + data[2][2] * vz;
        data[3][3] += data[0][3] * vx + data[1][3] * vy + data[2][3] * vz;

    }

    inline float * GetMatrixDataPtr()
    {
        float *ret = new float[16];

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                ret[4*i +j] = data[i][j];
            }
        }
        return ret;
    }

    inline float * GetMatrixDataTransposePtr()
    {
        float *ret = new float[16];

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                ret[4*j +i] = data[i][j];
            }
        }
        return ret;
    }


    inline float * GetMatrixData()
    {
        float *ret = new float[16];

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                ret[4*i +j] = data[i][j];
            }
        }
        return ret;
    }



    inline SPHMatrix4x4 operator*(const SPHMatrix4x4 &in)
    {
        SPHMatrix4x4 out = *(this);
        out.MultiplyBy(in);
        return out;
    }


    inline SPHVector4 operator*(const SPHVector4 &in)
    {
        SPHVector4 out = (in);
        out = MultipliedBy(in);
        return out;
    }

};

#endif // MATRIX4X4_H
