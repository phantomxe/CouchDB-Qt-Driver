/**
 *  @file    mqcouch.cpp
 *  @author  Hakan AFAT (Github: phantomxe)
 *  @date    02/10/2017
 *  @version 1.0
 *
 *  @brief CouchDB Qt Driver
 *
 *  @section DESCRIPTION
 *
 *  CouchDB Development Driver
 *  It requires mqhttp library for HTTP communucation
 */

#include "mqcouch.h"

mqcouch::mqcouch(mqhttp *t, bool debug, QObject *parent) : QObject(parent)
{
    //Set private objects
    m_mqhttp = t;
    showDebug = debug;

    m_list << mq_httpHeader{
              .key = "Content-Type",
              .value = "application/json"
            };
}

mqcouch::mqcouch(mqhttp *t, QString connectionUrl, bool debug, QObject *parent) : QObject(parent)
{
    //Set private objects
    m_mqhttp = t;
    databaseUrl = connectionUrl;
    showDebug = debug;

    m_list << mq_httpHeader{
              .key = "Content-Type",
              .value = "application/json"
            };
}

bool mqcouch::createDatabase(QString databaseName)
{
    QString _query = databaseUrl + "/" + databaseName;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "PUT", JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["result"] == "error")
    {
        if(showDebug)
          qDebug() << "Problem on creating database" << entity["desc"].toString();

        return false;
    }
    else
        return true;
}

bool mqcouch::removeDatabase(QString databaseName)
{
    QString _query = databaseUrl + "/" + databaseName;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "DELETE", JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["result"] == "error")
    {
        if(showDebug)
          qDebug() << "Problem on removing database" << entity["desc"].toString();

        return false;
    }
    else
        return true;
}

QJsonObject mqcouch::informationDatabase(QString databaseName)
{
    QString _query = databaseUrl + "/" + databaseName;
    //qDebug() << query;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["result"] == "error")
    {
        QJsonObject _d = {
          {"result", "error in database side"},
          {"details", entity["desc"]}
        };
        return _d;
    }
    else
        return doc.object();
}


_mq_databaseInfo mqcouch::informationDatabaseStruct(QString databaseName)
{
    QString _query = databaseUrl + "/" + databaseName;
    //qDebug() << query;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["result"] == "error")
        return _mq_databaseInfo();
    else
    {
        QJsonObject data = doc.object();

        _mq_databaseInfo info;
        info.db_name = data["db_name"].toString();
        info.data_size = data["data_size"].toInt();
        info.doc_count = data["doc_count"].toInt();
        info.doc_del_count = data["doc_del_count"].toInt();
        info.instance_start_time = data["instance_start_time"].toString();

        return info;
    }
}

QJsonDocument mqcouch::runDiagQuery(QString query)
{
    QString _query = databaseUrl + "/" + query;
    //qDebug() << query;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["result"] == "error")
    {
        QJsonObject _d = {
          {"result", "error in database side"},
          {"details", entity["desc"]}
        };
        return QJsonDocument(_d);
    }
    else
        return doc;
}

QString mqcouch::getUuid()
{
    QString _query = databaseUrl + "/" + "_uuids";
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    const QJsonObject entity = doc.object();

    if(entity["result"] != "error")
    {
        QJsonArray value = entity["uuids"].toArray();
        return value.first().toString();
    }

    return "";
}

QStringList mqcouch::getUuids(int limit)
{
    QString _query = databaseUrl + "/" + "_uuids?count=" + QString::number(limit);
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    const QJsonObject entity = doc.object();

    QStringList uuids;

    if(entity["result"] != "error")
    {
        QJsonArray values = entity["uuids"].toArray();

        for(auto value : values)
            uuids.append(value.toString());

        return uuids;
    }

    return QStringList();
}

bool mqcouch::isActive()
{
    QString _query = databaseUrl + "/";
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["couchdb"] == "Welcome")
    {
        if(showDebug)
        {
            qDebug() << "CouchDB version" << entity["version"].toString() << "Time to relax.";
            qDebug() << "Powered by" << QJsonObject(entity["vendor"].toObject())["name"].toString();
            qDebug() << "Uuid:" << entity["uuid"].toString();
            qDebug() << "Driver mqcouch and mqhttp is running, by HKN!";
        }
        return true;
    }
    else
        return false;
}

_mq_documentRaw mqcouch::getDocument(QString database, QString id)
{
    QString _query = databaseUrl + "/" + database + "/" + id;
    _mq_documentRaw data;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    /*
     * Sample Json
     * {"_id":"76aa2bb58c4996a414d321e7a80021d3","_rev":"1-8ecb908fbedda2e535121a19db7194d6","name":"test22"}
    */

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        data.id = entity["_id"].toString();
        data.rev = entity["_rev"].toString();
        data.data = doc;

        return data;
    }

    if(showDebug)
        qDebug() << doc.object();

    return data;
}

_mq_documentRaw mqcouch::getDocumentRevision(QString database, QString id, QString request_rev)
{
    QString _query = databaseUrl + "/" + database + "/" + id + "?rev=" + request_rev;
    _mq_documentRaw data;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    /*
     * Sample Json
     * {"_id":"76aa2bb58c4996a414d321e7a80021d3","_rev":"1-8ecb908fbedda2e535121a19db7194d6","name":"test22"}
    */

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        data.id = entity["_id"].toString();
        data.rev = entity["_rev"].toString();
        data.data = doc;

        return data;
    }

    if(showDebug)
        qDebug() << doc.object();

    return data;
}

QList<_mq_document> mqcouch::getDocumentList(QString database)
{
    QString _query = databaseUrl + "/" + database + "/_all_docs";
    QList<_mq_document> data;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    /*
     * Sample Json
     * {"total_rows":6,"offset":0,"rows":
     * [{"id":"76aa2bb58c4996a414d321e7a80021d3","key":"76aa2bb58c4996a414d321e7a80021d3","value":{"rev":"1-8ecb908fbedda2e535121a19db7194d6"}}]}
    */

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        QJsonArray rows = entity["rows"].toArray();

        for(QJsonValue row : rows)
        {
            QJsonObject _row = row.toObject();
            _mq_document _doc = { .id = _row["id"].toString(), .rev = QJsonObject(_row["value"].toObject())["rev"].toString(), .ok = true };
            data.push_back(_doc);
        }

        return data;
    }

    if(showDebug)
        qDebug() << doc.object();

    return data;
}

QList<_mq_document> mqcouch::getDocumentList(QString database, int limitValue, bool reversed)
{
    QString _query = databaseUrl + "/" + database + "/_all_docs" + "?limit=" + QString::number(limitValue) + "&descending=" + QString(reversed ? "true": "false");
    QList<_mq_document> data;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

    /*
     * Sample Json
     * {"total_rows":6,"offset":0,"rows":
     * [{"id":"76aa2bb58c4996a414d321e7a80021d3","key":"76aa2bb58c4996a414d321e7a80021d3","value":{"rev":"1-8ecb908fbedda2e535121a19db7194d6"}}]}
    */

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        QJsonArray rows = entity["rows"].toArray();

        for(QJsonValue row : rows)
        {
            QJsonObject _row = row.toObject();
            _mq_document _doc = { .id = _row["id"].toString(), .rev = QJsonObject(_row["value"].toObject())["rev"].toString(), .ok = true };
            data.push_back(_doc);
        }

        return data;
    }

    if(showDebug)
        qDebug() << doc.object();

    return data;
}

QList<QPair<int, QString>> mqcouch::getRevisionList(QString database, QString id, bool newFirstOrder)
{
     QString _query = databaseUrl + "/" + database + "/" + id + "?revs=true";
     QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();

     QList<QPair<int, QString>> list;

     /*
      *  QJsonObject({"_id":"76aa2bb58c4996a414d321e7a8002a21","_rev":"2-b85a6cbb064d3e0927b6bfe718321f5d",
      * "_revisions":{"ids":["b85a6cbb064d3e0927b6bfe718321f5d","8ecb908fbedda2e535121a19db7194d6"],"start":2},
      * "data":["sta","aka","dji"],"name":"a22test","result":null})
      */

     const QJsonObject entity = doc.object();
     if(entity["result"] != "error")
     {
         QJsonArray ary = QJsonObject(entity["_revisions"].toObject())["ids"].toArray();

         for(int i = 0; i < ary.count(); i++)
         {
             QPair<int, QString> val;
             // For reverse order of the array
             val.first = abs(ary.count() - i);
             val.second = ary[i].toString();

             if(NOT newFirstOrder)
                list.push_front(val);
             else
                 list.push_back(val);
         }

         return list;
     }

     if(showDebug)
         qCritical() << "Error message!!";
         qDebug() << entity;

     return QList<QPair<int, QString>>();
}

_mq_document mqcouch::addDocument(QString database, QJsonDocument body)
{
    QString _query = databaseUrl + "/" + database;
    _mq_document response;
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "POST", body.toJson(), JSON).toJsonDocument();

    /*
     * Sample Json
     * {"id":"76aa2bb58c4996a414d321e7a8001211","ok":true,"result":null,"rev":"1-49ce25e3db701c8cb613c1fd18d99619"}
    */

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        response.id = entity["id"].toString();
        response.rev = entity["rev"].toString();
        response.ok = entity["ok"].toBool();

        return response;
    }

    return response;
}

_mq_document mqcouch::updateDocument(QString database, QJsonDocument body, QString id)
{
    QString rev = getDocument(database, id).rev;
    QString _query = databaseUrl + "/" + database + "/" + id + "?rev=" + rev;

    _mq_document response;

    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "PUT", body.toJson(), JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        response.id = entity["id"].toString();
        response.rev = entity["rev"].toString();
        response.ok = entity["ok"].toBool();

        return response;
    }

    if(showDebug)
        qDebug() << doc.object();

    return response;
}

_mq_document mqcouch::updateDocument(QString database, QJsonDocument body, _mq_document fdoc)
{
    QString _query = databaseUrl + "/" + database + "/" + fdoc.id + "?rev=" + fdoc.rev;

    _mq_document response;

    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "PUT", body.toJson(), JSON).toJsonDocument();

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        response.id = entity["id"].toString();
        response.rev = entity["rev"].toString();
        response.ok = entity["ok"].toBool();

        return response;
    }

    if(showDebug)
        qDebug() << doc.object();

    return response;
}

bool mqcouch::removeDocument(QString database, QString id)
{
    QString rev = getDocument(database, id).rev;
    QString _query = databaseUrl + "/" + database + "/" + id + "?rev=" + rev;

    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "DELETE", JSON).toJsonDocument();

    /*
     * Sample Json
     * {"id":"76aa2bb58c4996a414d321e7a8001211","ok":true,"rev":"2-277a0d434467120336c479306bd814aa"}
    */

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        /*
        if(showDebug)
            qDebug() << doc.object();
        */

        return true;
    }

    return false;
}

bool mqcouch::removeDocument(QString database, _mq_document document)
{
    QString _query = databaseUrl + "/" + database + "/" + document.id + "?rev=" + document.rev;

    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "DELETE", JSON).toJsonDocument();

    /*
     * Sample Json
     * {"id":"76aa2bb58c4996a414d321e7a8001211","ok":true,"rev":"2-277a0d434467120336c479306bd814aa"}
    */

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        /*
        if(showDebug)
            qDebug() << doc.object();
        */

        return true;
    }

    return false;
}

QStringList mqcouch::getAttachmentList(QString database, _mq_document fdoc)
{
    _mq_documentRaw raw = getDocument(database, fdoc.id);

    QJsonObject obj = raw.data.object();
    QJsonObject final = obj["_attachments"].toObject();

    return final.keys();
}

QStringList mqcouch::getAttachmentList(QString database, QString id)
{
    _mq_documentRaw raw = getDocument(database, id);

    QJsonObject obj = raw.data.object();
    QJsonObject final = obj["_attachments"].toObject();

    return final.keys();
}

bool mqcouch::addAttachmentToDocument(QString database, _mq_document fdoc, QString fileurl)
{
    //Append attachment to last revision
    fdoc.rev = getDocument(database, fdoc.id).rev;

    //File object
    QFile *file = new QFile(fileurl);
    file->open(QIODevice::ReadOnly);

    if(NOT file->isOpen())
    {
        if(showDebug)
            qDebug() << "File is not readable!";

        return false;
    }

    QFileInfo info(*file);

    const QString mimetype = mimedb.mimeTypeForFile(info, QMimeDatabase::MatchContent).name();
    const QByteArray imported = file->readAll();

    file->close();

    _mq_attachment finalFile = {
        .name = info.fileName(),
        .mimeType = mimetype,
        .body = imported
    };

    return addAttachmentToDocumentRaw(database, fdoc, finalFile);
}

bool mqcouch::addAttachmentToDocumentRaw(QString database, _mq_document fdoc, _mq_attachment attachment)
{
    QString _query = databaseUrl + "/" + database + "/" + fdoc.id + "/" + attachment.name + "?rev=" + fdoc.rev;

    QList<mq_httpHeader> customList;
    customList << mq_httpHeader{ .key = "Content-Type", .value = attachment.mimeType };

    QJsonDocument m_doc = m_mqhttp->custom(_query, customList, "PUT", attachment.body, JSON).toJsonDocument();
    QJsonObject entity = m_doc.object();

    /*
     * Sample Json
     * {"ok":true,"id":"doc","rev":"1-287a28fa680ae0c7fb4729bf0c6e0cf2"}
    */
    if(entity["result"] != "error")
        return true;

    if(showDebug)
    {
        qDebug() << "Some problem on uploading!" << m_doc.toJson();
    }

    return false;
}

bool mqcouch::removeAttachmentFromDocument(QString database, _mq_document fdoc, QString attachmentName)
{
    QString _query = databaseUrl + "/" + database + "/" + fdoc.id + "/" + attachmentName + "?rev=" + fdoc.rev;

    QJsonDocument m_doc = m_mqhttp->custom(_query, m_list, "DELETE", JSON).toJsonDocument();
    QJsonObject entity = m_doc.object();

    /*
     * Sample Json
     * {"ok":true,"id":"doc","rev":"1-287a28fa680ae0c7fb4729bf0c6e0cf2"}
    */
    if(entity["result"] != "error")
        return true;

    if(showDebug)
    {
        qDebug() << "Some problem on removing!" << m_doc.toJson();
    }

    return false;
}

QStringList mqcouch::allDatabases()
{
    QString _query = databaseUrl + "/" + "_all_dbs";
    QJsonDocument doc = m_mqhttp->custom(_query, m_list, "GET", JSON).toJsonDocument();
    QStringList results;

    QJsonObject entity = doc.object();
    if(entity["result"] != "error")
    {
        QJsonArray ary = doc.array();
        for(QJsonValue _value : ary)
        {
            results << _value.toString();
        }
    }

    return results;
}
