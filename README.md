# CouchDB-Qt-Driver
simple couchDb driver for Qt

## Examples
open the main.cpp and add your changes

* Init the library
On local database,
```
_mqcouch = new mqcouch(_mqhttp, true);
```
Or a network connection,
```
_mqcouch = new mqcouch(_mqhttp, "http://192.168.2.5:5984", true);
```
  
* See database connection is alive
```
_mqcouch->isActive();
```

* Current database array
```
qDebug() << "Current databases:" <<_mqcouch->allDatabases();
```

* Create a database
```
_mqcouch->createDatabase("testDatabase");
```

* Add a document
```
_mqcouch->addDocument("testDatabase",  QJsonDocument(QJsonObject{
                                                       {"name", "test"},
                                                       {"title", "Hello World!"}
                                                   }));
```

* See revision of document
```
  qDebug() << "Revision List";
  //Sample id
  QString id = "76aa2bb58c4996a414d321e7a8002a21";   
  for(auto item : _mqcouch->getRevisionList("albums", id, false))
  {
      QString rev = _mqcouch->convertRevisionString(item);
      qDebug() << _mqcouch->getDocumentRevision("albums", id, rev).data;
  }  
```

* Get database(collection) with limit and order methods
```
  for(auto i : _mqcouch->getDocumentList("albums", 2, true))
  {
      qDebug() << "New Item:" << i.id << i.rev;
      qDebug() << _mqcouch->getDocument("albums", i.id).data;
  }
```
