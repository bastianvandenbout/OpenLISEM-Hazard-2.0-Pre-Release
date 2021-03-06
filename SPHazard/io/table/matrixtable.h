#ifndef MATRIXTABLE_H
#define MATRIXTABLE_H

#include "QString"
#include "QVector"

#include <QList>
#include "map.h"
#include "QFile"
#include "QFileInfo"
#include "QDir"
#include "spherror.h"

#include <QTextStream>
#include <iostream>

//matches the Attribute Type definitions
#define TABLE_TYPE_UNKNOWN 0
#define TABLE_TYPE_INT32 1
#define TABLE_TYPE_INT64 2
#define TABLE_TYPE_FLOAT64 3
#define TABLE_TYPE_BOOL 4
#define TABLE_TYPE_STRING 5


struct QComparer
{
    template<typename T1>
    bool operator()(const T1 & a, const T1 & b) const
    {
        return a < b;
    }
};

struct QPairFirstComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.first < b.first;
    }
};

struct QPairSecondComparer
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.second < b.second;
    }
};

struct QComparerDesc
{
    template<typename T1>
    bool operator()(const T1 & a, const T1 & b) const
    {
        return a > b;
    }
};

struct QPairFirstComparerDesc
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.first > b.first;
    }
};

struct QPairSecondComparerDesc
{
    template<typename T1, typename T2>
    bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
    {
        return a.second > b.second;
    }
};


class MatrixTable{

private:

    QVector<QVector<QString*>*> m_Data;


    QVector<std::string> m_ColumnTitles;
    QVector<std::string> m_RowTitles;

    //the data types for each row or column
    //can be either set for rows, or for columns
    QVector<int> m_ColumnTypes;
    QVector<int> m_RowTypes;


    bool m_HasColumnTypes = false;
    bool m_HasRowTypes = false;

public:

    inline MatrixTable()
    {

    }

    inline ~MatrixTable()
    {
        Empty();
    }
    inline bool SaveAs(QString file, QString sep, bool header = true)
    {

        QFile fin(file);
        if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&fin);
            for(int i = 0; i < m_Data.length(); i++)
            {
                for(int j = 0; j < m_Data.at(i)->length(); j++)
                {
                    out << QString(this->GetValueStringQ(i,j));
                    if(j != m_Data.at(i)->length() -1)
                    {
                        out << sep;
                    }
                }
                out << "\n";
            }
        }else
        {
            return false;
        }
        fin.close();

        return true;
    }

    inline bool LoadAs(QString file, QString sep, bool skipempty = false)
    {
        QFile fff(file);
        QFileInfo fi(file);
        QString S;
        QStringList Lines;

        fff.open(QIODevice::ReadOnly | QIODevice::Text);

        if (!fi.exists())
        {
            return false;
        }

        QString folder = fi.dir().absolutePath() + "/";


        while (!fff.atEnd())
        {
            S = fff.readLine();
            if (S.contains("\n"))
                S.remove(S.count()-1,1);
            // readLine also reads \n as a character on an empty line!
            if (!S.trimmed().isEmpty())
                Lines << S.trimmed();
            //qDebug() << S;
        }
        fff.close();

        QVector<QVector<QString*>*> data;
        for(int i = 0; i < Lines.length(); i++)
        {
            QStringList SL = Lines[i].split(QRegExp(sep));
            QVector<QString*> *row = new QVector<QString*>();
            for(int j = 0; j < SL.length(); j++)
            {
                QString * s = new QString();
                *s = SL.at(j);
                row->push_back(s);
            }
            data.push_back(row);
        }

        CopyFromData(data);

        fill();

        AS_FileName = file;

        return true;
    }

    inline bool LoadFromFileCSV(QString file)
    {
        return LoadAs(file,",");

    }
    inline bool LoadFromFileWST(QString file)
    {
        return LoadAs(file,"\t",true);
    }

    inline void SetRowTypesActive(bool x)
    {
        m_HasColumnTypes = x;
        if(x)
        {
            m_HasRowTypes = false;
        }
    }
    inline void SetColumnTypesActive(bool x)
    {
        m_HasRowTypes = x;
        if(x)
        {
            m_HasColumnTypes = false;
        }
    }
    inline void SetRowTitle(int i, QString s)
    {

        if(i < m_RowTitles.length())
        {
            m_RowTitles.replace(i,s.toStdString());
        }
    }
    inline void SetColumnTitle(int i,QString s)
    {
        if(i < m_ColumnTitles.length())
        {
            m_ColumnTitles.replace(i,s.toStdString());
        }
    }
    inline void SetRowTitle(int i, std::string s)
    {
        if(i < m_RowTitles.length())
        {
            m_RowTitles.replace(i,s);
        }
    }
    inline void SetColumnTitle(int i,std::string s)
    {
        if(i < m_ColumnTitles.length())
        {
            m_ColumnTitles.replace(i,s);
        }
    }

    inline QString GetColumnTitle(int i)
    {
        if(i < m_ColumnTitles.length())
        {
            return QString(m_ColumnTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline QString GetRowTitle(int i)
    {
        if(i < m_RowTitles.length())
        {
            return QString(m_RowTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline QString GetColumnTitleQ(int i)
    {
        if(i < m_ColumnTitles.length())
        {
            return QString(m_ColumnTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline QString GetRowTitleQ(int i)
    {
        if(i < m_RowTitles.length())
        {
            return QString(m_RowTitles.at(i).c_str());
        }else
        {
            return "";
        }
    }

    inline void SetRowType(int i,int t)
    {
        if(i < m_RowTypes.length())
        {
            m_RowTypes.replace(i,t);
        }
    }
    inline void SetColumnType(int i,int t)
    {
        if(i < m_ColumnTypes.length())
        {
            m_ColumnTypes.replace(i,t);
        }
    }

    inline int GetRowType(int i)
    {
        if(i < m_RowTypes.length())
        {
            return m_RowTypes.at(i);
        }else
        {
            return TABLE_TYPE_UNKNOWN;
        }
    }

    inline int GetColumnType(int i)
    {
        if(i < m_ColumnTypes.length())
        {
            return m_ColumnTypes.at(i);
        }else
        {
            return TABLE_TYPE_UNKNOWN;
        }
    }

    inline bool IsCalcInt()
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(0) == TABLE_TYPE_BOOL || m_ColumnTypes.at(0) == TABLE_TYPE_INT32 || m_ColumnTypes.at(0) == TABLE_TYPE_INT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsCalcDouble()
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(0) == TABLE_TYPE_FLOAT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsCalcString()
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(0) == TABLE_TYPE_BOOL || m_ColumnTypes.at(0) == TABLE_TYPE_INT32 || m_ColumnTypes.at(0) == TABLE_TYPE_INT64 || m_ColumnTypes.at(0) == TABLE_TYPE_FLOAT64)
            {
                return false;
            }
        }

        return true;
    }

    inline bool IsColumnCalcInt(int column)
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(column) == TABLE_TYPE_BOOL || m_ColumnTypes.at(column) == TABLE_TYPE_INT32 || m_ColumnTypes.at(column) == TABLE_TYPE_INT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsColumnCalcDouble(int column)
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(column) == TABLE_TYPE_FLOAT64)
            {
                return true;
            }
        }
        return false;
    }

    inline bool IsColumnCalcString(int column)
    {
        if(m_HasColumnTypes && m_ColumnTypes.length() > 0)
        {
            if(m_ColumnTypes.at(column) == TABLE_TYPE_BOOL || m_ColumnTypes.at(column) == TABLE_TYPE_INT32 || m_ColumnTypes.at(column) == TABLE_TYPE_INT64 || m_ColumnTypes.at(column) == TABLE_TYPE_FLOAT64)
            {
                return false;
            }
        }

        return true;
    }

    inline MatrixTable * GetColumn(int column)
    {
        MatrixTable * t = new MatrixTable();
        int rows = GetNumberOfRows();
        t->SetSize(rows,1);

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < rows; r++)
            {
                t->SetValue(r,0,QString(*m_Data.at(r)->at(column)));
            }
        }

        return t;

    }

    inline QList<QString> GetColumnString(int column)
    {
        int rows = GetNumberOfRows();

        QList<QString> ret;
        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < rows; r++)
            {
                ret.append( QString(*m_Data.at(r)->at(column)));
            }
        }

        return ret;

    }

    inline QList<QString*> GetColumnStringRef(int column)
    {
        int rows = GetNumberOfRows();

        QList<QString*> ret;
        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < rows; r++)
            {
                ret.append( m_Data.at(r)->at(column));
            }
        }

        return ret;
    }

    //replace inner section
    inline void ReplaceValues(MatrixTable * t, int rs, int cs)
    {


        int rows_rp = t->GetNumberOfRows();
        int cols_rp = t->GetNumberOfCols();
        int rows = GetNumberOfRows();
        int cols = GetNumberOfCols();
        for(int r = rs; r < std::min(rows,rs + rows_rp); r++)
        {
            for(int c = cs; c < std::min(cols,cs + cols_rp); c++)
            {
                SetValue(r,c,t->GetValueStringQ(r - rs,c- cs));
            }
        }

    }

    //replace functions
    inline void ReplaceColumn(int column, QList<int> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows));
            }
        }
    }

    inline void ReplaceColumn(int column, QList<double> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows));
            }
        }
    }

    inline void ReplaceColumn(int column, QList<QString> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = new_values.at(rows);
            }
        }
    }

    //replace functions used by sorting helpers
    inline void ReplaceColumn(int column, QList<QPair<int,int>> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows).first);
            }
        }
    }

    inline void ReplaceColumn(int column, QList<QPair<double,int>> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = QString::number(new_values.at(rows).first);
            }
        }
    }

    inline void ReplaceColumn(int column, QList<QPair<QString,int>> &new_values)
    {
        int rows = GetNumberOfRows();

        if(column > -1 && column < GetNumberOfCols())
        {
            for(int r = 0; r < std::min(new_values.length(),rows); r++)
            {
                 *(m_Data.at(r)->at(column)) = new_values.at(rows).first;
            }
        }

    }

    //re-ordering the data of columns
    inline void ReOrderColumn(int column, QList<int> &new_indices)
    {
        QList<QString> data_old = GetColumnString(column);
        QList<QString> data_new;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r);
            data_new.append(data_old.at(row_old));
        }


        ReplaceColumn(column,data_new);


    }

    template<typename T>
    inline void ReOrderColumn(int column, QList<QPair<T,int>> &new_indices)
    {
        QList<QString> data_old = GetColumnString(column);
        QList<QString> data_new;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r).second;
            data_new.append(data_old.at(row_old));
        }

        ReplaceColumn(column,data_new);
    }


    inline MatrixTable * ReOrderAllColumns(QList<int> &new_indices)
    {
        MatrixTable * t = new MatrixTable();
        t->SetSize(this->GetNumberOfRows(),this->GetNumberOfCols());
        t->m_HasColumnTypes = m_HasColumnTypes;
        t->m_ColumnTypes = m_ColumnTypes;
        t->m_ColumnTitles = m_ColumnTitles;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r);
            for(int c = 0; c < cols;c ++)
            {
                *(t->m_Data.at(r)->at(c)) = *(m_Data.at(row_old)->at(c));
            }
        }

        return t;

    }

    template<typename T>
    inline MatrixTable * ReOrderAllColumns(QList<QPair<T,int>> &new_indices)
    {
        MatrixTable * t = new MatrixTable();
        t->SetSize(this->GetNumberOfRows(),this->GetNumberOfCols());
        t->m_HasColumnTypes = m_HasColumnTypes;
        t->m_ColumnTypes = m_ColumnTypes;
        t->m_ColumnTitles = m_ColumnTitles;

        int rows = this->GetNumberOfRows();
        int cols = this->GetNumberOfCols();

        for(int r = 0; r < rows;r ++)
        {
            int row_old = new_indices.at(r).second;

            for(int c = 0; c < cols;c ++)
            {
                *(t->m_Data.at(r)->at(c)) = *(m_Data.at(row_old)->at(c));
            }
        }

        return t;
    }


    inline void SortColumn(int column, bool descending = false)
    {
        MatrixTable * in = this;
        if(column > -1 && in->GetNumberOfCols() > column)
        {
            if(in->IsColumnCalcInt(column))
            {
                //compare as ints
                QList<QPair<int,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    int x = in->GetValueInt(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                ReplaceColumn(column,array);

            }else if(in->IsColumnCalcDouble(column))
            {
                //compare as ints
                QList<QPair<double,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    double x = in->GetValueDouble(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                ReplaceColumn(column,array);

            }else if(in->IsColumnCalcString(column))
            {
                //compare as ints
                QList<QPair<QString,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    QString x = in->GetValueStringQ(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                ReplaceColumn(column,array);
            }
        }
    }

    inline MatrixTable * SortByColumn(int column, bool descending = false)
    {
        MatrixTable * in = this;
        if(column > -1 && in->GetNumberOfCols() > column)
        {
            if(in->IsColumnCalcInt(column))
            {
                //compare as ints
                QList<QPair<int,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    int x = in->GetValueInt(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                return this->ReOrderAllColumns(array);

            }else if(in->IsColumnCalcDouble(column))
            {
                //compare as ints
                QList<QPair<double,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    double x = in->GetValueDouble(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                return this->ReOrderAllColumns(array);

            }else if(in->IsColumnCalcString(column))
            {
                //compare as ints
                QList<QPair<QString,int> > array;
                int rmax = in->GetNumberOfRows();
                for (int r = 0; r < rmax; r++)
                {
                    QString x = in->GetValueStringQ(r,column);
                    array.append(qMakePair(x,r));
                }

                // Ordering ascending
                if(!descending)
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparer());
                }else
                {
                    std::sort(array.begin(), array.end(), QPairFirstComparerDesc());
                }

                return this->ReOrderAllColumns(array);
            }
        }
        return this->Copy();
    }

    inline MatrixTable * SortByColumn(QString name)
    {

        return nullptr;
    }

    inline void SortColumn(QString name)
    {

    }

    inline QString GetDataTypeName(int type)
    {
        if(type == TABLE_TYPE_UNKNOWN)
        {
            return "Unknown";
        }else if(type == TABLE_TYPE_INT32)
        {
            return "Integer(32bit)";
        }else if(type == TABLE_TYPE_INT64)
        {
            return "Integer(64bit)";
        }else if(type == TABLE_TYPE_FLOAT64)
        {
            return "Float(64bit)";
        }else if(type == TABLE_TYPE_BOOL)
        {
            return "Boolean";
        }else if(type == TABLE_TYPE_STRING)
        {
            return "String";
        }else
        {
            return "Unknown";
        }
    }
    inline void fill()
    {
        int wmax = 0;
        for(int i = 0; i < m_Data.length(); i++)
        {
            wmax= std::max(wmax,m_Data.at(i)->length());
        }
        for(int i = 0; i < m_Data.length(); i++)
        {
            for(int j = m_Data.at(i)->length(); j < wmax; j++)
            {
                m_Data.at(i)->append(new QString(""));
            }
        }

    }



    inline void AddItem(QString s)
    {
        QVector<QString*> * v = new QVector<QString*>();
        v->append(new QString(s));
        int rows = GetNumberOfRows();
        int cols = GetNumberOfCols();
        if(rows > 0 && cols > 0)
        {
            for(int j = v->length()-1; j < cols ; j++)
            {
                v->append(new QString(""));
            }
        }
        m_Data.append(v);

        //

    }

    inline void AddItem(std::string s)
    {
        AddItem(QString(s.c_str()));
    }
    inline void SetSize(int r)
    {
         SetSize(r,1);
    }

    inline int GetSizeR()
    {
         return GetNumberOfRows();
    }
    inline int GetSizeC()
    {
         return GetNumberOfCols();
    }

    inline void SetSize(int r, int c)
    {
        //set the table data to be the correct size
        if(m_Data.length() > r)
        {
            for(int i = m_Data.length()-1; i > r -1; i--)
            {
                int l = m_Data.at(i)->length();
                for(int j = 0; j < l; j++)
                {
                    delete m_Data.at(i)->at(j);
                }
                delete m_Data.at(i);
                m_Data.removeAt(i);
            }
        }else if (m_Data.length() < r)
        {
            for(int i = m_Data.length(); i < r; i++)
            {
                QVector<QString*> * row = new QVector<QString*>();
                m_Data.push_back(row);
            }
        }

        //remove items if a row is too long, or add if too short
        for(int i = 0; i < r; i++)
        {
            int l = m_Data.at(i)->length();
            QVector<QString*> * row = m_Data.at(i);
            if(l > c)
            {
                for(int j = l-1; j > c -1; j--)
                {
                    delete row->at(j);
                    row->removeAt(j);
                }
            }else if (l < c)
            {
                for(int j = l; j < c; j++)
                {
                    row->push_back(new QString(""));
                }
            }
        }

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());

    }
    inline void AddRow(int i)
    {
        int cols = GetNumberOfCols();
        QVector<QString*> * row = new QVector<QString*>();
        for(int j = row->length()-1; j < cols +1; j++)
        {
            row->append(new QString(""));
        }
        m_Data.insert(i,row);

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());
    }
    inline void AddRow()
    {
        int cols = GetNumberOfCols();
        QVector<QString*> * row = new QVector<QString*>();
        for(int j = row->length()-1; j < cols +1; j++)
        {
            row->append(new QString(""));
        }
        m_Data.append(row);

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());
    }

    inline void AddColumn()
    {
        for(int i = 0; i < m_Data.length(); i++)
        {
            m_Data.at(i)->append(new QString(""));
        }

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());

    }
    inline void AddColumn(int index )
    {
        for(int i = 0; i < m_Data.length(); i++)
        {
            if(index > m_Data.at(i)->length())
            {
                m_Data.at(i)->insert(m_Data.at(i)->length(),new QString(""));
            }else {
                m_Data.at(i)->insert(index,new QString(""));
            }
        }

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());

    }


    inline void AS_AppendColumns(MatrixTable * t)
    {
        for(int i = 0; i < m_Data.length(); i++)
        {
            for(int k = 0; k < t->GetNumberOfCols(); k++)
            {
                m_Data.at(i)->append(new QString(t->GetValueStringQ(i,k)));
            }
        }

        //now change the size of the titles and data type arrays
        m_RowTypes.resize(GetNumberOfRows());
        m_RowTitles.resize(GetNumberOfRows());
        m_ColumnTypes.resize(GetNumberOfCols());
        m_ColumnTitles.resize(GetNumberOfCols());

    }
    inline void AS_SetValue(int r, std::string value)
    {
        AS_SetValue(r, 0,value);
    }

    inline void AS_SetValue(int r, int c, std::string value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                m_Data.at(r)->at(c)->clear();
                *m_Data.at(r)->at(c) = QString(value.c_str());

            }
        }
    }

    inline void SetValue(int r, std::string value)
    {
        SetValue(r, 0,value);
    }

    inline void SetValue(int r, int c, std::string value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                m_Data.at(r)->at(c)->clear();
                *m_Data.at(r)->at(c) = QString(value.c_str());
            }
        }
    }

    inline void  SetValue(int r, int c, int value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                *m_Data.at(r)->at(c) = QString::number(value);
            }
        }
    }

    inline void  SetValue(int r, int c, double value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                *m_Data.at(r)->at(c) = QString::number(value);
            }
        }
    }
    inline void  SetValue(int r, int c, QString value)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                *m_Data.at(r)->at(c) = QString(value);
            }
        }
    }
    inline int GetNumberOfCols()
    {
        int wmax = 0;
        for(int i = 0; i < m_Data.length(); i++)
        {
            wmax= std::max(wmax,m_Data.at(i)->length());
        }
        return wmax;
    }

    inline int GetNumberOfRows()
    {
        return m_Data.length();
    }

    inline int GetNumberOfRowsAtColumn(int c)
    {
        if(c < 0 || m_Data.length() == 0 )
        {
            return 0;
        }
        int r;
        for(r = m_Data.length()-1; r > -1;r--)
        {
            if(!(m_Data.at(r)->length() > c))
            {
                break;
            }
            if(!(m_Data.at(r)->at(c)->isEmpty()))
            {
                break;
            }
        }
        return r + 1;
    }

    inline std::string *GetValueString(int r, int c)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                return new std::string(m_Data.at(r)->at(c)->toStdString());
            }
        }

        return nullptr;
    }

    inline QString *GetValueStringQRef(int r, int c)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                return m_Data.at(r)->at(c);
            }
        }

        return new QString("");
    }


    inline QString GetValueStringQ(int r, int c)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                return QString(*m_Data.at(r)->at(c));
            }
        }

        return "";
    }

    inline double GetValueDouble(int r, int c)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                bool ok = true;
                double val =  QString(*m_Data.at(r)->at(c)).toDouble(&ok);
                if(ok)
                {
                    return val;
                }else
                {
                    return 0.0;
                }
            }
        }

        return 0.0;
    }

    inline int GetValueInt(int r, int c)
    {
        if(r > -1 && r < m_Data.length())
        {
            if(c > -1 && c < m_Data.at(r)->length())
            {
                bool ok = true;
                double val =  QString(*m_Data.at(r)->at(c)).toInt(&ok);
                if(ok)
                {
                    return val;
                }else
                {
                    return 0;
                }
            }
        }

        return 0;
    }

    inline void RemoveRow(int i)
    {
        if(i > -1 && i < m_Data.length())
        {
            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                delete m_Data.at(i)->at(j);
            }
            m_Data.at(i)->clear();
            delete m_Data.at(i);
            m_Data.removeAt(i);
        }
    }
    inline void RemoveColumn(int i)
    {
        if(m_Data.length() > 0)
        {
            if(i > -1 && i < m_Data.at(0)->length())
            {
                for(int j =0; j < m_Data.length(); j++)
                {
                    if(i < m_Data.at(j)->length())
                    {
                        delete m_Data.at(j)->at(i);
                        m_Data.at(j)->removeAt(i);
                    }
                }
            }
        }
    }

    inline void Empty()
    {
        int count = 0;
        for(int i =0; i < m_Data.length(); i++)
        {
            if(m_Data.at(i) != nullptr)
            {
                for(int j =0; j < m_Data.at(i)->length(); j++)
                {
                    if(m_Data.at(i)->at(j) != nullptr)
                    {
                        count ++;
                        delete m_Data.at(i)->at(j);
                    }
                }
                m_Data.at(i)->clear();
                delete m_Data.at(i);
            }
        }
        m_Data.clear();
        m_RowTitles.clear();
        m_ColumnTitles.clear();
        m_RowTypes.clear();
        m_ColumnTypes.clear();
        m_HasRowTypes = false;
        m_HasColumnTypes =false;
    }

    inline void CopyFromData(QVector<QVector<QString*>*> &data)
    {
        Empty();

        int count  = 0;
        for(int i =0; i < data.length(); i++)
        {
            QVector<QString*> * row = new QVector<QString*>();
            for(int j = 0; j < data.at(i)->length(); j++)
            {
                count++;
                QString * s = new QString();
                *(s) = *(data.at(i)->at(j));
                row->push_back(s);
            }
            m_Data.push_back(row);
        }


    }
    inline MatrixTable * Copy()
    {
        MatrixTable * ret  = new MatrixTable();
        ret->CopyFrom(this);
        return ret;
    }

    inline void CopyFrom(MatrixTable * t)
    {
        CopyFromData(t->m_Data);
        m_HasColumnTypes = t->m_HasColumnTypes;
        m_HasRowTypes = t->m_HasRowTypes;
        m_ColumnTypes = t->m_ColumnTypes;
        m_RowTypes = t->m_RowTypes;
        m_ColumnTitles = t->m_ColumnTitles;
        m_RowTitles = t->m_RowTitles;

    }

    inline bool ConvertToInt(bool set  = false)
    {
        bool allok = true;
        for(int i =0; i < m_Data.length();i++)
        {
            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                if(m_Data.at(i)->at(j) != nullptr)
                {
                    QString text = *(m_Data.at(i)->at(j));
                    if(!text.trimmed().isEmpty())
                    {
                        bool ok = true;
                        int n = text.toInt(&ok);
                        allok = allok & ok;
                        if(!ok)
                        {
                            n = 0;
                            *(m_Data.at(i)->at(j)) = QString::number(n);
                        }
                    }else {
                        *(m_Data.at(i)->at(j)) = QString::number(0);
                    }



                }else {
                    m_Data.at(i)->replace(j,new QString("0"));
                }
            }
        }

        if(set)
        {
            if(!m_HasColumnTypes || m_HasRowTypes)
            {
                m_HasColumnTypes = true;
                m_HasRowTypes = false;
            }
            for(int i = 0; i < m_ColumnTypes.length() ; i++)
            {

                m_ColumnTypes.replace(i,TABLE_TYPE_INT64);
            }
            for(int i = 0; i < m_RowTypes.length() ; i++)
            {
                m_RowTypes.replace(i,TABLE_TYPE_INT64);
            }
        }
        return allok;
    }

    inline bool ConvertToString(bool set = false)
    {
        bool allok = true;
        for(int i =0; i < m_Data.length();i++)
        {
            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                if(m_Data.at(i)->at(j) != nullptr)
                {

                }else {
                    m_Data.at(i)->replace(j,new QString(""));
                }
            }
        }

        if(set)
        {
            for(int i = 0; i < m_ColumnTypes.length() ; i++)
            {
                m_ColumnTypes.replace(i,TABLE_TYPE_STRING);
            }
            for(int i = 0; i < m_RowTypes.length() ; i++)
            {
                m_RowTypes.replace(i,TABLE_TYPE_STRING);
            }

            if(!m_HasColumnTypes || m_HasRowTypes)
            {
                m_HasColumnTypes = true;
                m_HasRowTypes = false;
            }
        }
        return allok;
    }

    inline bool ConvertToDouble(bool set = false)
    {
        bool allok = true;
        for(int i =0; i < m_Data.length();i++)
        {
            for(int j =0; j < m_Data.at(i)->length(); j++)
            {
                if(m_Data.at(i)->at(j) != nullptr)
                {
                    QString text = *(m_Data.at(i)->at(j));
                    if(!text.trimmed().isEmpty())
                    {
                        bool ok = true;
                        double n = text.toDouble(&ok);
                        allok = allok & ok;
                        if(!ok)
                        {
                            n= 0.0f;
                            *(m_Data.at(i)->at(j)) = QString::number(n);
                        }
                    }else {
                        *(m_Data.at(i)->at(j)) = QString::number(0.0);
                    }
                }else {
                    m_Data.at(i)->replace(j,new QString("0.0"));
                }
            }
        }
        if(set)
        {
            for(int i = 0; i < m_ColumnTypes.length() ; i++)
            {
                m_ColumnTypes.replace(i,TABLE_TYPE_FLOAT64);
            }
            for(int i = 0; i < m_RowTypes.length() ; i++)
            {
                m_RowTypes.replace(i,TABLE_TYPE_FLOAT64);
            }

            if(!m_HasColumnTypes || m_HasRowTypes)
            {
                m_HasColumnTypes = true;
                m_HasRowTypes = false;
            }
        }
        return allok;
    }

    inline QList<QString> GetVerticalStringList(int c,int rmin, int rmax)
    {
        QList<QString> list;

        for(int r = rmin; r < rmax +1; r++)
        {
            list.append(this->GetValueStringQ(c,r));
        }

        return list;
    }

    //Angelscript related functions
    public:

        std::function<void(QString,int,QString)> AS_SetAttributeFunction;
        bool        AS_DoSetAttribute = false;
        QString     AS_DoSetAttributeName = "";

        bool          AS_HasParent= false;
        MatrixTable * AS_Parent = nullptr;
        int           AS_Parent_sr = 0;
        int           AS_Parent_sc = 0;
        inline void   AS_UpdateParent();

        QString        AS_FileName          = "";
        bool           AS_writeonassign     = false;
        std::function<void(MatrixTable *,QString)> AS_writefunc;
        std::function<MatrixTable *(QString)> AS_readfunc;
        bool           AS_Created           = false;
        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        MatrixTable*   AS_Assign            (MatrixTable*);
        MatrixTable*   AS_OpIndex           (int);
        QString*       AS_OpIndex           (int,int);

        MatrixTable*        AS_OpAdd             (MatrixTable*other);
        MatrixTable*        AS_OpMul             (MatrixTable*other);

        MatrixTable*        AS_OpNeg             ();
        MatrixTable*        AS_OpCom             ();
        MatrixTable*        AS_OpSub             (MatrixTable*other);
        MatrixTable*        AS_OpDiv             (MatrixTable*other);
        MatrixTable*        AS_OpPow             (MatrixTable*other);
        MatrixTable*        AS_OpAddAssign       (MatrixTable*other);
        MatrixTable*        AS_OpSubAssign       (MatrixTable*other);
        MatrixTable*        AS_OpMulAssign       (MatrixTable*other);
        MatrixTable*        AS_OpDivAssign       (MatrixTable*other);
        MatrixTable*        AS_OpPowAssign       (MatrixTable*other);

        MatrixTable*        AS_OpModAssign       (MatrixTable*other);
        MatrixTable*        AS_OpMod             (MatrixTable*other);

        MatrixTable*        AS_EqualTo           (MatrixTable*other);
        MatrixTable*        AS_LargerThen        (MatrixTable*other);
        MatrixTable*        AS_SmallerThen       (MatrixTable*other);
        MatrixTable*        AS_NotEqualTo        (MatrixTable*other);
        MatrixTable*        AS_LargerEqualThen   (MatrixTable*other);
        MatrixTable*        AS_SmallerEqualThen  (MatrixTable*other);

        MatrixTable*        AS_And               (MatrixTable*other);
        MatrixTable*        AS_Or                (MatrixTable*other);
        MatrixTable*        AS_Xor               (MatrixTable*other);

        MatrixTable*        AS_AndAssign         (MatrixTable*other);
        MatrixTable*        AS_OrAssign          (MatrixTable*other);
        MatrixTable*        AS_XorAssign         (MatrixTable*other);

        MatrixTable*        AS_Assign            (double other);
        MatrixTable*        AS_OpAdd             (double other);
        MatrixTable*        AS_OpMul             (double other);

        MatrixTable*        AS_OpSub             (double other);
        MatrixTable*        AS_OpDiv             (double other);
        MatrixTable*        AS_OpPow             (double other);
        MatrixTable*        AS_OpAddAssign       (double other);
        MatrixTable*        AS_OpSubAssign       (double other);
        MatrixTable*        AS_OpMulAssign       (double other);
        MatrixTable*        AS_OpDivAssign       (double other);
        MatrixTable*        AS_OpPowAssign       (double other);

        MatrixTable*        AS_OpModAssign       (double other);
        MatrixTable*        AS_OpMod             (double other);

        MatrixTable*        AS_EqualTo           (double other);
        MatrixTable*        AS_LargerThen        (double other);
        MatrixTable*        AS_SmallerThen       (double other);
        MatrixTable*        AS_NotEqualTo        (double other);
        MatrixTable*        AS_LargerEqualThen   (double other);
        MatrixTable*        AS_SmallerEqualThen  (double other);

        MatrixTable*        AS_And               (double other);
        MatrixTable*        AS_Or                (double other);
        MatrixTable*        AS_Xor               (double other);


        MatrixTable*        AS_AndAssign         (double other);
        MatrixTable*        AS_OrAssign          (double other);
        MatrixTable*        AS_XorAssign         (double other);

        MatrixTable*        AS_Assign            (int other);
        MatrixTable*        AS_OpAdd             (int other);
        MatrixTable*        AS_OpMul             (int other);

        MatrixTable*        AS_OpSub             (int other);
        MatrixTable*        AS_OpDiv             (int other);
        MatrixTable*        AS_OpPow             (int other);
        MatrixTable*        AS_OpAddAssign       (int other);
        MatrixTable*        AS_OpSubAssign       (int other);
        MatrixTable*        AS_OpMulAssign       (int other);
        MatrixTable*        AS_OpDivAssign       (int other);
        MatrixTable*        AS_OpPowAssign       (int other);

        MatrixTable*        AS_OpModAssign       (int other);
        MatrixTable*        AS_OpMod             (int other);

        MatrixTable*        AS_EqualTo           (int other);
        MatrixTable*        AS_LargerThen        (int other);
        MatrixTable*        AS_SmallerThen       (int other);
        MatrixTable*        AS_NotEqualTo        (int other);
        MatrixTable*        AS_LargerEqualThen   (int other);
        MatrixTable*        AS_SmallerEqualThen  (int other);

        MatrixTable*        AS_And               (int other);
        MatrixTable*        AS_Or                (int other);
        MatrixTable*        AS_Xor               (int other);


        MatrixTable*        AS_AndAssign         (int other);
        MatrixTable*        AS_OrAssign          (int other);
        MatrixTable*        AS_XorAssign         (int other);

        MatrixTable*        AS_Assign            (QString s);
        MatrixTable*        AS_OpAdd             (QString s);
        MatrixTable*        AS_OpAddAssign       (QString s);
        MatrixTable*        AS_EqualTo           (QString s);
        MatrixTable*        AS_NotEqualTo        (QString s);

        MatrixTable*        AS_OpAdd_r             (QString s);
        MatrixTable*        AS_EqualTo_r           (QString s);
        MatrixTable*        AS_NotEqualTo_r           (QString s);

        MatrixTable *        AS_OpMod_r           (double other);
        MatrixTable *        AS_OpAdd_r           (double other);
        MatrixTable *        AS_OpMul_r           (double other);
        MatrixTable *        AS_OpSub_r           (double other);
        MatrixTable *        AS_OpDiv_r           (double other);
        MatrixTable *        AS_OpPow_r           (double other);
        MatrixTable *        AS_LargerThen_r      (double other);
        MatrixTable *        AS_SmallerThen_r     (double other);
        MatrixTable *        AS_NotEqualTo_r      (double other);
        MatrixTable *        AS_LargerEqualThen_r (double other);
        MatrixTable *        AS_SmallerEqualThen_r(double other);
        MatrixTable *        AS_EqualTo_r         (double other);
        MatrixTable *        AS_And_r             (double other);
        MatrixTable *        AS_Or_r              (double other);
        MatrixTable *        AS_Xor_r             (double other);

        MatrixTable *        AS_OpMod_r           (int other);
        MatrixTable *        AS_OpAdd_r           (int other);
        MatrixTable *        AS_OpMul_r           (int other);
        MatrixTable *        AS_OpSub_r           (int other);
        MatrixTable *        AS_OpDiv_r           (int other);
        MatrixTable *        AS_OpPow_r           (int other);
        MatrixTable *        AS_LargerThen_r      (int other);
        MatrixTable *        AS_SmallerThen_r     (int other);
        MatrixTable *        AS_NotEqualTo_r      (int other);
        MatrixTable *        AS_LargerEqualThen_r (int other);
        MatrixTable *        AS_SmallerEqualThen_r(int other);
        MatrixTable *        AS_EqualTo_r         (int other);
        MatrixTable *        AS_And_r             (int other);
        MatrixTable *        AS_Or_r              (int other);
        MatrixTable *        AS_Xor_r             (int other);

};


//Angelscript related functionality

inline void MatrixTable::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;
}

inline void MatrixTable::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Empty();
        delete this;
    }
}



inline MatrixTable* MatrixTable::AS_Assign(MatrixTable* sh)
{

    CopyFrom(sh);
    AS_UpdateParent();

    return this;
}




inline MatrixTable* TableFactory()
{
    MatrixTable * ret = new MatrixTable();
    return ret;
}


inline MatrixTable *MatrixTable::AS_OpIndex(int column)
{

    if(AS_writeonassign)
    {
        MatrixTable*target = this;
        target = AS_readfunc(AS_FileName);
        this->CopyFrom(target);
    }

    MatrixTable * ret = GetColumn(column);
    ret->AS_HasParent = true;
    ret->AS_Parent = this;
    ret->AS_Parent_sc = column;
    ret->AS_Parent_sr = 0;
    return ret;
}

//return specific value at index (reference string so can be edited)
inline QString *MatrixTable::AS_OpIndex(int r,int c)
{
    return GetValueStringQRef(r,c);
}


inline void MatrixTable::AS_UpdateParent()
{

    if(AS_writeonassign)
    {
        AS_writefunc(this,AS_FileName);
    }
    this->AS_writeonassign = false;

    if(AS_HasParent)
    {
        if(AS_Parent != nullptr)
        {

            AS_Parent->ReplaceValues(this,AS_Parent_sr,AS_Parent_sc);
            AS_Parent->AS_UpdateParent();
            AS_HasParent = false;
            AS_Parent = nullptr;
            AS_Parent_sc = 0;
            AS_Parent_sr = 0;
        }
    }


}

inline static MatrixTable * AS_TToInt(MatrixTable * in)
{
    if(in != nullptr)
    {
        MatrixTable * ret = in->Copy();
        ret->ConvertToInt(true);
        return ret;
    }else {

        return new MatrixTable();
    }

}

inline static MatrixTable * AS_TToDouble(MatrixTable * in)
{
    MatrixTable * ret;

    asIScriptContext *ctx = asGetActiveContext();
    if( ctx )
    {
        asIScriptEngine *engine = ctx->GetEngine();
        ret = (MatrixTable *) engine->CreateScriptObject(engine->GetTypeInfoByName("Table"));
        if(in != nullptr)
        {
            ret->CopyFrom(in);
            ret->ConvertToDouble(true);

            return ret;
        }
    }

    throw 1;
}

inline static MatrixTable * AS_TToString(MatrixTable * in)
{
    if(in != nullptr)
    {
        MatrixTable * ret = in->Copy();
        ret->ConvertToString(true);
        return ret;
    }else {

        return new MatrixTable();
    }

}


inline static MatrixTable * AS_TableGetColumn(MatrixTable * in, int column)
{
    MatrixTable * ret = new MatrixTable();
    ret->SetSize(in->GetNumberOfRows(),1);

    for(int i = 0; i < in->GetNumberOfRows();i++)
    {
        ret->SetValue(i,0,in->GetValueStringQ(i,column));
    }

    return ret;
}




#include "metaTypes.h"


Q_DECLARE_METATYPE(MatrixTable*)


#endif // MATRIXTABLE_H
