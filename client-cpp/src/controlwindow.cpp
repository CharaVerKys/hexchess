#include "controlwindow.hpp"
#include "buttonswidget.hpp"
#include "matchlistwidget.hpp"
#include <qapplication.h>

ControlWindow::ControlWindow(QWidget* parent)
    : QWidget(parent)
    , buttonsWidget(new ButtonsWidget(this))
    , matchesList(new MatchListWidget(this))
{
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    assert(vboxLayout == layout());
    setStyleSheet("background-color: #c98c46;");
    vboxLayout->setContentsMargins(0,0,0,0);
    vboxLayout->addWidget(matchesList);
    vboxLayout->addWidget(buttonsWidget);
    connect(matchesList, &MatchListWidget::choosenMatch,this,&ControlWindow::onChoiceMatch);
    connect(buttonsWidget, &ButtonsWidget::exitClicked,this,&ControlWindow::exitClicked);
    connect(buttonsWidget, &ButtonsWidget::updateClicked,this,&ControlWindow::requestListOfAllMatches);
    connect(buttonsWidget, &ButtonsWidget::createMatchClicked,this,&ControlWindow::createMatch);
    connect(buttonsWidget, &ButtonsWidget::deleteMatchClicked,this,&ControlWindow::deleteMatch);
    setMaximumWidth(buttonsWidget->sizeHint().width());
}

void ControlWindow::onChoiceMatch(lhc::unique_id id){
    if(matchCreated){
        buttonsWidget->clickAtDeleteMatchBtn();
        //trigger signal
    }
    emit connectToMatch(id);
}

void ControlWindow::onListOfAllMatchesReceived(lhc::protocol::payload::listOfAllMatches all){
    matchesList->setParent(nullptr);
    matchesList->adjustSize();
    matchesList->initList(all);
    layout()->addWidget(matchesList);
    layout()->addWidget(buttonsWidget);
    setFixedWidth(std::max(matchesList->size().width(), buttonsWidget->size().width()));
}
void ControlWindow::cantCreateMatch(){
    buttonsWidget->clickAtDeleteMatchBtn();
}