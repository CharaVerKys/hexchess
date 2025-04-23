#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "types.hpp"

class MatchWidget : public QWidget
{
    Q_OBJECT

public:
    MatchWidget(QWidget *parent,figure_side side, lhc::unique_id id);

signals:
    void connectClicked(lhc::unique_id id);

private slots:
    void connectClicked_trigger();

private:
    void setStyles();

private:
    const lhc::unique_id id;
    QLabel *sideLabel;
    QLabel *numberLabel;
    QPushButton *connectButton;
};