#include "mainwindow.h"

#include <QtDebug>
#include <QThread>
#include <QPainter>
#include <QPixmap>
#include <QHeaderView>

Worker::Worker(StatisticsTab* st): st(st) {
}

Worker::~Worker() {
}

void Worker::doWork(unsigned int count) {
    unsigned long long ex = 0, ex2 = 0, hits = 0;
    Field field;
    Field::Combinations combs;
    unsigned int win;
    unsigned int combStats[3][8];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            combStats[i][j] = 0;
        }
    }
    for (unsigned long long i = 0; i < count; i++) {
        fieldGenerator.generate(field);
        combs = field.combinations();
        win = Field::win(combs);
        if (win > 0) {
            hits++;
            ex += win;
            ex2 += win * win;
            for (unsigned int j = 0; j < combs.size; j++) {
                bool unique = 1;
                for (unsigned int k = 0; k < j; k++) {
                    unique = unique && (combs.a[j][1] != combs.a[k][1] || combs.a[j][0] != combs.a[k][0]);
                }
                if (unique) {
                    combStats[combs.a[j][1] - 3][combs.a[j][0]]++;
                }
            }
        }
        if (i * 100 / count != (i + 1) * 100 / count) {
            emit progress(int((i + 1) * 100 / count));
        }
    }
    Field::Statistics* stats = new Field::Statistics;
    stats->expectedValue = double(ex) / count / 5;
    stats->hit = double(hits) / count;
    stats->standartDeviation = sqrt((double(ex2) / count / 25 - stats->expectedValue * stats->expectedValue) / count);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            stats->a[i][j] = double(combStats[i][j]) / count;
        }
    }
    emit done(stats);
}

GameTab::GameTab(MainWindow* parent): parent(parent), fieldImage(QImage(5 * 150, 3 * 150, QImage::Format_ARGB32)) {
    currentWin = 0;
    totalWin = 0;
    totalBet = 0;
    startButton.setText(tr("Start"));
    mainLayout.addWidget(&fieldLabel);
    mainLayout.addLayout(&infoLayout);
    infoLayout.addWidget(&currentWinLabel);
    infoLayout.addWidget(&totalWinLabel);
    infoLayout.addWidget(&totalBetLabel);
    infoLayout.addWidget(&startButton);
    infoLayout.setAlignment(Qt::AlignTop);
    setLayout(&mainLayout);
    update();
    QObject::connect(&startButton, SIGNAL(pressed()), this, SLOT(play()));
}

GameTab::~GameTab() {
}

void GameTab::play() {
    fieldGenerator.generate(field);
    totalBet += 5;
    currentWin = field.win();
    totalWin += currentWin;
    update();
}

void GameTab::update() {
    QPainter painter(&fieldImage);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 5; j++) {
            painter.drawImage(150 * j, 150 * i, parent->icons[field.at(i, j)]);
        }
    }
    painter.end();
    fieldLabel.setPixmap(QPixmap::fromImage(fieldImage));
    currentWinLabel.setText(tr("Current win %1").arg(currentWin));
    totalWinLabel.setText(tr("Total win %1").arg(totalWin));
    totalBetLabel.setText(tr("Total bet %1").arg(totalBet));
}


StatisticsTab::StatisticsTab(MainWindow* parent): parent(parent) {
    runButton.setText(tr("Run"));
    expectedValueLabel.setText(tr("Expected value"));
    standartDeviationLabel.setText(tr("Standart deviation"));
    hitLabel.setText(tr("Hit"));
    progressBar.setRange(0, 100);
    mainLayout.addLayout(&tableLayout);
    mainLayout.addWidget(&expectedValueLabel);
    mainLayout.addWidget(&standartDeviationLabel);
    mainLayout.addWidget(&hitLabel);
    mainLayout.addWidget(&lineEdit);
    mainLayout.addWidget(&runButton);
    mainLayout.addWidget(&progressBar);
    mainLayout.setAlignment(Qt::AlignTop);
    setLayout(&mainLayout);
    for (int i = 0; i < 4; i++) {
        for (int j  = 0; j < 9; j++) {
            tableLayout.addWidget(&table[i][j], i, j);
        }
    }
    for (int i = 0; i < 8; i++) {
        table[0][i + 1].setPixmap(QPixmap::fromImage(parent->icons[i].scaled(98, 98)));
    }
    for (int i = 0; i < 3; i++) {
        table[i + 1][0].setText((QString("%1   ").arg(i + 3)));
    }

    lineEdit.setFixedWidth(100);
    runButton.setFixedWidth(100);
    progressBar.setFixedWidth(134);

    worker = new Worker(this);
    worker->moveToThread(&thread);

    connect(&runButton, SIGNAL(pressed()), this, SLOT(run()));
    connect(this, SIGNAL(calculate(unsigned int)), worker, SLOT(doWork(unsigned int)));
    connect(worker, SIGNAL(done(Field::Statistics*)), this, SLOT(update(Field::Statistics*)));
    connect(worker, SIGNAL(progress(int)), this, SLOT(updateProgress(int)));

    thread.start();
}

StatisticsTab::~StatisticsTab() {
    thread.quit();
    thread.wait();
    delete worker;
}

void StatisticsTab::run() {
    unsigned int count = lineEdit.text().toUInt();
    if (count == 0) {
        return;
    }
    progressBar.reset();
    runButton.setDisabled(1);
    emit calculate(count);
}

void StatisticsTab::updateProgress(int value) {
    progressBar.setValue(value);
}

void StatisticsTab::update(Field::Statistics* stats) {
    expectedValueLabel.setText(tr("Expected value: %1").arg(stats->expectedValue));
    standartDeviationLabel.setText(tr("Standart deviation: %1").arg(stats->standartDeviation));
    hitLabel.setText(tr("Hit: %1").arg(stats->hit));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            table[i + 1][j + 1].setText(QString("%1").arg(stats->a[i][j]));
        }
    }
    runButton.setDisabled(0);
    delete stats;
}


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    for (int i = 0; i < 8; i++) {
        QImage icon(QString(":/images/%1.png").arg(i + 1));
        icon = icon.scaled(128, 128);
        QImage background(150, 150, QImage::Format_RGB32);
        background.fill(Qt::white);
        QPainter painter(&background);
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.drawImage(11, 11, icon);
        icons[i] = background;
    }
    gameTab = new GameTab(this);
    statisticsTab = new StatisticsTab(this);
    tabWidget = new QTabWidget;
    tabWidget->addTab(gameTab, tr("Game"));
    tabWidget->addTab(statisticsTab, tr("Statistics"));
    setCentralWidget(tabWidget);
    setWindowTitle(tr("Slot machine & statistics"));
}

MainWindow::~MainWindow() {
}
