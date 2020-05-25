#ifndef MULTITRIANGLE_H
#define MULTITRIANGLE_H


#include "shape.h"
#include "triangle.h"
#include "QList"

class SPHMultiTriangle: public SPHShape
{

    QList<SPHTriangle*> m_List;

    BoundingBox m_BoundingBox;
public:
    inline SPHMultiTriangle() : SPHShape(SPH_SHAPE_MULTITRIANGLE)
    {
    }

    inline void Add(SPHTriangle * p)
    {
        m_List.append(p);
        if(m_List.size() == 0)
        {
            m_BoundingBox = p->GetBoundingBox();
        }else {
            m_BoundingBox.MergeWith(p->GetBoundingBox());
        }
    }

    inline int GetTriangleCount()
    {
        return m_List.length();
    }

    inline SPHTriangle * GetTriangle(int i)
    {
        return m_List.at(i);
    }

    inline SPHShape * Copy() override
    {

        SPHMultiTriangle *mp = new SPHMultiTriangle();

        for(int j = 0; j < GetTriangleCount(); j++)
        {
            mp->Add((SPHTriangle *)GetTriangle(j)->Copy());
        }

        return mp;
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

        for(int j = 0; j < GetTriangleCount(); j++)
        {
            m_List.at(j)->Destroy();
        }
    }

    virtual bool Contains(SPHVector2 pos, double tolerence)
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


    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

};

#endif // MULTITRIANGLE_H
