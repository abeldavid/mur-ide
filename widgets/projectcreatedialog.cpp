#include "projectcreatedialog.h"
#include "projectmanager.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>

ProjectCreateDialog::ProjectCreateDialog(QWidget *parent) :
    QDialog(parent),
    m_dirLocationEdit(new QLineEdit(this)),
    m_nameEdit(new QLineEdit(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Новый проект");

    QFormLayout *layout = new QFormLayout(this);
    setLayout(layout);

    QLabel *nameLabel = new QLabel("Название проекта: ", this);
    layout->addRow(nameLabel);
    layout->addRow(m_nameEdit);
    m_nameEdit->setText(ProjectManager::instance().defaultNewProjectName());

    QLabel *dirLocationLabel = new QLabel("Расположение: ", this);
    layout->addRow(dirLocationLabel);
    m_dirLocationEdit->setText(ProjectManager::instance().projectsRoot());
    m_dirLocationEdit->setFixedWidth(300);
    QPushButton *loadPathButton = new QPushButton(tr("Обзор"), this);
    layout->addRow(m_dirLocationEdit, loadPathButton);

    QFrame* myFrame = new QFrame();
    myFrame->setFrameShape(QFrame::HLine);
    layout->addRow(myFrame);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal,
                                                       this);
    layout->addRow(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(createProject()));

    connect(loadPathButton, SIGNAL(clicked(bool)), this, SLOT(selectFolder()));
}

void ProjectCreateDialog::selectFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Директория с проектами"),
                                                    ProjectManager::instance().projectsRoot());
    if (!dir.isEmpty()) {
        m_dirLocationEdit->setText(dir);
    }
}

void ProjectCreateDialog::createProject()
{
    emit createProjectConfirmed(m_dirLocationEdit->text(), m_nameEdit->text());
}
