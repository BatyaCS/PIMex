#pragma once

#include <QWidget>
//#include "MainWindow.h"

namespace Ui {
class settingsWindow;
}

class MainWindow;

class settingsWindow : public QWidget
{
    Q_OBJECT

    const int BUTTONS_COUNT = 3;

public:
    explicit settingsWindow(MainWindow& mWindow, QWidget *parent = nullptr);
    ~settingsWindow();

private slots:
    void on_save_clicked();

    void on_exit_clicked();

    void on_AutoReplace_stateChanged(int arg1);


#ifdef Q_OS_WIN64
    void on_levelCount_textChanged(const QString &arg1);
#endif

#ifdef Q_OS_LINUX
    void on_levelCount_valueChanged(int arg1);
#endif




private:

    MainWindow& _mainWindow;

    Ui::settingsWindow *ui;
    int level = 3;
    bool isAutoReplace = false;

    void resizeEvent(QResizeEvent*);
};

