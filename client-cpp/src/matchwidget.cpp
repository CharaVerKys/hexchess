#include "matchwidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

MatchWidget::MatchWidget(QWidget *parent, figure_side side, lhc::unique_id id)
    : QWidget(parent),id(id)
{
    setStyles();


    if(side == figure_side::black){
        sideLabel = new QLabel("Black", this);
    }else if(side == figure_side::white){
        sideLabel = new QLabel("White", this);
    }
    numberLabel = new QLabel(QString::number(id), this);
    connectButton = new QPushButton("Connect", this);

    sideLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sideLabel->adjustSize();
    numberLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto *infoLayout = new QHBoxLayout;
    infoLayout->addWidget(sideLabel);
    infoLayout->addWidget(numberLabel);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(infoLayout);
    mainLayout->addWidget(connectButton);

    connect(connectButton, &QPushButton::clicked, this, &MatchWidget::connectClicked_trigger);
}

void MatchWidget::connectClicked_trigger(){
    emit connectClicked(id);
}

void MatchWidget::setStyles(){
    setStyleSheet(R"(
    QWidget {
        background-color: #e6c29f;
        border: 1px solid #b87b4b;
        border-radius: 10px;
        padding: 10px;
    }
    QLabel {
        font-size: 14px;
        color: #5b3e2b; 
    }
    QPushButton {
        background-color: #8b5e3c;
        color: white;
        padding: 6px 12px;
        border-radius: 6px;
    }
    QPushButton:hover {
        background-color: #a66834;
    }
    QPushButton:pressed {
        background-color: #7a4c26;
    }
)");
}