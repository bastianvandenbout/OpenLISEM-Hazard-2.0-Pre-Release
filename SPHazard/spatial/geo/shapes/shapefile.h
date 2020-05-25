#pragma once

#include "shapelayer.h"
#include "geoobject.h"
#include "spherror.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include <QList>
#include "functional"
#include "feature.h"
#include "matrixtable.h"

class ShapeFile : public GeoObject
{

public:

    ShapeFile()=default;

private:
    QList<ShapeLayer*> m_Layers;

public:

    ShapeFile               (ShapeFile const& other)=delete;
    ShapeFile               (ShapeFile&& other)=default;
    ~ShapeFile              ()=default;
    ShapeFile&         operator=           (ShapeFile const& other)=delete;
    ShapeFile&         operator=           (ShapeFile&& other)=default;

    inline void AddLayer(ShapeLayer* l)
    {
        m_Layers.append(l);
    }

    inline ShapeLayer * GetLayer(int i)
    {
        return m_Layers.at(i);
    }
    inline int GetLayerCount()
    {
        return m_Layers.length();
    }
    inline int GetShapeCount()
    {
        int count = 0;
        for(int i =0;i < m_Layers.length(); i++)
        {
            count += m_Layers.at(i)->GetShapeCount();
        }
        return count;
    }

    inline void CopyFrom(ShapeFile * s)
    {

        AS_Created = s->AS_Created;
        AS_FileName = s->AS_FileName;
        AS_writeonassign = false;

        for(int i =0; i < s->GetLayerCount(); i++)
        {
            this->AddLayer(s->GetLayer(i)->Copy());
        }

        m_Projection = s->m_Projection;
        m_BoundingBox = s->m_BoundingBox;
    }

    inline void Destroy()
    {
        for(int i =0; i < m_Layers.length(); i++)
        {
            ShapeLayer * l = m_Layers.at(i);
            if(l != nullptr)
            {
                l->Destroy();
                delete l;
            }
        }
        m_Layers.clear();
    }

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline BoundingBox GetAndCalcBoundingBox()
    {
        BoundingBox b;
        if(m_Layers.length() > 0)
        {
            b = m_Layers.at(0)->GetAndCalcBoundingBox();

            for(int i = 1; i < m_Layers.length(); i++)
            {
                b.MergeWith(m_Layers.at(i)->GetAndCalcBoundingBox());
            }
        }
        m_BoundingBox = b;

        return b;
    }

    //presupposed that the input is in the same CRS as the native data from this layer
    //additionaly assumes that the pos vector is also in this CRS (transform before calling this function)
    inline QList<Feature *> GetFeaturesAt(SPHVector2 pos, double tolerence)
    {
        QList<Feature *> result;

        //for each layer get the features that math the location and add to list
        for(int i = 0; i < m_Layers.length(); i++)
        {
            QList<Feature *> layerresult = m_Layers.at(i)->GetFeaturesAt(pos, tolerence);

            result.append(layerresult);
        }

        return result;
    }

    inline MatrixTable * GetFeatureAttributes(Feature * f)
    {
        MatrixTable * t = new MatrixTable();

        //for each layer get the features that math the location and add to list
        for(int i = 0; i < m_Layers.length(); i++)
        {
            int index = m_Layers.at(i)->HasFeature(f);
            if(index > -1)
            {
                int n = m_Layers.at(i)->GetNumberOfAttributes();
                t->SetSize(n,2);

                for(int j = 0; j < m_Layers.at(i)->GetNumberOfAttributes(); j++)
                {

                    t->SetValue(j,0,m_Layers.at(i)->GetAttributeName(j));
                    t->SetValue(j,1,m_Layers.at(i)->GetAttributeList(j)->GetValueAsString(index));

                }
                return t;
            }
        }



        return t;
    }

    SPHVector2 GetClosestEdgeLocation(SPHVector2 pos, ShapeLayer ** l_ret = nullptr, Feature ** f_ret = nullptr, SPHShape ** s_ret = nullptr)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }if(l_ret != nullptr)
        {
            *l_ret = nullptr;
        }

        for(int j = 0; j < m_Layers.length(); j++)
        {
            Feature ** f_retj = new (Feature *)();
            SPHShape ** s_retj  = new (SPHShape *)();
            SPHVector2 p = m_Layers.at(j)->GetClosestEdgeLocation(pos,f_retj,s_retj);
            float distance = SPHVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret = *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret =  *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }
            }
            delete f_retj;
            delete s_retj;
        }

        return closest;
    }

    SPHVector2 GetClosestVertex(SPHVector2 pos, ShapeLayer ** l_ret = nullptr, Feature ** f_ret = nullptr, SPHShape ** s_ret = nullptr)
    {
        SPHVector2 closest = SPHVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }if(l_ret != nullptr)
        {
            *l_ret = nullptr;
        }

        for(int j = 0; j < m_Layers.length(); j++)
        {
            Feature ** f_retj = new (Feature *)();
            SPHShape ** s_retj  = new (SPHShape *)();
            SPHVector2 p = m_Layers.at(j)->GetClosestVertex(pos,f_retj,s_retj);
            float distance = SPHVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret = *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret =  *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }
            }
            delete f_retj;
            delete s_retj;
        }

        return closest;
    }


//Angelscript related functions
public:

    QString    AS_FileName          = "";
    bool           AS_writeonassign     = false;
    std::function<void(ShapeFile *,QString)> AS_writefunc;
    std::function<ShapeFile *(QString)> AS_readfunc;
    bool           AS_Created           = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    ShapeFile*     AS_Assign            (ShapeFile*);

    ShapeFile *    AS_And               (ShapeFile * other);
    ShapeFile *    AS_Or                (ShapeFile * other);
    ShapeFile *    AS_Xor               (ShapeFile * other);
    ShapeFile *    AS_Add               (ShapeFile * other);
    ShapeFile *    AS_Sub               (ShapeFile * other);

    void           AS_SetAttributeValue (QString name, int index, QString value);
    MatrixTable *  AS_GetTable          ();
    MatrixTable *  AS_GetTable          (QString name);
};

//Angelscript related functionality

inline void ShapeFile::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void ShapeFile::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        delete this;
    }

}

inline ShapeFile* ShapeFile::AS_Assign(ShapeFile* sh)
{
    if(AS_writeonassign)
    {
        AS_writefunc(sh,AS_FileName);
    }
    this->AS_writeonassign = false;
    CopyFrom(sh);
    return this;
}

inline ShapeFile* ShapesFactory()
{
    return new ShapeFile();

}

inline ShapeFile* FromGDALDataset(GDALDataset * d)
{
    ShapeFile * ret = new ShapeFile();

    int nlayers = d->GetLayerCount();
    std::cout << "layer count " << nlayers << std::endl;
    for(int i = 0; i < nlayers; i++)
    {
        ShapeLayer * l = FromGDALLayer(d->GetLayer(i));
        ret->AddLayer(l);
    }

    std::cout << "shape count " << ret->GetShapeCount() << std::endl;

    return ret;
}

inline GDALDataset * ToGDALDataset(GDALDataset * to, ShapeFile* shapefile)
{
    //return new ShapeFile();

    int nlayers = shapefile->GetLayerCount();

    for(int i = 0; i < nlayers;i++)
    {
        int count_pre = shapefile->GetLayer(i)->GetFeatureCount();
        OGRLayer *poLayer = ToGDALLayer(to,shapefile->GetLayer(i),shapefile->GetProjection());
        int count_post = poLayer->GetFeatureCount();

    }

    std::cout << "shape count " << to->GetLayer(0)->GetFeatureCount() << std::endl;

    return to;
}

inline void ShapeFile::AS_SetAttributeValue (QString name, int index, QString value)
{
    if(m_Layers.length() > 0)
    {
        m_Layers.at(0)->SetFeatureAttribute(index,name,value);
    }

}

inline MatrixTable *  ShapeFile::AS_GetTable()
{
    if(m_Layers.length() > 0)
    {
        MatrixTable * ret = m_Layers.at(0)->GetAttributeTable();
        return ret;
    }else {
        return new MatrixTable();
    }
}


inline MatrixTable *  ShapeFile::AS_GetTable(QString name)
{
    if(m_Layers.length() > 0)
    {
        MatrixTable * ret = m_Layers.at(0)->GetAttributeTable(name);
        ret->AS_SetAttributeFunction = [this](QString name ,int index,QString val)
        {
            this->AS_SetAttributeValue(name,index,val);
        };
        ret->AS_DoSetAttribute = true;
        ret->AS_DoSetAttributeName = name;
        return ret;
    }
    return new MatrixTable();
}

