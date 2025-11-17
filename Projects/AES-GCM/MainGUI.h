#pragma once

#include "InputWidget.h"
#include "ProgressWidget.h"
#include "Worker.h"
#include "header.h"

#include <QStackedWidget>
#include <QThread>
#include <QWidget>

class MainGUI : public QWidget {
    Q_OBJECT

public:
    explicit MainGUI(QWidget *parent = nullptr);

    ~MainGUI();

    UserInput getUserInput();
    bool hasValidInput();

public slots:
    int startWork();

private slots:
    void onStartRequested(const UserInput &input);
    void onProgressUpdated(int percentage, QString status);
    void onWorkFinished(bool success, QString message);
    void onThreadFinished();
    void onCloseRequested();

private:
    FILE *srcFile, *dstFile;
    InputWidget *iWidget;
    ProgressWidget *pWidget;
    QStackedWidget *sWidget;
    QThread *thread;
    QVBoxLayout *vBox;
    UserInput userInput;
    Worker *worker;
    
    int openFiles();
    void clear();
};