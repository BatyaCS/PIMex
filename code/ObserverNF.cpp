#include "ObserverNF.h"



ObserverNF::ObserverNF()
{
    //connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChangedByUser(const QString&)));

    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkAllFiles()));
    timer->start();

#ifdef Q_OS_WIN64
    watchedFilesDir = QDir::currentPath() + "/" + watchedFilesLastDir + "/";
    dir.mkdir(watchedFilesDir);
#endif

#ifdef Q_OS_LINUX
    watchedFilesDir =  QDir::currentPath() + "/" + watchedFilesLastDir + "/";
    dir.mkdir(watchedFilesDir + watchedFilesLastDir);
#endif
}

ObserverNF::~ObserverNF()
{
    deleteFiles();
}

void ObserverNF::addFile(ObserverNF::FileInfo *file)
{
    bool contains = false;
    for(int i = 0; i < _files.length(); ++i)
        if(_files[i] == file)
            contains = true;
    if(!contains)
    {
        _files.append(file);
        dir.mkdir(watchedFilesLastDir + "/" + QString::number(_files.length()));
        QFile::copy(file->fileDir + file->name, watchedFilesDir + "/" + QString::number(_files.length()) + "/" + file->name);
    }
}

void ObserverNF::setSettings(const ObserverNF::Settings &settings)
{
     _settings = settings;
     for(int i = 0; i < _files.length(); ++i)
         _files[i]->levelsForWatch = _settings.level;
}

void ObserverNF::deleteFiles()
{
    int len = _files.length();
    for(int i = 0; i < len; ++i)
        deleteFile(0);
}

void ObserverNF::deleteFile(int i)
{
    if(i >= 0 && i < _files.length())
    {
        //watcher.removePath(_files[i]->fileDir + _files[i]->name);
        QFile::remove(watchedFilesDir + "/" + QString::number(i + 1) + "/" + _files[i]->name);
        dir.rmdir(watchedFilesLastDir + "/" + QString::number(i + 1));
        _files.remove(i);

        QDir delDir(watchedFilesDir);
        //QDirIterator it(watchedFilesLastDir);
        int j = -1;
        foreach (QFileInfo info, delDir.entryInfoList())
        {
            //qDebug() << info.filePath() << info.path() + QString::number(j);
            if(j > 0)
                QFile::rename(info.filePath(), info.path() + "/" + QString::number(j));
            j++;
        }

    }
}

QVector<ObserverNF::FileInfo *> *ObserverNF::getFiles()
{
    return &_files;
}

void ObserverNF::checkCurrentFiles(FileInfo *file)
{
    if(!QFile::exists(file->toPath()))
    {
        QString founded_file = searchForDeletedFile(*file);

        if(!founded_file.isEmpty())
        {
            int ret = execMessageBox("Файл " + file->toPath() + " был перемещен, новый путь " + founded_file,
                                     "Вы хотите переместить его обратно?",
                                     QMessageBox::Yes | QMessageBox::No, { "Да", "Нет" }, QMessageBox::No);

            if(ret == QMessageBox::No || _settings.isAutoReplace)
            {
                FileInfo * tmp_file = getFileByPath(file->toPath());
                tmp_file->fileDir = FileInfo(founded_file).fileDir;
            }
            else if(ret == QMessageBox::Yes)
            {
                QFile::copy(founded_file, file->toPath());
                QFile::remove(founded_file);
            }
        }
        else
        {
            int ret = execMessageBox("Файл " + file->toPath() + " был удален", "Вы хотите восстановить его?", QMessageBox::Yes | QMessageBox::No, { "Да", "Нет"}, QMessageBox::No);
            if(ret == QMessageBox::Yes)
            {
                QFile::copy(watchedFilesDir + "/" + QString::number(getFileIndexByPath(file->toPath()) + 1) + "/" + file->name, file->toPath());
                //watcher.addPath(file->toPath());
            }
            else if(ret == QMessageBox::No)
            {
                FileInfo * tmp_file = getFileByPath(file->toPath());
                //deleteFile(getFileIndexByPath(file->toPath()));
                tmp_file->fileDir = "Deleted";
            }
        }
    }
}

void ObserverNF::checkAnotherFiles(FileInfo *fileInfo)
{
    for(int i = 0; i < fileInfo->dirsForWatch.length(); ++i)
    {
        QDirIterator it(fileInfo->dirsForWatch[i], QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString filename = it.next();
            QFileInfo file(filename);

            if (file.isDir() || _exeptedFiles.contains(FileInfo(file.filePath())))
                continue;

            if(!checkIsFileInTargetLevel(fileInfo->toPath(), filename, fileInfo->levelsForWatch))
                continue;

            int compare_result = 0;

            QStringList filePath = file.path().split("/");
            if(filePath.length() > 1)
            {
                filePath.takeLast();
                if(filePath.last() != watchedFilesLastDir)
                    compare_result = compareFiles(fileInfo->toPath(), file.filePath());
            }

            if(compare_result == NAMES_SAME_CONTENT_SAME)
            {
                int ret = execMessageBox("Была найдена копия файла " + file.filePath() + " " + fileInfo->toPath(),
                                         "Вы хотите удалить его, оставить или заменить отслеживаемый файл на него?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, { "Заменить", "Удалить", "Оставить"}, QMessageBox::No);
                if(ret == QMessageBox::Yes)
                    QFile::remove(file.filePath());
                if(ret == QMessageBox::No)
                    _exeptedFiles.append(FileInfo(file.filePath()));
                if(ret == QMessageBox::Cancel)
                {
                    QFile::remove(fileInfo->toPath());
                    fileInfo->fileDir = FileInfo(file.filePath()).fileDir;
                    fileInfo->name = FileInfo(file.filePath()).name;
                }
            }
            if(compare_result == NAMES_DIFF_CONTENT_SAME)
            {
                int ret = execMessageBox("Была найдена копия файла " + file.filePath() + " с другим названием " + fileInfo->toPath(),
                                         "Вы хотите удалить его, оставить или заменить отслеживаемый файл на него?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, { "Заменить", "Удалить", "Оставить"}, QMessageBox::No);
                if(ret == QMessageBox::Yes)
                    QFile::remove(file.filePath());
                if(ret == QMessageBox::No)
                    _exeptedFiles.append(FileInfo(file.filePath()));
                if(ret == QMessageBox::Cancel)
                {
                    QFile::remove(fileInfo->toPath());
                    fileInfo->fileDir = FileInfo(file.filePath()).fileDir;
                    fileInfo->name = FileInfo(file.filePath()).name;
                }
            }
            if(compare_result == NAMES_SAME_CONTENT_DIFF)
            {
                int ret = execMessageBox("Был найден файл " + file.filePath() + " схожий по названию но различный по содержанию с отслеживаемым файлом " + fileInfo->toPath(),
                                         "Вы хотите удалить его, оставить или заменить отслеживаемый файл на него?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, { "Заменить", "Удалить", "Оставить"}, QMessageBox::No);
                if(ret == QMessageBox::Yes)
                    QFile::remove(file.filePath());
                if(ret == QMessageBox::No)
                    _exeptedFiles.append(FileInfo(file.filePath()));
                if(ret == QMessageBox::Cancel)
                {
                    QFile::remove(fileInfo->toPath());
                    fileInfo->fileDir = FileInfo(file.filePath()).fileDir;
                    fileInfo->name = FileInfo(file.filePath()).name;
                }
            }
        }
    }
}

void ObserverNF::checkAllFiles()
{
    for(int i = 0; i < _files.length(); ++i)
    {
        checkCurrentFiles(_files[i]);

        checkAnotherFiles(_files[i]);
    }
}

const QString ObserverNF::searchForDeletedFile(const FileInfo& info)
{
    QString targetStr = info.name;
    FileInfo foundedFile("");

    for(int i = 0; i < info.dirsForWatch.length(); ++i)
    {
        QDirIterator it(info.dirsForWatch[i], QDirIterator::Subdirectories);

        while (it.hasNext())
        {
            QString filename = it.next();
            QFileInfo file(filename);

            if (file.isDir() || _exeptedFiles.contains(FileInfo(file.filePath())))
                continue;

            if(!checkIsFileInTargetLevel(info.toPath(), file.filePath(), info.levelsForWatch))
                continue;

            if (file.fileName().contains(targetStr, Qt::CaseInsensitive) && file.path().split("/").last() != watchedFilesLastDir)
            {
                foundedFile = FileInfo(file.filePath());
                break;
            }
        }
    }
    if(foundedFile.isEmpty())
        return "";
    else
        return foundedFile.toPath();
}

ObserverNF::FileInfo* ObserverNF::getFileByPath(const QString &path)
{
    for(int i = 0; i < _files.length(); i++)
        if(path == _files[i]->fileDir + _files[i]->name)
            return _files[i];
    return nullptr;
}

bool ObserverNF::checkIsFileInTargetLevel(const QString& path, const QString& filePath, int level)
{
    return (filePath.split("/").length() - path.split("/").length()) <= level;
}

QString ObserverNF::getDirectoryMinusLevel(ObserverNF::FileInfo file, int level)
{
    QStringList fileDir = file.fileDir.split("/");
    for(int i = 0; i < level && fileDir.length() > 1; ++i)
        fileDir.pop_back();
    return fileDir.join("/") + "/";
}



int ObserverNF::getFileIndexByPath(const QString &path)
{
    for(int i = 0; i < _files.length(); i++)
        if(path == _files[i]->fileDir + _files[i]->name)
            return i;
    return -1;
}

int ObserverNF::compareFiles(const QString& fileName1, const QString& fileName2)
{
    if(fileName1 == fileName2)
        return 0;

    QCryptographicHash md5genF1(QCryptographicHash::Md5);
    QCryptographicHash md5genF2(QCryptographicHash::Md5);

    QFile file1(fileName1);
    if(file1.open(QIODevice::ReadOnly))
        md5genF1.addData(file1.readAll());
    else
        return 0;
    file1.close();

    QFile file2(fileName2);
    if(file2.open(QIODevice::ReadOnly))
        md5genF2.addData(file2.readAll());
    else
        return 0;
    file2.close();

    const QString name1 = fileName1.split("/").last();
    const QString name2 = fileName2.split("/").last();

    const QByteArray bytesF1 = md5genF1.result().toHex();
    const QByteArray bytesF2 = md5genF2.result().toHex();

    if(bytesF1 == bytesF2 && name1 == name2)
        return NAMES_SAME_CONTENT_SAME;
    if(bytesF1 == bytesF2 && name1 != name2)
        return NAMES_DIFF_CONTENT_SAME;
    if(bytesF1 != bytesF2 && name1 == name2)
        return NAMES_SAME_CONTENT_DIFF;
    if(bytesF1 != bytesF2 && name1 != name2)
        return NAMES_DIFF_CONTENT_DIFF;

    return 0;
}

int ObserverNF::execMessageBox(QString text, QString informativeText, QMessageBox::StandardButtons buttons, QStringList buttonsTexts, QMessageBox::Button default_button)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(buttons);
    QList<QAbstractButton*> buttonsList = msgBox.buttons();
    for(int i = 0; i < buttonsList.length(); ++i)
        buttonsList[i]->setText(buttonsTexts.at(i));
    msgBox.setDefaultButton(default_button);
    return msgBox.exec();
}
