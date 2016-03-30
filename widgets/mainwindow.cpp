#include "mainwindow.h"
#include "roboideconsole.h"
#include "wifipasswordwidget.h"
#include "settingsmanager.h"
#include "helpwidget.h"
#include "project.h"
#include "projectmanager.h"

#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QClipboard>
#include <QDockWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QCloseEvent>

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
      m_projectTree(new ProjectTree(this)),
      m_ftpWidget(new FtpWidget(this)),
      m_projectTreeContextMenu(new QMenu(this)),
      m_projectCreateDialog(new ProjectCreateDialog(this)),
      m_fileCreateDialog(new FileCreateDialog(this)),
      m_wifiConnectionThread(new QThread(this)),
      m_inCombinedRunState(false)
{
    setCentralWidget(m_roboIdeTextEdit);
    createActions();
    createMenus();
    createToolBars();
    createDockWindows();
    connectActionsToSlots();

    QByteArray state = SettingsManager::instance().mainWindowState();
    if (state.size() != 0) {
        restoreState(state);
    }

    QFile mainWindowSheet(":/dark/styles/mainwindow.css");
    mainWindowSheet.open(QFile::ReadOnly);
    QString styleSheet = mainWindowSheet.readAll();
    setStyleSheet(styleSheet);
    mainWindowSheet.close();

    showMaximized();
    projectCreateDialog();

    m_wifiConnection->moveToThread(m_wifiConnectionThread);
    m_wifiConnectionThread->start();

}

MainWindow::~MainWindow()
{
    QByteArray state = saveState();
    SettingsManager::instance().setMainWindowState(state);
    m_wifiConnectionThread->terminate();
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    event->ignore();
    if (m_roboIdeTextEdit->isModified()){
        saveFilePromt();
    }
    event->accept();
}

void MainWindow::runCompilation()
{
    if (m_edisonCompileAct->isChecked()) {
        SETTINGS.setCurrentTarget(SettingsManager::TARGET::EDISON);
    }
    else if (m_mingwCompileAct->isChecked()) {
        SETTINGS.setCurrentTarget(SettingsManager::TARGET::MINGW);
    }
    m_buildAct->setEnabled(false);
    if (m_roboIdeTextEdit->isModified()) {
        saveFilePromt();
    }
    if (ProjectManager::instance().isProjectOpened()) {
        ProjectManager::instance().generateMakeFile();
    }
    m_sourceCompiller->onRunCompilation(QString());
}

void MainWindow::compilationFinished()
{
    m_buildAct->setEnabled(true);
    if (m_inCombinedRunState) {
        if (m_sourceCompiller->isCompiled()) {
            bool isUploadOk = true;
            if (m_edisonCompileAct->isChecked()) {
                isUploadOk = uploadApp();
            }
            if (isUploadOk and runApp()) {
                m_stopAppAct->setEnabled(true);
            }
        }
        m_combinedRunAct->setEnabled(true);
        m_inCombinedRunState = false;
    }
}

bool MainWindow::uploadApp()
{
    emit sendFile(m_sourceCompiller->pathToBinary());
    return true;

}

bool MainWindow::runApp()
{
    bool isOK = false;
    if (m_edisonCompileAct->isChecked()) {
        m_runAppAct->setDisabled(true);
        emit startApp();
        m_runAppAct->setEnabled(true);
        return true;
    }
    else if (m_mingwCompileAct->isChecked()) {
        if (m_localApp->isOpen()) {
            m_localApp->kill();
        }
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", SettingsManager::instance().mingwBinarysPath());
        m_localApp->setProcessEnvironment(env);
        m_localApp->start(m_sourceCompiller->pathToBinary());
        if (m_localApp->state() != QProcess::NotRunning) {
            isOK = true;
        }
    }

    if (isOK) {
        m_roboIdeConsole->appendMessage("Программа запущена!\n");
    }
    else {
        m_roboIdeConsole->appendMessage("Во время запуска программы произошла ошибка.\n", true);
        if (m_mingwCompileAct->isChecked()) {
            m_roboIdeConsole->appendMessage(m_localApp->errorString(), true);
        }
        else if (m_edisonCompileAct->isChecked()) {
            m_roboIdeConsole->appendMessage("Ошибка передачи. Проверьте соединение с аппаратом.\n", true);
        }
    }
    return isOK;
}

void MainWindow::onEndFileUpload(bool isOk)
{
    if (isOk) {
        m_roboIdeConsole->appendMessage("Программа отправлена.\n");
    }
    else {
        m_roboIdeConsole->appendMessage("Ошибка передачи. Программа не может быть отправлена. Проверьте соединение с аппаратом.\n", true);
    }
}

void MainWindow::onAppKilled(bool isOk)
{
    if (isOk) {
        m_roboIdeConsole->appendMessage("Программа остановлена!\n");
    }
    else {
        m_roboIdeConsole->appendMessage("Ошибка передачи. Программа не может быть остановлена. Проверьте соединение с аппаратом.\n", true);
    }
}

void MainWindow::onAppStarted(bool isOk)
{
    if (isOk) {
        m_roboIdeConsole->appendMessage("Программа запущена!\n");
    }
    else {
        m_roboIdeConsole->appendMessage("Ошибка передачи. Программа не может быть запущена. Проверьте соединение с аппаратом.\n", true);
    }
}

void MainWindow::combinedRunApp()
{
    m_inCombinedRunState = true;
    m_combinedRunAct->setDisabled(true);
    runCompilation();
}

bool MainWindow::killApp()
{
    bool isOk = false;
    if (m_edisonCompileAct->isChecked()) {
        m_stopAppAct->setDisabled(true);
        emit stopApp();
        m_stopAppAct->setEnabled(true);
        return true;
    }
    else if (m_mingwCompileAct->isChecked()) {
        if (m_localApp->isOpen()) {
            m_localApp->kill();
            isOk = true;
        }

    }
    if (isOk) {
        m_roboIdeConsole->appendMessage("Программа остановлена!\n");
    }
    else {
        if (m_mingwCompileAct->isChecked()) {
            m_roboIdeConsole->appendMessage("Программа не была запущена. Нечего останавливать.\n");
        }
        else if (m_edisonCompileAct->isChecked()) {
            m_roboIdeConsole->appendMessage("Ошибка передачи. Проверьте соединение с аппаратом.\n", true);
        }
    }
    m_stopAppAct->setEnabled(false);
    return true;
}

void MainWindow::projectCreateDialog()
{
    m_projectCreateDialog->exec();
}

void MainWindow::projectCreate(const QString &path, const QString &name)
{
    projectClose();
    ProjectManager::instance().createProject(path, name);
}

void MainWindow::projectOpenDialog()
{
    QString filePath = QFileDialog::getOpenFileName(
                        this,
                        tr("Открыть проект"),
                        ProjectManager::instance().projectsRoot(),
                        "MUR Projects (" + Project::projectFileName + ")"
                        );
    if (!filePath.isNull()) {
        projectClose();
        ProjectManager::instance().openProject(filePath);
    }
}

void MainWindow::fileCreateDialog()
{
    m_fileCreateDialog->exec();
}

void MainWindow::filesCreate(const QStringList &fileNames)
{
    for (QString fileName: fileNames) {
        ProjectManager::instance().createFile(fileName);
    }
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

void MainWindow::projectClose()
{
    if (m_roboIdeTextEdit->isModified()) {
        saveFilePromt();
    }
    m_roboIdeTextEdit->clear();
    m_roboIdeTextEdit->setModified(false);
    ProjectManager::instance().closeProject();
}


void MainWindow::openHelp()
{
    this->m_toggleHelpVisibilityAct->trigger();
}

void MainWindow::onProjectOpened()
{
    enableProject();
}

void MainWindow::onProjectClosed()
{
    disableProject();
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
    // saves as .cpp if no file extension specified
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

void MainWindow::projectContextMenu(const QPoint &point, const QString &fileName)
{
//    qDebug() << point << fileName << fileName.isEmpty();
    if (!fileName.isEmpty()) {
        m_deleteFileAct->setEnabled(true);
        m_renameFileAct->setEnabled(true);
        m_deleteFileAct->setData(fileName);
        m_renameFileAct->setData(fileName);
    } else {
        m_deleteFileAct->setEnabled(false);
        m_renameFileAct->setEnabled(false);
    }
    m_projectTreeContextMenu->exec(point);
}

void MainWindow::treeContextMenuTriggered(QAction *action)
{
    if (action == m_deleteFileAct) {
        QMessageBox::StandardButton reply;
        QString fileName = action->data().toString();
        reply = QMessageBox::question(this,
                                      "Удалить?", "Вы уверены, что хотите удалить " + fileName + " ?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            ProjectManager::instance().deleteFile(fileName);
        }
    }
    else if(action == m_renameFileAct) {
        bool ok;
        QString fileName = action->data().toString();
        QString newFileName = QInputDialog::getText(this, "Переименовать",
                                                "Новое имя", QLineEdit::Normal,
                                                fileName, &ok,
                                                Qt::Popup);
        if (ok && !newFileName.isEmpty()) {
            ProjectManager::instance().renameFile(fileName, newFileName);
        }
    }
}

void MainWindow::switchCompilationTargetToEdison()
{
    m_mingwCompileAct->setChecked(false);
    m_edisonCompileAct->setChecked(true);
    SETTINGS.setCurrentTarget(SettingsManager::TARGET::EDISON);
    if (ProjectManager::instance().isProjectOpened()) {
        ProjectManager::instance().generateMakeFile();
    }
}

void MainWindow::switchCompilationTargetToDesktop()
{
    m_edisonCompileAct->setChecked(false);
    m_mingwCompileAct->setChecked(true);
    SETTINGS.setCurrentTarget(SettingsManager::TARGET::MINGW);
    if (ProjectManager::instance().isProjectOpened()) {
        ProjectManager::instance().generateMakeFile();
    }
}

void MainWindow::processOutReceived()
{
    QString str = m_localApp->readAllStandardOutput().simplified();
    if (str != "") {
        m_roboIdeConsole->appendMessage(str + "\n");
    }
    str = m_localApp->readAllStandardError().simplified();
    if (str != "") {
        m_roboIdeConsole->appendMessage(str + "\n", true);
    }
    str = m_localApp->readAll().simplified();
    if (str != "") {
        m_roboIdeConsole->appendMessage(str + "\n");
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

    m_createProjectAct = new QAction(tr("Новый проект"), this);
    m_createProjectAct->setIconVisibleInMenu(false);

    m_openProjectAct = new QAction(QIcon(":/icons/icons/tools/open.png"), tr("Открыть проект"), this);
    m_openProjectAct->setShortcut(QKeySequence::Open);
    m_openProjectAct->setIconVisibleInMenu(false);

    m_closeProjectAct = new QAction(tr("Закрыть проект"), this);
    m_closeProjectAct->setIconVisibleInMenu(false);

    m_createFileAct = new QAction(QIcon(":/icons/icons/tools/new-file.png"), tr("Новый файл"), this);
    m_createFileAct->setShortcut(QKeySequence::New);
    m_createFileAct->setIconVisibleInMenu(false);

    m_addFileAct = new QAction(tr("Добавить файл"), this);
    m_addFileAct->setIconVisibleInMenu(false);

    m_deleteFileAct = new QAction(tr("Удалить файл"), this);
    m_deleteFileAct->setIconVisibleInMenu(false);

    m_renameFileAct = new QAction(tr("Переименовать файл"), this);
    m_renameFileAct->setIconVisibleInMenu(false);

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

    m_uploadAct = new QAction(QIcon(":/icons/icons/tools/upload.png"), tr("Отправить программу"), this);
    m_uploadAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    m_uploadAct->setIconVisibleInMenu(false);

    m_stopAppAct = new QAction(QIcon(":/icons/icons/tools/stop.png"), tr("Остановить программу"), this);
    m_stopAppAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    m_stopAppAct->setIconVisibleInMenu(false);

    m_runAppAct = new QAction(QIcon(":/icons/icons/tools/start.png"), tr("Запустить программу"), this);
    m_runAppAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    m_runAppAct->setIconVisibleInMenu(false);

    m_combinedRunAct = new QAction(QIcon(":/icons/icons/tools/start.png"), tr("Собрать и выполнить программу"), this);
    m_combinedRunAct->setIconVisibleInMenu(false);

    m_showSettingsAct = new QAction(QIcon(":/icons/actions/preferences-desktop.png"), tr("Настройки"), this);
    m_showSettingsAct->setIconVisibleInMenu(false);

    m_openHelpAct = new QAction(tr("Справка"), this);
    m_openHelpAct->setShortcut(QKeySequence(Qt::Key_F1));

    m_showFtpAct = new QAction(tr("FTP подключение"), this);

    m_edisonCompileAct = new QAction(tr("Робот"), this);
    m_edisonCompileAct->setCheckable(true);
    m_edisonCompileAct->setChecked(true);
    m_mingwCompileAct = new QAction(tr("Компьютер"), this);
    m_mingwCompileAct->setCheckable(true);

    disableProject();
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

    m_toolBar->addAction(m_combinedRunAct);
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
    m_fileMenu->addAction(m_saveAsAct);

    m_editMenu = menuBar()->addMenu(tr("&Правка"));
    m_editMenu->addAction(m_redoAct);
    m_editMenu->addAction(m_undoAct);
    m_editMenu->addAction(m_copyAct);
    m_editMenu->addAction(m_pasteAct);
    m_editMenu->addAction(m_findAct);

    m_projectTreeContextMenu->addAction(m_createFileAct);
    m_projectTreeContextMenu->addAction(m_addFileAct);
    m_projectTreeContextMenu->addAction(m_deleteFileAct);
    m_projectTreeContextMenu->addAction(m_renameFileAct);

    m_compilationMenu = menuBar()->addMenu(tr("&Компиляция"));
    m_compilationMenu->addAction(m_buildAct);
    m_compilationMenu->addAction(m_uploadAct);
    m_compilationMenu->addAction(m_edisonCompileAct);
    m_compilationMenu->addAction(m_mingwCompileAct);

    m_settingsMenu = menuBar()->addMenu(tr("&Настройки"));
    m_settingsMenu->addAction(m_showSettingsAct);

    m_viewMenu = menuBar()->addMenu(tr("Окно"));
    m_viewMenu->addAction(m_showFtpAct);
    m_viewMenu->addSeparator();

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
    dock->setFeatures(QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    m_viewMenu->addAction(dock->toggleViewAction());
    dock->setStyleSheet(styleSheet);

    dock = new QDockWidget(tr("Устройства"), this);
    dock->setObjectName("ConnectedDevicesListDockWidget");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
                          | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    dock->setWidget(m_connectedDevicesList);
    dock->setFeatures(QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    m_viewMenu->addAction(dock->toggleViewAction());
    dock->setStyleSheet(styleSheet);

    dock = new QDockWidget(tr("Справка"), this);
    dock->setObjectName("HelpWidget");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea
                          | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dock->setWidget(m_helpWidget);
    dock->setFeatures(QDockWidget::DockWidgetClosable);

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
    dock->setFeatures(QDockWidget::DockWidgetClosable);

    addDockWidget(Qt::LeftDockWidgetArea, dock);
    m_viewMenu->addAction(dock->toggleViewAction());
    dock->setStyleSheet(styleSheet);
    dock->setVisible(true);
}

void MainWindow::enableProject()
{
    m_roboIdeTextEdit->setReadOnly(false);
    m_addFileAct->setEnabled(true);
    m_createFileAct->setEnabled(true);
    m_closeProjectAct->setEnabled(true);
    m_deleteFileAct->setEnabled(true);
    m_renameFileAct->setEnabled(true);
    m_saveAct->setEnabled(true);
    m_saveAsAct->setEnabled(true);
    m_buildAct->setEnabled(true);
    m_uploadAct->setEnabled(true);
    m_runAppAct->setEnabled(true);
    m_stopAppAct->setEnabled(false);
}

void MainWindow::disableProject()
{
    m_roboIdeTextEdit->setReadOnly(true);
    m_addFileAct->setEnabled(false);
    m_createFileAct->setEnabled(false);
    m_closeProjectAct->setEnabled(false);
    m_deleteFileAct->setEnabled(false);
    m_renameFileAct->setEnabled(false);
    m_saveAct->setEnabled(false);
    m_saveAsAct->setEnabled(false);
    m_buildAct->setEnabled(false);
    m_uploadAct->setEnabled(false);
    m_runAppAct->setEnabled(false);
    m_stopAppAct->setEnabled(false);
}

void MainWindow::connectActionsToSlots()
{
    QObject::connect(m_createProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectCreateDialog()));
    QObject::connect(m_projectCreateDialog, SIGNAL(createProjectConfirmed(QString, QString)), this, SLOT(projectCreate(QString,QString)));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectCreated(QString)), m_projectTree, SLOT(loadProject()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectCreated(QString)), this, SLOT(onProjectOpened()));

    QObject::connect(m_openProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectOpenDialog()));
    QObject::connect(m_fileCreateDialog, SIGNAL(createFileConfirmed(QStringList)), this, SLOT(filesCreate(QStringList)));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectOpened(QString)), m_projectTree, SLOT(loadProject()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectOpened(QString)), this, SLOT(onProjectOpened()));

    QObject::connect(m_closeProjectAct, SIGNAL(triggered(bool)), this, SLOT(projectClose()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectClosed()), m_projectTree, SLOT(closeProject()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(projectClosed()), this, SLOT(onProjectClosed()));

    QObject::connect(m_createFileAct, SIGNAL(triggered(bool)), this, SLOT(fileCreateDialog()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileCreated(QString)), m_projectTree, SLOT(loadProject()));
    QObject::connect(m_addFileAct, SIGNAL(triggered(bool)), this, SLOT(fileAddDialog()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileAdded()), m_projectTree, SLOT(loadProject()));
    QObject::connect(m_saveAct, SIGNAL(triggered(bool)), this, SLOT(saveFile()));
    QObject::connect(m_saveAsAct, SIGNAL(triggered(bool)), this, SLOT(saveFileAs()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileSaved(QString)), m_roboIdeTextEdit, SLOT(onFileSaved(QString)));
    QObject::connect(m_projectTreeContextMenu, SIGNAL(triggered(QAction*)), this, SLOT(treeContextMenuTriggered(QAction*)));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileDeleted(QString)), m_projectTree, SLOT(loadProject()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileRenamed()), m_projectTree, SLOT(loadProject()));

    QObject::connect(m_projectTree, SIGNAL(fileSelected(QString)), this, SLOT(openFile(QString)));
    QObject::connect(&ProjectManager::instance(), SIGNAL(fileOpened(QString, QString)), m_roboIdeTextEdit, SLOT(showContent(QString, QString)));
    QObject::connect(m_projectTree, SIGNAL(projectContextMenu(QPoint, QString)), this, SLOT(projectContextMenu(QPoint,QString)));

    QObject::connect(m_openHelpAct, SIGNAL(triggered(bool)), this, SLOT(openHelp()));
    QObject::connect(m_redoAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(redo()));
    QObject::connect(m_undoAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(undo()));
    QObject::connect(m_copyAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(copy()));
    QObject::connect(m_pasteAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(paste()));
     //QObject::connect(m_findAct, SIGNAL(triggered(bool)), m_roboIdeTextEdit, SLOT(()));
    QObject::connect(m_buildAct, SIGNAL(triggered(bool)), this, SLOT(runCompilation()));
    QObject::connect(&ProjectManager::instance(), SIGNAL(makeFileGenerated()), this, SLOT(runCompilation()));
    QObject::connect(m_sourceCompiller, SIGNAL(onCompilationOutput(QString, bool)), m_roboIdeConsole, SLOT(appendMessage(QString, bool)));
    QObject::connect(m_sourceCompiller, SIGNAL(finished()), this, SLOT(compilationFinished()));
    QObject::connect(m_uploadAct, SIGNAL(triggered(bool)), this, SLOT(uploadApp()));
    QObject::connect(m_showSettingsAct, SIGNAL(triggered(bool)), m_settingsWidget, SLOT(show()));
    QObject::connect(m_wifiConnection, SIGNAL(onExecOutput(QString)), m_roboIdeConsole, SLOT(appendMessage(QString)));
    QObject::connect(m_runAppAct, SIGNAL(triggered(bool)), this, SLOT(runApp()));
    QObject::connect(m_stopAppAct, SIGNAL(triggered(bool)), this, SLOT(killApp()));
    QObject::connect(m_combinedRunAct, SIGNAL(triggered(bool)), this, SLOT(combinedRunApp()));
    QObject::connect(m_edisonCompileAct, SIGNAL(triggered(bool)), this, SLOT(switchCompilationTargetToEdison()));
    QObject::connect(m_mingwCompileAct, SIGNAL(triggered(bool)), this, SLOT(switchCompilationTargetToDesktop()));

    QObject::connect(m_localApp, SIGNAL(readyReadStandardError()), SLOT(processOutReceived()));
    QObject::connect(m_localApp, SIGNAL(readyReadStandardOutput()), SLOT(processOutReceived()));
    QObject::connect(m_localApp, SIGNAL(readyRead()), SLOT(processOutReceived()));

    QObject::connect(m_showFtpAct, SIGNAL(triggered(bool)), m_ftpWidget, SLOT(exec()));

    QObject::connect(m_wifiConnection, SIGNAL(appKilled(bool)), this, SLOT(onAppKilled(bool)));
    QObject::connect(m_wifiConnection, SIGNAL(appSend(bool)), this, SLOT(onEndFileUpload(bool)));
    QObject::connect(m_wifiConnection, SIGNAL(appStarted(bool)), this, SLOT(onAppStarted(bool)));

    QObject::connect(m_wifiConnection, SIGNAL(statusUpdated(StatusInfo)), m_connectedDevicesList, SLOT(updateDevices(StatusInfo)));

    QObject::connect(this, SIGNAL(startApp()), m_wifiConnection, SLOT(runApp()));
    QObject::connect(this, SIGNAL(stopApp()), m_wifiConnection, SLOT(killApp()));
    QObject::connect(this, SIGNAL(sendFile(QString)), m_wifiConnection, SLOT(send(QString)));
}

