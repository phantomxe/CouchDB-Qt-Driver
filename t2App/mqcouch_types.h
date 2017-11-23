#ifndef MQCOUCH_TYPES_H
#define MQCOUCH_TYPES_H

#include <QList>
#include <QByteArray>
#include <QString>
#include <QStringList>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

//Simple redefining
#define NOT !

typedef struct _mq_databaseInfo{
    QString db_name;
    QString instance_start_time;
    quint32 doc_count;
    quint32 doc_del_count;
    quint32 data_size;
} _mq_databaseInfo;

typedef struct _mq_document{
    QString id;
    QString rev;
    bool ok;
} _mq_document;

typedef struct _mq_documentRaw{
    QString id;
    QString rev;
    QJsonDocument data;
} _mq_documentRaw;

typedef struct _mq_attachment{
    QString name;
    QString mimeType;
    QByteArray body;
} _mq_attachment;

#endif // MQCOUCH_TYPES_H
