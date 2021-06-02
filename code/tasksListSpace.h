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
#include <QTimer>
#include "tasks.h"
#include "TaskWorkspace.h"

class tasksListSpace : public QWidget
{
    Q_OBJECT
public:

    class myListWidgetItem : public QListWidgetItem
    {
        public:
            explicit myListWidgetItem(Tasks::Task *Task) :
                task(Task) {}
            Tasks::Task *task = nullptr;
    };

    struct project
    {
        QTreeWidgetItem *item;
        QDate *date;
        project() { item = nullptr; date = nullptr; }
        project(QTreeWidgetItem *itm) { item = itm; date = nullptr; }
        project(QDate *dt) { date = dt; item = nullptr; }
    };

    explicit tasksListSpace(Tasks &tasks, TaskWorkspace &taskWorkspace, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent*) override;

    void set_project(project);
    project get_current_project();
    void set_label(QString);
    void set_list_widget_data(QVector<Tasks::Task*>);

protected:
    void paintEvent(QPaintEvent *e) override;
    void drawWidget(QPainter &qp);

public slots:
    void itemClickedByListWidget(QListWidgetItem *);
    void addTask();
    void deleteTask();

    void customMenuRequestedByListWidget(QPoint pos);

    void update_myself();

signals:

private:
    Tasks &_tasks;
    TaskWorkspace &_taskWorkspace;
    QWidget *m_parent;

    project current_project;
    QLabel *label;
    QListWidget *listWidget;
    QTimer *timer;

    QVBoxLayout *layout;

};

