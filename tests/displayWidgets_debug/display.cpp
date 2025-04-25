#include "clientcontroller.hpp"
#include "controlwindow.hpp"
#include "matchlistwidget.hpp"
#include <QApplication>
#include <buttonswidget.hpp>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    lhc::protocol::payload::listOfAllMatches all;
    all.vec =  
    {{figure_side::black, 123123},
    {figure_side::black, 55555555},
    {figure_side::white, 132333},};

    ClientController controller;
    controller.show();
    controller.receiveListOfAllMatches(all);

    // ControlWindow w;
    // w.show();
    // w.onListOfAllMatchesReceived(all);
    
    // MatchListWidget w_;
    // w_.show();
    // w_.initList(all);

    return app.exec();
}