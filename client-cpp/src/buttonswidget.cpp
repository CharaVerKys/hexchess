#include "buttonswidget.hpp"
#include <QButtonGroup>
#include <QToolButton>

ButtonsWidget::ButtonsWidget(QWidget *parent): QWidget(parent) {
    status = new QLabel("connect to match\nor create one",this);
    createMatchBtn = new QPushButton("Create Match",this);
    deleteMatchBtn = new QPushButton("Delete Match",this);
    updateBtn = new QPushButton("Update",this);
    exitBtn = new QPushButton("Exit",this);

    QString baseDir = "/home/charaverk/Pictures/chess/";//todo
    QPixmap map1(baseDir+"pawn-w.svg");
    QPixmap map2(baseDir+"pawn-b.svg");
    QPixmap ico1(map1.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QPixmap ico2(map2.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QHBoxLayout *choiceLayout = new QHBoxLayout;
    assert(choiceLayout->parent() == nullptr);
    (void)choiceLayout;

    btnWhite = new QToolButton(this);
    btnBlack = new QToolButton(this);

    btnWhite->setIcon(ico1);
    btnWhite->setContentsMargins(0,0,0,0);
    btnWhite->setIconSize(ico1.size());
    btnWhite->setCheckable(true);

    btnBlack->setIcon(ico2);
    btnBlack->setContentsMargins(0,0,0,0);
    btnBlack->setIconSize(ico2.size());
    btnBlack->setCheckable(true);

    QButtonGroup *group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(btnWhite, 0);
    group->addButton(btnBlack, 1);

    choiceLayout->addWidget(btnWhite);
    choiceLayout->addWidget(btnBlack);


    deleteMatchBtn->setEnabled(false);
    connect(createMatchBtn, &QPushButton::clicked, this, [this]() {
        createMatchBtn->setEnabled(false);
        deleteMatchBtn->setEnabled(false);
        status->setText("waiting\nfor players...");
        onCreateMatchClicked();
    });
    connect(deleteMatchBtn, &QPushButton::clicked, this, [this]() {
        deleteMatchBtn->setEnabled(false);
        createMatchBtn->setEnabled(true);
        status->setText("connect to match\nor create one");
        emit deleteMatchClicked();
    });
    connect(updateBtn, &QPushButton::clicked, this, &ButtonsWidget::updateClicked);
    connect(exitBtn, &QPushButton::clicked, this, &ButtonsWidget::exitClicked);
    connect(group,&QButtonGroup::buttonClicked,this,&ButtonsWidget::onChoiceSide);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(status);
    mainLayout->addLayout(choiceLayout);
    assert(choiceLayout->parent() == mainLayout);
    btnWhite->setVisible(false);
    btnBlack->setVisible(false);
    mainLayout->addWidget(createMatchBtn);
    mainLayout->addWidget(deleteMatchBtn);
    mainLayout->addWidget(updateBtn);

    QHBoxLayout *exitLayout = new QHBoxLayout;
    exitLayout->addStretch(); 
    exitLayout->addWidget(exitBtn);
    exitBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    mainLayout->addLayout(exitLayout);
    setLayout(mainLayout);

    setStyleSheet(R"(
        QWidget {
            background-color: #c98c46;
            border: 1px solid #b87b4b;
            border-radius: 10px;
            padding: 10px;
        }
        QLabel {
            font-size: 14px;
            color: #5b3e2b; 
            background-color: #e6c29f;
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
        QPushButton:disabled {
            background-color: #937233; 
            color: #aaaaaa;        
            border: none;
        }
        QPushButton:pressed {
            background-color: #7a4c26;
        }
    )");
}
void ButtonsWidget::onCreateMatchClicked(){
    assert(status->isVisible());
    status->setVisible(false);
    btnBlack->setVisible(true);
    btnWhite->setVisible(true);
}
void ButtonsWidget::onChoiceSide(QAbstractButton* btn){
    assert(not status->isVisible());
    btnBlack->setVisible(false);
    btnWhite->setVisible(false);
    status->setVisible(true);
    deleteMatchBtn->setEnabled(true);
    if(btn == btnWhite){
        emit createMatchClicked(figure_side::white);
    }else if(btn == btnBlack){
        emit createMatchClicked(figure_side::black);
    }
}