#include <QCoreApplication>
#include <QDebug>

#include "mqhttp.h"
#include "mqcouch.h"

#include <QJsonDocument>
#include <QPair>

mqhttp *_mqhttp;
mqcouch *_mqcouch;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    _mqhttp = new mqhttp();

    /* code */

    return 0;
}