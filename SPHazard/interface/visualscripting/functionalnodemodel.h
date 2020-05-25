#pragma once
#include <nodes/NodeDataModel>
#include "scriptmanager.h"

#include <QtCore/QJsonValue>

#include "QList"
#include "QString"

class FunctionalNodeModel : public QtNodes::NodeDataModel
{

    Q_OBJECT;

public:

    inline FunctionalNodeModel()
    {

    }

    inline ~FunctionalNodeModel()
    {

    }

    inline QJsonObject
    save() const
    {
      QJsonObject modelJson = QtNodes::NodeDataModel::save();



      return modelJson;
    }

    inline void
    restore(QJsonObject const &p)
    {


    }

    inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
    {
        QList<QString> l;

        return l;
    }

    inline void SetValidationState(QtNodes::NodeValidationState s)
    {
        modelValidationState = s;
    }

    inline void SetValidationMessage(QString message)
    {
        modelValidationError = message;
    }
    inline QtNodes::NodeValidationState validationState() const override
    {
       return modelValidationState;
    }

    inline QString  validationMessage() const override
    {
      return modelValidationError;
    }


    QtNodes::NodeValidationState modelValidationState = QtNodes::NodeValidationState::Valid;
    QString modelValidationError = QString("");




};
