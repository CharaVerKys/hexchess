#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include "protocol.hpp"
#include "types.hpp"

class MatchListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchListWidget(QWidget *parent = nullptr);

    void initList(lhc::protocol::payload::listOfAllMatches const&);

signals:
    void choosenMatch(lhc::unique_id id);

private:
    void addMatchWidget(figure_side side, lhc::unique_id id);
    // void setStyles();

private:
    QVBoxLayout *containerLayout;
    QWidget *containerWidget;
};