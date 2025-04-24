#pragma once

#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qwidget.h>
#include "types.hpp"

class ButtonsWidget : public QWidget {
    Q_OBJECT
public:
    ButtonsWidget(QWidget *parent = nullptr);
    void clickAtDeleteMatchBtn(){deleteMatchBtn->click();}

signals:
    void createMatchClicked(figure_side);
    void deleteMatchClicked();
    void updateClicked();
    void exitClicked();

private slots:
    void onCreateMatchClicked();
    void onChoiceSide(QAbstractButton* btn);

private:
    QLabel* status;
    QToolButton *btnWhite;
    QToolButton *btnBlack;
    QPushButton *createMatchBtn;
    QPushButton *deleteMatchBtn;
    QPushButton *updateBtn;
    QPushButton *exitBtn;
};