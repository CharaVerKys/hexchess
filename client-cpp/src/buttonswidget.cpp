#include "buttonswidget.hpp"

ButtonsWidget::ButtonsWidget(QWidget *parent): QWidget(parent) {
    status = new QLabel("connect to match\nor create",this);
    createMatchBtn = new QPushButton("Create Match",this);
    deleteMatchBtn = new QPushButton("Delete Match",this);
    updateBtn = new QPushButton("Update",this);
    exitBtn = new QPushButton("Exit",this);

    deleteMatchBtn->setEnabled(false);
    connect(createMatchBtn, &QPushButton::clicked, this, [this]() {
        createMatchBtn->setEnabled(false);
        deleteMatchBtn->setEnabled(true);
        status->setText("waiting\nfor players...");
        emit createMatchClicked();
    });
    connect(deleteMatchBtn, &QPushButton::clicked, this, [this]() {
        deleteMatchBtn->setEnabled(false);
        createMatchBtn->setEnabled(true);
        status->setText("connect to match\nor create");
        emit deleteMatchClicked();
    });
    connect(updateBtn, &QPushButton::clicked, this, &ButtonsWidget::updateClicked);
    connect(exitBtn, &QPushButton::clicked, this, &ButtonsWidget::exitClicked);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(status);
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