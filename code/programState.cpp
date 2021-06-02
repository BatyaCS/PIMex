#include "programState.h"
#include "tasks.h"
#include "ObserverNF.h"
#include "workspace.h"
#include <QListWidgetItem>

ProgramState::ProgramState(Tasks & tasks, ObserverNF &observerNF, workspace &ws) :
    _tasks(tasks)
  , _observerNF(observerNF)
  , _workspace(ws)
{

}

void ProgramState::save(QString filename)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);

    QString fileDir = ObserverNF::FileInfo(filename).fileDir;

    QVector<QTreeWidgetItem> items = _workspace.getTreeItems();
    for(int i = 0; i < items.length(); ++i)
        out << items[i].text(0);
    out << "ENDITEMS";

    QVector<Tasks::Task*> tasks = _tasks.get_tasks();
    for(int i = 0; i < tasks.length(); ++i)
    {
        if(!tasks[i]->date.isNull())
            out << tasks[i]->date.toString();
        else
            out << tasks[i]->item.text(0);
        out << tasks[i]->name;
        out << tasks[i]->data;
        for(int j = 0; j < tasks[i]->files.length(); ++j)
            out << tasks[i]->files[j]->toPath();
        out << "ENDTASK";
    }
    out << "ENDTASKS";

    QVector<ObserverNF::FileInfo*> *files = _observerNF.getFiles();
    for(int i = 0; i < files->length(); ++i)
    {
        QFile::copy(files->at(i)->toPath(), fileDir + files->at(i)->name);
        out << files->at(i)->toPath();
    }
    out << "ENDFILES";

    buffer.close();

    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(buffer.data());
        file.close();
    }
}

void ProgramState::load(QString filename)
{
    QFile file(filename);

    if(file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        QString buf;

        QVector<QTreeWidgetItem> items;
        do
        {
            in >> buf;
            if(buf == "ENDITEMS")
                break;
            QTreeWidgetItem item;
            item.setText(0, buf);
            items.append(item);
        } while(true);

        QVector<Tasks::Task> tasks;
        do
        {
            Tasks::Task task;
            in >> buf;
            if(buf == "ENDTASKS")
                break;
            if(QDate::fromString(buf).isValid())
                task.date = QDate::fromString(buf);
            else
                task.item.setText(0, buf);
            in >> task.name;
            in >> task.data;

            QString task_buf;
            do
            {
                in >> task_buf;
                if(task_buf == "ENDTASK")
                    break;
                //task.files.append(ObserverNF::FileInfo(task_buf));
            } while(true);
        } while(true);

        QVector<ObserverNF::FileInfo> files;
        do
        {
            in >> buf;
            if(buf == "ENDFILES")
                break;
            files.append(ObserverNF::FileInfo(buf));
        }while(true);

        file.close();
    }
}
