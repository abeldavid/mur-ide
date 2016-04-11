#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QObject>
#include <QProcess>

class ProcessRunner : public QObject
{
    Q_OBJECT
public:
    explicit ProcessRunner(QObject *parent = 0);
    virtual ~ProcessRunner();
    QString path() const;
    QStringList arguments() const;
public slots:
    void terminate();
    void run();
    void setPath(QString path);
    void setArguments(QStringList arguments);
private slots:
    void processOutReceived();
signals:
    // TODO: remove bad interface dependency on consoleWidget
    void processOutputReady(QString output, bool isError, bool isIDEMessage);
    void finished(int);
private:
    QString m_path;
    QStringList m_arguments;
    QProcess *m_process;
};

#endif // PROCESSRUNNER_H
