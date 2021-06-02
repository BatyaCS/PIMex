#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _taskWorkspace = new TaskWorkspace(_tasks, this->ui->centralwidget);
    _tasksListSpace = new tasksListSpace(_tasks, *_taskWorkspace, this->ui->centralwidget);
    _workspace = new workspace(_tasks, *_tasksListSpace, this->ui->centralwidget);

    _settingsWindow = new settingsWindow(*this);
    _state = new ProgramState(_tasks, *_tasks.get_observerNF(), *_workspace);

    setWindowTitle("Персональный информационный менеджер");

    createMenu();
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent* event)
{
    //qDebug() << "mainwindow resize";
    QRect workspace_rect(0, 0, ui->centralwidget->size().width()/3, ui->centralwidget->size().height());
    _workspace->setGeometry(workspace_rect);
    _workspace->resizeEvent(event);

    QRect tasks_rect(ui->centralwidget->geometry().width()/3, 0, ui->centralwidget->geometry().width()/3, ui->centralwidget->geometry().height());
    _tasksListSpace->setGeometry(tasks_rect);
    //qDebug() << tasks_rect;
    _tasksListSpace->resizeEvent(event);

    QRect task_workspace_rect((ui->centralwidget->geometry().width()/3) * 2, 0, ui->centralwidget->geometry().width()/3, ui->centralwidget->geometry().height());
    _taskWorkspace->setGeometry(task_workspace_rect);
    _taskWorkspace->resizeEvent(event);
    //ui->tasks_workspace->setGeometry(tasks_workspace_rect);


}

void MainWindow::createMenu()
{
    quit = new QAction("&Выход", this);
    settings = new QAction("&Настройки модуля контроля файлов", this);
    save = new QAction("&Сохранить задачи", this);
    load = new QAction("&Загрузить задачи", this);


    _file = menuBar()->addMenu("&Файл");
    _settings = menuBar()->addMenu("&Настройки");
    //_quit = menuBar()->addMenu("&Выход");


    //_quit->addAction(quit);
    _file->addAction(save);
    _file->addAction(load);
    _file->addAction(quit);
    _settings->addAction(settings);
    connect(quit, &QAction::triggered, qApp, QApplication::quit);
    connect(settings, SIGNAL(triggered()), this, SLOT(interfaceSettings()));
    connect(load, SIGNAL(triggered()), this, SLOT(onLoadClicked()));
    connect(save, SIGNAL(triggered()), this, SLOT(onSaveClicked()));
}

void MainWindow::updateObserverMainThread()
{
    //observer.mainThread(&fileNames, "E:/!winda/Ucheba/Ucheba_7sem/Arseniev/KPProj/build-KPProj-Desktop_Qt_5_14_2_MSVC2017_64bit-Debug/debug");
}

void MainWindow::interfaceSettings()
{
    QDesktopWidget desktop;
    QRect rect = desktop.availableGeometry(this);
    QPoint center = rect.center();
    int x = center.x() - (width()/2);
    int y = center.y() - (height()/2);
    center.setX(x);
    center.setY(y);

    _settingsWindow->move(center);
    _settingsWindow->show();
}

void MainWindow::onSaveClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Выберите файл для сохранения"), "", tr("*.save"));
    _state->save(fileName);
}

void MainWindow::onLoadClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Выберите файл для загрузки"), "", tr("*.save"));
    _state->load(fileName);
}

void MainWindow::saveInterfaceSettings(bool autoSave, int level)
{
    _tasks.set_settings(ObserverNF::Settings(autoSave, level));
}

void settingsWindow::on_save_clicked()
{
    _mainWindow.saveInterfaceSettings(this->isAutoReplace, this->level);
    this->hide();
}
