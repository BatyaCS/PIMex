#include "tasks.h"

Tasks::Tasks()
{
    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_tasks()));
    timer->start();

    overdue_task.setText(0, "Просроченные задачи");


}

Tasks::~Tasks()
{
    observerNF.deleteFiles();
}

QVector<Tasks::Task*> Tasks::check_tasks(QDate date)
{
    QVector<Tasks::Task*> tasks;
    for(int i = 0; i < _tasks.length(); i++)
        if(!_tasks[i].date.isNull())
            if(_tasks[i].date == date)
                tasks.append(&_tasks[i]);
    return tasks;
}

QVector<Tasks::Task*> Tasks::check_tasks(QTreeWidgetItem item)
{
    QVector<Tasks::Task*> tasks;
    for(int i = 0; i < _tasks.length(); i++)
        if(_tasks[i].item.text(0) == item.text(0))
            tasks.append(&_tasks[i]);
    return tasks;
}

ObserverNF *Tasks::get_observerNF()
{
    return &observerNF;
}

void Tasks::add_task(Tasks::Task task)
{
    _tasks.append(task);
}

void Tasks::delete_task(int i)
{
    if(i >= 0 && i < _tasks.length())
        _tasks.remove(i);
    updateObserverFiles();
}

void Tasks::addTaskFile(Tasks::Task *task, ObserverNF::FileInfo file)
{
    bool isContains = false;
    for(int i = 0; i < _tasksFiles.length(); ++i)
        if(_tasksFiles[i] == file)
        {
            if(!task->isContainsFile(&_tasksFiles[i]))
                task->files.append(&_tasksFiles[i]);
            isContains = true;
        }
    if(!isContains)
    {
        _tasksFiles.append(file);
        task->files.append(&_tasksFiles.last());
    }
    updateObserverFiles();
}

void Tasks::deleteTaskFile(Task* task, ObserverNF::FileInfo *file)
{
    task->removeFileByPtr(file);
    ObserverNF::FileInfo *containts_in_any_task = nullptr;

    for(int i = 0; i < _tasks.length(); ++i)
    {
        containts_in_any_task = _tasks[i].isContainsFile(file);
        if(containts_in_any_task)
            break;
    }

    if(!containts_in_any_task)
    {
        observerNF.deleteFile(observerNF.getFileIndexByPath(file->toPath()));
        for(int i = 0; i < _tasksFiles.length(); ++i)
            if(*file == _tasksFiles[i])
            {
                _tasksFiles.takeAt(i);
            }
    }
    updateObserverFiles();
}

int Tasks::get_task_index(Tasks::Task *task)
{
    return _tasks.indexOf(*task);
}

QVector<Tasks::Task*> Tasks::get_tasks()
{
    QVector<Tasks::Task*> tasks;
    for(int i = 0; i < _tasks.length(); i++)
         tasks.append(&_tasks[i]);
    return tasks;
}

void Tasks::update_tasks()
{
    for(int i = 0; i < _tasks.length(); i++)
    {
        if(!_tasks[i].date.isNull())
            if((_tasks[i].date < QDate::currentDate()) && _tasks[i].item.text(0) != overdue_task.text(0))
            {
                //_tasks[i].date = nullptr;
                _tasks[i].name += " (Просрочена)";
                _tasks[i].item = overdue_task;
            }
        for(int j = 0; j < _tasks[i].files.length(); j++)
            if(_tasks[i].files[j]->fileDir == "Deleted")
                deleteTaskFile(&_tasks[i], _tasks[i].files[j]);
    }
}

void Tasks::updateObserverFiles()
{
    QVector<ObserverNF::FileInfo*> *files = observerNF.getFiles();

    for(int i = 0; i < _tasksFiles.length(); ++i)
        observerNF.addFile(&_tasksFiles[i]);

    for(int i = 0; i < files->length(); ++i)
    {
        bool isContains = false;
        for(int j = 0; j < _tasksFiles.length(); ++j)
            if(files->at(i) == &_tasksFiles[j])
            {
                isContains = true;
                break;
            }
        if(!isContains)
            observerNF.deleteFile(i);
    }
}
