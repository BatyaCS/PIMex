#include "workspace.h"
#include "tasksListSpace.h"

workspace::workspace(Tasks &tasks, tasksListSpace &tasksLS, QWidget *parent) : QWidget(parent)
  , _tasks(tasks)
  , _tasksListSpace(tasksLS)
  , m_parent(parent)
{
    //layout = new QVBoxLayout(this);

    calendar = new QCalendarWidget(this);
    calendar->setGridVisible(true);

    //calendar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    calendar->setContextMenuPolicy(Qt::CustomContextMenu);
    calendar->setHorizontalHeaderFormat(QCalendarWidget::HorizontalHeaderFormat::ShortDayNames);
    QLocale locale = QLocale::Russian;
    calendar->setLocale(locale);
    connect(calendar, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequestedByCalendar(QPoint)));
    connect(calendar, SIGNAL(clicked(const QDate&)), SLOT(calendarItemClicked(const QDate&)));

    //layout->addWidget(calendar);

    tree = new QTreeWidget(this);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    //tree->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    connect(tree, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequestedByTree(QPoint)));
    connect(tree, SIGNAL(itemClicked(QTreeWidgetItem *,int)), this , SLOT(treeItemClicked(QTreeWidgetItem*)));
    //layout->addWidget(tree);

    tree_items_icon.addFile("icon.png");

    QTreeWidgetItem *item = new QTreeWidgetItem; //= _tasks.overdue_task;
    item->setText(0, "Просроченные задачи");
    item->setIcon(0, tree_items_icon);
    tree->addTopLevelItem(item);

    QStringList column_names;
    column_names << "Проекты";
    this->tree->setHeaderLabels(column_names);
}

void workspace::resizeEvent(QResizeEvent* event)
{
    QRect calendar_rect(this->size().width() / 20, (this->size().height() / 3) / 20,
                        this->size().width() - (this->size().width() / 20) * 2, this->size().height() / 3 - ((this->size().height() / 3) / 20) * 2);

    //calendar->setMaximumSize(calendar_rect.size());
    calendar->setGeometry(calendar_rect);
    //qDebug() << calendar_rect;

    QRect tree_rect(this->size().width() / 20 , (this->size().height() / 3) + (this->size().height()) / 20,
                         this->size().width() - (this->size().width() / 20) * 2, this->size().height() - (this->size().height() / 3) - (this->size().height() / 20) * 1.5);
    //tree->setMaximumSize(tree_rect.size());
    tree->setGeometry(tree_rect);
    //qDebug() << tree_rect;
}

void workspace::paintEvent(QPaintEvent *e)
{
  QPainter qp(this);
  drawWidget(qp);

  QWidget::paintEvent(e);
}

void workspace::drawWidget(QPainter &qp)
{
    qp.setPen(QPen(Qt::black, 4));

    qp.drawLine(0, 0, this->width(), 0);
    qp.drawLine(this->width(), 0, this->width(), this->height());
    qp.drawLine(this->width(), this->height(), 0, this->height());
    qp.drawLine(0, this->height(), 0, 0);
}

void workspace::treeItemClicked(QTreeWidgetItem * item)
{
    _tasksListSpace.set_list_widget_data(_tasks.check_tasks(*item));
    _tasksListSpace.set_project(tasksListSpace::project(item));
    _tasksListSpace.set_label(item->text(0));
}

void workspace::calendarItemClicked(const QDate &date)
{
    QDate *_date = new QDate(calendar->selectedDate());
    _tasksListSpace.set_list_widget_data(_tasks.check_tasks(date));
    _tasksListSpace.set_project(tasksListSpace::project(_date));
    QLocale locale = QLocale::Russian;
    _tasksListSpace.set_label(locale.toString(date));
}

void workspace::customMenuRequestedByTree(QPoint pos)
{
    QModelIndex index = tree->indexAt(pos);

    QMenu *menu = new QMenu(this);
    if(index.column() == -1)
    {
        menu->addAction(new QAction("Добавить проект", this));
        connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addProject()));
    }
    else
    {
        menu->addAction(new QAction("Добавить проект", this));
        connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addProject()));
        menu->addAction(new QAction("Добавить задачу", this));
        connect(menu->actions()[1], SIGNAL(triggered()), this, SLOT(addTaskByTree()));
        menu->addAction(new QAction("Добавить подпроект", this));
        connect(menu->actions()[2], SIGNAL(triggered()), this, SLOT(addSubProject()));
        menu->addAction(new QAction("Удалить проект", this));
        connect(menu->actions()[3], SIGNAL(triggered()), this, SLOT(deleteProject()));
    }
    menu->popup(tree->viewport()->mapToGlobal(pos));
}

void workspace::customMenuRequestedByCalendar(QPoint pos)
{

    QMenu *menu = new QMenu(this);

    menu->addAction(new QAction("Добавить задачу", this));
    connect(menu->actions()[0], SIGNAL(triggered()), this, SLOT(addTaskByCalendar()));

    menu->popup(calendar->mapToGlobal(pos));
}

void workspace::addProject()
{
    QString text = QInputDialog::getText(this, "Ввод названия проекта", "Введите название проекта");
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setIcon(0, tree_items_icon);
    tree->addTopLevelItem(item);
}

void workspace::addSubProject()
{
    QTreeWidgetItem *item = tree->currentItem();
    QString text = QInputDialog::getText(this, "Ввод названия подпроекта", "Введите название подпроекта");
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setIcon(0, tree_items_icon);
    child->setText(0, text);
    item->addChild(child);
}

void workspace::addTaskByTree()
{
    QTreeWidgetItem *item = tree->currentItem();
    Tasks::Task task;
    task.name = QInputDialog::getText(this, "Ввод имени задачи", "Введите имя задачи");
    task.item.setText(0, item->text(0));
    _tasks.add_task(task);
}

void workspace::addTaskByCalendar()
{
    QDate *date = new QDate(calendar->selectedDate());
    Tasks::Task task;
    task.name = QInputDialog::getText(this, "Ввод имени задачи", "Введите имя задачи");
    task.date.setDate(date->year(), date->month(), date->day());
    _tasks.add_task(task);
}

void workspace::deleteProject()
{
    QList<QTreeWidgetItem *>  items = tree->selectedItems();
    QTreeWidgetItem          *pp    = nullptr;

    if ( !items.isEmpty() )
    {
        foreach (QTreeWidgetItem *item, items)
        {
            if(item->text(0) != _tasks.overdue_task.text(0))
            {
                pp = item->parent();
                if(pp)
                    pp->removeChild(item);
                delete item;
            }
        }
    }
}

QVector<QTreeWidgetItem> workspace::getTreeItems()
{
    QVector<QTreeWidgetItem> items;
    for(int i = 0; i < tree->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        items.append(*item);
        //delete item;
    }
    return items;
}
