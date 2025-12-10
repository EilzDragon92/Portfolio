#pragma once

#include "Core/Worker.h"
#include "GUI/InputGUI.h"
#include "GUI/ProgressGUI.h"
#include "Utils/File.h"

class MainGUI : public QWidget {
    Q_OBJECT

public:
    explicit MainGUI(QWidget *parent = nullptr);

    ~MainGUI();

    UserInput getUserInput();
    bool hasValidInput();

signals:
    void readyToStart();

private slots:
    void onStartRequested(const UserInput &input);
    void onProgressUpdated(int perc, QString status);
    void onWorkFinished(QString msg, bool shouldDelete);
    void onThreadFinished();
    void onCloseRequested();

private:
    FILE *srcFile = nullptr, *dstFile = nullptr;
    InputGUI *inputGUI;
    ProgressGUI *prgGUI;
    QStackedWidget *widget;
    QThread *thread;
    QVBoxLayout *vBox;
    UserInput userInput;
    Worker *worker;
    bool shouldDelete = false;

    int openFiles();
    void clear();
};