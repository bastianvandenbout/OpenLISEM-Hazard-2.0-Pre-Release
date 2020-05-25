#ifndef OPENCLPROGRAM_H
#define OPENCLPROGRAM_H



#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"

#include <QList>
#include <QString>
#include "openglcldatabuffer.h"
#include "openglcltexture.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "spherror.h"


template <typename A>
struct CLProgramArgument
{
    int id;
    A data;
    int length;

};


class OpenCLProgram
{
public:
    QString m_programpath;

    cl::Program m_program;
    cl::Kernel m_kernel;

    int m_RunDimCount = 0;
    int m_RunDims[3] = {0,0,0};

    QList<CLProgramArgument<OpenGLCLBuffer*>> argumentlist_buffer;
    QList<CLProgramArgument<OpenGLCLTexture*>> argumentlist_texture;
    QList<CLProgramArgument<float*>> argumentlist_float_ref;
    QList<CLProgramArgument<int*>> argumentlist_int_ref;
    QList<CLProgramArgument<double*>> argumentlist_double_ref;
    QList<CLProgramArgument<char*>> argumentlist_char_ref;
    QList<CLProgramArgument<float>> argumentlist_float;
    QList<CLProgramArgument<int>> argumentlist_int;
    QList<CLProgramArgument<double>> argumentlist_double;
    QList<CLProgramArgument<char>> argumentlist_char;

public:

    OpenCLProgram();

    inline int LoadProgram(cl::Context c, cl::Device d, QString file,QString name)
    {
        return LoadProgram(c,d,file.toStdString(),name.toStdString());
    }

    inline int LoadProgram(cl::Context c, cl::Device d, std::string file,std::string name)
    {
        SPH_DEBUG("compiling " + QString(file.c_str()) + " " + QString(name.c_str()));

        ClearArguments();

        std::ostringstream options;
        options << "-I " << std::string(ASSETS_DIR);

        m_programpath = QString(file.c_str());

        cl_int error;
        m_program = getProgram(c,file,error);
        cl_int err = 0;
        try{

            err = m_program.build(std::vector<cl::Device>(1, d),options.str().c_str());
        } catch (cl::Error err) {
            SPH_ERROR(QString("compilation error ") + err.what() + "(" + err.err() + ") " + OCL_GetErrorString(err.err()));


            if (err.err() == CL_BUILD_PROGRAM_FAILURE) {

                  // Check the build status
                  cl_build_status status = m_program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(d);

                  // Get the build log
                  std::string name     = d.getInfo<CL_DEVICE_NAME>();
                  std::string buildlog = m_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(d);
                  SPH_ERROR(QString("Build log for ")+ name.c_str() + ": \n" + buildlog.c_str());

            }
            if(err.err() == CL_BUILD_SUCCESS)
            {
                 SPH_DEBUG(QString("sucessfully compiled ")+ name.c_str());
            }
        }



        m_kernel = cl::Kernel(m_program,name.c_str());

        return 1;
    }

    inline int SetRunDims(int dim0, int dim1 = 0, int dim2 = 0)
    {
        if(dim2 > 0)
        {
            m_RunDimCount = 3;
        }else if (dim1 > 0)
        {
            m_RunDimCount = 2;

        }else
        {
            m_RunDimCount = 1;
        }

        m_RunDims[0] = dim0;
        m_RunDims[1] = dim1;
        m_RunDims[2] = dim2;

        return 0;
    }

    inline int Run(cl::Context &c, cl::CommandQueue &q)
    {
        SetArgumentsForExecution(c,q);

        cl::Event ev;


        glad_glFinish();

        std::vector<cl::Memory> objs;
        objs.clear();


        for(int i = 0; i < argumentlist_buffer.length(); i++)
        {
            CLProgramArgument<OpenGLCLBuffer*> arg = argumentlist_buffer.at(i);
            objs.push_back(arg.data->m_buffergl);
        }

        for(int i = 0; i < argumentlist_texture.length(); i++)
        {
            CLProgramArgument<OpenGLCLTexture*> arg = argumentlist_texture.at(i);
            objs.push_back(arg.data->m_texcl);
        }

        cl_int res = q.enqueueAcquireGLObjects(&objs,NULL,&ev);
        ev.wait();
        if (res!=CL_SUCCESS) {
            std::cout<<"Failed releasing GL object: "<<res<<std::endl;
            exit(247);
        }

        cl::NDRange local1d(16);
        cl::NDRange local2d(16, 16);
        cl::NDRange local3d(16, 16,16);
        cl::NDRange global;



        if(m_RunDimCount == 1)
        {
            global = cl::NDRange( local1d[0] * divup((int)m_RunDims[0], local1d[0]));

            q.enqueueNDRangeKernel(m_kernel,cl::NullRange,global,local1d);

        }else if (m_RunDimCount == 2)
        {
            global = cl::NDRange( local2d[0] * divup((int)m_RunDims[0], local2d[0]),
                            local2d[1] * divup((int)m_RunDims[1], local2d[1]));

            q.enqueueNDRangeKernel(m_kernel,cl::NullRange,global,local2d);


        }else if (m_RunDimCount == 3)
        {
            global = cl::NDRange( local3d[0] * divup((int)m_RunDims[0], local3d[0]),
                            local3d[1] * divup((int)m_RunDims[1], local3d[1]),
                            local3d[1] * divup((int)m_RunDims[2], local3d[1]));

            q.enqueueNDRangeKernel(m_kernel,cl::NullRange,global,local3d);

        }else
        {
            return -1;
        }


        for(int i = 0; i < argumentlist_buffer.length(); i++)
        {
            CLProgramArgument<OpenGLCLBuffer*> arg = argumentlist_buffer.at(i);
            res = q.enqueueCopyBuffer(arg.data->m_buffercl, arg.data->m_buffergl, 0, 0, arg.data->m_size, NULL, NULL);
            if (res!=CL_SUCCESS) {
                std::cout<<"Failed releasing GL object: "<<res<<std::endl;
                exit(247);
            }

        }



        res = q.enqueueReleaseGLObjects(&objs);
        ev.wait();

        if (res!=CL_SUCCESS) {
            std::cout<<"Failed releasing GL object: "<<res<<std::endl;
            exit(247);
        }

        res = q.finish();

        if (res!=CL_SUCCESS) {
            std::cout<<"Failed releasing GL object: "<<res<<std::endl;
            exit(247);
        }

        return 0;
    }

    inline int SetArgumentsForExecution(cl::Context &c, cl::CommandQueue &q)
    {


        //textures and dat abuffers require additional functionality
        for(int i = 0; i < argumentlist_buffer.length(); i++)
        {
            CLProgramArgument<OpenGLCLBuffer*> arg = argumentlist_buffer.at(i);
            m_kernel.setArg(arg.id,arg.data->m_buffercl);
        }

        for(int i = 0; i < argumentlist_texture.length(); i++)
        {
            CLProgramArgument<OpenGLCLTexture*> arg = argumentlist_texture.at(i);
            m_kernel.setArg(arg.id,arg.data->m_texcl);
        }

        //normal data using template function
        SetArgumentForExecutionFromList_R(argumentlist_float_ref);
        SetArgumentForExecutionFromList_R(argumentlist_int_ref);
        SetArgumentForExecutionFromList_R(argumentlist_double_ref);
        SetArgumentForExecutionFromList_R(argumentlist_char_ref);
        SetArgumentForExecutionFromList_NR(argumentlist_float);
        SetArgumentForExecutionFromList_NR(argumentlist_int);
        SetArgumentForExecutionFromList_NR(argumentlist_double);
        SetArgumentForExecutionFromList_NR(argumentlist_char);

        return 0.0;
    }

    template <typename B>
    inline int SetArgumentForExecutionFromList_R(QList<CLProgramArgument<B>> &argumentlist)
    {
        for(int i = 0; i < argumentlist.length(); i++)
        {
            CLProgramArgument<B> arg = argumentlist.at(i);
            m_kernel.setArg(arg.id,arg.length * sizeof(arg),arg.data);
        }
        return 0;
    }

    template <typename B>
    inline int SetArgumentForExecutionFromList_NR(QList<CLProgramArgument<B>> &argumentlist)
    {
        for(int i = 0; i < argumentlist.length(); i++)
        {
            CLProgramArgument<B> arg = argumentlist.at(i);
            m_kernel.setArg(arg.id,arg.data);
        }
        return 0;
    }

    template <typename B>
    inline void RemoveArgumentWithIndexFromList(int index,QList<CLProgramArgument<B>> &argumentlist)
    {
        for(int i =  argumentlist.length()-1; i > -1; i--)
        {
            CLProgramArgument<B> arg = argumentlist.at(i);
            if(arg.id == index)
            {
                argumentlist.removeAt(i);
            }
        }


    }

    inline void RemoveArgumentWithIndex(int index)
    {

        RemoveArgumentWithIndexFromList(index,argumentlist_buffer);
        RemoveArgumentWithIndexFromList(index,argumentlist_texture);
        RemoveArgumentWithIndexFromList(index,argumentlist_float_ref);
        RemoveArgumentWithIndexFromList(index,argumentlist_int_ref);
        RemoveArgumentWithIndexFromList(index,argumentlist_double_ref);
        RemoveArgumentWithIndexFromList(index,argumentlist_char_ref);
        RemoveArgumentWithIndexFromList(index,argumentlist_float);
        RemoveArgumentWithIndexFromList(index,argumentlist_int);
        RemoveArgumentWithIndexFromList(index,argumentlist_double);
        RemoveArgumentWithIndexFromList(index,argumentlist_char);

    }


    inline void PlaceArgument(int index, OpenGLCLBuffer *arg)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<OpenGLCLBuffer*> argument = {index,arg,1};
        argumentlist_buffer.append(argument);
    }

    inline void PlaceArgument(int index, OpenGLCLTexture *arg)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<OpenGLCLTexture*> argument = {index,arg,1};
        argumentlist_texture.append(argument);


    }

    inline void PlaceArgument(int index, float *arg, int length = 1)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<float*> argument = {index,arg,length};
        argumentlist_float_ref.append(argument);
    }

    inline void PlaceArgument(int index, int *arg, int length = 1)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<int*> argument = {index,arg,length};
        argumentlist_int_ref.append(argument);
    }

    inline void PlaceArgument(int index, double *arg, int length = 1)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<double*> argument = {index,arg,length};
        argumentlist_double_ref.append(argument);
    }

    inline void PlaceArgument(int index, char *arg, int length = 1)
    {
        CLProgramArgument<char*> argument = {index,arg,length};
        argumentlist_char_ref.append(argument);
    }

    inline void PlaceArgument(int index, float arg)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<float> argument = {index,arg,1};
        argumentlist_float.append(argument);
    }

    inline void PlaceArgument(int index, int arg)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<int> argument = {index,arg,1};
        argumentlist_int.append(argument);
    }

    inline void PlaceArgument(int index, double arg)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<double> argument = {index,arg,1};
        argumentlist_double.append(argument);
    }

    inline void PlaceArgument(int index, char arg)
    {
        RemoveArgumentWithIndex(index);
        CLProgramArgument<char> argument = {index,arg,1};
        argumentlist_char.append(argument);
    }

    inline void ClearArguments()
    {

        argumentlist_buffer.clear();
        argumentlist_texture.clear();
        argumentlist_float_ref.clear();
        argumentlist_int_ref.clear();
        argumentlist_double_ref.clear();
        argumentlist_char_ref.clear();
        argumentlist_float.clear();
        argumentlist_int.clear();
        argumentlist_double.clear();
        argumentlist_char.clear();
    }

    inline void Destroy()
    {


    }



};

#endif // OPENCLPROGRAM_H
