#pragma once
#include <QDate>
#include <QString>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QObject>
#include <QVector>
#include "ObserverNF.h"

class Tasks : public QObject
{
    Q_OBJECT

public:
    Tasks();
    ~Tasks();
    struct Task
    {
        QTreeWidgetItem item;
        QDate date;
        QString name;
        QString data;
        QVector<ObserverNF::FileInfo*> files;
        friend bool operator==(Task t1, Task t2)
        {
            return (t1.name == t2.name) && (t1.data == t2.data) && (t1.item.text(0) == t2.item.text(0)) && (t1.date == t2.date) && (t1.files == t2.files);
        }
        ObserverNF::FileInfo* isContainsFile(ObserverNF::FileInfo *file)
        {
            for(int i = 0; i < files.length(); ++i)
                if(files[i] == file)
                    return files[i];
            return nullptr;
        }
        void removeFileByPtr(ObserverNF::FileInfo *file)
        {
            for(int i = 0; i < files.length(); ++i)
                if(files[i] == file)
                    files.remove(i);
        }
    };

    QVector<Task*> check_tasks(QDate date);
    QVector<Task*> check_tasks(QTreeWidgetItem item);

    ObserverNF* get_observerNF();

    void add_task(Task task);
    void delete_task(int i);
    void addTaskFile(Task* task, ObserverNF::FileInfo file);
    void deleteTaskFile(Task* task, ObserverNF::FileInfo* file);
    int get_task_index(Task* task);
    void set_settings(ObserverNF::Settings settings) { observerNF.setSettings(settings); };
    QVector<Task*> get_tasks();
    QTreeWidgetItem overdue_task;

    void updateObserverFiles(void);
public slots:
    void update_tasks();

private:  

    ObserverNF observerNF;
    QTimer *timer;
    QVector<Task> _tasks;
    QList<ObserverNF::FileInfo> _tasksFiles;

};

