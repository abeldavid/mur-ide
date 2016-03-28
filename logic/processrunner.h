#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QObject>
#include <QRunnable>
#include <QProcess>
#include <QThread>

class ProcessRunner : public QObject, public QRunnable
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
    void processOutputReady(QString, bool);
    void finished(int);
private:
    QString m_path;
    QStringList m_arguments;
    QProcess *m_process;
    QThread *m_compilationThread;
};

#endif // PROCESSRUNNER_H
