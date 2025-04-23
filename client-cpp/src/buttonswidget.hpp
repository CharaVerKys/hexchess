#pragma once

#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>

class ButtonsWidget : public QWidget {
    Q_OBJECT
public:
    ButtonsWidget(QWidget *parent = nullptr);
signals:
    void createMatchClicked();
    void deleteMatchClicked();
    void updateClicked();
    void exitClicked();

private:
    QLabel* status;
    QPushButton *createMatchBtn;
    QPushButton *deleteMatchBtn;
    QPushButton *updateBtn;
    QPushButton *exitBtn;
};