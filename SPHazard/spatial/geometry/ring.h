#pragma once

#include <vector>
#include "sph_vector2.h"
#include "shapealgebra.h"

#include "iostream"

class Ring
{

    std::vector<double> vertices;

   public:
    inline void AddVertex(double x, double y)
    {
        vertices.push_back(x);
        vertices.push_back(y);
    }

    inline double * GetVertexData()
    {
        return vertices.data();
    }

    inline int GetVertexCount()
    {
        return vertices.size()/2;
    }

    inline SPHVector2 GetVertex(int i)
    {
        return SPHVector2(vertices.at(i* 2),vertices.at(i* 2 + 1));

    }

    SPHVector2 GetClosestEdgeLocationP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double**x2 = nullptr, double **y2 = nullptr)
    {
        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(x2 != nullptr)
        {
            *x2 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }
        if(y2 != nullptr)
        {
            *y2 = nullptr;
        }


        SPHVector2 closest = SPHVector2(1e31,1e31);
        double mindist = 1e31;
        for(int i = 0; i < vertices.size() - 2; i += 2)
        {
            double lx1 = vertices[i +0];
            double lx2 = vertices[i +2];
            double ly1 = vertices[i +1];
            double ly2 = vertices[i +3];

            double retx =1e31;
            double rety =1e31;

            GetClosestPointOnLine(lx1,ly1,lx2,ly2,pos.x,pos.y, &retx, &rety);

            float distance = SPHVector2(pos - SPHVector2(retx,rety)).length();

            if(i == 0 || distance < mindist)
            {
                closest =SPHVector2(retx,rety);
                mindist = distance;
                if(x1 != nullptr)
                {
                    *x1 = &vertices[i +0];
                }
                if(x2 != nullptr)
                {
                    *x2 = &vertices[i +2];
                }
                if(y1 != nullptr)
                {
                    *y1 = &vertices[i +1];
                }
                if(y2 != nullptr)
                {
                    *y2 = &vertices[i +3];
                }

            }
        }

        return closest;
    }

    SPHVector2 GetClosestVertexP(SPHVector2 pos, double **x1 = nullptr, double**y1 = nullptr)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;

        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }

        for(int i = 0; i < vertices.size()-1; i += 2)
        {
            double lx1 = vertices[i +0];
            double ly1 = vertices[i +1];
            float distance = SPHVector2(pos - SPHVector2(lx1,ly1)).length();
            if(i == 0 || distance < mindist)
            {
                mindist = distance;
                closest = SPHVector2(lx1,ly1);
                if(x1 != nullptr)
                {
                    *x1 = &vertices[i +0];
                }
                if(y1 != nullptr)
                {
                    *y1 = &vertices[i +1];
                }

            }
        }

        return closest;
    }

    virtual void MoveVertex(double *x, double *y, SPHVector2 d)
    {

        for(int i = 0; i < ((int)vertices.size())-1; i += 2)
        {
            if(&vertices[i +0] == x)
            {
                vertices[i +0] += d.x;
            }
            if(&vertices[i +1] == y)
            {
                vertices[i +1] += d.y;
            }
        }
    }

    inline void Move(SPHVector2 d)
    {
        for(int i = 0; i < ((int)vertices.size())-1; i += 2)
        {
            vertices[i +0] += d.x;
            vertices[i +1] += d.y;
        }
    }

    SPHVector2 GetClosestEdgeLocation(SPHVector2 pos)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        double mindist = 1e31;
        for(int i = 0; i < vertices.size() - 2; i += 2)
        {
            double x1 = vertices[i +0];
            double x2 = vertices[i +2];
            double y1 = vertices[i +1];
            double y2 = vertices[i +3];

            double retx =1e31;
            double rety =1e31;

            GetClosestPointOnLine(x1,y1,x2,y2,pos.x,pos.y, &retx, &rety);

            float distance = SPHVector2(pos - SPHVector2(retx,rety)).length();

            if(i== 0||distance < mindist)
            {
                closest =SPHVector2(retx,rety);
                mindist = distance;
            }
        }

        return closest;
    }

    SPHVector2 GetClosestVertex(SPHVector2 pos)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;

        for(int i = 0; i < vertices.size()-1; i += 2)
        {
            double x1 = vertices[i +0];
            double y1 = vertices[i +1];
            float distance = SPHVector2(pos - SPHVector2(x1,y1)).length();
            if(i == 0)
            {
                mindist = distance;
                closest = SPHVector2(x1,y1);
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = SPHVector2(x1,y1);
            }
        }

        return closest;
    }

    inline Ring * Copy()
    {
        Ring * r = new Ring();

        r->vertices = this->vertices;
        return r;
    }
};

