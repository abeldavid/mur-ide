#include <QDebug>
#include <QApplication>

#include "compilationoptionsview.h"

CompilationOptionsView::CompilationOptionsView(QWidget *parent) : QListWidget(parent)
{
    setup();
    setContextMenuPolicy(Qt::CustomContextMenu);
    createMenu();
    setMouseTracking(true);
}

void CompilationOptionsView::setOptions(const QStringList &options)
{
    clear();

    QString option;
    foreach(option, options) {
        QListWidgetItem* item = new QListWidgetItem(option);
        item->setFlags (item->flags() | Qt::ItemIsEditable);
        addItem(item);
    }
}

void CompilationOptionsView::addOption(const QString &option)
{
    QListWidgetItem* item = new QListWidgetItem(option);
    item->setFlags (item->flags() | Qt::ItemIsEditable);
    addItem(item);
}

void CompilationOptionsView::setModified(bool modified)
{
    m_isModified = modified;
}

bool CompilationOptionsView::isModified() const
{
    return m_isModified;
}

QStringList CompilationOptionsView::getOptions() const
{
    QStringList options;

    for (int index = 0; index < count(); ++index) {
        options << item(index)->text();
    }

    return options;
}

void CompilationOptionsView::mouseMoveEvent(QMouseEvent *e)
{
    QListWidget::mouseMoveEvent(e);
    QPoint globalCursorPos = QCursor::pos();

    QModelIndex index = indexAt(mapFromGlobal(globalCursorPos));
    if (index.isValid()) {
        setCursor(Qt::IBeamCursor);
    }
    else {
        QApplication::restoreOverrideCursor();
    }
}

void CompilationOptionsView::showContextMenu(const QPoint &pos)
{
    m_contextMenu->exec(mapToGlobal(pos));
}

void CompilationOptionsView::createMenu()
{
    m_addOptionAct = new QAction(tr("Добавить опцию"), this);
    m_removeOptionAct = new QAction(tr("Удалить опцию"), this);
    m_contextMenu = new QMenu(this);

    m_contextMenu->addAction(m_removeOptionAct);
    m_contextMenu->addAction(m_addOptionAct);


    //Remove option
    QObject::connect(m_removeOptionAct, &QAction::triggered, [this]() {
        QModelIndex index = this->indexAt(this->mapFromGlobal(m_contextMenu->pos()));
        if (index.isValid()) {
            delete takeItem(index.row());
            m_isModified = true;
        }
    });

    //Add option
    QObject::connect(m_addOptionAct, &QAction::triggered, [this]() {
        QListWidgetItem* item = new QListWidgetItem("%new_option%");
        item->setFlags (item->flags() | Qt::ItemIsEditable);
        addItem(item);
        item->setSelected(true);
        setFocus();
        m_isModified = true;
    });

    QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void CompilationOptionsView::setup()
{

}
