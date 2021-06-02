#pragma once

#include <QWidget>
#include <QPainter>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QCalendarWidget>
#include <QDebug>
#include <QInputDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include "tasks.h"

class TaskWorkspace : public QWidget
{
    class myTreeWidgetItem : public QTreeWidgetItem
    {
        public:
            explicit myTreeWidgetItem(ObserverNF::FileInfo *File) :
                file(File) {}
            explicit myTreeWidgetItem() { }
            ObserverNF::FileInfo *file = nullptr;
    };

    Q_OBJECT
public:
    explicit TaskWorkspace(Tasks &tasks, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent*) override;
    void setTask(Tasks::Task *task);

protected:
    void paintEvent(QPaintEvent *e) override;
    void drawWidget(QPainter &qp);

public slots:
    void textChangedByTextEdit();
    void addFilesButtonClicked();
    void deleteFilesButtonClicked();
    void addPathesButtonClicked();
    void deletePathButtonClicked();
    void customMenuRequestedByTreeWidget(QPoint);
signals:

private:
    Tasks &_tasks;
    QVBoxLayout *layout;
    Tasks::Task *currentTask = nullptr;

    QLabel name;
    QLabel data_label;
    QLabel type_date;
    QLabel watched_files_label;
    QPushButton add_files_button;
    QPushButton check_files_button;
    QTextEdit data;
    QTreeWidget watchedFiles;
};

