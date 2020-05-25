#pragma once

#include "shape.h"
#include "shapealgebra.h"
#include "iostream"

class SPHLine: public SPHShape
{

    std::vector<double> m_vertices;

    BoundingBox m_BoundingBox;

public:
    inline SPHLine() : SPHShape(SPH_SHAPE_LINE)
    {
    }

    inline void AddVertex(double x, double y)
    {
        m_vertices.push_back(x);
        m_vertices.push_back(y);

        if(m_vertices.size() == 1)
        {
            m_BoundingBox.SetAs(SPHVector2(x,y));
        }else {
            m_BoundingBox.MergeWith(SPHVector2(x,y));
        }
    }

    inline void SetAsRegularNGon(SPHVector2 center, double radius, int n_points = 25)
    {

        m_vertices.clear();
        for(int i = 0; i < n_points; i++)
        {
            double thetai = float(i) * 2.0 * 3.14159/((float)(n_points));
            double thetaip = float(i+1) * 2.0 * 3.14159/((float)(n_points));

            double x_i = center.x + radius*sin(thetai);
            double y_i = center.y + radius*cos(thetai);

            double x_ip = center.x + radius*sin(thetaip);
            double y_ip = center.y + radius*cos(thetaip);

            AddVertex(x_i,y_i);
        }

    }

    inline void SetAsSquare(SPHVector2 tl, SPHVector2 br)
    {
        m_vertices.clear();
        AddVertex(tl.x,tl.Y());
        AddVertex(tl.x,br.Y());
        AddVertex(br.x,br.Y());
        AddVertex(br.x,tl.Y());
        AddVertex(tl.x,tl.Y());
    }

    inline int GetVertexCount()
    {
        return m_vertices.size()/2;
    }

    inline double* GetVertexData()
    {
        return m_vertices.data();
    }

    inline SPHVector2 GetVertex(int i)
    {
        return SPHVector2(m_vertices.at(i*2),m_vertices.at(i*2 +1));
    }

    /*inline void getLine(double x1, double y1, double x2, double y2, double &a, double &b, double &c)
    {
        // (x- p1X) / (p2X - p1X) = (y - p1Y) / (p2Y - p1Y)
        a = y1 - y2;
        b = x2 - x1;
        c = x1 * y2 - x2 * y1;
    }

    inline double LineDist(double pct1X, double pct1Y, double pct2X, double pct2Y, double pct3X, double pct3Y)
    {
        double a, b, c;
        getLine(pct2X, pct2Y, pct3X, pct3Y, a, b, c);
        return abs(a * pct1X + b * pct1Y + c) / sqrt(a * a + b * b);
    }*/

    inline double LineDist(double pct1X, double pct1Y, double pct2X, double pct2Y, double pct3X, double pct3Y)
    {
        double retx =1e31;
        double rety =1e31;

        GetClosestPointOnLine(pct2X,pct2Y,pct3X,pct3Y,pct1X,pct1Y, &retx, &rety);

        if(std::isnan(retx) || std::isnan(rety))
        {
            retx =1e30;
            rety =1e30;
        }

        return (SPHVector2(retx,rety) - SPHVector2(pct1X,pct1Y)).length();


    }

    inline double GetDistanceToPoint(SPHVector2 pos)
    {
        double mindist = 1e31;
        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double x1 = m_vertices[i +0];
            double x2 = m_vertices[i +2];
            double y1 = m_vertices[i +1];
            double y2 = m_vertices[i +3];

            mindist = std::min((double)(LineDist(pos.x,pos.y,x1,y1,x2,y2)),mindist);
        }
        return mindist;
    }

    SPHVector2 GetClosestEdgeLocationP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
    {
        SPHVector2 closest = SPHVector2(1e30,1e30);
        double mindist = 1e30;

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

        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double lx1 = m_vertices[i +0];
            double lx2 = m_vertices[i +2];
            double ly1 = m_vertices[i +1];
            double ly2 = m_vertices[i +3];

            double retx =lx1;
            double rety =ly1;

            GetClosestPointOnLine(lx1,ly1,lx2,ly2,pos.x,pos.y, &retx, &rety);

            float distance = SPHVector2(pos - SPHVector2(retx,rety)).length();

                if(distance < mindist)
                {
                    if(x1 != nullptr)
                    {
                        *x1 = &m_vertices[i +0];
                    }
                    if(x2 != nullptr)
                    {
                        *x2 = &m_vertices[i +2];
                    }
                    if(y1 != nullptr)
                    {
                        *y1 = &m_vertices[i +1];
                    }
                    if(y2 != nullptr)
                    {
                        *y2 = &m_vertices[i +3];
                    }
                    closest =SPHVector2(retx,rety);
                    mindist = distance;
                }

        }

        return closest;
    }

    SPHVector2 GetClosestVertexP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
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

        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {
            double lx1 = m_vertices[i +0];
            double ly1 = m_vertices[i +1];
            float distance = SPHVector2(pos - SPHVector2(lx1,ly1)).length();
            if(i == 0 || distance < mindist)
            {
                mindist = distance;
                closest = SPHVector2(lx1,ly1);
                if(x1 != nullptr)
                {
                    *x1 = &m_vertices[i +0];
                }
                if(y1 != nullptr)
                {
                    *y1 = &m_vertices[i +1];
                }

            }
        }

        return closest;
    }


    virtual void MoveVertex(double *x, double *y, SPHVector2 d) override
    {
        m_BoundingBox.Set(0,0,0,0);
        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {

            if(&m_vertices[i +0] == x)
            {
                m_vertices[i +0] += d.x;
            }
            if(&m_vertices[i +1] == y)
            {
                m_vertices[i +1] += d.y;
            }
            if(i == 0)
            {
                m_BoundingBox.SetAs(SPHVector2(m_vertices[i +0],m_vertices[i +1]));
            }else {
                m_BoundingBox.MergeWith(SPHVector2(m_vertices[i +0],m_vertices[i +1]));
            }
        }
    }

    inline void Move(SPHVector2 d) override
    {
        m_BoundingBox.Move(d);

        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {
            m_vertices[i +0] += d.x;
            m_vertices[i +1] += d.y;
        }
    }

    SPHVector2 GetClosestEdgeLocation(SPHVector2 pos) override
    {
        SPHVector2 closest = SPHVector2(1e30,1e30);
        double mindist = 1e30;
        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double x1 = m_vertices[i +0];
            double x2 = m_vertices[i +2];
            double y1 = m_vertices[i +1];
            double y2 = m_vertices[i +3];

            double retx =x1;
            double rety =y1;

            GetClosestPointOnLine(x1,y1,x2,y2,pos.x,pos.y, &retx, &rety);

            float distance = SPHVector2(pos - SPHVector2(retx,rety)).length();

                if(distance < mindist)
                {
                    closest =SPHVector2(retx,rety);
                    mindist = distance;
                }

        }

        return closest;
    }

    SPHVector2 GetClosestVertex(SPHVector2 pos) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;

        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {
            double x1 = m_vertices[i +0];
            double y1 = m_vertices[i +1];
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


    bool Contains(SPHVector2 pos, double tolerence) override
    {
        SPHVector2 hit = GetClosestEdgeLocation(pos);
        float distance = (hit - pos).length();


        if(distance <= tolerence)
        {
            return true;
        }else {
            return false;
        }
    }


    inline SPHShape * Copy() override
    {
        SPHLine * l = new SPHLine();
        l->m_vertices = m_vertices;
        l->m_BoundingBox = m_BoundingBox;
        return l;

    }

    inline BoundingBox GetBoundingBox() override
    {
        return m_BoundingBox;
    }


};
