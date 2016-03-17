#include "filecreatedialog.h"
#include "projectmanager.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

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
    m_nameEdit->setText(ProjectManager::instance().defaultNewFileName(".cpp"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(createFile()));
}

void FileCreateDialog::createFile()
{
    qDebug()<<"create";
    ProjectManager::instance().createFile(m_nameEdit->text());
}
