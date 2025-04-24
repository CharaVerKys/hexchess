#pragma once

#include "QMainWindow"
#include "protocol.hpp"
#include "types.hpp"

class MatchListWidget;
class ButtonsWidget;

class ControlWindow : public QMainWindow{
    Q_OBJECT
public:
    ControlWindow();

public slots:
    void onChoiceMatch(lhc::unique_id id);
    void onListOfAllMatchesReceived(lhc::protocol::payload::listOfAllMatches);


signals:
    void connectToMatch(lhc::unique_id id);
    void createMatch(figure_side);
    void deleteMatch();
    void exitClicked();
    void requestListOfAllMatches();


private:
    bool matchCreated = false;
    ButtonsWidget* buttonsWidget;
    MatchListWidget* matchesList;
};