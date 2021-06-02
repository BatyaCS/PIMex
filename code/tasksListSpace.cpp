#include "tasksListSpace.h"

tasksListSpace::tasksListSpace(Tasks &tasks, TaskWorkspace &taskWorkspace, QWidget *parent) : QWidget(parent)
  , _tasks(tasks)
  , _taskWorkspace(taskWorkspace)
{
    m_parent = parent;

    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_myself()));
    timer->start();

    label = new QLabel(this);
    listWidget = new QListWidget(this);
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(itemClickedByListWidget(QListWidgetItem*)));


    //layout = new QVBoxLayout(this);
    //layout->addWidget(listWidget);

    connect(listWidget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequestedByListWidget(QPoint)));
}

void tasksListSpace::resizeEvent(QResizeEvent* event)
{
    QRect label_rect(this->size().width() / 20, this->size().height() / 20 * 1.5,
                        this->size().width() - (this->size().width() / 20) * 2, this->size().height() / 20);
    label->setGeometry(label_rect);

    QRect listWidget_rect(this->size().width() / 20, (this->size().height() / 20) * 3,
                         this->size().width() - (this->size().width() / 20) * 2, this->size().height() - (this->size().height() / 20) * 3.5);
    listWidget->setGeometry(listWidget_rect);
}

void tasksListSpace::set_project(tasksListSpace::project prj)
{
    current_project = prj;
}

tasksListSpace::project tasksListSpace::get_current_project()
{
    return current_project;
}

void tasksListSpace::set_label(QString text)
{
    label->setText(text);
}

void tasksListSpace::set_list_widget_data(QVector<Tasks::Task*> tasks)
{
    listWidget->clear();
    for(int i = 0; i < tasks.length(); i++)
    {
        myListWidgetItem *item = new myListWidgetItem(tasks[i]);
        item->setText(tasks.at(i)->name);
        listWidget->addItem(item);
    }
}

void tasksListSpace::paintEvent(QPaintEvent *e)
{
  QPainter qp(this);
  drawWidget(qp);

  QWidget::paintEvent(e);
}

void tasksListSpace::drawWidget(QPainter &qp)
{
    qp.setPen(QPen(Qt::black, 4));

    QRect text(this->size().width() / 20, this->size().height() / 20,
               this->size().width() - (this->size().width() / 20) * 2, this->size().height() / 20);

    qp.drawText(text, "Задачи выбранного дня или проекта");
    qp.drawLine(0, 0, this->width(), 0);
    //qp.drawLine(this->width(), 0, this->width(), this->height());
    qp.drawLine(this->width(), this->height(), 0, this->height());
    //qp.drawLine(0, this->height(), 0, 0);
}

void tasksListSpace::itemClickedByListWidget(QListWidgetItem *item)
{
    _taskWorkspace.setTask(dynamic_cast<myListWidgetItem*>(item)->task);
}

void tasksListSpace::addTask()
{
    Tasks::Task task;
    if(current_project.item)
    {
        task.name = QInputDialog::getText(this, "Ввод имени задачи", "Введите имя задачи");
        task.item.text(0) = current_project.item->text(0);
        _tasks.add_task(task);
    }

    if(current_project.date)
    {
        task.name = QInputDialog::getText(this, "Ввод имени задачи", "Введите имя задачи");
        task.date.setDate(current_project.date->year(), current_project.date->month(), current_project.date->day());
        _tasks.add_task(task);
    }
    update_myself();
}

void tasksListSpace::deleteTask()
{
    myListWidgetItem *item = dynamic_cast<myListWidgetItem*>(listWidget->currentItem());
    if(item)
        _tasks.delete_task(_tasks.get_task_index(item->task));
    update_myself();
}

void tasksListSpace::customMenuRequestedByListWidget(QPoint pos)
{
    QModelIndex index = listWidget->indexAt(pos);

    QMenu *menu = new QMenu(this);
    if(index.column() == -1)
    {
        menu->addAction(new QAction("Добавить задачу", this));
        connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addTask()));
    }
    else
    {
        menu->addAction(new QAction("Добавить задачу", this));
        connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addTask()));
        menu->addAction(new QAction("Удалить задачу", this));
        connect(menu->actions()[1], SIGNAL(triggered()), this, SLOT(deleteTask()));
    }
    menu->popup(listWidget->viewport()->mapToGlobal(pos));
}

void tasksListSpace::update_myself()
{
    QModelIndex index = listWidget->currentIndex();
    if(current_project.date)
        set_list_widget_data(_tasks.check_tasks(*current_project.date));
    if(current_project.item)
        set_list_widget_data(_tasks.check_tasks(*current_project.item));
    listWidget->setCurrentIndex(index);
}
