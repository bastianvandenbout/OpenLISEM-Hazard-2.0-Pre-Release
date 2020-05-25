#ifndef SCRIPTOPTIMIZATION_H
#define SCRIPTOPTIMIZATION_H

#include "Eigen/Core"
#include "leastsquares.h"
#include "angelscript.h"
#include "scriptarray.h"
#include "QList"
#include "sphscriptengine.h"
#include "spherror.h"


inline static CScriptArray * OptimizeGumbel(CScriptArray * data_x, CScriptArray * data_y, double initial_c1, double initial_c2)
{
    if(!(data_x->IsNumberType() && data_y->IsNumberType()))
    {
        SPHS_ERROR("array<> does not contain numbers");
        throw 1;
    }

    QList<double> *ListX = data_x->ToQListDouble();
    QList<double> *ListY = data_y->ToQListDouble();


    if((ListX == nullptr || ListY == nullptr))
    {
        SPHS_ERROR("array<> could not be converted to numbers");
        throw 1;
    }

    if((ListX->length() == 0 || ListY->length() == 0))
    {
        SPHS_ERROR("array<> can not contain zero elements");
        throw 1;
    }
    if(!(ListX->length() == ListY->length() ))
    {
        SPHS_ERROR("array<> arguments are not of identical size");
        throw 1;
    }

    int n = 0;
    double x_avg = 0.0;
    double x_min = 1e30;
    double x_max = -1e30;

    for(int i = 0; i < ListX->length(); i++)
    {
        n++;
        x_avg += ListX->at(i);
        x_min = std::min(x_min,ListX->at(i));
        x_max = std::max(x_max,ListX->at(i));
    }
    n = std::max(1,n);
    x_avg = x_avg/n;

    lsq::GaussNewton<double, GumbelError, lsq::ArmijoBacktracking<double>> optimizer;

    // Set number of iterations as stop criterion.
    // Set it to 0 or negative for infinite iterations (default is 0).
    optimizer.setMaxIterations(200);

    // Set the minimum length of the gradient.
    // The optimizer stops minimizing if the gradient length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinGradientLength(0);

    // Set the minimum length of the step.
    // The optimizer stops minimizing if the step length falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 1e-9).
    optimizer.setMinStepLength(0);

    // Set the minimum least squares error.
    // The optimizer stops minimizing if the error falls below this
    // value.
    // Set it to 0 or negative to disable this stop criterion (default is 0).
    optimizer.setMinError(0);

    // Set the the parametrized StepSize functor used for the step calculation.
    optimizer.setStepSize(lsq::ArmijoBacktracking<double>(0.8, 0.1, 1e-10, 1.0, 0));


    optimizer.setVerbosity(2);

    GumbelError err;
    err.data_x = ListX;
    err.data_y = ListY;
    optimizer.setErrorFunction(err);

    // Set initial guess for parameters
    Eigen::VectorXd initialGuess(2);

    //just a guess about what would be some reasonable values for the constants
    double guess_c1 = x_min + 0.25*(x_avg - x_min);
    double guess_c2 = (x_max-x_min)*0.5;

    std::cout << initial_c1 << " " << initial_c2 << std::endl;
    if(!(initial_c1 <-1e30 ))
    {
        guess_c1 = initial_c1;
    }
    if(!(initial_c2 <-1e30 ))
    {
        guess_c2 = initial_c2;
    }

    initialGuess << guess_c1,guess_c2;

    try
    {

        // Start the optimization.
        auto result = optimizer.minimize(initialGuess);

        //delete fitting data
        delete ListX;
        delete ListY;

        std::cout << "xavg  " << x_avg << " y min, max " << x_min  << "  " << x_max << std::endl;
        std::cout << "Done! Converged: " << (result.converged ? "true" : "false")
            << " Iterations: " << result.iterations << std::endl;

        // do something with final function value
        std::cout << "Final fval: " << result.fval.transpose() << std::endl;

        // do something with final x-value
        std::cout << "Final xval: " << result.xval.transpose() << std::endl;


        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        if(ctx != nullptr)
        {
            asIScriptEngine *engine = ctx->GetEngine();
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<double>");

            // Create the array object
            CScriptArray *array = CScriptArray::Create(arrayType);

            array->Resize(result.xval.size());

            for(int i = 0; i < result.xval.size(); ++i)
            {
                array->SetValue(i,(void*)(new double(result.xval(i))));
            }

            return array;

        }
        SPHS_ERROR("Could not get virtual machine");
        throw 1;


    }catch(...)
    {
        delete ListX;
        delete ListY;
        SPHS_ERROR("Error in minimalization of objective function for data fitting");
        throw 1;
    }


}


inline static CScriptArray * OptimizeGumbel(CScriptArray * data_x, CScriptArray * data_y)
{
    return OptimizeGumbel(data_x,data_y,-1e31,-1e31);
}

/*inline static CScriptArray * OptimizePolynomial()
{


}

//this is much slower as it has to call a function within the scripting environment many times
inline static CScriptArray * OptimizeCustom()
{


}*/





inline static void RegisterLeastSquaresToScriptEngine(SPHScriptEngine * sm)
{

    int r = sm->RegisterGlobalFunction("array<double> @FitGumbel(array<double> & in x, array<double> & in y, double guess_c1 , double guess_c2)",asFUNCTIONPR(OptimizeGumbel,(CScriptArray*,CScriptArray*,double,double),CScriptArray*),asCALL_CDECL);  assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting array<double>  &FitGumbel(array<double> x, array<double> y)");};

    r = sm->RegisterGlobalFunction("array<double> @FitGumbel(array<double> & in x, array<double> & in y)",asFUNCTIONPR(OptimizeGumbel,(CScriptArray*,CScriptArray*),CScriptArray*),asCALL_CDECL);  assert( r >= 0 );
    if(r < 0){SPH_DEBUG("error in registering scripting array<double>  &FitGumbel(array<double> x, array<double> y)");};



}



#endif // SCRIPTOPTIMIZATION_H
