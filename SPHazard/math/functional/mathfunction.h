#include "muParser.h"
#include "QString"
#include "QList"

class MathFunction
{
    mu::Parser  p;
    QString Expression;

    inline void SetExpression(QString s)
    {
        p.SetExpr(Expression.toStdString());
    }

    inline int GetParameterCount()
    {
        p.SetExpr(Expression.toStdString());
        return p.GetVar().size();
    }

    inline void IsValid()
    {


    }

    inline float Evaluate(QList<double> params)
    {


    }

};
