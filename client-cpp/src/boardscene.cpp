#include "boardscene.hpp"
#include <constants.hpp>
#include <field_ranges.hpp>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <cmath>
#include <QVBoxLayout>

BoardSceneWidget::BoardSceneWidget(QWidget* parent)
    : QWidget(parent), scene(new QGraphicsScene(this)), view(new QGraphicsView(scene, this)) {
    view->setRenderHint(QPainter::Antialiasing);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    initAllCells();
}

QPolygonF BoardSceneWidget::createHexagon(double x, double y){
    QPolygonF hex;
    for (int i = 0; i < 6; ++i) {
        double radian = std::numbers::pi / 3 * i;
        hex << QPointF(x + hex_size * std::cos(radian), y + hex_size * std::sin(radian));
    }
    return hex;
}

QBrush BoardSceneWidget::choiceBrushColor(Color const& color){
    switch (color) {
        case Color::first:{
            return QBrush(QColor(255,207,159));
        }break;
        case Color::second:{
            return QBrush(QColor(233,173,112));
        }break;
        case Color::third:{
            return QBrush(QColor(210, 140, 70));
        }break;
        /*unreachable*/case Color::invalid: std::abort();
    }
}
void BoardSceneWidget::initAllCells(){
    assert(not allCells);
    allCells = std::array<QGraphicsPolygonItem*,91>{};

    [[maybe_unused]] constexpr std::uint8_t skipForGridLogic[11] = {3,2,2,1,1,0,1,1,2,2,3};
    auto curFirstColor = lhc::constants::firstColor.begin();
    auto ranges = lhc::field_ranges();
    auto range = ranges.begin();
    std::uint8_t indexInAllCells = 0;
    for(std::uint8_t currentColumn = 0; range not_eq ranges.end();++currentColumn, ++range){
        Color curColor = *curFirstColor;
        std::uint16_t currentX = currentColumn*hex_size*3/2;
        for(std::uint8_t currentRow = 0; currentRow < range->take; ++currentRow, ++indexInAllCells){
            QBrush brushColor = choiceBrushColor(curColor);
            lhc::color::next(curColor);
            QPolygonF hex = createHexagon(currentX, std::sqrt(3) / 2 * hex_size * 2 * (currentRow+skipForGridLogic[currentColumn]) + (currentColumn%2 * sqrt(3)/2*hex_size));
            scene->addPolygon(hex, QPen(Qt::black), brushColor);
        }
        ++curFirstColor;
    }
}