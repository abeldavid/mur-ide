#include "mainwindow.h"
#include "roboideconsole.h"
#include "wifipasswordwidget.h"
#include "settingsmanager.h"
#include "helpwidget.h"
#include "filecreatedialog.h"
#include "project.h"
#include "projectmanager.h"
#include "projectcreatedialog.h"

#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QClipboard>
#include <QDockWidget>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_roboIdeTextEdit(new RoboIdeTextEditor(this)),
      m_roboIdeConsole(new RoboIdeConsole(this)),
      m_sourceCompiller(new SourceCompiler(this)),
      m_connectedDevicesList(new ConnectedDevicesList(this)),
      m_settingsWidget(new SettingsWidget(this)),
      m_wifiConnection(new WiFiConnection(this)),
      m_localApp(new QProcess(this)),
      m_helpWidget(new HelpWidget(this)),
      m_projectTree(new ProjectTree(this))
{
    setCentralWidget(m_roboIdeTextEdit);
    createActions();
    createMenus();
    createToolBars();
    createDockWindows();
    connectActionsToSlots();
    showMaximized();

    QByteArray state = SettingsManager::instance().mainWindowState();
    if (state.size() != 0) {
        restoreState(state);
    }

    setStyleSheet("QMainWindow::separator \
    { \
        background-color: #464646;\
        width: 1px;\
        border: none;\
    }");
    m_connectedDevicesList->setVisible(false);
}

MainWindow::~MainWindow()
{
    QByteArray state = saveState();
    SettingsManager::instance().setMainWindowState(state);
}

void MainWindow::runCompilation()
{
    if (m_edisonCompileAct->isChecked())
    {
        m_sourceCompiller->setTarget(SourceCompiler::TARGET::EDISON);
    }
    if (m_mingwCompileAct->isChecked())
    {
        m_sourceCompiller->setTarget(SourceCompiler::TARGET::MINGW);
    }
    m_sourceCompiller->onRunCompilation(ProjectManager::instance().pathToFile(m_roboIdeTextEdit->fileName()));
}

void MainWindow::generateMakeFile()
{
    m_buildAct->setEnabled(false);
    if (m_roboIdeTextEdit->isModified()) {
        saveFilePromt();
    }
    ProjectManager::instance().generateMakeFile("compiler_path", "sysroot", "");
}

void MainWindow::compilationFinished()
{
    qDebug() << "Compilation finished";
    m_buildAct->setEnabled(true);
}

void MainWindow::uploadAndRun()
{
    m_uploadAndRunAct->setDisabled(true);
    m_wifiConnection->sendAndRun(m_sourceCompiller->pathToBinary());
    m_uploadAndRunAct->setEnabled(true);
    m_roboIdeConsole->append("Программа отправлена!");
}

void MainWindow::runApp()
{
    bool isOkay = false;
    if (m_edisonCompileAct->isChecked()) {
        m_runAppAct->setDisabled(true);
        m_wifiConnection->runApp();
        m_runAppAct->setEnabled(true);
        isOkay = true;
    }
    if (m_mingwCompileAct->isChecked()) {
        if (m_localApp->isOpen()) {
            m_localApp->kill();
        }
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", SettingsManager::instance().mingwBinarysPath());
        m_localApp->setProcessEnvironment(env);
        m_localApp->start(m_sourceCompiller->pathToBinary());

        if (m_localApp->state() != QProcess::NotRunning) {
            isOkay = true;
        }
    }

    if (isOkay) {
        m_roboIdeConsole->append("Программа запущена!");
    }
    else {
        m_roboIdeConsole->append("Во время запуска программы произошла ошибка.");
        if (m_mingwCompileAct->isChecked()) {
            m_roboIdeConsole->append(m_localApp->errorString());
        }
    }
}

void MainWindow::killApp()
{
    bool isOk = false;
    if (m_edisonCompileAct->isChecked()) {
        m_stopAppAct->setDisabled(true);
        m_wifiConnection->killApp();
        m_stopAppAct->setEnabled(true);
    }
    if (m_mingwCompileAct->isChecked()) {
        if (m_localApp->isOpen()) {
            m_localApp->kill();
            isOk = true;
        }

    }
    if (isOk) {
        m_roboIdeConsole->append("Программа остановлена!");
    }
    else {
        if (m_mingwCompileAct->isChecked()) {
            m_roboIdeConsole->append("Программа не была запущена. Нечего остонавливать.");
        }
    }
}

void MainWindow::projectCreateDialog() {
    ProjectCreateDialog dialog(this);
    dialog.exec();
}

void MainWindow::fileCreateDialog() {
    FileCreateDialog dialog(this);
    dialog.exec();
}

void MainWindow::fileAddDialog() {
    QString filePath = QFileDialog::getOpenFileName(
                        this,
                        tr("Добавить существующий файл"),
                        ProjectManager::instance().projectsRoot(),
                        Project::availableFileExtensions
                        );
    if (!filePath.isNull()) {
        ProjectManager::instance().addExistingFile(filePath);
    }
}

void MainWindow::projectOpenDialog() {
    QString filePath = QFileDialog::getOpenFileName(
                        this,
                        tr("Открыть проект"),
                        ProjectManager::instance().projectsRoot(),
                        "MUR Projects (" + Project::projectFileName + ")"
                        );
    if (!filePath.isNull()) {
        ProjectManager::instance().openProject(filePath);
    }
}

void MainWindow::projectClose()
{
    ProjectManager::instance().closeProject();
    m_addFileAct->setEnabled(false);
    m_createFileAct->setEnabled(false);
    m_closeProjectAct->setEnabled(false);
}


void MainWindow::openHelp()
{
    this->m_toggleHelpVisibilityAct->trigger();
}

void MainWindow::onProjectOpened()
{
    m_addFileAct->setEnabled(true);
    m_createFileAct->setEnabled(true);
    m_closeProjectAct->setEnabled(true);
}

void MainWindow::onProjectClosed()
{
    if (m_roboIdeTextEdit->isModified()) {
        saveFilePromt();
    }
}

void MainWindow::openFile(const QString &fileName)
{
    if (m_roboIdeTextEdit->isModified()) {
        saveFilePromt();
    }
    ProjectManager::instance().openFile(fileName);
}

void MainWindow::saveFile()
{
    QString fileName = m_roboIdeTextEdit->fileName();
    if (fileName.isEmpty() or !m_roboIdeTextEdit->fileExists()) {
        fileName = saveFileAsDialog();
    }
    if (!fileName.isEmpty()) {
        ProjectManager::instance().saveFile(fileName, m_roboIdeTextEdit->text());
    }
}

void MainWindow::saveFileAs()
{
    QString fileName = saveFileAsDialog();
    if (!fileName.isEmpty()) {
        ProjectManager::instance().saveFile(fileName, m_roboIdeTextEdit->text());
    }
}

QString MainWindow::saveFileAsDialog()
{
    return QFileDialog::getSaveFileName(this,
                                        tr("Сохранить файл как"),
                                        ProjectManager::instance().getProjectPath(),
                                        Project::availableFileExtensions
                                        );
}

void MainWindow::saveFilePromt()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Сохранить?", "Данный файл не был сохранен. Сохранить?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        saveFile();
    }
}

void MainWindow::onProjectCreated()
{
    m_addFileAct->setEnabled(true);
    m_createFileAct->setEnabled(true);
    m_closeProjectAct->setEnabled(true);
    openFile(ProjectManager::instance().defaultOpenFilePath());
}

void MainWindow::switchCompilationTargetToEdison()
{
    m_mingwCompileAct->setChecked(false);
    m_edisonCompileAct->setChecked(true);
}

void MainWindow::switchCompilationTargetToDesktop()
{
    m_edisonCompileAct->setChecked(false);
    m_mingwCompileAct->setChecked(true);
}

void MainWindow::processOutReceived()
{
    QString str = m_localApp->readAllStandardOutput().simplified();
    if (str != "") {
        m_roboIdeConsole->append(str);
    }
    str = m_localApp->readAllStandardError().simplified();
    if (str != "") {
        m_roboIdeConsole->append(str);
    }
    str = m_localApp->readAll().simplified();
    if (str != "") {
        m_roboIdeConsole->append(str);
    }
}

void MainWindow::createActions()
{
    //!Edit file actions

    m_redoAct = new QAction(QIcon(":/icons/icons/tools/redo.png"), tr("Повторить"), this);
    m_redoAct->setShortcut(QKeySequence::Redo);
    m_redoAct->setIconVisibleInMenu(false);

    m_undoAct = new QAction(QIcon(":/icons/icons/tools/undo.png"), tr("Отменить"), this);
    m_undoAct->setShortcut(QKeySequence::Undo);
    m_undoAct->setIconVisibleInMenu(false);

    m_saveAct = new QAction(QIcon(":/icons/icons/tools/save.png"), tr("Сохранить"), this);
    m_saveAct->setShortcut(QKeySequence::Save);
    m_saveAct->setIconVisibleInMenu(false);

    m_saveAsAct = new QAction(QIcon(":/icons/icons/tools/save-as.png"), tr("Сохранить как..."), this);
    m_saveAsAct->setShortcut(QKeySequence::SaveAs);
    m_saveAsAct->setIconVisibleInMenu(false);

    m_createProjectAct = new QAction(QIcon(":/icons/icons/tools/new-file.png"), tr("Новый проект"), this);
//    m_createProjectAct->setShortcut(QKeySequence::New);
    m_createProjectAct->setIconVisibleInMenu(false);

    m_openProjectAct = new QAction(QIcon(":/icons/icons/tools/open.png"), tr("Открыть проект"), this);
    m_openProjectAct->setShortcut(QKeySequence::Open);
    m_openProjectAct->setIconVisibleInMenu(false);

    m_closeProjectAct = new QAction(tr("Закрыть проект"), this);
    m_closeProjectAct->setIconVisibleInMenu(false);
    m_closeProjectAct->setEnabled(false);

    m_createFileAct = new QAction(QIcon(":/icons/icons/tools/new-file.png"), tr("Новый файл"), this);
    m_createFileAct->setShortcut(QKeySequence::New);
    m_createFileAct->setIconVisibleInMenu(false);
    m_createFileAct->setEnabled(false);

    m_addFileAct = new QAction(tr("Добавить файл"), this);
    m_addFileAct->setIconVisibleInMenu(false);
    m_addFileAct->setEnabled(false);

    m_pasteAct = new QAction(QIcon(":/icons/icons/tools/paste.png"), tr("Вставить"), this);
    m_pasteAct->setShortcut(QKeySequence::Paste);
    m_pasteAct->setIconVisibleInMenu(false);

    m_copyAct = new QAction(QIcon(":/icons/icons/tools/copy.png"), tr("Копировать"), this);
    m_copyAct->setShortcut(QKeySequence::Copy);
    m_copyAct->setIconVisibleInMenu(false);

    m_findAct = new QAction(QIcon(":/icons/icons/tools/search.png"), tr("Поиск"), this);
    m_findAct->setShortcut(QKeySequence::Find);
    m_findAct->setIconVisibleInMenu(false);

    //!Compilation actions
    m_buildAct = new QAction(QIcon(":/icons/icons/tools/compile.png"), tr("Скомпилировать"), this);
    m_buildAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    m_buildAct->setIconVisibleInMenu(false);

    m_uploadAndRunAct = new QAction(QIcon(":/icons/icons/tools/upload.png"), tr("Отправить программу"), this);
    m_uploadAndRunAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    m_uploadAndRunAct->setIconVisibleInMenu(false);

    m_stopAppAct = new QAction(QIcon(":/icons/icons/tools/stop.png"), tr("Остановить программу"), this);
    m_stopAppAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    m_stopAppAct->setIconVisibleInMenu(false);

    m_runAppAct = new QAction(QIcon(":/icons/icons/tools/start.png"), tr("Запустить программу"), this);
    m_runAppAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    m_runAppAct->setIconVisibleInMenu(false);

    m_showSettingsAct = new QAction(QIcon(":/icons/actions/preferences-desktop.png"), tr("Настройки"), this);
    m_showSettingsAct->setIconVisibleInMenu(false);

    m_openHelpAct = new QAction(tr("Справка"), this);
    m_openHelpAct->setShortcut(QKeySequence(Qt::Key_F1));

    m_edisonCompileAct = new QAction(tr("Робот"), this);
    m_edisonCompileAct->setCheckable(true);
    m_edisonCompileAct->setChecked(true);
    m_mingwCompileAct = new QAction(tr("Компьютер"), this);
    m_mingwCompileAct->setCheckable(true);
}

void MainWindow::createToolBars()
{

    QFile toolBar(":/dark/styles/toolbar.css");
    toolBar.open(QFile::ReadOnly);
    QString styleSheet = toolBar.readAll();
    toolBar.close();

    m_toolBar = addToolBar(tr("Панель устройств"));
    m_toolBar->setStyleSheet(styleSheet);
    m_toolBar->setMovable(false);
    m_toolBar->setObjectName("ToolBar");
    m_viewMenu->addAction(m_toolBar->toggleViewAction());
    m_toolBar->setIconSize(QSize(22, 22));

    m_toolBar->addAction(m_createFileAct);
//    m_toolBar->addAction(m_addFileAct);
    m_toolBar->addAction(m_saveAct);
    m_toolBar->addAction(m_saveAsAct);
    m_toolBar->addSeparator();

    m_toolBar->addAction(m_copyAct);
    m_toolBar->addAction(m_pasteAct);
    m_toolBar->addAction(m_findAct);
    m_toolBar->addAction(m_undoAct);
    m_toolBar->addAction(m_redoAct);
    m_toolBar->addSeparator();

    m_toolBar->addAction(m_buildAct);
    m_toolBar->addAction(m_uploadAndRunAct);
    m_toolBar->addAction(m_runAppAct);
    m_toolBar->addAction(m_stopAppAct);
}

void MainWindow::createMenus()
{
    QFile fileMenuStyle(":/dark/styles/filemenu.css");
    fileMenuStyle.open(QFile::ReadOnly);
    QString styleSheet = fileMenuStyle.readAll();
    fileMenuStyle.close();
    m_fileMenu = menuBar()->addMenu(tr("&Файл"));
    menuBar()->setStyleSheet(styleSheet);

//    m_fileMenu->addAction(m_openFileAct);
    m_fileMenu->addAction(m_createProjectAct);
    m_fileMenu->addAction(m_openProjectAct);
    m_fileMenu->addAction(m_closeProjectAct);
    m_fileMenu->addAction(m_createFileAct);
    m_fileMenu->addAction(m_addFileAct);
    m_fileMenu->addAction(m_saveAct);
//    m_fileMenu->addAction(m_saveAsAct);

    m_editMenu = menuBar()->addMenu(tr("&Правка"));
    m_editMenu->addAction(m_redoAct);
    m_editMenu->addAction(m_undoAct);
    m_editMenu->addAction(m_copyAct);
    m_editMenu->addAction(m_pasteAct);
    m_editMenu->addAction(m_findAct);

    m_compilationMenu = menuBar()->addMenu(tr("&Компиляция"));
    m_compilationMenu->addAction(m_buildAct);
    m_compilationMenu->addAction(m_uploadAndRunAct);
    m_compilationMenu->addAction(m_edisonCompileAct);
    m_compilationMenu->addAction(m_mingwCompileAct);

    m_settingsMenu = menuBar()->addMenu(tr("&Настройки"));
    m_settingsMenu->addAction(m_showSettingsAct);

    m_viewMenu = menuBar()->addMenu(tr("Окно"));

    m_helpMenu = menuBar()->addMenu(tr("Справка"));
    m_helpMenu->addAction(m_openHelpAct);
}

void MainWindow::createDockWindows()
{
    QFile dockSheet(":/dark/styles/dock.css");
    dockSheet.open(QFile::ReadOnly);
    QString styleSheet = dockSheet.readAll();
    dockSheet.close();

    QDockWidget *dock = new QDockWidget(tr("Консоль"), this);
    dock->setObjectName("ConsoleDockWidget");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
                          | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dock->setWidget(m_roboIdeConsole);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    m_viewMenu->addAction(dock->toggleViewAction());
    dock->setStyleSheet(styleSheet);

    dock = new QDockWidget(tr("Устройства"), this);
    dock->setObjectName("ConnectedDevicesListDockWidget");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
                          | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    dock->setWidget(m_connectedDevicesList);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    m_viewMenu->addAction(dock->toggleViewAction());
    dock->setStyleSheet(styleSheet);
    dock->toggleViewAction()->setDisabled(true);
    dock->setVisible(false);

    dock = new QDockWidget(tr("Справка"), this);
    dock->setObjectName("HelpWidget");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
                          | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dock->setWidget(m_helpWidget);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    m_toggleHelpVisibilityAct = dock->toggleViewAction();
    m_viewMenu->addAction(m_toggleHelpVisibilityAct);
    dock->setStyleSheet(styleSheet);
    dock->setVisible(false);

    dock = new QDockWidget(tr("Проект"), this);
    dock->setObjectName("ProjectTreeWidget");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
                          | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dock->setWidget(m_projectTree);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    m_viewMenu->addAction(dock->toggleViewAction());
    dock->setStyleSheet(styleSheet);
    dock->setVisible(true);
}

void MainWindow::connectActionsToSlots()
{
    QObject::connect(m_createProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectClose()));
    QObject::connect(m_createProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectCreateDialog()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectCreated(QString)), m_projectTree, SLOT(loadProject(QString)));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectCreated(QString)), this, SLOT(onProjectCreated()));
    QObject::connect(m_openProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectClose()));
    QObject::connect(m_openProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectOpenDialog()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectOpened(QString)), m_projectTree, SLOT(loadProject(QString)));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectOpened(QString)), this, SLOT(onProjectOpened()));
    QObject::connect(m_closeProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectClose()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectClosed()), m_projectTree, SLOT(closeProject()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectClosed()), this, SLOT(onProjectClosed()));

    QObject::connect(m_createFileAct, SIGNAL(triggered(bool)), this, SLOT(fileCreateDialog()));
    QObject::connect(m_addFileAct, SIGNAL(triggered(bool)), this, SLOT(fileAddDialog()));
    QObject::connect(m_saveAct, SIGNAL(triggered(bool)), this, SLOT(saveFile()));
    QObject::connect(m_saveAsAct, SIGNAL(triggered(bool)), this, SLOT(saveFileAs()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileOpened(QString, QString)), m_roboIdeTextEdit, SLOT(showContent(QString, QString)));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileSaved(QString)), m_roboIdeTextEdit, SLOT(onFileSaved(QString)));

//    QObject::connect(m_roboIdeTextEdit, SIGNAL(fileModified()), this);
    QObject::connect(m_openHelpAct, SIGNAL(triggered(bool)), this, SLOT(openHelp()));
    QObject::connect(m_redoAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(redo()));
    QObject::connect(m_undoAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(undo()));
    QObject::connect(m_copyAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(copy()));
    QObject::connect(m_pasteAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(paste()));
     //QObject::connect(m_findAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(()));
    QObject::connect(m_buildAct, SIGNAL(triggered(bool)), this, SLOT(generateMakeFile()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(makeFileGenerated()), this, SLOT(runCompilation()));
    QObject::connect(m_sourceCompiller, SIGNAL(onCompilationOutput(QString)), m_roboIdeConsole, SLOT(append(QString)));
    QObject::connect(m_sourceCompiller, SIGNAL(finished()), this, SLOT(compilationFinished()));
    QObject::connect(m_uploadAndRunAct, SIGNAL(triggered(bool)), this, SLOT(uploadAndRun()));
    QObject::connect(m_showSettingsAct, SIGNAL(triggered(bool)), m_settingsWidget, SLOT(show()));
    QObject::connect(m_wifiConnection, SIGNAL(onExecOutput(QString)), m_roboIdeConsole, SLOT(append(QString)));
    QObject::connect(m_runAppAct, SIGNAL(triggered(bool)), this, SLOT(runApp()));
    QObject::connect(m_stopAppAct, SIGNAL(triggered(bool)), this, SLOT(killApp()));
    QObject::connect(m_edisonCompileAct, SIGNAL(triggered(bool)), this, SLOT(switchCompilationTargetToEdison()));
    QObject::connect(m_mingwCompileAct, SIGNAL(triggered(bool)), this, SLOT(switchCompilationTargetToDesktop()));

    QObject::connect(m_localApp, SIGNAL(readyReadStandardError()), SLOT(processOutReceived()));
    QObject::connect(m_localApp, SIGNAL(readyReadStandardOutput()), SLOT(processOutReceived()));
    QObject::connect(m_localApp, SIGNAL(readyRead()), SLOT(processOutReceived()));
}

