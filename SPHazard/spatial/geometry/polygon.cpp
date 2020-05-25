#include "polygon.h"
#include "triangulate.h"
#include <vector>
#include <array>

SPHMultiPolygon *SPHPolygon::Triangulate()
{
    SPHMultiPolygon * mp = new SPHMultiPolygon();


    //set up this polygon in required structure

    std::vector<std::vector<std::array<double,2>>> polygon;
    std::vector<std::array<double,2>> polygonflat;
    std::vector<std::array<double,2>> ring_ext;

    if(m_Ext_Ring != nullptr)
    {
        int nvertex = m_Ext_Ring->GetVertexCount();
        double*vertexdata = m_Ext_Ring->GetVertexData();
        for(int i= 0; i < nvertex; i++)
        {
            ring_ext.push_back({vertexdata[i*2],vertexdata[i*2+1]});
            polygonflat.push_back({vertexdata[i*2],vertexdata[i*2+1]});
        }
        polygon.push_back(ring_ext);


        for(int i = 0; i < GetIntRingCount(); i++)
        {
            Ring * rint = GetIntRing(i);
            int nvertex = rint->GetVertexCount();
            double*vertexdata = rint->GetVertexData();
            std::vector<std::array<double,2>> ring_int;
            for(int i= 0; i < nvertex; i++)
            {
                ring_int.push_back({vertexdata[i*2],vertexdata[i*2+1]});
                polygonflat.push_back({vertexdata[i*2],vertexdata[i*2+1]});
            }

            polygon.push_back(ring_int);
        }
        //run the triangulation algorithm

        std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);

        //get back all the triangles

        for(int i = 0; i < indices.size()/3; i++)
        {
            double x1 = polygonflat.at(indices.at(i*3 + 0)).at(0);
            double x2 = polygonflat.at(indices.at(i*3 + 1)).at(0);
            double x3 = polygonflat.at(indices.at(i*3 + 2)).at(0);
            double y1 = polygonflat.at(indices.at(i*3 + 0)).at(1);
            double y2 = polygonflat.at(indices.at(i*3 + 1)).at(1);
            double y3 = polygonflat.at(indices.at(i*3 + 2)).at(1);

            mp->Add(new SPHPolygon(x1,y1,x2,y2,x3,y3));
        }
    }

    return mp;
}
