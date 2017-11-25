/**
 *  @file    mqhttp.cpp
 *  @author  Hakan AFAT (Github: phantomxe)
 *  @date    02/10/2017
 *  @version 1.0
 *
 *  @brief Basic Sync-HTTP Client
 *
 *  @section DESCRIPTION
 *
 *  Basic Sync-HTTP Client
 *  Powered by QNetworkAccessManager
 */

#include "mqhttp.h"

mqhttp::mqhttp(QObject *parent) : QObject(parent)
{
    sslConf = new QSslConfiguration(QSslConfiguration::defaultConfiguration());
    sslConf->setProtocol(QSsl::TlsV1SslV3);

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));
}

QVariant mqhttp::get(QString url, QList<mq_httpHeader> headers, responseType type)
{
    QEventLoop q_eventLoop;
    QVariant q_response;

    QNetworkRequest q_request(url);
    QNetworkReply *m_response;

    q_request.setSslConfiguration(*sslConf);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), &q_eventLoop, SLOT(quit()));

    for(auto header : headers)
    {
        q_request.setRawHeader(QByteArray::fromStdString(header.key.toStdString().c_str()),
                               QByteArray::fromStdString(header.value.toStdString().c_str()));
    }

    m_response = m_manager->get(q_request);
    q_eventLoop.exec();

    if(m_response->error() == QNetworkReply::NoError)
    {
        QByteArray response = m_response->readAll();

        if(type == JSON)
        {
            QJsonDocument doc = QJsonDocument::fromJson(response);
            return doc;
        }
        else
            return QString::fromLatin1(response);
    }
    else
    {
        const QString errorCode = m_response->errorString();
        return errorCode;
    }

    return q_response;
}

QVariant mqhttp::post(QString url, QList<mq_httpHeader> headers, QJsonDocument body, responseType type)
{
    QEventLoop q_eventLoop;
    QVariant q_response;

    QNetworkRequest q_request(url);
    QNetworkReply *m_response;

    q_request.setSslConfiguration(*sslConf);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), &q_eventLoop, SLOT(quit()));

    bool hasContentType = false;
    for(auto t_header : headers)
    {
        if(t_header.key == "Content-Type")
            hasContentType = true;
    }

    if( !hasContentType )
        headers.append(mq_httpHeader{ .key = "Content-Type", .value = "application/json"});

    for(auto header : headers)
    {
        q_request.setRawHeader(QByteArray::fromStdString(header.key.toStdString().c_str()),
                               QByteArray::fromStdString(header.value.toStdString().c_str()));
    }

    const QByteArray data = body.toJson();

    m_response = m_manager->post(q_request, data);
    q_eventLoop.exec();

    if(m_response->error() == QNetworkReply::NoError)
    {
        QByteArray response = m_response->readAll();
        qDebug() << response;

        if(type == JSON)
        {
            QJsonDocument doc = QJsonDocument::fromJson(response);
            return doc;
        }
        else
            return QString::fromLatin1(response);
    }
    else
    {
        const QString errorCode = m_response->errorString();
        return errorCode;
    }

    return q_response;
}

QVariant mqhttp::put(QString url, QList<mq_httpHeader> headers, QJsonDocument body, responseType type)
{
    QEventLoop q_eventLoop;
    QVariant q_response;

    QNetworkRequest q_request(url);
    QNetworkReply *m_response;

    q_request.setSslConfiguration(*sslConf);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), &q_eventLoop, SLOT(quit()));

    bool hasContentType = false;
    for(auto t_header : headers)
    {
        if(t_header.key == "Content-Type")
            hasContentType = true;
    }

    if( !hasContentType )
        headers.append(mq_httpHeader{ .key = "Content-Type", .value = "application/json"});

    for(auto header : headers)
    {
        q_request.setRawHeader(QByteArray::fromStdString(header.key.toStdString().c_str()),
                               QByteArray::fromStdString(header.value.toStdString().c_str()));
    }

    const QByteArray data = body.toJson();

    m_response = m_manager->put(q_request, data);
    q_eventLoop.exec();

    if(m_response->error() == QNetworkReply::NoError)
    {
        QByteArray response = m_response->readAll();

        if(type == JSON)
        {
            QJsonDocument doc = QJsonDocument::fromJson(response);
            return doc;
        }
        else
            return QString::fromLatin1(response);
    }
    else
    {
        const QString errorCode = m_response->errorString();
        return errorCode;
    }

    return q_response;
}

QVariant mqhttp::custom(QString url, QList<mq_httpHeader> headers, QString verb, responseType type)
{
    QEventLoop q_eventLoop;
    QVariant q_response;

    QNetworkRequest q_request(url);
    QNetworkReply *m_response;

    q_request.setSslConfiguration(*sslConf);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), &q_eventLoop, SLOT(quit()));

    for(auto header : headers)
    {
        q_request.setRawHeader(QByteArray::fromStdString(header.key.toStdString().c_str()),
                               QByteArray::fromStdString(header.value.toStdString().c_str()));
    }

    m_response = m_manager->sendCustomRequest(q_request, QByteArray::fromStdString(verb.toStdString()));
    q_eventLoop.exec();

    QJsonDocument doc;
    if(m_response->error() == QNetworkReply::NoError)
    {
        QByteArray response = m_response->readAll();

        if(type == JSON)
        {
            doc = QJsonDocument::fromJson(response);
            return doc;
        }
        else if (type == STATUS)
        {
            QString status = QString::number(m_response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            return status;
        }
        else
            return QString::fromLatin1(response);
    }
    else
    {
        const QString errorCode = m_response->errorString();
        QJsonObject obj = {
            {"result", "error"},
            {"desc", QJsonValue(errorCode)}
        };

        return QJsonDocument(obj);
    }

    return q_response;
}

QVariant mqhttp::custom(QString url, QList<mq_httpHeader> headers, QString verb, QByteArray data, responseType type)
{
    QEventLoop q_eventLoop;
    QVariant q_response;

    QNetworkRequest q_request(url);
    QNetworkReply *m_response;

    q_request.setSslConfiguration(*sslConf);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), &q_eventLoop, SLOT(quit()));

    for(auto header : headers)
    {
        q_request.setRawHeader(QByteArray::fromStdString(header.key.toStdString().c_str()),
                               QByteArray::fromStdString(header.value.toStdString().c_str()));
    }

    m_response = m_manager->sendCustomRequest(q_request, verb.toStdString().c_str(), data);
    q_eventLoop.exec();

    if(m_response->error() == QNetworkReply::NoError)
    {
        QByteArray response = m_response->readAll();

        if(type == JSON)
        {
            QJsonDocument doc = QJsonDocument::fromJson(response);
            return doc;
        }
        else if (type == STATUS)
        {
            QString status = QString::number(m_response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            return status;
        }
        else
            return QString::fromLatin1(response);
    }
    else
    {
        const QString errorCode = m_response->errorString();
        return errorCode;
    }

    return q_response;
}

void mqhttp::handleSslErrors(QNetworkReply *reply, QList<QSslError> errors)
{
    qDebug() << "I gained an error " << reply->errorString();
}
