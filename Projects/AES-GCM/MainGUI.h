#pragma once

#include "InputGUI.h"
#include "ProgressGUI.h"
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
    void onProgressUpdated(int perc, QString status);
    void onWorkFinished(QString msg);
    void onThreadFinished();
    void onCloseRequested();

private:
    FILE *srcFile, *dstFile;
    InputGUI *inputGUI;
    ProgressGUI *prgGUI;
    QStackedWidget *widget;
    QThread *thread;
    QVBoxLayout *vBox;
    UserInput userInput;
    Worker *worker;
    
    int openFiles();
    void clear();
};