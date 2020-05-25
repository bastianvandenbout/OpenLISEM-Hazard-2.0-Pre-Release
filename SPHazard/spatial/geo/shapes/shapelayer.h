#ifndef SHAPELAYER_H
#define SHAPELAYER_H


#include "shape.h"
#include "feature.h"
#include "QString"
#include "QList"
#include "spherror.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "glgeometrylist.h"
#include "matrixtable.h"

#define ATTRIBUTE_TYPE_UNKNOWN 0
#define ATTRIBUTE_TYPE_INT32 1
#define ATTRIBUTE_TYPE_INT64 2
#define ATTRIBUTE_TYPE_FLOAT64 3
#define ATTRIBUTE_TYPE_BOOL 4
#define ATTRIBUTE_TYPE_STRING 5

//matches the Table Type definitions
class  AttributeList
{
public:
    QString m_Name;
    int m_type;

    QList<int64_t> m_ListInt64;
    QList<int32_t> m_ListInt32;
    QList<double> m_ListFloat64;
    QList<bool> m_ListBool;
    QList<QString> m_ListString;

    QString GetValueAsString(int i)
    {
        if(m_type == ATTRIBUTE_TYPE_INT32)
        {
                return QString::number(m_ListInt32.at(i));
        }else if(m_type == ATTRIBUTE_TYPE_INT64)
        {
                return QString::number(m_ListInt64.at(i));
       }else if(m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
                return QString::number(m_ListFloat64.at(i));
       }else if(m_type == ATTRIBUTE_TYPE_BOOL)
        {
                return m_ListBool.at(i)? "true":  "false";
        }else if(m_type == ATTRIBUTE_TYPE_STRING)
        {
                return m_ListString.at(i);
       }else
        {
                return m_ListString.at(i);
        }

    }

    inline bool isNumberType()
    {
        if(m_type == ATTRIBUTE_TYPE_INT32)
        {
                return true;
        }else if(m_type == ATTRIBUTE_TYPE_INT64)
        {
                return true;
       }else if(m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
                return true;
       }else if(m_type == ATTRIBUTE_TYPE_BOOL)
        {
                return true;
        }else if(m_type == ATTRIBUTE_TYPE_STRING)
        {
                return false;
       }else
        {
                return false;
        }
    }
};


class ShapeLayer
{

private:
    QString m_Name;
    int m_ShapeType;
    QList<Feature*> m_FeatureList;

    QList<AttributeList *> m_AttributeList;

    BoundingBox m_BoundingBox;

    float xmin;
    float ymin;
    float zmin;
    float xmax;
    float ymax;
    float zmax;

public:

    inline ShapeLayer * Copy()
    {

        ShapeLayer * layer = new ShapeLayer();

        layer->m_Name = m_Name;
        layer->m_ShapeType = m_ShapeType;

        layer->xmin = xmin;
        layer->xmax = xmax;
        layer->ymin = ymin;
        layer->ymax = ymax;
        layer->zmin = zmin;
        layer->zmax = zmax;

        for(int i = 0; i < m_FeatureList.length(); i++)
        {
            layer->AddFeature(m_FeatureList.at(i)->Copy());
        }

        for(int i = 0; i < m_AttributeList.length(); i++)
        {
            layer->AddAttribute(m_AttributeList.at(i));
        }

        return layer;

    }


    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline BoundingBox GetAndCalcBoundingBox()
    {
        BoundingBox b;
        if(m_FeatureList.length() > 0)
        {
            b = m_FeatureList.at(0)->GetBoundingBox();

            for(int i = 1; i < m_FeatureList.length(); i++)
            {
                BoundingBox b2 = m_FeatureList.at(i)->GetBoundingBox();
                if(!(b2.GetMaxX() == 0.0 && b2.GetMinX() == 0.0 && b2.GetMaxY() == 0.0 && b2.GetMinY() == 0.0))
                {
                    b.MergeWith(b2);
                }


            }
        }
        m_BoundingBox = b;

        return b;
    }

    //virtual
    inline int GetShapeCount()
    {
        int count = 0;
        for(int i =0; i < m_FeatureList.length(); i++)
        {
            count += m_FeatureList.at(i)->GetShapeCount();
        }
        return count;
    }
    inline void SetType(int x)
    {
        m_ShapeType = x;
    }
    inline int GetType()
    {
        return m_ShapeType;
    }
    inline QString GetName()
    {
        return m_Name;
    }
    inline void SetName(QString m)
    {
        m_Name = m;
    }
    inline int GetFeatureCount()
    {
        return m_FeatureList.length();
    }
    inline Feature * GetFeature(int i)
    {
        return m_FeatureList.at(i);
    }
    inline void RemoveFeature(int i)
    {
        m_FeatureList.removeAt(i);
        for(int j = 0; j < m_AttributeList.length(); j++)
        {
            AttributeList *AL =m_AttributeList.at(j);
            int type = m_AttributeList.at(j)->m_type;
            if(type == ATTRIBUTE_TYPE_INT32)
            {
                    AL->m_ListInt32.removeAt(i);
            }else if(type == ATTRIBUTE_TYPE_INT64)
            {
                    AL->m_ListInt64.removeAt(i);
           }else if(type == ATTRIBUTE_TYPE_FLOAT64)
            {
                    AL->m_ListFloat64.removeAt(i);
           }else if(type == ATTRIBUTE_TYPE_BOOL)
            {
                    AL->m_ListBool.removeAt(i);
           }else if(type == ATTRIBUTE_TYPE_STRING)
            {
                    AL->m_ListString.removeAt(i);
           }else
            {
                    AL->m_ListString.removeAt(i);
            }
        }
        //GetAndCalcBoundingBox();
    }
    inline void RemoveFeature(Feature* f)
    {
        RemoveFeature(m_FeatureList.indexOf(f));

        //GetAndCalcBoundingBox();
    }
    inline void ReplaceFeature(Feature* f,Feature* f2)
    {
        int i = m_FeatureList.indexOf(f);
        if( i > -1)
        {
            m_FeatureList.replace(i,f2);
            GetBoundingBox();
        }
    }
    inline int AddFeature(Feature* f)
    {
        m_FeatureList.append(f);

        for(int i = 0; i < m_AttributeList.length(); i++)
        {
            AttributeList *AL =m_AttributeList.at(i);
            int type = m_AttributeList.at(i)->m_type;
            if(type == ATTRIBUTE_TYPE_INT32)
            {
                    AL->m_ListInt32.append(0);
            }else if(type == ATTRIBUTE_TYPE_INT64)
            {
                    AL->m_ListInt64.append(0);
           }else if(type == ATTRIBUTE_TYPE_FLOAT64)
            {
                    AL->m_ListFloat64.append(0);
           }else if(type == ATTRIBUTE_TYPE_BOOL)
            {
                    AL->m_ListBool.append(0);
           }else if(type == ATTRIBUTE_TYPE_STRING)
            {
                    AL->m_ListString.append(QString(" "));
           }else
            {
                    AL->m_ListString.append(QString(" "));
            }
        }

        return m_FeatureList.length() -1;
    }

    inline int HasFeature(Feature * f)
    {
        int index =m_FeatureList.indexOf(f);
        return index;
    }

    inline void AddAttribute(AttributeList * tocopy)
    {

        AttributeList * AL = new AttributeList();
        AL->m_ListBool = tocopy->m_ListBool;
        AL->m_ListInt32 = tocopy->m_ListInt32;
        AL->m_ListInt64 = tocopy->m_ListInt64;
        AL->m_ListFloat64 = tocopy->m_ListFloat64;
        AL->m_ListString = tocopy->m_ListString;
        AL->m_Name = tocopy->m_Name;
        AL->m_type = tocopy->m_type;

        m_AttributeList.append(AL);
    }

    inline void AddAttribute(QString name, int type)
    {
        int count = m_FeatureList.length();

        AttributeList * AL = new AttributeList();
        AL->m_ListBool.clear();
        AL->m_ListInt32.clear();
        AL->m_ListInt64.clear();
        AL->m_ListFloat64.clear();
        AL->m_ListString.clear();
        AL->m_Name = name;
        AL->m_type = type;

        if(type == ATTRIBUTE_TYPE_INT32)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListInt32.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
        if(type ==  ATTRIBUTE_TYPE_INT64)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListInt64.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
        if(type == ATTRIBUTE_TYPE_FLOAT64)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListFloat64.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
       if(type ==  ATTRIBUTE_TYPE_BOOL)
            {
                for(int i =0; i < count; i++)
                {
                    AL->m_ListBool.append(0);
                }
                this->m_AttributeList.append(AL);
            }else
       if(type == ATTRIBUTE_TYPE_STRING)
        {
            for(int i =0; i < count; i++)
            {
                AL->m_ListString.append(QString(""));
            }
            this->m_AttributeList.append(AL);
        }else
        {
            SPH_DEBUG("Could not load attribute type for: " + name + " Loaded instead as string");
            AL->m_type = ATTRIBUTE_TYPE_STRING;
            for(int i =0; i < count; i++)
            {
                AL->m_ListString.append(QString(""));
            }
            this->m_AttributeList.append(AL);
        }

    }

    inline void SetFeatureAttribute(int indexf,QString name, int64_t attribute)
    {
        bool founda = 0;

        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
            {
                founda = true;
                break;
            }
        }

        if(!founda)
        {
            return;
        }

        int type = AL->m_type;

        if(type == ATTRIBUTE_TYPE_INT32)
            {
                if(AL->m_ListInt32.length() > indexf)
                {
                    AL->m_ListInt32.replace(indexf,attribute);
                }
            }
        else if(type == ATTRIBUTE_TYPE_INT64)
            {
                if(AL->m_ListInt64.length() > indexf)
                {
                    AL->m_ListInt64.replace(indexf,attribute);
                }
            }
       else if(type == ATTRIBUTE_TYPE_FLOAT64)
        {
                if(AL->m_ListFloat64.length() > indexf)
                {
                    AL->m_ListFloat64.replace(indexf,attribute);
                }
        }
       else if(type == ATTRIBUTE_TYPE_BOOL)
        {
                if(AL->m_ListBool.length() > indexf)
                {
                    AL->m_ListBool.replace(indexf,(bool)attribute == 0);
                }
        }
        else if(type == ATTRIBUTE_TYPE_STRING)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }else if(type == ATTRIBUTE_TYPE_UNKNOWN)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }

    }

    inline void SetFeatureAttribute(Feature*f,QString name, int64_t attribute)
    {
        //get index of f
        //because of regular usage, it is faster to start at the end

        bool found = 0;

        int indexf;
        for(int i = m_FeatureList.length()-1; i > -1; i--)
        {
            if(f == m_FeatureList.at(i))
            {
                indexf = i;
                found = true;
                break;
            }
        }

        if(!found)
        {
            return;
        }
        SetFeatureAttribute(indexf,name,attribute);

    }

    inline void SetFeatureAttribute(int indexf,QString name, double attribute)
    {
        bool founda = 0;

        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
            {
                founda = true;
                break;
            }
        }

        if(!founda)
        {
            return;
        }

        int type = AL->m_type;
        if(type == ATTRIBUTE_TYPE_INT32)
        {
                if(AL->m_ListInt32.length() > indexf)
                {
                    AL->m_ListInt32.replace(indexf,attribute);
                }
        }else if(type == ATTRIBUTE_TYPE_INT64)
        {
                if(AL->m_ListInt64.length() > indexf)
                {
                    AL->m_ListInt64.replace(indexf,attribute);
                }
        }else if(type == ATTRIBUTE_TYPE_FLOAT64)
        {
                if(AL->m_ListFloat64.length() > indexf)
                {
                    AL->m_ListFloat64.replace(indexf,attribute);
                }
        }else if(type == ATTRIBUTE_TYPE_BOOL)
        {
                if(AL->m_ListBool.length() > indexf)
                {
                    AL->m_ListBool.replace(indexf,(bool)attribute == 0);
                }
        }else if(type ==  ATTRIBUTE_TYPE_STRING)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }else if(type ==  ATTRIBUTE_TYPE_UNKNOWN)
        {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,QString::number(attribute));
                }
        }
    }

    inline void SetFeatureAttribute(Feature*f,QString name, double attribute)
    {
        //get index of f
        //because of regular usage, it is faster to start at the end

        bool found = 0;

        int indexf;
        for(int i = m_FeatureList.length()-1; i > -1; i--)
        {
            if(f == m_FeatureList.at(i))
            {
                indexf = i;
                found = true;
                break;
            }
        }

        if(!found)
        {
            return;
        }

        SetFeatureAttribute(indexf,name,attribute);


    }

    inline void SetFeatureAttribute(int indexf,QString name, QString attribute)
    {
        bool founda = 0;

        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            if(name.compare(m_AttributeList.at(i)->m_Name) == 0)
            {
                founda = true;
                break;
            }
        }

        if(!founda)
        {
            return;
        }

        int type = AL->m_type;

            if(type == ATTRIBUTE_TYPE_INT32)
            {
                if(AL->m_ListInt32.length() > indexf)
                {
                    AL->m_ListInt32.replace(indexf,attribute.toInt());
                }
            }
            else if(type == ATTRIBUTE_TYPE_INT64)
            {
                if(AL->m_ListInt64.length() > indexf)
                {
                    AL->m_ListInt64.replace(indexf,attribute.toInt());
                }
            }
             else if(type ==  ATTRIBUTE_TYPE_FLOAT64)
            {
                if(AL->m_ListFloat64.length() > indexf)
                {
                    AL->m_ListFloat64.replace(indexf,attribute.toFloat());
                }
            }
             else if(type == ATTRIBUTE_TYPE_BOOL)
            {
                if(AL->m_ListBool.length() > indexf)
                {
                    AL->m_ListBool.replace(indexf,attribute.toInt()==1);
                }
            }
             else if(type ==  ATTRIBUTE_TYPE_STRING)
            {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,attribute);
                }
            }else if(type == ATTRIBUTE_TYPE_UNKNOWN)
            {
                if(AL->m_ListString.length() > indexf)
                {
                    AL->m_ListString.replace(indexf,attribute);
                }
            }
    }

    inline void CopyFeatureAttributesFrom(int from, int to)
    {
        int indexf = to;
        AttributeList * AL;
        for(int i = m_AttributeList.length()-1; i > -1; i--)
        {
            AL =m_AttributeList.at(i);
            int type = AL->m_type;

                if(type == ATTRIBUTE_TYPE_INT32)
                {
                    if(AL->m_ListInt32.length() > indexf && AL->m_ListInt32.length() >from)
                    {
                        AL->m_ListInt32.replace(indexf,AL->m_ListInt32.at(from));
                    }
                }
                else if(type == ATTRIBUTE_TYPE_INT64)
                {
                    if(AL->m_ListInt64.length() > indexf && AL->m_ListInt64.length() > from)
                    {
                        AL->m_ListInt64.replace(indexf,AL->m_ListInt64.at(from));
                    }
                }
                 else if(type ==  ATTRIBUTE_TYPE_FLOAT64)
                {
                    if(AL->m_ListFloat64.length() > indexf && AL->m_ListFloat64.length() > from)
                    {
                        AL->m_ListFloat64.replace(indexf,AL->m_ListFloat64.at(from));
                    }
                }
                 else if(type == ATTRIBUTE_TYPE_BOOL)
                {
                    if(AL->m_ListBool.length() > indexf && AL->m_ListBool.length() > from)
                    {
                        AL->m_ListBool.replace(indexf,AL->m_ListBool.at(from));
                    }
                }
                 else if(type ==  ATTRIBUTE_TYPE_STRING)
                {
                    if(AL->m_ListString.length() > indexf && AL->m_ListString.length() > from)
                    {
                        AL->m_ListString.replace(indexf,AL->m_ListString.at(from));
                    }
                }else if(type == ATTRIBUTE_TYPE_UNKNOWN)
                {
                    if(AL->m_ListString.length() > indexf && AL->m_ListString.length() > from)
                    {
                        AL->m_ListString.replace(indexf,AL->m_ListString.at(from));
                    }
                }
        }


    }

    inline void SetFeatureAttribute(Feature*f,QString name, QString attribute)
    {
        //get index of f
        //because of regular usage, it is faster to start at the end

        bool found = 0;

        int indexf;
        for(int i = m_FeatureList.length()-1; i > -1; i--)
        {
            if(f == m_FeatureList.at(i))
            {
                indexf = i;
                found = true;
                break;
            }
        }

        if(!found)
        {
            return;
        }
        SetFeatureAttribute(indexf,name,attribute);

    }

    //attributes are done by this parent class
    inline int GetNumberOfAttributes()
    {
        return m_AttributeList.length();
    }
    inline QString GetAttributeName(int i)
    {
        return m_AttributeList.at(i)->m_Name;
    }

    inline int GetAttributeType(int i)
    {
        return m_AttributeList.at(i)->m_type;
    }

    inline QString GetAttributeValue(int i,int j)
    {
        return m_AttributeList.at(i)->GetValueAsString(j);
    }

    inline QString GetAttributeValue(int i,Feature * f)
    {
        int index = m_FeatureList.indexOf(f);

        if(index > -1)
        {
            return GetAttributeValue(i,index);
        }
        return "";
    }

    inline AttributeList * GetAttributeList(int i)
    {
        return m_AttributeList.at(i);
    }



    SPHVector2 GetClosestEdgeLocation(SPHVector2 pos,Feature **f_ret,SPHShape ** s_ret = nullptr)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }


        for(int j = 0; j < m_FeatureList.length(); j++)
        {
            SPHShape ** s_retj = new (SPHShape *)();
            SPHVector2 p = m_FeatureList.at(j)->GetClosestEdgeLocation(pos,s_retj);
            float distance = SPHVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }
            }
            delete s_retj;
        }

        return closest;
    }

    SPHVector2 GetClosestVertex(SPHVector2 pos,Feature **f_ret,SPHShape ** s_ret = nullptr)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }


        for(int j = 0; j < m_FeatureList.length(); j++)
        {

            SPHShape ** s_retj = new (SPHShape *)();
            SPHVector2 p = m_FeatureList.at(j)->GetClosestVertex(pos,s_retj);
            float distance = SPHVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr &&  s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr)
                    {
                        *f_ret = m_FeatureList.at(j);
                    }
                }
            }
            delete s_retj;
        }

        return closest;
    }


    inline void Destroy()
    {
        //create attribute fields
        for(int i = 0; i < m_AttributeList.length(); i++)
        {
            AttributeList * al = m_AttributeList.at(i);
            if(al != nullptr)
            {
                delete al;
            }
        }
        m_AttributeList.clear();

        for(int i =0; i < m_FeatureList.length(); i++)
        {
            Feature* f = m_FeatureList.at(i);
            if(f!= nullptr)
            {
                f->Destroy();
                delete f;
            }
        }
        m_FeatureList.clear();

    }

    inline QList<Feature *> GetFeaturesAt(SPHVector2 pos, double tolerence)
    {
        QList<Feature *> result;

        for(int i = 0; i < m_FeatureList.length(); i++)
        {
            Feature *f = m_FeatureList.at(i);

            if(f->GetBoundingBox().Contains(pos.x,pos.y,tolerence))
            {
                if(f->Contains(pos, tolerence))
                {
                    result.append(f);
                }
            }

        }

        return result;

    }

    inline void Flatten()
    {

        for(int i =0; i < GetFeatureCount(); i++)
        {

            Feature *f = GetFeature(i);

            for(int j = f->GetShapeCount()-1; j >0; j--)
            {
                SPHShape * s = f->GetShape(j);
                f->RemoveShape(s);
                Feature* fnew = new Feature();
                fnew->AddShape(s);
                int index = this->AddFeature(fnew);
                this->CopyFeatureAttributesFrom(i, index);
            }
        }


    }

    inline GLGeometryList * GetGLGeometryList()
    {
        struct GLGeometryList * gl_list = new GLGeometryList();
        //we will fil this struct with the geometry used by the display code for drawing with opengl
        //this means creating triangulations of points with a radius and polygons
        //borders of points and polygons and lines are given as line geometry

        for(int i =0; i < GetFeatureCount(); i++)
        {

            Feature *f = GetFeature(i);

            f->AddToGlGeometryList(gl_list,i);
        }

        return gl_list;

    }

    inline MatrixTable * GetAttributeTable()
    {

        int h = GetFeatureCount();
        int w = m_AttributeList.length();

        MatrixTable * t = new MatrixTable();
        t->SetSize(h,w);

        for(int i = 0; i < h; i++)
        {
            t->SetRowTitle(i,QString::number(i+1));
        }

        for(int j = 0; j < w; j++)
        {
            t->SetColumnTitle(j,m_AttributeList.at(j)->m_Name);
        }

        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                t->SetValue(i,j,m_AttributeList.at(j)->GetValueAsString(i));
            }
        }

        return t;

    }

    inline MatrixTable * GetAttributeTable(QString name)
    {

        int h = GetFeatureCount();
        int w = m_AttributeList.length();

        int j = 0;
        bool found = false;
        for(int j = 0; j < w; j++)
        {
            if(m_AttributeList.at(j)->m_Name.compare(name) == 0)
            {
                found = true;
                break;
            }
        }

        if(found)
        {
            MatrixTable * t = new MatrixTable();
            t->SetSize(h,1);

            for(int i = 0; i < h; i++)
            {
                t->SetRowTitle(i,QString::number(i+1));
            }

            t->SetColumnTitle(0,m_AttributeList.at(j)->m_Name);

            for(int i = 0; i < h; i++)
            {
                for(int j = 0; j < w; j++)
                {
                    t->SetValue(i,0,m_AttributeList.at(j)->GetValueAsString(i));
                }
            }

            return t;
        }else {
            return new MatrixTable();
        }
    }
};


inline ShapeLayer* FromGDALLayer(OGRLayer * d)
{
    ShapeLayer * l = new ShapeLayer();
    const char * name = d->GetName();l->SetName(d->GetName());
    if(name != NULL)
    {
        l->SetName(name);
    }

    OGRwkbGeometryType layertype = wkbFlatten(d->GetGeomType());

    std::cout << "Layertype " <<  layertype << std::endl;

    if(layertype == wkbUnknown)
    {
        l->SetType(SPH_SHAPE_UNKNOWN);
    }else if(layertype == wkbPoint)
    {
        l->SetType(SPH_SHAPE_POINT);
    }else if(layertype == wkbLineString)
    {
        l->SetType(SPH_SHAPE_LINE);
    }else if(layertype == wkbPolygon)
    {
        l->SetType(SPH_SHAPE_POLYGON);
    }else if(layertype == wkbMultiPoint)
    {
        l->SetType(SPH_SHAPE_MULTIPOINT);
    }else if(layertype == wkbMultiLineString)
    {
        l->SetType(SPH_SHAPE_MULTILINE);
    }else if(layertype == wkbMultiPolygon)
    {
        l->SetType(SPH_SHAPE_MULTIPOLYGON);
    }else {
        SPH_ERROR("Unsupported layer type (shape not implemented, can not be loaded)")
        return l;
    }

    OGREnvelope Envelope;
    int error = d->GetExtent(&Envelope,true);

    OGRFeatureDefn *poFDefn = d->GetLayerDefn();

    std::cout << "field count " << poFDefn->GetFieldCount() << std::endl;

    //get attribute names and types
    for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
    {
        OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

        if(!QString(poFieldDefn->GetNameRef()).startsWith("OBJECTID"))
        {
            switch( poFieldDefn->GetType() )
            {
                case OFTInteger:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_INT32);
                    break;
                case OFTInteger64:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_INT64);
                    break;
                case OFTReal:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_FLOAT64);
                    break;
                case OFTString:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_STRING);
                    break;
                default:
                    l->AddAttribute(poFieldDefn->GetNameRef(),ATTRIBUTE_TYPE_UNKNOWN);
                    break;
            }
        }
    }

    int count = d->GetFeatureCount(true);

    std::cout << "feature count " << count << std::endl;
    d->ResetReading();
    OGRFeature *poFeature;
    for(int j = 0; j < count; j++)
    {
        poFeature = d->GetNextFeature();


        Feature *f = FromGDALFeature(poFeature);
        int indexf = l->AddFeature(f);

        //for each feature we must also get the attributes
        for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

            if(!QString(poFieldDefn->GetNameRef()).startsWith("OBJECTID"))
            {
                switch( poFieldDefn->GetType() )
                {
                    case OFTInteger:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), (int64_t) poFeature->GetFieldAsInteger( iField ) );
                        break;
                    case OFTInteger64:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), (int64_t) poFeature->GetFieldAsInteger64( iField ) );
                        break;
                    case OFTReal:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), (double) poFeature->GetFieldAsDouble(iField) );
                        break;
                    case OFTString:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), QString(poFeature->GetFieldAsString(iField)) );
                        break;
                    default:
                        l->SetFeatureAttribute(indexf, poFieldDefn->GetNameRef(), QString(poFeature->GetFieldAsString(iField) ));
                        break;
                }
            }
        }


    }

    return l;
}

inline OGRLayer * ToGDALLayer(GDALDataset * to, ShapeLayer* layer, GeoProjection p)
{
    //wkbUnknown = 0          /** unknown non-standard */
    //wkbPoint = 1,           /**< 0-dimensional geometric object, standard WKB */
    //wkbLineString = 2,      /**< 1-dimensional geometric object with linear
    //                         *   interpolation between Points, standard WKB */
    //wkbPolygon = 3,         /**< planar 2-dimensional geometric object defined
    //                         *   by 1 exterior boundary and 0 or more interior
    //                         *   boundaries, standard WKB */
    //wkbMultiPoint = 4,      /**< GeometryCollection of Points, standard WKB */
    //wkbMultiLineString = 5, /**< GeometryCollection of LineStrings, standard WKB */
    //wkbMultiPolygon = 6,    /**< GeometryCollection of Polygons, standard WKB */

    int layertype = layer->GetType();
    OGRwkbGeometryType ogrtype;

    if(layertype == SPH_SHAPE_UNKNOWN)
    {
        ogrtype = wkbUnknown;
    }else if(layertype == SPH_SHAPE_POINT)
    {
        ogrtype = wkbPoint;
    }else if(layertype ==SPH_SHAPE_LINE )
    {
        ogrtype =wkbLineString;
    }else if(layertype == SPH_SHAPE_POLYGON)
    {
        ogrtype = wkbPolygon;
    }else if(layertype == SPH_SHAPE_MULTIPOINT)
    {
        ogrtype = wkbMultiPoint;
    }else if(layertype == SPH_SHAPE_MULTILINE)
    {
        ogrtype = wkbMultiLineString;
    }else if(layertype == SPH_SHAPE_MULTIPOLYGON)
    {
        ogrtype = wkbMultiPolygon;
    }else {

        SPH_ERROR("Could not recognize layer type")
        return nullptr;
    }

    QString name = layer->GetName();
    if(name.isEmpty())
    {
        name = QString::number(to->GetLayerCount());
    }


    OGRSpatialReference * ref = new OGRSpatialReference(p.ToGDALRef());
    OGRLayer * poLayer = to->CreateLayer( name.toStdString().c_str(),ref, ogrtype, NULL );
    if( poLayer == NULL )
    {
        SPH_ERROR("Could not create layer in gdal dataset");
        return nullptr;
    }

    //create attribute fields
    for(int i = 0; i < layer->GetNumberOfAttributes(); i++)
    {
        AttributeList * al = layer->GetAttributeList(i);
        QString alname = al->m_Name;


        if(al->m_type == ATTRIBUTE_TYPE_STRING)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTString );
            oField.SetWidth(50);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                SPH_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_BOOL)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTInteger);
            oField.SetWidth(32);
            oField.SetPrecision(20);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                SPH_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_INT32)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTInteger);
            oField.SetWidth(32);
            oField.SetPrecision(20);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                SPH_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_INT64)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTInteger64 );
            oField.SetWidth(32);
            oField.SetPrecision(20);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                SPH_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_FLOAT64)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTReal );
            oField.SetWidth(32);
            oField.SetPrecision(20);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                SPH_ERROR("Could not add attribute of to gdal layer: " + alname);
            }
        }else if(al->m_type == ATTRIBUTE_TYPE_UNKNOWN)
        {
            OGRFieldDefn oField( alname.toStdString().c_str(), OFTString );
            oField.SetWidth(50);
            if( poLayer->CreateField( &oField ) != OGRERR_NONE )
            {
                SPH_ERROR("Could not add attribute of unknown type to gdal layer");
            }
        }
    }

    for(int i =0; i < layer->GetFeatureCount(); i++)
    {
        //add feature
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );

        for(int j = 0; j < layer->GetNumberOfAttributes(); j++)
        {
            AttributeList * al = layer->GetAttributeList(j);
            QString alname = al->m_Name;

            //set feature attribute values
            if(al->m_type == ATTRIBUTE_TYPE_STRING)
            {
                if(al->m_ListString.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListString.at(i).toStdString().c_str());
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_BOOL)
            {
                if(al->m_ListBool.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListBool.at(i)? 1:0);
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_INT32)
            {
                if(al->m_ListInt32.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListInt32.at(i));
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_INT64)
            {
                if(al->m_ListInt64.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),(int32_t)al->m_ListInt64.at(i));
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_FLOAT64)
            {
                if(al->m_ListFloat64.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),(double)al->m_ListFloat64.at(i));
                }
            }else if(al->m_type == ATTRIBUTE_TYPE_UNKNOWN)
            {
                if(al->m_ListString.length() > i)
                {
                    poFeature->SetField( alname.toStdString().c_str(),al->m_ListString.at(i).toStdString().c_str());
                }
            }
        }

        ToGDALFeature(poFeature,layer->GetFeature(i));


        if(poLayer->CreateFeature(poFeature)!= OGRERR_NONE)
        {
            SPH_ERROR("Could not add feature");
        };

        OGRFeature::DestroyFeature( poFeature );
    }

    return poLayer;
}

#endif // SHAPELAYER_H
