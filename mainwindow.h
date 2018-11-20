#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "field.h"

#include <QMainWindow>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QProgressBar>
#include <QThread>
#include <QGridLayout>

class StatisticsTab;
class MainWindow;

class Worker : public QObject {
    Q_OBJECT
public:
    Worker(StatisticsTab* st);
    ~Worker();
public slots:
    void doWork(unsigned int count);
signals:
    void done(Field::Statistics* stats);
    void progress(int value);
private:
    StatisticsTab* st;
    FieldGenerator fieldGenerator;
};

class GameTab : public QWidget {
    Q_OBJECT
public:
    explicit GameTab(MainWindow* parent);
    ~GameTab();
public slots:
    void play();
private:
    void update();

    size_t currentWin, totalWin, totalBet;
    Field field;
    FieldGenerator fieldGenerator;
    MainWindow* parent;
    QHBoxLayout mainLayout;
    QVBoxLayout infoLayout;
    QLabel fieldLabel;
    QPushButton startButton;
    QLabel currentWinLabel;
    QLabel totalWinLabel;
    QLabel totalBetLabel;
    QImage fieldImage;
};


class StatisticsTab : public QWidget {
    Q_OBJECT
public:
    explicit StatisticsTab(MainWindow* parent);
    ~StatisticsTab();
public slots:
    void run();
    void updateProgress(int value);
    void update(Field::Statistics* stats);
signals:
    void calculate(unsigned int count);
private:
    MainWindow* parent;
    Worker* worker;
    QThread thread;

    QVBoxLayout mainLayout;
    QGridLayout tableLayout;
    QLineEdit lineEdit;
    QPushButton runButton;
    QLabel expectedValueLabel;
    QLabel standartDeviationLabel;
    QLabel hitLabel;
    QProgressBar progressBar;
    QLabel table[4][9];

    friend Worker;
};




class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QTabWidget* tabWidget;
    GameTab* gameTab;
    StatisticsTab* statisticsTab;
    QImage icons[8];

    friend GameTab;
    friend StatisticsTab;
};


#endif // MAINWINDOW_H
