#ifndef MQHTTP_H
#define MQHTTP_H

#include <QObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkReply>

#include <QSsl>
#include <QSslConfiguration>

#include <QUrl>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QEventLoop>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

typedef struct mq_httpHeader{
    QString key;
    QString value;
} mq_httpHeader;

typedef struct mq_response{
    QVariant data;
    QVariant error;
} mq_response;

enum responseType{
    JSON = 0,
    HTML = 1,
    XML  = 2,
    STATUS = 3
};

class mqhttp : public QObject
{
    Q_OBJECT
public:
    explicit mqhttp(QObject *parent = 0);

    QVariant get(QString url, QList<mq_httpHeader> headers, responseType type);
    QVariant post(QString url, QList<mq_httpHeader> headers, QJsonDocument body, responseType type);
    QVariant put(QString url, QList<mq_httpHeader> headers, QJsonDocument body, responseType type);
    QVariant custom(QString url, QList<mq_httpHeader> headers, QString verb, responseType type);
    QVariant custom(QString url, QList<mq_httpHeader> headers, QString verb, QByteArray data, responseType type);
signals:

public slots:
    void handleSslErrors(QNetworkReply *reply, QList<QSslError> errors);
private:
    QNetworkAccessManager *m_manager;
    QSslConfiguration *sslConf;
};

#endif // MQHTTP_H
