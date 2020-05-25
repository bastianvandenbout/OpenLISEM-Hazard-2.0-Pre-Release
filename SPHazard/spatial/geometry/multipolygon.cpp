#include "multipolygon.h"
#include "shape.h"

SPHShape * SPHMultiPolygon::Copy()
{

    SPHMultiPolygon *mp = new SPHMultiPolygon();

    for(int j = 0; j < GetPolygonCount(); j++)
    {
        mp->Add((SPHPolygon*)GetPolygon(j)->Copy());
    }
    return mp;
}


void SPHMultiPolygon::Add(SPHPolygon * p)
{
    m_List.append(p);
    if(m_List.size() == 1)
    {
        m_BoundingBox = p->GetBoundingBox();
    }else {
        m_BoundingBox.MergeWith(p->GetBoundingBox());
    }
}


void SPHMultiPolygon::Destroy()
{

    for(int j = 0; j < GetPolygonCount(); j++)
    {
        m_List.at(j)->Destroy();
    }
}

bool SPHMultiPolygon::Contains(SPHVector2 pos, double tolerence)
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
SPHVector2 SPHMultiPolygon::GetClosestEdgeLocationP(SPHVector2 pos, double **x1, double **y1, double **x2, double **y2)
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

SPHVector2 SPHMultiPolygon::GetClosestVertexP(SPHVector2 pos, double **x1, double **y1)
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

void SPHMultiPolygon::MoveVertex(double *x, double *y, SPHVector2 d)
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

void SPHMultiPolygon::Move(SPHVector2 d)
{
    m_BoundingBox.Move(d);
    for(int j = 0; j < m_List.length(); j++)
    {
        m_List.at(j)->Move(d);
    }
}


SPHVector2 SPHMultiPolygon::GetClosestEdgeLocation(SPHVector2 pos)
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
            closest = p;
        }else if(distance < mindist)
        {
            mindist = distance;
            closest = p;
        }
    }

    return closest;
}

SPHVector2 SPHMultiPolygon::GetClosestVertex(SPHVector2 pos)
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
