#include "compilationoptionsview.h"

CompilationOptionsView::CompilationOptionsView(QWidget *parent) : QListWidget(parent)
{
    setup();
    setContextMenuPolicy(Qt::CustomContextMenu);

}

void CompilationOptionsView::setOptions(const QStringList &options)
{
    m_compilationOptions = options;
}

void CompilationOptionsView::addOption(const QString &option)
{
    m_compilationOptions << option;
}

QStringList CompilationOptionsView::getOptions()
{
    return m_compilationOptions;
}

void CompilationOptionsView::showContextMenu(const QPoint &pos)
{

}

void CompilationOptionsView::createMenu()
{
    m_addOptionAct = new QAction(tr("Добавить опцию"), this);
    m_removeOptionAct = new QAction(tr("Удалть опцию"), this);


}

void CompilationOptionsView::setup()
{
    QListWidgetItem* itemOne = new QListWidgetItem("-O3");
    QListWidgetItem* itemTwo = new QListWidgetItem("-lmraa");
    QListWidgetItem* itemThree = new QListWidgetItem("-lopencvcore");
    QListWidgetItem* itemFour = new QListWidgetItem("-lsomething_highgui");

    addItem(itemOne);
    addItem(itemTwo);
    addItem(itemThree);
    addItem(itemFour);
}
