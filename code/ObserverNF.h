#pragma once
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QAbstractButton>
#include <QTimer>

#define NAMES_SAME_CONTENT_SAME 1
#define NAMES_DIFF_CONTENT_SAME 2
#define NAMES_SAME_CONTENT_DIFF 3
#define NAMES_DIFF_CONTENT_DIFF 4



class ObserverNF : public QObject
{
    Q_OBJECT


public:

    QString watchedFilesDir;

    struct Settings
    {
        bool isAutoReplace = false;
        int level = 3;
        Settings() { }
        Settings(bool isAR, int lvl) { isAutoReplace = isAR; level = lvl; }
    };

    const QString watchedFilesLastDir = "watched_files";
    struct FileInfo{
        QString name;
        QString fileDir;
        QStringList dirsForWatch;
        int levelsForWatch = 3;
        FileInfo() { }
        FileInfo(QString file)
        {
            name = file.split("/").last();
            QStringList tmpFileDir = file.split("/");
            tmpFileDir.pop_back();
            fileDir = tmpFileDir.join("/");
            fileDir += "/";
            dirsForWatch.append(fileDir);
        }
        friend bool operator==(FileInfo f1, FileInfo f2)
        {
            return (f1.name == f2.name) && (f1.fileDir == f2.fileDir);
        }
        const QString toPath() const { return fileDir + name; }
        bool isEmpty() const { return name.isEmpty() && fileDir == "/"; }

    };

    ObserverNF();
    ~ObserverNF();
    void addFile(FileInfo *file);
    bool isContains(FileInfo &file);
    FileInfo* getFileByFileInfo(FileInfo &info);
    void setSettings(const Settings& settings);
    const Settings& getSettings() const { return _settings; }
    void deleteFile(int i);
    void deleteFileByFileInfo(FileInfo *fileInfo);
    void deleteFiles();
    int getFileIndexByPath(const QString &path);
    QVector<FileInfo*>* getFiles(void);

public slots:
    void checkAllFiles();

private:

    QDir dir;
    const QString searchForDeletedFile(const FileInfo& info);
    FileInfo* getFileByPath(const QString &path);
    bool checkIsFileInTargetLevel(const QString &path, const QString &filePath, int level);
    QString getDirectoryMinusLevel(FileInfo file, int level);
    void checkCurrentFiles(FileInfo *file);
    void checkAnotherFiles(FileInfo *file);
    //int checkFilesInDir(QVector<FileInfo> *files);
    int compareFiles(const QString& fileName1, const QString& fileName2);
    int execMessageBox(QString text, QString informativeText, QMessageBox::StandardButtons buttons, QStringList buttonsTexts, QMessageBox::Button default_button);

    //QFileSystemWatcher watcher;
    QVector<FileInfo*> _files;
    QVector<FileInfo> _copyedFiles;
    QVector<FileInfo> _exeptedFiles;
    QTimer *timer;
    Settings _settings;
};

