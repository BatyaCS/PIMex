#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "settingsWindow.h"
#include "workspace.h"
#include "tasksListSpace.h"
#include "tasks.h"
#include "TaskWorkspace.h"
#include "programState.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void saveInterfaceSettings(bool autoSave, int level);

private slots:

    void updateObserverMainThread();
    void interfaceSettings();
    void onSaveClicked();
    void onLoadClicked();

protected:
    void resizeEvent(QResizeEvent*) override;
private:

    void createMenu(void);

    workspace *_workspace;
    tasksListSpace *_tasksListSpace;
    TaskWorkspace *_taskWorkspace;
    Tasks _tasks;
    ProgramState *_state;

    settingsWindow* _settingsWindow;

    QMenu *_file;
    QMenu *_settings;
    QAction *settings;
    QMenu *_quit;
    QAction *quit;
    QAction *save;
    QAction *load;

    Ui::MainWindow *ui;
    QTimer *mainThreadTmr;
    QVector<QString> fileNames;
};
