#include "TaskWorkspace.h"

TaskWorkspace::TaskWorkspace(Tasks &tasks, QWidget *parent) : QWidget(parent)
  , _tasks(tasks)
  , name(this)
  , data_label(this)
  , type_date(this)
  , watched_files_label(this)
  , add_files_button(this)
  , check_files_button(this)
  , data(this)
  , watchedFiles(this)

{
    connect(&data, SIGNAL(textChanged()), SLOT(textChangedByTextEdit()));
    connect(&add_files_button, SIGNAL(clicked()), SLOT(addFilesButtonClicked()));
    connect(&check_files_button, SIGNAL(clicked()), _tasks.get_observerNF(), SLOT(checkAllFiles()));
    add_files_button.setText("Добавить файлы");
    check_files_button.setText("Проверить файлы");
    watched_files_label.setText("Прикрепленные файлы");
    data_label.setText("Описание задачи");
    //layout = new QVBoxLayout(this);
    //layout->addWidget(&watched_files);

    watchedFiles.setContextMenuPolicy(Qt::CustomContextMenu);
    connect(&watchedFiles, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequestedByTreeWidget(QPoint)));
    QStringList column_names;
    column_names << "Файл    Пути";
    watchedFiles.setHeaderLabels(column_names);
}

void TaskWorkspace::resizeEvent(QResizeEvent* event)
{
    QRect label_rect(this->size().width() / 20, (this->size().height() / 20) * 0.3,
                        this->size().width() - (this->size().width() / 20) * 2, (this->size().height() / 20));
    name.setGeometry(label_rect);

    QRect type_date_rect((this->size().width() / 2) + this->size().width() / 20, (this->size().height() / 20) * 0.3,
                         this->size().width() - (this->size().width() / 20) * 2, (this->size().height() / 20));
    type_date.setGeometry(type_date_rect);

    QRect data_label_rect(this->size().width() / 20, (this->size().height() / 20) * 1.2,
                        this->size().width() - (this->size().width() / 20), this->size().height() / 20);
    data_label.setGeometry(data_label_rect);


    QRect data_rect(this->size().width() / 20, (this->size().height() / 20) * 2,
                    this->size().width() - (this->size().width() / 20) * 2, this->size().height() - (this->size().height() / 20) * 10);
    data.setGeometry(data_rect);

    QRect watched_files_rect(this->size().width() / 20, (this->size().height() / 20) * 14,
                             this->size().width() - (this->size().width() / 20) * 2, this->size().height() - (this->size().height() / 20) * 14.5);
    watchedFiles.setGeometry(watched_files_rect);

    QRect watched_files_label_rect(this->size().width() / 20, (this->size().height() / 20) * 13,
                             this->size().width() - (this->size().width() / 20) * 2, (this->size().height() / 20));
    watched_files_label.setGeometry(watched_files_label_rect);

    QRect add_files_button_rect(this->size().width() / 20, (this->size().height() / 20) * 12.5,
                                (this->size().width() / 2) - (this->size().width() / 20) * 2, (this->size().height() / 20) * 0.5);
    add_files_button.setGeometry(add_files_button_rect);

    QRect check_files_button_rect((this->size().width() / 2) + this->size().width() / 20, (this->size().height() / 20) * 12.5,
                                (this->size().width() / 2) - (this->size().width() / 20) * 2, (this->size().height() / 20) * 0.5);
    check_files_button.setGeometry(check_files_button_rect);
}

void TaskWorkspace::paintEvent(QPaintEvent *e)
{
  QPainter qp(this);
  drawWidget(qp);

  QWidget::paintEvent(e);
}

void TaskWorkspace::drawWidget(QPainter &qp)
{
    qp.setPen(QPen(Qt::black, 4));

    QRect name_rect(this->size().width() / 20, (this->size().height() / 20) * 0.5,
                        this->size().width() - (this->size().width() / 20) * 2, (this->size().height() / 20));
    QRect type_rect((this->size().width() / 2) + this->size().width() / 20, (this->size().height() / 20) * 0.5,
                    this->size().width() - (this->size().width() / 20) * 2, (this->size().height() / 20));

    qp.drawText(name_rect.x(), name_rect.y(), "Название задачи");
    qp.drawText(type_rect.x(), type_rect.y(), "Назначение задачи");

    qp.drawLine(0, 0, this->width(), 0);
    qp.drawLine(this->width(), 0, this->width(), this->height());
    qp.drawLine(this->width(), this->height(), 0, this->height());
    qp.drawLine(0, this->height(), 0, 0);
}

void TaskWorkspace::setTask(Tasks::Task *task)
{
    currentTask = task;

    //myTreeWidgetItem *item = dynamic_cast<myTreeWidgetItem*>(watched_files.currentItem());

    if(currentTask)
    {
        name.setText(currentTask->name);
        data.setText(currentTask->data);
        if(!currentTask->date.isNull())
        {
            QLocale locale = QLocale::Russian;
            type_date.setText(locale.toString(currentTask->date));
        }
        else
            type_date.setText(currentTask->item.text(0));
        watchedFiles.clear();
        for(int i = 0; i < currentTask->files.length(); ++i)
        {
            myTreeWidgetItem *item = new myTreeWidgetItem(currentTask->files[i]);
            item->setText(0, currentTask->files[i]->toPath());
            watchedFiles.addTopLevelItem(item);
            for(int j = 0; j < currentTask->files[i]->dirsForWatch.length(); ++j)
            {   myTreeWidgetItem *child = new myTreeWidgetItem();
                child->setText(0, currentTask->files[i]->dirsForWatch[j]);
                item->addChild(child);
            }
        }
    }
}


void TaskWorkspace::textChangedByTextEdit()
{
    if(currentTask)
        currentTask->data = data.toPlainText();
}

void TaskWorkspace::addFilesButtonClicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                         tr("Выберите файлы для контроля"), "", tr("*"));
    if(currentTask)
        for(int i = 0; i < fileNames.length(); ++i)
        {
            _tasks.addTaskFile(currentTask, ObserverNF::FileInfo(fileNames[i]));
        }

    setTask(currentTask);
    _tasks.updateObserverFiles();
}

void TaskWorkspace::deleteFilesButtonClicked()
{
    myTreeWidgetItem *item = dynamic_cast<myTreeWidgetItem*>(watchedFiles.currentItem());
    if(item)
        if(item->file)
        {
            item->takeChildren();
            watchedFiles.takeTopLevelItem(watchedFiles.indexOfTopLevelItem(item));
            _tasks.deleteTaskFile(currentTask, item->file);
        }
}

void TaskWorkspace::addPathesButtonClicked()
{
    myTreeWidgetItem *item = dynamic_cast<myTreeWidgetItem*>(watchedFiles.currentItem());
    if(item)
    {
        QString dirName = QFileDialog::getExistingDirectory(this,
                                        tr("Выберите путь для отслеживания файла"), "");
        if(!dirName.isEmpty())
        {
            myTreeWidgetItem *newItem = new myTreeWidgetItem();
            newItem->setText(0, dirName);
            if(item->file)
            {
                item->file->dirsForWatch.append(dirName);
                item->addChild(newItem);
            }
            else
            {
                dynamic_cast<myTreeWidgetItem*>(item->parent())->file->dirsForWatch.append(dirName);
                item->parent()->addChild(newItem);
            }
        }
    }
}

void TaskWorkspace::deletePathButtonClicked()
{
    myTreeWidgetItem *item = dynamic_cast<myTreeWidgetItem*>(watchedFiles.currentItem());
    myTreeWidgetItem *parent = dynamic_cast<myTreeWidgetItem*>(item->parent());

    int index = parent->indexOfChild(item);
    parent->file->dirsForWatch.removeAt(index);
    parent->removeChild(item);
    //watched_files.removeItemWidget(item, 0);

    //set_task(current_task);
}

void TaskWorkspace::customMenuRequestedByTreeWidget(QPoint pos)
{
    //QModelIndex index = watched_files.indexAt(pos);
    myTreeWidgetItem *item = dynamic_cast<myTreeWidgetItem*>(watchedFiles.currentItem());

    QMenu *menu = new QMenu(this);
    if(!item)
    {
        menu->addAction(new QAction("Добавить файлы", this));
        connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addFilesButtonClicked()));
    }
    else
    {
        if(item->file)
        {
            menu->addAction(new QAction("Добавить файлы", this));
            connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addFilesButtonClicked()));
            menu->addAction(new QAction("Удалить файл", this));
            connect(menu->actions()[1], SIGNAL(triggered()), this, SLOT(deleteFilesButtonClicked()));
            menu->addAction(new QAction("Добавить пути для отслеживания", this));
            connect(menu->actions()[2], SIGNAL(triggered()), this, SLOT(addPathesButtonClicked()));
        }
        else
        {
            menu->addAction(new QAction("Добавить пути для отслеживания", this));
            connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addPathesButtonClicked()));
            menu->addAction(new QAction("Удалить путь для отслеживания", this));
            connect(menu->actions()[1], SIGNAL(triggered()), this, SLOT(deletePathButtonClicked()));
        }
    }
    menu->popup(watchedFiles.viewport()->mapToGlobal(pos));
}
