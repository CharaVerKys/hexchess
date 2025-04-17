#include "boardscene.hpp"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    BoardSceneWidget w;
    w.show();

    return app.exec();
}