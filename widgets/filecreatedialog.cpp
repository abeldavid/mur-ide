#include "filecreatedialog.h"
#include "projectmanager.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QListView>
#include <QLabel>
#include <QDebug>
#include <QStringListModel>
#include <QSortFilterProxyModel>

FileCreateDialog::FileCreateDialog(QWidget *parent) :
    QDialog(parent),
    m_nameEdit(new QLineEdit(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Создать файл");

    QFormLayout *layout = new QFormLayout(this);
    setLayout(layout);

    QLabel *nameLabel = new QLabel("Название файла: ", this);
    layout->addRow(nameLabel);

    m_comboBox = new QComboBox(this);
    m_comboBox ->setView(new QListView()); // style problem workaround

    QStringListModel *model = new QStringListModel(Project::defaultFilePrefixes.keys(), this);
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model);
    proxy->sort(0);
    m_comboBox->setModel(proxy);

    m_nameEdit->setFixedWidth(300);
    layout->addRow(m_nameEdit, m_comboBox);

    QFrame* myFrame = new QFrame();
    myFrame->setFrameShape(QFrame::HLine);
    layout->addRow(myFrame);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal,
                                                       this);
    layout->addRow(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(createFile()));

    connect(m_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDefaultFileName(QString)));
}

void FileCreateDialog::init()
{
    this->setDefaultFileName(m_comboBox->currentText());
}

void FileCreateDialog::createFile()
{
    emit createFileConfirmed(m_nameEdit->text().split(Project::multiFileSeparator));
}

void FileCreateDialog::setDefaultFileName(QString extension)
{
    QStringList extensionList = extension.split(Project::multiFileSeparator);
    for (int i = 0; i < extensionList.size(); ++i) {
        extensionList[i] = ProjectManager::instance().defaultNewFileName(extension) + extensionList[i];
    }
    m_nameEdit->setText(extensionList.join(Project::multiFileSeparator));
}
