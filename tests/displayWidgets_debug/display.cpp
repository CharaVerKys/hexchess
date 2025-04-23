#include "matchlistwidget.hpp"
#include <QApplication>
#include <buttonswidget.hpp>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MatchListWidget w(nullptr);
    lhc::protocol::payload::listOfAllMatches all;
    all.vec =  
    {{figure_side::black, 123123},
    {figure_side::black, 55555555},
    {figure_side::white, 132333},};
    w.initList(all);
    w.setContentsMargins(0,0,0,0);
    w.show();
    

    ButtonsWidget w_;
    w_.show();

    return app.exec();
}