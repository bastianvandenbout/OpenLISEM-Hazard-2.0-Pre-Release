#ifndef NUMBERSOURCEMODEL_H
#define NUMBERSOURCEMODEL_H

#include "functionalnodemodel.h"
#include "scriptmanager.h"
#include "mapdatatype.h"
#include "QLineEdit"
#include "QDoubleSpinBox"
#include "QSpinBox"
#include "QCheckBox"
#include "toolparameterwidget.h"

class StringSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline StringSourceModel()
    {
        m_ValueEdit = new QLineEdit();
        //connect(m_ValueEdit,&QLineEdit::textChanged,this,&StringSourceModel::OnValueChanged);
    }
    inline ~StringSourceModel() {}


public:

  QString
  caption() const override
  { return QString("String Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (String)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("String Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
      return GetDataFromTypeName("string");
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
      return m_ValueEdit;
  }

  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("string " + outputs.at(0) + "=" + "\"" + m_ValueEdit->text() + "\";");
      return l;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->text();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->setText(str);
      }

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



  QLineEdit * m_ValueEdit = nullptr;


public slots:

  inline void OnValueChanged(QString s)
  {

  }


};

class DoubleSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline DoubleSourceModel()
    {
        m_ValueEdit = new QDoubleSpinBox();
        //connect(m_ValueEdit,&QDoubleSpinBox::valueChanged,this,&DoubleSourceModel::OnValueChanged);
    }
    inline ~DoubleSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Number Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Number)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Number Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {


    return GetDataFromTypeName("double");
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
      return m_ValueEdit;
  }


  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] =m_ValueEdit->value();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        int str = v.toInt(0);

        m_ValueEdit->setValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("double " + outputs.at(0) + "=" + "" + QString::number(m_ValueEdit->value()) + ";");
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


  QDoubleSpinBox * m_ValueEdit = nullptr;


public slots:

  inline void OnValueChanged(double s)
  {

  }


};


class IntSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline IntSourceModel()
    {
        m_ValueEdit = new QSpinBox();
        //connect(m_ValueEdit,&QSpinBox::valueChanged,this,&IntSourceModel::OnValueChanged);
    }
    inline ~IntSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Integer Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Integer)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {
    return QString("Integer Source");

  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {


   return GetDataFromTypeName("int");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] =m_ValueEdit->value();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        int str = v.toInt(0);

        m_ValueEdit->setValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("int " + outputs.at(0) + "=" + "" + QString::number(m_ValueEdit->value()) + ";");
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


  QSpinBox * m_ValueEdit = nullptr;


public slots:

  inline void OnValueChanged(int s)
  {

  }


};


class BoolSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline BoolSourceModel()
    {
        m_ValueEdit = new QCheckBox();
    }
    inline ~BoolSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Boolean Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Boolean)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Boolean Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("bool");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] =m_ValueEdit->isChecked()?1:0;

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        int str = v.toInt(0);

        m_ValueEdit->setChecked(str > 0);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("bool " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->isChecked()? "true": "false") + ";");
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


  QCheckBox * m_ValueEdit = nullptr;


public slots:



};


class MapSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline MapSourceModel()
    {
        m_ValueEdit = new MapSelectWidget();
    }
    inline ~MapSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Map Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Map)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Map Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("Map");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] =m_ValueEdit->GetValueAsString();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("Map " + outputs.at(0) + "=" + "" + m_ValueEdit->GetValueAsString() + ";");
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

  MapSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class ShapesSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline ShapesSourceModel()
    {
        m_ValueEdit = new ShapeFileSelectWidget();
    }
    inline ~ShapesSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Shapes Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Shapes)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Shapes Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("Shapes");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("Shapes " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
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


  ShapeFileSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class TableSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline TableSourceModel()
    {
        m_ValueEdit = new TableSelectWidget();
    }
    inline ~TableSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Table Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Table)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Table Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("Table");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("Table " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
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

  TableSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class GeoProjectionSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline GeoProjectionSourceModel()
    {
        m_ValueEdit = new GeoProjectionSelectWidget();
    }
    inline ~GeoProjectionSourceModel() {}


public:

  QString
  caption() const override
  { return QString("GeoProjection Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (GeoProjection)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("GeoProjection Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("GeoProjection");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("Shapes " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
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

  GeoProjectionSelectWidget * m_ValueEdit = nullptr;


public slots:



};

















class PointCloudSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline PointCloudSourceModel()
    {
        m_ValueEdit = new PointCloudSelectWidget();
    }
    inline ~PointCloudSourceModel() {}


public:

  QString
  caption() const override
  { return QString("PointCloud Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (PointCloud)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("PointCloud Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("PointCloud");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("PointCloud " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
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

  PointCloudSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class MapArraySourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline MapArraySourceModel()
    {
        m_ValueEdit = new MapArraySelectWidget();
    }
    inline ~MapArraySourceModel() {}


public:

  QString
  caption() const override
  { return QString("MapArray Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (MapArray)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("MapArray Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("MapArray");
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
      return m_ValueEdit;
  }


  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("MapArray " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
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

  MapArraySelectWidget * m_ValueEdit = nullptr;


public slots:



};


class BoundingBoxSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline BoundingBoxSourceModel()
    {
        m_ValueEdit = new BoundingBoxSelectWidget();
    }
    inline ~BoundingBoxSourceModel() {}


public:

  QString
  caption() const override
  { return QString("BoundingBox Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (BoundingBox)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("BoundingBox Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("BoundingBox");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, SPHScriptEngine * e)
  {
      QList<QString> l;

      l.append("BoundingBox " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
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

  BoundingBoxSelectWidget * m_ValueEdit = nullptr;


public slots:



};



#endif // NUMBERSOURCEMODEL_H
