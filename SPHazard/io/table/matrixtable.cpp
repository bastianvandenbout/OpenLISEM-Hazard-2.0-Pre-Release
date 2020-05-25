#include "matrixtable.h"
#include "tablealgebra.h"

#define SPH_ASMAP_BOOLFROMDOUBLE(x) (x>0.5?true:false)
#define SPH_ASMAP_BOOLFROMINT(x) (x > 0?true:false)
#define SPH_ASMAP_BOOLFROMSTRING(x) (x.compare("True",Qt::CaseInsensitive) == 0? true: (x.compare("False",Qt::CaseInsensitive) == 0? false: (x.toInt() > 0)))


MatrixTable*        MatrixTable::AS_OpNeg             ()
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,-1,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::AS_OpCom            ()
{

    std::function<double(double,double)> powfuncd = [](double a, double b){return (!(SPH_ASMAP_BOOLFROMDOUBLE(a)))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return ((!SPH_ASMAP_BOOLFROMINT(a)))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return ((!SPH_ASMAP_BOOLFROMSTRING(a)))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,"1",powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::AS_OpAdd             (MatrixTable*other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    return ApplyOperatorToMatrixTables(this,other,std::plus<int>(),std::plus<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpMul             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::AS_OpSub             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::minus<int>(),std::minus<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpDiv             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::divides<int>(),std::divides<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpPow             (MatrixTable*other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_OpMod             (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable*        MatrixTable::AS_EqualTo           (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_LargerThen        (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_SmallerThen       (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_NotEqualTo        (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_LargerEqualThen   (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a >= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a >= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a >= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_SmallerEqualThen  (MatrixTable*other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::AS_And               (MatrixTable*other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) && SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) && SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) && SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_Or                (MatrixTable*other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) || SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) || SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) || SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_Xor               (MatrixTable*other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) ^ SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) ^ SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) ^ SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::AS_OpAddAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    ApplyOperatorToMatrixTables(target,other,std::plus<int>(),std::plus<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;
}

MatrixTable*        MatrixTable::AS_OpSubAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::minus<int>(),std::minus<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpMulAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::multiplies<int>(),std::multiplies<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpDivAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::divides<int>(),std::divides<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpPowAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpModAssign       (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;
}


MatrixTable*        MatrixTable::AS_AndAssign         (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) && SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) && SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) && SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OrAssign          (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) || SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) || SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) || SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

    target->AS_UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::AS_XorAssign         (MatrixTable*other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) ^ SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) ^ SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) ^ SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;
}




MatrixTable*        MatrixTable::AS_OpAdd             (double other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    return ApplyOperatorToMatrixTables(this,other,std::plus<int>(),std::plus<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpMul             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::AS_OpSub             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::minus<int>(),std::minus<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpDiv             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::divides<int>(),std::divides<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpPow             (double other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_OpMod             (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable*        MatrixTable::AS_EqualTo           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_LargerThen        (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_SmallerThen       (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_NotEqualTo        (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_LargerEqualThen   (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a >= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a >= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a >= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_SmallerEqualThen  (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::AS_And               (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) && SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) && SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) && SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_Or                (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) || SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) || SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) || SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_Xor               (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) ^ SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) ^ SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) ^ SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::AS_OpAddAssign       (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    ApplyOperatorToMatrixTables(target,other,std::plus<int>(),std::plus<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;
}

MatrixTable*        MatrixTable::AS_OpSubAssign       (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::minus<int>(),std::minus<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpMulAssign       (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::multiplies<int>(),std::multiplies<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpDivAssign       (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::divides<int>(),std::divides<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpPowAssign       (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpModAssign       (double  other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;
}


MatrixTable*        MatrixTable::AS_AndAssign         (double  other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) && SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) && SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) && SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OrAssign          (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) || SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) || SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) || SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->AS_UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::AS_XorAssign         (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) ^ SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) ^ SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) ^ SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;
}




MatrixTable*        MatrixTable::AS_OpAdd             (int other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    return ApplyOperatorToMatrixTables(this,other,std::plus<int>(),std::plus<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpMul             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::multiplies<int>(),std::multiplies<double>(),addfunc);
}


MatrixTable*        MatrixTable::AS_OpSub             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::minus<int>(),std::minus<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpDiv             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    return ApplyOperatorToMatrixTables(this,other,std::divides<int>(),std::divides<double>(),addfunc);
}

MatrixTable*        MatrixTable::AS_OpPow             (int other)
{

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_OpMod             (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable*        MatrixTable::AS_EqualTo           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_LargerThen        (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_SmallerThen       (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_NotEqualTo        (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_LargerEqualThen   (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a >= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a >= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a >= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_SmallerEqualThen  (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::AS_And               (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) && SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) && SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) && SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}
MatrixTable*        MatrixTable::AS_Or                (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) || SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) || SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) || SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}
MatrixTable*        MatrixTable::AS_Xor               (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) ^ SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) ^ SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) ^ SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::AS_OpAddAssign       (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a+b;};
    ApplyOperatorToMatrixTables(target,other,std::plus<int>(),std::plus<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;
}

MatrixTable*        MatrixTable::AS_OpSubAssign       (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::minus<int>(),std::minus<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpMulAssign       (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::multiplies<int>(),std::multiplies<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpDivAssign       (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,std::divides<int>(),std::divides<double>(),addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpPowAssign       (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(a,b);};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OpModAssign       (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(a,b);};
    std::function<int(int,int)> powfunci = [](int a, int b){return a%b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;
}


MatrixTable*        MatrixTable::AS_AndAssign         (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) && SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) && SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) && SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_OrAssign          (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) || SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) || SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) || SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc);

    target->AS_UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::AS_XorAssign         (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(a) ^ SPH_ASMAP_BOOLFROMDOUBLE(b))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(a) ^ SPH_ASMAP_BOOLFROMINT(b))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(a) ^ SPH_ASMAP_BOOLFROMSTRING(b))? "True":"False";};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;
}



MatrixTable*        MatrixTable::AS_Assign            (double other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_Assign            (int other)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b;};
    ApplyOperatorToMatrixTables(target,other,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;

}
MatrixTable*        MatrixTable::AS_Assign            (QString s)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(double,double)> powfuncd = [](double a, double b){return b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b;};
    ApplyOperatorToMatrixTables(target,s,powfunci,powfuncd,addfunc,true);

    target->AS_UpdateParent();
    return target;

}

MatrixTable*        MatrixTable::AS_OpAdd             (QString s)
{
    return ApplyOperatorToMatrixTables(this,s,std::plus<int>(),std::plus<double>(),std::plus<QString>());
}
MatrixTable*        MatrixTable::AS_OpAddAssign       (QString s)
{
    MatrixTable*target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    ApplyOperatorToMatrixTables(this,s,std::plus<int>(),std::plus<double>(),std::plus<QString>(),true);

    target->AS_UpdateParent();
    return target;
}
MatrixTable*        MatrixTable::AS_EqualTo           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);
}



MatrixTable*        MatrixTable::AS_OpAdd_r             (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b+a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b+a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b+a;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);
}

MatrixTable*        MatrixTable::AS_EqualTo_r           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::AS_NotEqualTo_r           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);

}

MatrixTable*        MatrixTable::AS_NotEqualTo           (QString s)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,s,powfunci,powfuncd,addfunc);

}



MatrixTable *        MatrixTable::AS_OpMod_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return b%a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_OpAdd_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b+a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b+a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b+a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::AS_OpMul_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b*a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b*a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::AS_OpSub_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b-a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b-a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_OpDiv_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b/a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b/a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_OpPow_r           (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(b,a);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_LargerThen_r      (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_SmallerThen_r     (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}


MatrixTable *        MatrixTable::AS_NotEqualTo_r      (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_LargerEqualThen_r (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_SmallerEqualThen_r(double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_EqualTo_r         (double other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_And_r             (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(b) && SPH_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(b) && SPH_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(b) && SPH_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable *        MatrixTable::AS_Or_r              (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(b) || SPH_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(b) || SPH_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(b) || SPH_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_Xor_r             (double other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(b) ^ SPH_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(b) ^ SPH_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(b) ^ SPH_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}





MatrixTable *        MatrixTable::AS_OpMod_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::fmod(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return b%a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_OpAdd_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return b+a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b+a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b+a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::AS_OpMul_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b*a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b*a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


MatrixTable *        MatrixTable::AS_OpSub_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b-a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b-a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_OpDiv_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return b/a;};
    std::function<int(int,int)> powfunci = [](int a, int b){return b/a;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_OpPow_r           (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString /*b*/){return a;};
    std::function<double(double,double)> powfuncd = [](double a, double b){return std::pow(b,a);};
    std::function<int(int,int)> powfunci = [](int a, int b){return std::pow(b,a);};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);


}


MatrixTable *        MatrixTable::AS_LargerThen_r      (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a > b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a > b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a > b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_SmallerThen_r     (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a < b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a < b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a < b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}


MatrixTable *        MatrixTable::AS_NotEqualTo_r      (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a != b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a != b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a != b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_LargerEqualThen_r (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_SmallerEqualThen_r(int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a <= b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a <= b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a <= b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_EqualTo_r         (int other)
{
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return a == b? "1":"0";};
    std::function<double(double,double)> powfuncd = [](double a, double b){return a == b;};
    std::function<int(int,int)> powfunci = [](int a, int b){return a == b;};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_And_r             (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(b) && SPH_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(b) && SPH_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(b) && SPH_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}

MatrixTable *        MatrixTable::AS_Or_r              (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(b) || SPH_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(b) || SPH_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(b) || SPH_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);
}

MatrixTable *        MatrixTable::AS_Xor_r             (int other)
{
    std::function<double(double,double)> powfuncd = [](double a, double b){return (SPH_ASMAP_BOOLFROMDOUBLE(b) ^ SPH_ASMAP_BOOLFROMDOUBLE(a))? 1.0f:0.0f;};
    std::function<int(int,int)> powfunci = [](int a, int b){return (SPH_ASMAP_BOOLFROMINT(b) ^ SPH_ASMAP_BOOLFROMINT(a))? 1.0f:0.0f;};
    std::function<QString(QString,QString)> addfunc = [](QString a, QString b){return (SPH_ASMAP_BOOLFROMSTRING(b) ^ SPH_ASMAP_BOOLFROMSTRING(a))? "True":"False";};
    return ApplyOperatorToMatrixTables(this,other,powfunci,powfuncd,addfunc);

}


