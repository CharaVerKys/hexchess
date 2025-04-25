#include "boardscene.hpp"
#include <constants.hpp>
#include <field_ranges.hpp>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <cmath>
#include <QVBoxLayout>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>


BoardSceneWidget::BoardSceneWidget(QWidget* parent)
    : QWidget(parent), scene(new QGraphicsScene(this)), view(new QGraphicsView(scene, this)) {
    view->setRenderHint(QPainter::Antialiasing);
    scene->installEventFilter(this);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    initAllCells();
}

void BoardSceneWidget::setAllPieces(std::vector<lhc::protocol::payload::piece> const& initialPieces){
    for(auto const& piece : initialPieces){
        setPiece_private(piece.type, piece.position, piece.side);
    }
}

void BoardSceneWidget::deletePiece(lhc::position pos, figure_type type, figure_side side){
    auto& cell = getCellAt(pos);
    assert(cell.piece->type == type);
    assert(cell.piece->side == side);
    cell.piece = std::nullopt;
}

void BoardSceneWidget::setPiece(lhc::position pos, figure_type type, figure_side side){
    setPiece_private(type, pos, side);
    if(oldColor){
        oldColor->first->setBrush(oldColor->second);
        oldColor = std::nullopt;
    } 
}

void BoardSceneWidget::colorizeCell(QGraphicsPolygonItem* item){
    assert(item);
    oldColor = std::make_pair(item,item->brush());
    item->setBrush({QColor{0x80,0x4D,0x62}});
}

HexAndPiece const& BoardSceneWidget::findCellByGraphItem(QGraphicsPolygonItem*ptr){
    assert(ptr);
    for(HexAndPiece const& cell : *allCells){
        if(ptr == cell.hex){
            return cell;
        }
    }
    std::abort();
}

HexAndPiece const& BoardSceneWidget::findCellByGraphItem(QGraphicsPixmapItem*ptr){
    assert(ptr);
    for(HexAndPiece const& cell : *allCells){
        if(cell.piece.has_value()){
            if(ptr == cell.piece->item.get()){
                return cell;
            }
        }
    }
    std::abort();
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
    //?gcc fix
    std::abort();
}
void BoardSceneWidget::initAllCells(){
    assert(not allCells);
    allCells = std::array<HexAndPiece,91>{};

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
            allCells->at(indexInAllCells) = {item,std::nullopt,{currentColumn,currentRow}};
            ++indexInAllCells;
        }
        ++curFirstColor;
    }
}

bool BoardSceneWidget::eventFilter(QObject* obj, QEvent* ev) {
    if (obj == scene && ev->type() == QEvent::GraphicsSceneMousePress) {
        bool clickWasOnCell = false;
        if(oldColor){
            oldColor->first->setBrush(oldColor->second);
            oldColor = std::nullopt;
        } 
        auto* me = static_cast<QGraphicsSceneMouseEvent*>(ev);
        auto hits = scene->items(me->scenePos());
        if (!hits.isEmpty()) {
            auto *hex = qgraphicsitem_cast<QGraphicsPolygonItem*>(hits.first());
            auto *pix = qgraphicsitem_cast<QGraphicsPixmapItem*>(hits.first());
            if (hex) {
                clickWasOnCell = true;
                auto& cell = findCellByGraphItem(hex);
                colorizeCell(cell.hex);
                if(cell.piece){
                    emit clicked(cell.position, cell.piece->type, cell.piece->side);
                }else{
                    emit clicked(cell.position, std::nullopt, std::nullopt);
                }
            }
            else if (pix) {
                clickWasOnCell = true;
                auto& cell = findCellByGraphItem(pix);
                colorizeCell(cell.hex);
                emit clicked(cell.position, cell.piece->type, cell.piece->side);
            }
        }
        if(not clickWasOnCell){
            emit clicked(std::nullopt,std::nullopt,std::nullopt);
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void BoardSceneWidget::setPiece_private(figure_type const& type, lhc::position const& pos, figure_side const& side){
    HexAndPiece& cell = getCellAt(pos);
    assert(cell.piece == std::nullopt);
    GraphicPiece piece{getPieceGraphicItem(type,side),type,side};

    QPointF center = cell.hex->boundingRect().center();
    center = cell.hex->mapToScene(center); // переводим в координаты сцены
    piece.item->setOffset(
        center.x() - static_cast<float>(piece.item->pixmap().width()  )/2,
        center.y() - static_cast<float>(piece.item->pixmap().height() )/2
    );
    scene->addItem(piece.item.get());

    cell.piece = std::move(piece);
}

HexAndPiece& BoardSceneWidget::getCellAt(lhc::position const& pos){
    assert(pos.column <12);
    auto range = lhc::field_ranges();
    auto depth = range.begin();
    for(std::uint8_t i{0};i<pos.column;++i){++depth;}
    auto pair = *depth;
    assert(pos.row < pair.take);
    return allCells->at(pair.drop + pos.row);
}

GraphicPiece::ptr_type BoardSceneWidget::getPieceGraphicItem(figure_type const& type, figure_side const& side){
     QString baseDir = "/home/charaverk/Pictures/chess/";//todo
     QPixmap map;
     QString wb;
     switch (side) {
        case figure_side::white:{
            wb = "w";
        }break;
        case figure_side::black:{
            wb = "b";
        }break;
        /*unreachable*/case figure_side::invalid: std::abort();
     }

     switch (type) {
        case figure_type::pawn:{
            map.load(baseDir.append("pawn-%1.svg").arg(wb));
        }break;
        case figure_type::bishop:{
            map.load(baseDir.append("bishop-%1.svg").arg(wb));
        }break;
        case figure_type::knight:{
            map.load(baseDir.append("knight-%1.svg").arg(wb));
        }break;
        case figure_type::rook:{
            map.load(baseDir.append("rook-%1.svg").arg(wb));
        }break;
        case figure_type::queen:{
            map.load(baseDir.append("queen-%1.svg").arg(wb));
        }break;
        case figure_type::king:{
            map.load(baseDir.append("king-%1.svg").arg(wb));
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