#include "matchlistwidget.hpp"
#include "matchwidget.hpp"

MatchListWidget::MatchListWidget(QWidget *parent)
    : QWidget(parent)
{
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    containerWidget = new QWidget;
    containerLayout = new QVBoxLayout(containerWidget);
    containerLayout->setAlignment(Qt::AlignTop);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(1);

    scrollArea->setWidget(containerWidget);
    scrollArea->setMinimumHeight(250);
    scrollArea->setMaximumHeight(750);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");
    scrollArea->viewport()->setStyleSheet("background-color: #c97046;");

    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MatchListWidget::initList(lhc::protocol::payload::listOfAllMatches const& all){

    for(auto const& each : all.vec){
        addMatchWidget(each.side, each.id);
    }
    setFixedWidth(size().width());
}

void MatchListWidget::addMatchWidget(figure_side side, lhc::unique_id id)
{
    // static bool first = true;
    // if(first){
    //     first = false;
    // }else{
    // }

    auto *widget = new MatchWidget(this, side, id);
    containerLayout->addWidget(widget);
    connect(widget, &MatchWidget::connectClicked, this, &MatchListWidget::choosenMatch);

    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setLineWidth(1);
    separator->setMidLineWidth(0);
    containerLayout->addWidget(separator);
}