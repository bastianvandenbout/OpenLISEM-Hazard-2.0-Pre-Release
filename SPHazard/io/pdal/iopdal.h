#ifndef IOPDAL_H
#define IOPDAL_H


#include "pointcloud.h"

#include "pdal/PointTable.hpp"
#include "pdal/PointView.hpp"
#include "pdal/io/LasHeader.hpp"
#include "pdal/io/LasReader.hpp"
#include "pdal/Options.hpp"
#include "pdal/pdal.hpp"

#include "spherror.h"

using namespace pdal::Dimension;

inline static PointCloud * ReadPointCloud(QString filename)
{


    PointCloud * ret = new PointCloud();


    //set options and use a las-reader for now
    pdal::Option las_opt("filename", filename.toStdString());
    pdal::Options las_opts;
    las_opts.add(las_opt);
    pdal::PointTable table;
    pdal::LasReader las_reader;

    //do the actual work within a catch block (to catch the pdal_error)
    try {
        las_reader.setOptions(las_opts);
        las_reader.prepare(table);
        pdal::PointViewSet point_view_set = las_reader.execute(table);
        pdal::PointViewPtr point_view = *point_view_set.begin();
        pdal::Dimension::IdList dims = point_view->dims();
        pdal::LasHeader las_header = las_reader.header();


        std::string projstr = table.spatialReference().getWKT();

        double scale_x = las_header.scaleX();
        double scale_y = las_header.scaleY();
        double scale_z = las_header.scaleZ();

        double offset_x = las_header.offsetX();
        double offset_y = las_header.offsetY();
        double offset_z = las_header.offsetZ();

        double xmin = las_header.minX();
        double xmax = las_header.maxX();
        double ymin = las_header.minY();
        double ymax = las_header.maxY();

        unsigned int n_features = las_header.pointCount();


        bool have_time = las_header.hasTime();
        bool have_color = las_header.hasColor();

        ret->Resize(n_features);
        ret->SetHasColor(have_color);
        ret->SetHasTime(have_time);

        std::cout << "num_points " << n_features << std::endl;
        int i = 0;
        for (pdal::PointId idx = 0; idx < point_view->size(); ++idx) {


            double x = point_view->getFieldAs<double>(Id::X, idx);
            double y = point_view->getFieldAs<double>(Id::Y, idx);
            double z = point_view->getFieldAs<double>(Id::Z, idx);
            int return_no = point_view->getFieldAs<int>(Id::ReturnNumber, idx);
            int n_returns = point_view->getFieldAs<int>(Id::NumberOfReturns, idx);
            int point_class = point_view->getFieldAs<int>(Id::Classification, idx);
            float red = 0;
            float green = 0;
            float blue = 0;
            double time = 0.0;
            if(have_time)
            {
                time = point_view->getFieldAs<double>(Id::GpsTime, idx);
            }
            if(have_color)
            {
                red = ((float)point_view->getFieldAs<int>(Id::Red, idx))/255.0;
                green = ((float)point_view->getFieldAs<int>(Id::Green, idx))/255.0;
                blue = ((float)point_view->getFieldAs<int>(Id::Blue, idx))/255.0;

            }

            ret->SetPoint(i,x,y,z,time,red/255.0,green/255.0,blue/255.0,point_class,n_returns, return_no);
            i++;
        }

        ret->CalculateBoundingBox();

        return ret;

    } catch(pdal::pdal_error err)
    {
        SPH_ERROR(QString(err.what()));

        return nullptr;
    }

}

inline void WritePointCloud(PointCloud * pc, QString filename)
{

    /*view->setField(pdal::Dimension::Id::X, i, p.x);
    view->setField(pdal::Dimension::Id::Y, i, p.y);
    view->setField(pdal::Dimension::Id::Z, i, p.z);


    using namespace pdal;
    Options options;
    options.add("filename", "myfile.las");
    PointTable table;
    table.layout()->registerDim(Dimension::Id::X);
    table.layout()->registerDim(Dimension::Id::Y);
    table.layout()->registerDim(Dimension::Id::Z);
    PointViewPtr view(new PointView(table));
    fillView(view);
    BufferReader reader;
    reader.addView(view);
    StageFactory factory;
    // Set second argument to 'true' to let factory take ownership of
    // stage and facilitate clean up.
    Stage *writer = factory.createStage("writers.las");


    writer->setInput(reader);
    writer->setOptions(options);
    writer->prepare(table);
    writer->execute(table);*/


}


#endif // IOPDAL_H
