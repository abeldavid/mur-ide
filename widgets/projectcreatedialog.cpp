#include "projectcreatedialog.h"
#include "projectmanager.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

ProjectCreateDialog::ProjectCreateDialog(QWidget *parent) :
    QDialog(parent),
    m_dirLocationEdit(new QLineEdit(this)),
    m_nameEdit(new QLineEdit(this))
{
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    QLabel *dirLocationLabel = new QLabel("Расположение: ", this);
    QLabel *nameLabel = new QLabel("Название проекта: ", this);

    layout->addWidget(dirLocationLabel);
    m_dirLocationEdit->setText(ProjectManager::instance().projectsRoot());
    layout->addWidget(m_dirLocationEdit);
    QPushButton *loadPathButton = new QPushButton(tr("Обзор"), this);
    layout->addWidget(loadPathButton);

    layout->addWidget(nameLabel);
    layout->addWidget(m_nameEdit);
    m_nameEdit->setText(ProjectManager::instance().defaultNewProjectName());

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(loadPathButton, SIGNAL(clicked(bool)), this, SLOT(onLoadClicked()));

    layout->addWidget(buttonBox);

//    QPushButton *okButton = new QPushButton(tr("OK"), this);
//    layout->addWidget(okButton, 2, 1);
//    okButton->setDefault(true);
}

void ProjectCreateDialog::onLoadClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Директория с проектами"),
                                                    ProjectManager::instance().projectsRoot());
    if (!dir.isEmpty()) {
        m_dirLocationEdit->setText(dir);
    }
}
