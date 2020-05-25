#pragma once

#include "line.h"
#include "QList"
#include "shape.h"

class SPHMultiLine: public SPHShape
{

    QList<SPHLine*> m_List;
   BoundingBox m_BoundingBox;
public:
    inline SPHMultiLine() : SPHShape(SPH_SHAPE_MULTILINE)
    {
    }

    inline void Add(SPHLine * l)
    {
        m_List.append(l);
        if(m_List.size() == 1)
        {
            m_BoundingBox = l->GetBoundingBox();
        }else {
            m_BoundingBox.MergeWith(l->GetBoundingBox());
        }
    }


    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }


    inline int GetLineCount()
    {
        return m_List.length();
    }
    inline SPHLine * GetLine(int i)
    {
        return m_List.at(i);
    }

    inline SPHShape * Copy() override
    {

        SPHMultiLine *mp = new SPHMultiLine();

        for(int j = 0; j < GetLineCount(); j++)
        {
            mp->Add((SPHLine*)GetLine(j)->Copy());
        }

        return mp;
    }

    virtual SPHVector2 GetClosestEdgeLocationP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;

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

        double ** x1t = new (double*);
        double ** y1t = new (double*);
        double ** x2t = new (double*);
        double ** y2t = new (double*);


        for(int j = 0; j < m_List.length(); j++)
        {
            SPHVector2 p = m_List.at(j)->GetClosestEdgeLocationP(pos,x1t,y1t,x2t,y2t);
            float distance = SPHVector2(pos - p).length();
            if(j == 0 || distance < mindist)
            {
                mindist = distance;
                closest = p;
                *x1 = *x1t;*y1 = *y1t;*x2 = *x2t;*y2 = *y2t;

            }
        }
        delete x1t;delete y1t; delete x2t; delete y2t;

        return closest;
    }


    virtual SPHVector2 GetClosestVertexP(SPHVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
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
        double ** x1t = new (double*);
        double ** y1t = new (double*);

        for(int j = 0; j < m_List.length(); j++)
        {
            SPHVector2 p = m_List.at(j)->GetClosestVertexP(pos,x1t,y1t);
            float distance = SPHVector2(pos - p).length();
            if(j == 0 || distance < mindist)
            {
                mindist = distance;
                closest = p;
                *x1 = *x1t;*y1 = *y1t;
            }
        }
        delete x1t;delete y1t;

        return closest;
    }



    virtual void MoveVertex(double *x, double *y, SPHVector2 d) override
    {
        for(int j = 0; j < m_List.length(); j++)
        {
            m_List.at(j)->MoveVertex(x,y,d);
            if(j == 0)
            {
                m_BoundingBox = m_List.at(j)->GetBoundingBox();
            }else {
                m_BoundingBox.MergeWith(m_List.at(j)->GetBoundingBox());
            }
        }

    }

    inline void Move(SPHVector2 d) override
    {
        m_BoundingBox.Move(d);
        for(int j = 0; j < m_List.length(); j++)
        {
            m_List.at(j)->Move(d);
        }
    }

    virtual SPHVector2 GetClosestEdgeLocation(SPHVector2 pos) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;

        for(int j = 0; j < m_List.length(); j++)
        {
            SPHVector2 p = m_List.at(j)->GetClosestEdgeLocation(pos);
            float distance = SPHVector2(pos - p).length();
            if(j == 0)
            {
                mindist = distance;
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = p;
            }
        }

        return closest;
    }

    virtual SPHVector2 GetClosestVertex(SPHVector2 pos) override
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;

        for(int j = 0; j < m_List.length(); j++)
        {
            SPHVector2 p = m_List.at(j)->GetClosestVertex(pos);
            float distance = SPHVector2(pos - p).length();
            if(j == 0)
            {
                mindist = distance;
                closest = p;
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = p;
            }
        }

        return closest;
    }

    inline void Destroy() override
    {

        for(int j = 0; j < GetLineCount(); j++)
        {
            m_List.at(j)->Destroy();
        }
    }

    bool Contains(SPHVector2 pos, double tolerence)
    {
        for(int i = 0; i < m_List.length(); i++)
        {
            if(m_List.at(i)->GetBoundingBox().Contains(pos.x,pos.y,tolerence))
            {
                if(m_List.at(i)->Contains(pos,tolerence))
                {
                    return true;
                }
            }
        }
        return false;
    }

};
