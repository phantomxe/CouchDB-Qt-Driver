#ifndef MQCOUCH_H
#define MQCOUCH_H

#include <QObject>

#include "mqhttp.h"
#include "mqcouch_types.h"

#include <QDebug>
#include <QPair>
#include <QList>
#include <QString>
#include <QStringList>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>

class mqcouch : public QObject
{
    Q_OBJECT
public:
    explicit mqcouch(mqhttp *t, bool debug = true, QObject *parent = 0);
    explicit mqcouch(mqhttp *t, QString connectionUrl, bool debug = true, QObject *parent = 0);

    /// @return Returns database connection is alive
    bool isActive();

    /**
     * @brief Run Query on database
     * @param query specifies for going to send request to database
     * @note query is not escaped, must be use in attention
     * @return Raw response from database type in QJsonDocument
     */
    QJsonDocument runDiagQuery(QString query);

    /// @return uuid string generated from database
    QString getUuid();

    /**
     * @brief Generate uuid
     * @param limit How many uuids?
     * @return Uuid string list from database
     */
    QStringList getUuids(int limit);


    /**
     * @brief Get document from database
     * @note last revision is returned
     * @param database collection name
     * @param id document's id for getting data
     * @return _mq_documentRaw type returns id, last revision and QJsonDocument of document
     */
    _mq_documentRaw getDocument(QString database, QString id);

    /**
     * @brief Get document from database
     * @param database collection name
     * @param id document's id for getting data
     * @param request_rev specifies getting true revision
     * @return _mq_documentRaw type returns id, needed-revision and QJsonDocument of document
     */
    _mq_documentRaw getDocumentRevision(QString database, QString id, QString request_rev);


    /**
     * @brief Get All Document in database(collection for my understanding, i used mongo before)
     * @param database collection name
     * @return List of documents with last revision keys, id and raw QJsonDocument
     */
    QList<_mq_document> getDocumentList(QString database);

    /**
     * @brief Get All Document in database(collection for my understanding, i used mongo before)
     * @param database collection name
     * @param limitValue for limiting requested rows
     * @param reversed is defaultly false, it's connected directly to query input, has not any list manipulation
     * @return List of documents with last revision keys, id and raw QJsonDocument
     */
    QList<_mq_document> getDocumentList(QString database, int limitValue, bool reversed = false);

    /**
     * @brief It converts revision pair to raw string for usable to query
     * @param data from revision list
     * @return raw revision string
     */
    inline QString convertRevisionString(QPair<int, QString> data)
    {
        return QString::number(data.first) + "-" + data.second;
    }

    /**
     * @brief Get revision history on a document
     * @param database collection name
     * @param id document identification
     * @param newFirstOrder is defaultly false, if you need new commented data in first, you migth be set it to true
     * @return List of a QPair type, ex. 2, b85a6cbb064d3e0927b6bfe718321f5d
     */
    QList<QPair<int, QString>> getRevisionList(QString database, QString id, bool newFirstOrder = false);


    /**
     * @brief Add a new document to database
     * @param database collection name
     * @param body QJsonDocument raw data
     * @return _mq_document type includes first revision, id and ok states
     */
    _mq_document addDocument(QString database, QJsonDocument body);

    /**
     * @brief Update document with using id
     * @param database collection name
     * @param body QJsonDocument new data
     * @param id for finding document
     * @return _mq_document type includes last revision, id and ok states
     */
    _mq_document updateDocument(QString database, QJsonDocument body, QString id);

    /**
     * @brief Update document with using _mq_document type
     * @param database collection name
     * @param body QJsonDocument new data
     * @param fdoc for finding document
     * @return _mq_document type includes last revision, id and ok states
     */
    _mq_document updateDocument(QString database, QJsonDocument body, _mq_document fdoc);


    /**
     * @brief Remove document on database with using id
     * @param database collection name
     * @param id for finding document
     * @return state of success
     */
    bool removeDocument(QString database, QString id);

    /**
     * @brief Remove document on database with using _mq_document
     * @param database collection name
     * @param document for finding it
     * @return state of success
     */
    bool removeDocument(QString database, _mq_document document);


    /**
     * @brief Get attachments list
     * @param database collection name
     * @param id for finding it
     * @return list of attachment names
     */
    QStringList getAttachmentList(QString database, QString id);

    /**
     * @brief Get attachments list
     * @param database collection name
     * @param fdoc for finding it
     * @return list of attachment names
     */
    QStringList getAttachmentList(QString database, _mq_document fdoc);

    /**
     * @brief Add attachment into document (direct file uplaod method)
     * @param database collection name
     * @param fdoc for finding it
     * @param fileurl file's url for uploading it
     * @return state of success
     */
    bool addAttachmentToDocument(QString database, _mq_document fdoc, QString fileurl);

    /**
     * @brief Add attachment into document
     * @param database collection name
     * @param fdoc for finding it
     * @param fileurl file's url for uploading it
     * @return state of success
     */
    bool addAttachmentToDocumentRaw(QString database, _mq_document fdoc, _mq_attachment attachment);

    /**
     * @brief removeAttachmentFromDocument remove with filename
     * @param database collection name
     * @param fdoc for finding it
     * @param attachmentName finding file and removing
     * @return state of success
     */
    bool removeAttachmentFromDocument(QString database, _mq_document fdoc, QString attachmentName);

    //Database templating
    class mqdatabase
    {
        public:
            mqdatabase(mqcouch *connection, const QString databaseName)
            {
                m_databaseName = databaseName;
                m_connection = connection;
            }

            inline mqdatabase &operator <<(const QJsonDocument &doc)
            {
                m_connection->addDocument(m_databaseName, doc);
                return *this;
            }

            inline void operator = (const mqdatabase &d)
            {
                this->m_databaseName = d.m_databaseName;
                this->m_connection = d.m_connection;
            }

        private:
            QString m_databaseName;
            mqcouch *m_connection;
    };

    /**
     * @brief getDatabase If couchDB contains that collection, returns it back. Except that, it will be create automatically.
     * @param database collection name
     * @return object
   */
    mqdatabase getDatabase(QString databaseName)
    {
        if(checkDatabase(databaseName))
        {
            return mqdatabase(this, databaseName);
        }
        else
        {
            createDatabase(databaseName);
            return mqdatabase(this, databaseName);
        }
    }

    /**
     * @brief checkDatabase Check the database on couchDB
     * @param databaseName collection name
     * @return state of success
     */
    bool checkDatabase(QString databaseName);

    /**
     * @brief createDatabase Create new empty database
     * @param databaseName collection name
     * @return state of success
     */
    bool createDatabase(QString databaseName);

    /**
     * @brief removeDatabase Delete full collection (Dangerous!!)
     * @param databaseName collection name
     * @return state of success
     */
    bool removeDatabase(QString databaseName);

    /**
     * @brief returns all databases
     * @return all databases in the couchdb
     */
    QStringList allDatabases();

    /**
     * @brief informationDatabase
     * @param databaseName
     * @return
     */
    QJsonObject informationDatabase(QString databaseName);
    _mq_databaseInfo informationDatabaseStruct(QString databaseName);
private:
    //Main http request/response object
    mqhttp *m_mqhttp;
    //Requests headers
    QList<mq_httpHeader> m_list;
    //Default connection url
    QString databaseUrl = "http://localhost:5984";

    //Finding file type(MIME Database), gets data from own OS
    QMimeDatabase mimedb;
    //Debug Status
    bool showDebug;
};

#endif // MQCOUCH_H
