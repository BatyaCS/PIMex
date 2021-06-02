#pragma once
#include <QString>
#include <QBuffer>
#include <QDataStream>
#include <QFile>
#include <iostream>

class Tasks;
class ObserverNF;
class workspace;

class ProgramState
{
public:
    ProgramState(Tasks &, ObserverNF &, workspace &);
    void save(QString filename);
    void load(QString filename);
private:
    Tasks &_tasks;
    ObserverNF &_observerNF;
    workspace &_workspace;
};

