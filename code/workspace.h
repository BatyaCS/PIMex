#pragma once

#include <QWidget>
#include <QPainter>
#include <QCalendarWidget>
//#include <QCalendar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include <QInputDialog>
#include <QIcon>
#include "tasks.h"

class tasksListSpace;

class workspace : public QWidget
{
    Q_OBJECT
public:
    explicit workspace(Tasks &tasks, tasksListSpace &tasksLS, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent*) override;

protected:
    void paintEvent(QPaintEvent *e) override;
    void drawWidget(QPainter &qp);

signals:

public slots:

    void treeItemClicked(QTreeWidgetItem*);
    void calendarItemClicked(const QDate&);
    void customMenuRequestedByTree(QPoint pos);
    void customMenuRequestedByCalendar(QPoint pos);
    void addProject();
    void addSubProject();
    void addTaskByTree();
    void addTaskByCalendar();
    void deleteProject();

    QVector<QTreeWidgetItem> getTreeItems(void);

private:
    Tasks &_tasks;
    tasksListSpace &_tasksListSpace;

    QWidget *m_parent;
    QCalendarWidget *calendar;

    QTreeWidget *tree;
    QIcon tree_items_icon;

    QVBoxLayout *layout;
};

