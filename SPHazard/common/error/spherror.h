#ifndef SPHERROR_H
#define SPHERROR_H

#include <QString>
#include <QList>
#include <QMutex>
#include "iostream"

#define SPH_MESSAGE_ERROR 0
#define SPH_MESSAGE_WARNING 1
#define SPH_MESSAGE_STATUS 2
#define SPH_MESSAGE_DEBUG 3
#define SPH_MESSAGE_SCRIPT 4

#define SAFE_DELETE(x) if(x != NULL){delete x;x = NULL;}
#define SAFE_DELETENS(x) if(x != NULL){delete x;;}

#define SPH_CODE_RETURN_NORMAL 0x000000
#define SPH_CODE_RETURN_RESTART 0x000001

extern QMutex *MessageMutex_ALL;
extern QList<QString> *Messages_ALL;
extern QList<int> *Levels_ALL;

extern QMutex *MessageMutex;
extern QList<QString> *Messages;
extern QList<int> *Levels;

typedef struct LeveledMessage
{
    QString Message;
    int Level;
} LeveledMessage;

inline static void InitMessages()
{
    Messages = new QList<QString>();
    Levels = new QList<int>();
    MessageMutex = new QMutex();

    Messages_ALL = new QList<QString>();
    Levels_ALL = new QList<int>();
    MessageMutex_ALL = new QMutex();

}

inline static void AddMessage(QString message, int level)
{
    std::cout << level << "   " << message.toStdString() <<std::endl;
    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages != NULL && Levels != NULL)
        {
            Messages->append(message);
            Levels->append(level);
            Messages_ALL->append(message);
            Levels_ALL->append(level);
        }

        MessageMutex->unlock();
    }

}

inline static QList<LeveledMessage> GetMessages()
{

    QList<LeveledMessage> ret;

    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages != NULL && Levels != NULL)
        {
            for(int i = 0; i < Messages->length(); i++)
            {
                LeveledMessage m;
                QString s = Messages->at(i);
                int level = Levels->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            Messages->clear();
            Levels->clear();
        }
        MessageMutex->unlock();
    }

    return ret;
}

inline static QList<LeveledMessage> GetMessages_ALL()
{

    QList<LeveledMessage> ret;

    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages_ALL != NULL && Levels_ALL != NULL)
        {
            for(int i = 0; i < Messages_ALL->length(); i++)
            {
                LeveledMessage m;
                QString s = Messages_ALL->at(i);
                int level = Levels_ALL->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            Messages_ALL->clear();
            Levels_ALL->clear();
        }
        MessageMutex->unlock();
    }

    return ret;
}



#define SPH_ERROR(x) AddMessage(x,SPH_MESSAGE_ERROR);

#define SPH_WARNING(x) AddMessage(x,SPH_MESSAGE_WARNING);

#define SPH_STATUS(x) AddMessage(x,SPH_MESSAGE_STATUS);

#define SPH_DEBUG(x) AddMessage(x,SPH_MESSAGE_DEBUG);

#define SPH_SCRIPTOUTPUT(x) AddMessage(x,SPH_MESSAGE_SCRIPT);


extern QMutex *MessageMutexS; // S means Script
extern QList<QString> *MessagesS;
extern QList<int> *LevelsS;


extern QMutex *MessageMutexSTD; //STD means Script Tool Dialog
extern QList<QString> *MessagesSTD;
extern QList<int> *LevelsSTD;

inline static void InitMessagesS()
{
    MessagesS = new QList<QString>();
    LevelsS = new QList<int>();
    MessageMutexS = new QMutex();

    MessagesSTD = new QList<QString>();
    LevelsSTD = new QList<int>();
    MessageMutexSTD = new QMutex();

}

inline static void AddMessageS(QString message, int level)
{
    if(MessageMutexS != NULL)
    {
        MessageMutexS->lock();

        if(MessagesS != NULL && LevelsS != NULL)
        {
            MessagesS->append(message);
            LevelsS->append(level);
        }

        MessageMutexS->unlock();
    }
    if(MessageMutexSTD != NULL)
    {
        MessageMutexSTD->lock();

        if(MessagesSTD != NULL && LevelsSTD != NULL)
        {
            MessagesSTD->append(message);
            LevelsSTD->append(level);
        }

        MessageMutexSTD->unlock();
    }

}

inline static QList<LeveledMessage> GetMessagesS()
{

    QList<LeveledMessage> ret;

    if(MessageMutexS != NULL)
    {
        MessageMutexS->lock();

        if(MessagesS != NULL && LevelsS != NULL)
        {
            for(int i = 0; i < MessagesS->length(); i++)
            {
                LeveledMessage m;
                QString s = MessagesS->at(i);
                int level = LevelsS->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            MessagesS->clear();
            LevelsS->clear();
        }
        MessageMutexS->unlock();
    }

    return ret;
}

inline static QList<LeveledMessage> GetMessagesSTD()
{

    QList<LeveledMessage> ret;

    if(MessageMutexSTD != NULL)
    {
        MessageMutexSTD->lock();

        if(MessagesSTD != NULL && LevelsSTD != NULL)
        {
            for(int i = 0; i < MessagesSTD->length(); i++)
            {
                LeveledMessage m;
                QString s = MessagesSTD->at(i);
                int level = LevelsSTD->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            MessagesSTD->clear();
            LevelsSTD->clear();
        }
        MessageMutexSTD->unlock();
    }

    return ret;
}



#define SPHS_ERROR(x) AddMessageS(x,SPH_MESSAGE_ERROR);

#define SPHS_WARNING(x) AddMessageS(x,SPH_MESSAGE_WARNING);

#define SPHS_STATUS(x) AddMessageS(x,SPH_MESSAGE_STATUS);

#define SPHS_DEBUG(x) AddMessageS(x,SPH_MESSAGE_DEBUG);

#define SPHS_SCRIPTOUTPUT(x) AddMessageS(x,SPH_MESSAGE_SCRIPT);


#endif // SPHERROR_H
