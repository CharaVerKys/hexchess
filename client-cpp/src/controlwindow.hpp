#pragma once

#include "QMainWindow"
#include "protocol.hpp"
#include "types.hpp"

class MatchListWidget;
class ButtonsWidget;

class ControlWindow : public QWidget{
    Q_OBJECT
public:
    ControlWindow(QWidget* parent = nullptr);

public slots:
    void onChoiceMatch(lhc::unique_id id);
    void onListOfAllMatchesReceived(lhc::protocol::payload::listOfAllMatches);
    void cantCreateMatch();
    void setText(QString);

signals:
    void connectToMatch(lhc::unique_id id);
    void createMatch(figure_side);
    void deleteMatch();
    void exitClicked();
    void requestListOfAllMatches();
    void setId(lhc::unique_id);


private:
    bool matchCreated = false;
    ButtonsWidget* buttonsWidget;
    MatchListWidget* matchesList;
};