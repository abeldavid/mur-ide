#include "filecreatedialog.h"
#include "projectmanager.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QDebug>
#include <QHash>
#include <QStringList>
#include <QStringListModel>
#include <QSortFilterProxyModel>

FileCreateDialog::FileCreateDialog(QWidget *parent) :
    QDialog(parent),
    m_nameEdit(new QLineEdit(this))
{
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    QLabel *nameLabel = new QLabel("Название файла: ", this);

    layout->addWidget(nameLabel);
    layout->addWidget(m_nameEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    QComboBox *comboBox = new QComboBox(this);

    QStringListModel *model = new QStringListModel(Project::defaultFilePrefixes.keys(), this);
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model);
    proxy->sort(0);
    comboBox->setModel(proxy);

    layout->addWidget(comboBox);

    this->setDefaultFileName(comboBox->currentText());

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(createFile()));

    connect(comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDefaultFileName(QString)));
}

void FileCreateDialog::createFile()
{
    QStringList fileNames = m_nameEdit->text().split(Project::multiFileSeparator);
    for (int i = 0; i < fileNames.size(); ++i) {
        ProjectManager::instance().createFile(fileNames[i]);
    }
}

void FileCreateDialog::setDefaultFileName(QString extension)
{
    QStringList extensionList = extension.split(Project::multiFileSeparator);
    for (int i = 0; i < extensionList.size(); ++i) {
        extensionList[i] = ProjectManager::instance().defaultNewFileName(extension) + extensionList[i];
    }
    m_nameEdit->setText(extensionList.join(Project::multiFileSeparator));
}
