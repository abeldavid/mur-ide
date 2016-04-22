#ifndef BLUETOOTHSELECTDIALOG_H
#define BLUETOOTHSELECTDIALOG_H

#include <QDialog>
#include <QString>
#include <QListWidget>

class BluetoothSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BluetoothSelectDialog(QWidget *parent = 0);

public slots:
    void showDeviceSelect(QByteArray devices);

signals:
    void deviceSelected(QString);

private:
    const QString m_nameSeparator = QString(" - ");
    QListWidget* m_deviceList;

private slots:
    void listItemSelected(QListWidgetItem* item);
};

#endif // BLUETOOTHSELECTDIALOG_H
