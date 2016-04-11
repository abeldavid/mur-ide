#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include "abstractconnection.h"

class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionManager(QObject *parent = 0);

signals:

public slots:

private:
    AbstractConnection *m_connection;
};

#endif // CONNECTIONMANAGER_H
