#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "widgets/roboidetexteditor.h"
#include "widgets/roboideconsole.h"
#include "widgets/projecttreewidget.h"
#include "logic/sourcecompiler.h"
#include "widgets/connecteddeviceslist.h"
#include "widgets/settingswidget.h"
#include "widgets/cameraswidget.h"
#include "filetransfer/wificonnection.h"
#include "widgets/helpwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void runCompilation();
    void compilationFinished();
    void uploadAndRun();
    void runApp();
    void killApp();
    void openFileOrProject();
    void openHelp();
    void switchCompilationTargetToEdison();
    void switchCompilationTargetToDesktop();
    void processOutReceived();
    void projectCreateDialog();
//    void projectOpenDialog();
    void fileAddDialog();

private:
    void createMenus();
    void createActions();
    void createToolBars();
    void createDockWindows();
    void connectActionsToSlots();

    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;
    QMenu *m_compilationMenu;
    QMenu *m_settingsMenu;

    QToolBar *m_toolBar;

    QAction *m_createProjectAct;
    QAction *m_openProjectAct;
    QAction *m_createFileAct;
    QAction *m_saveAct;
    QAction *m_saveAsAct;
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_copyAct;
    QAction *m_pasteAct;
    QAction *m_aboutAct;
    QAction *m_openFileAct;
    QAction *m_findAct;
    QAction *m_buildAct;
    QAction *m_uploadAndRunAct;
    QAction *m_showSettingsAct;
    QAction *m_runAppAct;
    QAction *m_stopAppAct;
    QAction *m_openHelpAct;
    QAction *m_edisonCompileAct;
    QAction *m_mingwCompileAct;
    QAction *m_toggleHelpVisibilityAct;

    RoboIdeTextEditor *m_roboIdeTextEdit;
    RoboIdeConsole *m_roboIdeConsole;
    SourceCompiler *m_sourceCompiller;
    ConnectedDevicesList *m_connectedDevicesList;
    SettingsWidget *m_settingsWidget;
    WiFiConnection *m_wifiConnection;
    QProcess *m_localApp;
    HelpWidget *m_helpWidget;
    ProjectTree *m_projectTree;
};

#endif // MAINWINDOW_H
