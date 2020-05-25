#include "spherror.h"

QMutex *MessageMutex_ALL = NULL;
QList<QString> *Messages_ALL = NULL;
QList<int> *Levels_ALL = NULL;

QMutex *MessageMutex = NULL;
QList<QString> *Messages = NULL;
QList<int> *Levels = NULL;

QMutex *MessageMutexS = NULL;
QList<QString> *MessagesS = NULL;
QList<int> *LevelsS = nullptr;

QMutex *MessageMutexSTD = NULL;
QList<QString> *MessagesSTD = NULL;
QList<int> *LevelsSTD = NULL;

