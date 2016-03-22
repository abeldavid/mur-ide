#ifndef FTPWIDGET_H
#define FTPWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QDialogButtonBox>
#include <QProgressDialog>
#include <QtNetwork>
#include <QNetworkConfigurationManager>
#include <QHash>

#include "logic/qftp.h"

class FtpWidget : public QDialog
{
    Q_OBJECT
public:
    explicit FtpWidget(QWidget *parent = 0);
    QSize sizeHint() const;

private slots:
    void connectOrDisconnect();
    void downloadFile();
    void cancelDownload();
    void connectToFtp();

    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem *item, int column);
    void cdToParent();
    void updateDataTransferProgress(qint64 readBytes,
                                    qint64 totalBytes);
    void enableDownloadButton();
    void enableConnectButton();

private:
    QLabel *ftpServerLabel;
    QLineEdit *ftpServerLineEdit;
    QLabel *statusLabel;
    QTreeWidget *fileList;
    QPushButton *cdToParentButton;
    QPushButton *connectButton;
    QPushButton *downloadButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;
    //QProgressDialog *progressDialog;

    QHash<QString, bool> isDirectory;
    QString currentPath;
    QFtp *ftp;
    QFile *file;

    QNetworkSession *networkSession;
    QNetworkConfigurationManager manager;

signals:

public slots:
};

#endif // FTPWIDGET_H
