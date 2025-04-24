#include "controlwindow.hpp"
#include "buttonswidget.hpp"
#include "matchlistwidget.hpp"

ControlWindow::ControlWindow()
    : QMainWindow(nullptr)
    , buttonsWidget(new ButtonsWidget(this))
    , matchesList(new MatchListWidget(this))
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout* vboxLayout = new QVBoxLayout(centralWidget);
    centralWidget->setStyleSheet("background-color: #c98c46;");
    vboxLayout->setContentsMargins(0,0,0,0);
    vboxLayout->addWidget(matchesList);
    vboxLayout->addWidget(buttonsWidget);
    this->setCentralWidget(centralWidget);
    connect(matchesList, &MatchListWidget::choosenMatch,this,&ControlWindow::onChoiceMatch);
    connect(buttonsWidget, &ButtonsWidget::exitClicked,this,&ControlWindow::exitClicked);
    connect(buttonsWidget, &ButtonsWidget::updateClicked,this,&ControlWindow::requestListOfAllMatches);
    connect(buttonsWidget, &ButtonsWidget::createMatchClicked,this,&ControlWindow::createMatch);
    connect(buttonsWidget, &ButtonsWidget::deleteMatchClicked,this,&ControlWindow::deleteMatch);
}

void ControlWindow::onChoiceMatch(lhc::unique_id id){
    if(matchCreated){
        buttonsWidget->clickAtDeleteMatchBtn();
        //trigger signal
    }
    emit connectToMatch(id);
}

void ControlWindow::onListOfAllMatchesReceived(lhc::protocol::payload::listOfAllMatches all){
    matchesList->initList(all);
}