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

void BoardSceneWidget::setAllPeaces(std::vector<lhc::protocol::payload::peace> const& initialPeaces){
    for(auto const& peace : initialPeaces){
        setPeace(peace.type, peace.position);
    }
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
        for(std::uint8_t currentRow = 0; currentRow < range->take; ++currentRow){
            QBrush brushColor = choiceBrushColor(curColor);
            lhc::color::next(curColor);
            QPolygonF hex = createHexagon(currentX, std::sqrt(3) / 2 * hex_size * 2 * (currentRow+skipForGridLogic[currentColumn]) + (currentColumn%2 * sqrt(3)/2*hex_size));
            QGraphicsPolygonItem* item = scene->addPolygon(hex, QPen(Qt::black), brushColor);
            allCells->at(indexInAllCells) = item;
            ++indexInAllCells;
        }
        ++curFirstColor;
    }
}

void BoardSceneWidget::setPeace(figure_type const& type, lhc::position const& pos){
    QGraphicsPolygonItem* polyItem = getCellAt(pos);
    GraphicPeace peace{getPeaceGraphicItem(type),type,pos};

    QPointF center = polyItem->boundingRect().center();
    center = polyItem->mapToScene(center); // переводим в координаты сцены
    peace.item->setOffset(
        center.x() - static_cast<float>(peace.item->pixmap().width()  )/2,
        center.y() - static_cast<float>(peace.item->pixmap().height() )/2
    );
    scene->addItem(peace.item.get());

    allPeaces.push_back(std::move(peace));
}

QGraphicsPolygonItem* BoardSceneWidget::getCellAt(lhc::position const& pos){
    assert(pos.column <12);
    auto range = lhc::field_ranges();
    auto depth = range.begin();
    for(std::uint8_t i{0};i<pos.column;++i){++depth;}
    auto pair = *depth;
    assert(pos.row < pair.take);
    return allCells->at(pair.drop + pos.row);
}

GraphicPeace::ptr_type BoardSceneWidget::getPeaceGraphicItem(figure_type const& type){
     std::string baseDir = "/home/charaverk/Pictures/chess/";
     QPixmap map;
     switch (type) {
        case figure_type::pawn:{
            map.load(baseDir.append("pawn-w.svg").c_str());
        }break;
        case figure_type::bishop:{
            map.load(baseDir.append("bishop-w.svg").c_str());
        }break;
        case figure_type::knight:{
            map.load(baseDir.append("knight-w.svg").c_str());
        }break;
        case figure_type::rook:{
            map.load(baseDir.append("rook-w.svg").c_str());
        }break;
        case figure_type::queen:{
            map.load(baseDir.append("queen-w.svg").c_str());
        }break;
        case figure_type::king:{
            map.load(baseDir.append("king-w.svg").c_str());
        }break;
        /*unreachable*/case figure_type::invalid: std::abort();
    }//switch 
    QPixmap scaled = map.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QGraphicsPixmapItem *pixItem = new QGraphicsPixmapItem(scaled);
    return {
        pixItem,
        [scene = this->scene](QGraphicsPixmapItem* item){
            scene->removeItem(item); delete item;
        }
    };
}