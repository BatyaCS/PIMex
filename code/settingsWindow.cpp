#include "settingsWindow.h"
#include "ui_settingsWindow.h"

settingsWindow::settingsWindow(MainWindow& mWindow, QWidget *parent) :
    QWidget(parent),
    _mainWindow(mWindow),
    ui(new Ui::settingsWindow)
{
    ui->setupUi(this);

    setWindowTitle("Настройки модуля контроля файлов");

    resize(400, 300);
    //setMinimumHeight(300);
    //setMinimumWidth(400);

    int w = width();
    int h = height();

    ui->AutoReplace->move(w/2 - ui->AutoReplace->width()/2, (h/(BUTTONS_COUNT + 1)) * 1);
    ui->levelCount->move(w/2 - ui->levelCount->width()/2, (h/(BUTTONS_COUNT + 1)) * 2);
    ui->levelCount->setMinimum(0);
    ui->levelCount->setValue(level);
    ui->levelCountLabel->move(w/2 - ui->levelCountLabel->width()/2, (h/(BUTTONS_COUNT + 1)) * 2 - ui->levelCount->height());

    ui->save->move((w/3) * 1, (h/(BUTTONS_COUNT + 1)) * 3);
    ui->exit->move((w/3) * 2, (h/(BUTTONS_COUNT + 1)) * 3);

}

settingsWindow::~settingsWindow()
{
    delete ui;
}

void settingsWindow::resizeEvent(QResizeEvent *)
{
    int w = width();
    int h = height();

    ui->AutoReplace->move(w/2 - ui->AutoReplace->width()/2, (h/(BUTTONS_COUNT + 1)) * 0.6);
    ui->levelCount->move(w/2 - ui->levelCount->width()/2, (h/(BUTTONS_COUNT + 1)) * 2);
    ui->levelCountLabel->move(w/2 - ui->levelCountLabel->width()/2, (h/(BUTTONS_COUNT + 1)) * 2 - ui->levelCount->height());

    ui->save->move(((w/3) - ui->save->width()/3) * 1, (h/(BUTTONS_COUNT + 1)) * 3);
    ui->exit->move(((w/3) - ui->exit->width()/3) * 2, (h/(BUTTONS_COUNT + 1)) * 3);
}

/*void settingsWindow::on_save_clicked()
{
    //_mainWindow.save_interface_settings(ui->AutoReplace->isEnabled(), ui->levelCount->value());
    hide();
}*/

void settingsWindow::on_exit_clicked()
{
    hide();
}

void settingsWindow::on_AutoReplace_stateChanged(int arg1)
{
    isAutoReplace = arg1;
}

#ifdef Q_OS_WIN64
void settingsWindow::on_levelCount_textChanged(const QString &arg1)
{
    level = arg1.toInt();
}
#endif

#ifdef Q_OS_LINUX
void settingsWindow::on_levelCount_valueChanged(int arg1)
{
    level = arg1;
}
#endif


