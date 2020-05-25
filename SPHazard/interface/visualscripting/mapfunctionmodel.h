#pragma once

#include "functionalnodemodel.h"
#include "scriptmanager.h"
#include "mapdatatype.h"
#include "functionalnodemodel.h"

class MapFunctionModel : public FunctionalNodeModel
{

    Q_OBJECT;

private:

    ScriptFunctionInfo m_Function;
    ScriptManager *m_ScriptManager;

public:

    inline MapFunctionModel(ScriptFunctionInfo func, ScriptManager * sm)
    {
        m_ScriptManager = sm;
        m_Function = func;

    }
    inline ~MapFunctionModel() {}


public:

  QString
  caption() const override
  {
      if(m_Function.is_basetypeoperator)
      {
          QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);
          QString opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.basetypeoperatorname);

          if(m_Function.basetypeoperatorparametertype.isEmpty())
          {
                return op + " " + m_Function.ObjectName + " (" + opname + ")";
          }else {
                return m_Function.ObjectName + " " +  op + " " +m_Function.basetypeoperatorparametertype + " (" + opname + ")";
            }

      }else {
          QString parameters;
          parameters += "(";

          int paramcount = m_Function.Function->GetParamCount();

          for(int i = 0; i < paramcount; i++)
          {
              int id = 0;
              m_Function.Function->GetParam(i,&id);
              parameters += m_ScriptManager->m_Engine->GetTypeName(id);
              if(i != paramcount -1)
              {
                  parameters += ",";
              }
          }
          parameters += ")";

          if(m_Function.is_constructor)
          {
              return QString(m_Function.ObjectName);
          }else if(m_Function.is_member)
          {
              QString op;
              QString opname;
              bool opinv;

              if(QString(m_Function.Function->GetName()).compare("eq") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("eq_r") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = true;

              }else if(QString(m_Function.Function->GetName()).compare("neq") == 0)
              {
                  op = "!=";
                  opname = "not equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("neq_r") == 0)
              {
                    op = "!=";
                    opname = "not equal to";
                    opinv = true;

              }else {

                  op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.Function->GetName());
                  opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.Function->GetName());
                  opinv = m_ScriptManager->m_Engine->GetOperatorIsReverseFromMemberFunctionName(m_Function.Function->GetName());
              }

              if(!op.isEmpty())
              {

                 return opname;
              }else {
                  return m_Function.ObjectName;
              }
          }else {
              return QString(m_Function.Function->GetName());
         }

      }

  }


  bool
  captionVisible() const override
  { return true; }

  bool
  portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex ) const override
  { return true; }

  QString
  portCaption(QtNodes::PortType portType,QtNodes::PortIndex portIndex) const override
  {
    switch (portType)
    {
      case QtNodes::PortType::In:
            {
                if(m_Function.is_basetypeoperator)
                {

                    if(portIndex == 0)
                    {
                        return "Input 1 (" + m_Function.ObjectName + ")";

                    }else {
                        return "Input 2 (" + m_Function.basetypeoperatorparametertype + ")";

                    }
                }else {
                    if(m_Function.is_member)
                    {
                        if(portIndex == 0)
                        {
                            QString name;
                            QString type = m_Function.ObjectName;

                            QString out  = name.isEmpty()? QString("Input ") + QString::number(portIndex) + " (" + type + ")": name + " (" + type + ")";
                            return out;
                        }else {
                            portIndex = portIndex -1;

                            QString name;
                            if(portIndex < m_Function.Parameters.length())
                            {
                                name =m_Function.Parameters.at(portIndex);
                            }
                            int tid = 0;
                            m_Function.Function->GetParam(portIndex, &tid);
                            QString type = m_ScriptManager->m_Engine->GetTypeName(tid);

                            QString out  = name.isEmpty()? QString("Input ") + QString::number(portIndex+1) + " (" + type + ")": name + " (" + type + ")";
                            return out;
                        }
                    }else {
                        QString name;
                        if(portIndex < m_Function.Parameters.length())
                        {
                            name =m_Function.Parameters.at(portIndex);
                        }
                        int tid = 0;
                        m_Function.Function->GetParam(portIndex, &tid);
                        QString type = m_ScriptManager->m_Engine->GetTypeName(tid);

                        QString out  = name.isEmpty()? QString("Input ") + QString::number(portIndex) + " (" + type + ")": name + " (" + type + ")";
                        return out;

                    }
                }
            }

      case QtNodes::PortType::Out:
            {
                if(m_Function.is_basetypeoperator)
                {
                    return m_Function.basetypeoperatorreturntype;

                }else
                {
                    if(m_Function.is_valueobjectconstructor)
                    {
                        return QString("Result (") + m_Function.ObjectName + ")";

                    }else {
                        int tid = m_Function.Function->GetReturnTypeId();
                        m_Function.Function->GetParam(portIndex, &tid);
                        QString type = m_ScriptManager->m_Engine->GetTypeName(tid);


                        return QString("Result (") + type + ")";

                    }

                }

            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {
      if(m_Function.is_basetypeoperator)
      {
          QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);
          QString opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.basetypeoperatorname);

          if(m_Function.basetypeoperatorparametertype.isEmpty())
          {
                return op + " " + m_Function.ObjectName + " (" + opname + ")";
          }else {
                return m_Function.ObjectName + " " +  op + " " +m_Function.basetypeoperatorparametertype +  " (" + opname + ")";
            }

      }else {
          QString parameters;
          parameters += "(";

          int paramcount = m_Function.Function->GetParamCount();

          for(int i = 0; i < paramcount; i++)
          {
              int id = 0;
              m_Function.Function->GetParam(i,&id);
              parameters += m_ScriptManager->m_Engine->GetTypeName(id);
              if(i != paramcount -1)
              {
                  parameters += ",";
              }
          }
          parameters += ")";

          if(m_Function.is_constructor)
          {
              return QString(m_Function.ObjectName)+"::"+ parameters;
          }else if(m_Function.is_member)
          {
              QString op;
              QString opname;
              bool opinv;

              if(QString(m_Function.Function->GetName()).compare("eq") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("eq_r") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = true;

              }else if(QString(m_Function.Function->GetName()).compare("neq") == 0)
              {
                  op = "!=";
                  opname = "not equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("neq_r") == 0)
              {
                    op = "!=";
                    opname = "not equal to";
                    opinv = true;

              }else {

                  op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.Function->GetName());
                  opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.Function->GetName());
                  opinv = m_ScriptManager->m_Engine->GetOperatorIsReverseFromMemberFunctionName(m_Function.Function->GetName());
              }

              if(!op.isEmpty())
              {

                  QString object2name;
                  if(paramcount > 0)
                  {
                      int id = 0;
                      m_Function.Function->GetParam(0,&id);
                      object2name = m_ScriptManager->m_Engine->GetTypeName(id);
                  }

                  if(opinv)
                  {
                      return m_Function.ObjectName + " " + op + " " + object2name + "(" + opname + ")";
                  }else {
                      return object2name + " " + op + " " +m_Function.ObjectName +  "(" + opname + ")";
                    }
              }else {
                  return m_Function.ObjectName + QString("::") + QString(m_Function.Function->GetName()) + parameters;
              }
          }else {
              return QString(m_Function.Function->GetName())+ parameters;
         }
      }
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          if(m_Function.is_basetypeoperator)
          {
               if(m_Function.basetypeoperatorparametertype.isEmpty())
               {
                   return 1;
               }else {
                   return 2;
               }

          }else
          {
              if(!(m_Function.is_member))
              {
                  result = m_Function.Function->GetParamCount();
              }else {
                result = m_Function.Function->GetParamCount() + 1;
              }
          }
      }
      else
      {
          if(m_Function.is_basetypeoperator)
          {
            return 1;

          }else
          {
              if(m_Function.is_valueobjectconstructor)
              {
                  return 1;

              }else {

                  int typeId =m_Function.Function->GetReturnTypeId();
                  if(typeId == asTYPEID_VOID)
                  {
                      return 0;
                  }else {
                      return 1;
                  }

              }
          }

      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {

      switch (portType)
      {
        case QtNodes::PortType::In:
              {
                  if(m_Function.is_basetypeoperator)
                  {
                      if(portIndex == 0)
                      {
                            return GetDataFromTypeName(m_Function.ObjectName);
                      }else {
                            return GetDataFromTypeName(m_Function.basetypeoperatorparametertype);
                        }

                  }else {

                      if(m_Function.is_member)
                      {
                          if(portIndex == 0)
                          {
                              m_Function.ObjectType->GetTypeId();
                          }else
                          {
                              QString name;
                              if(portIndex-1 < m_Function.Parameters.length())
                              {
                                  name =m_Function.Parameters.at(portIndex-1);
                              }
                              int tid = 0;
                              m_Function.Function->GetParam(portIndex-1, &tid);
                              return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(tid));
                          }


                      }else if(m_Function.is_constructor)
                      {
                          QString name;
                          if(portIndex < m_Function.Parameters.length())
                          {
                              name =m_Function.Parameters.at(portIndex);
                          }
                          int tid = 0;
                          m_Function.Function->GetParam(portIndex, &tid);
                          return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(tid));

                      }else
                      {
                          QString name;
                          if(portIndex < m_Function.Parameters.length())
                          {
                              name =m_Function.Parameters.at(portIndex);
                          }
                          int tid = 0;
                          m_Function.Function->GetParam(portIndex, &tid);
                          return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(tid));
                      }
                  }

              }

        case QtNodes::PortType::Out:
              {
                  if(m_Function.is_basetypeoperator)
                  {
                      return GetDataFromTypeName(m_Function.basetypeoperatorreturntype);
                  }else {
                      if(m_Function.is_valueobjectconstructor)
                      {
                          return GetDataFromTypeName(m_Function.ObjectName);
                           }else {
                          int typeId = m_Function.Function->GetReturnTypeId();
                          return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(typeId));

                    }
                  }
              }


        default:
          break;
      }

      return MapData().type();
  }

  inline std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override
  {
        return std::static_pointer_cast<QtNodes::NodeData>(_result);
  }

  inline void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex portIndex) override
  {
      auto Data =
        std::dynamic_pointer_cast<MapData>(data);

      if (portIndex == 0)
      {
        _number1 = Data;
      }
      else
      {
        _number2 = Data;
      }

      compute();

  }

  inline QWidget * embeddedWidget() override
  {
      return nullptr;
  }

  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e) override
  {
      QList<QString> l;

      if(m_Function.is_basetypeoperator)
      {

          bool error = false;
          if(!((outputs.size() ==  1) ))
          {
              std::cout << "output parameters do not match parameter count " << std::endl;
              error = true;
          }

          if(!((inputs.size() == 1 && m_Function.basetypeoperatorparametertype.isEmpty() )|| ((inputs.size() == 2 && !m_Function.basetypeoperatorparametertype.isEmpty() ))))
          {
              std::cout << "input parameters do not match parameter count " << std::endl;
              error = true;
          }

          if(!error)
          {
              if(m_Function.basetypeoperatorparametertype.isEmpty())
              {
                  QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);

                  QString line;
                  line = m_Function.basetypeoperatorreturntype + " " + outputs.at(0) + " = " + op +  inputs.at(0)+ ";";

                  l.append(line);
              }else {


                  QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);

                  QString line;
                  line = m_Function.basetypeoperatorreturntype + " " + outputs.at(0) + " = " + inputs.at(0)  + op + inputs.at(1) + ";";

                  l.append(line);

                }
          }
      }else {



          //check consistencty
          //returning an empty stringlist indicates an error occured
          bool error = false;

          int r_typeId =m_Function.Function->GetReturnTypeId();
          if(!m_Function.is_valueobjectconstructor)
          {
              if(!(r_typeId == asTYPEID_VOID))
              {
                if(!((outputs.size() ==  1) || (outputs.size() == 0)))
                {
                    std::cout << "output parameters do not match parameter count " << std::endl;
                    error = true;
                }
              }else
              {
                if(outputs.size() > 0)
                {
                    std::cout << "output parameters do not match parameter count " << std::endl;
                    error = true;
                }
              }
          }else {

              if(outputs.size() != 1)
              {
                  std::cout << "output parameters do not match parameter count " << std::endl;
                  error = true;
              }

            }
          int paramcount = m_Function.Function->GetParamCount() ;

          if(!((paramcount+ (m_Function.is_member?  1:0)) == inputs.size()))
          {
              std::cout << "input parameters do not match parameter count " << inputs.size() << "  " << paramcount << " " << paramcount+ (m_Function.is_member?  1:0) << std::endl;
              error = true;
          }

          if(!error)
          {
              if(m_Function.is_member)
              {
                  QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.Function->GetName());
                  QString opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.Function->GetName());
                  bool opinv = m_ScriptManager->m_Engine->GetOperatorIsReverseFromMemberFunctionName(m_Function.Function->GetName());

                  if(!op.isEmpty())
                  {

                      std::cout << "operator code " << paramcount << " " << inputs.size() << std::endl;
                      //use the operator notation
                      //Unary operator (such as negate: -a)
                      if(paramcount == 0 && inputs.size() == 1)
                      {
                          QString line;
                          if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                          {
                                line += e->GetTypeName(r_typeId) + " " + outputs.at(0) + " = ";
                          }

                          //use object name as function name (since it is a constructor)
                          line += op + inputs.at(0);
                          line += ";";

                          l.append(line);
                      }else if(inputs.size() == 2){ //binary operator (addition: a + b)

                          QString line;
                          if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                          {
                                line += e->GetTypeName(r_typeId) + " " + outputs.at(0) + " = ";
                          }

                          QString obj1 = inputs.at(0);
                          QString obj2 = inputs.at(1);

                          //use object name as function name (since it is a constructor)
                          line += opinv? obj1 + op + obj2: obj2 + op + obj1;
                          line += ";";

                          l.append(line);
                      }

                  }else {

                      //use normal member function notation
                      //make the code
                      QString line;
                      QString line2;
                      if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0 && inputs.size() > 0)
                      {
                          QString outputobjname;
                          if(!m_Function.is_valueobjectconstructor)
                          {
                              outputobjname = e->GetTypeName(r_typeId);
                          }else {
                              outputobjname = m_Function.ObjectName;
                          }
                            line += outputobjname + " " + outputs.at(0) + " = " +  inputs.at(0) + ";";
                            l.append(line);

                            line2 += outputs.at(0) + "." + QString(m_Function.Function->GetName()) + "(";

                            for(int i = 0; i < inputs.size()-1; i++)
                            {
                                line += inputs.at(i+1);
                                if(i != inputs.size()-2)
                                {
                                    line += ",";
                                }
                            }
                            line += ");";

                            l.append(line);

                      }

                    }
              }else if(m_Function.is_constructor)
              {
                  QString outputobjname;
                  if(!m_Function.is_valueobjectconstructor)
                  {
                      outputobjname = e->GetTypeName(r_typeId);
                  }else {
                      outputobjname = m_Function.ObjectName;
                  }

                  //use constructor notation
                  QString line;
                  if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                  {
                        line += outputobjname + " " + outputs.at(0) + " = ";
                  }

                  //use object name as function name (since it is a constructor)
                  line += QString(m_Function.ObjectName) + "(";

                  for(int i = 0; i < paramcount; i++)
                  {
                      line += inputs.at(i);
                      if(i != paramcount-1)
                      {
                          line += ",";
                      }
                  }
                  line += ");";

                  l.append(line);

              }else {
                  //make the code
                  QString line;
                  if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                  {
                        line += e->GetTypeName(r_typeId) + " " + outputs.at(0) + " = ";
                  }

                  line += QString(m_Function.Function->GetName()) + "(";

                  for(int i = 0; i < paramcount; i++)
                  {
                      line += inputs.at(i);
                      if(i != paramcount-1)
                      {
                          line += ",";
                      }
                  }
                  line += ");";

                  l.append(line);
              }

          }



      }
      return l;
  }


protected:

  //bool
  //eventFilter(QObject *object, QEvent *event) override;

protected:

  inline void compute()
  {

  }

  std::weak_ptr<MapData> _number1;
  std::weak_ptr<MapData> _number2;

  std::shared_ptr<MapData> _result;

};

