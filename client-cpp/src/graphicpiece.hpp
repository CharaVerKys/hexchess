#include "types.hpp"
#include <qgraphicsitem.h>


struct GraphicPiece{
    using ptr_type = std::unique_ptr<QGraphicsPixmapItem,std::function<void(QGraphicsPixmapItem*)>>;

    GraphicPiece(const GraphicPiece &) = delete;
    GraphicPiece(GraphicPiece &&) = default;
    GraphicPiece &operator=(const GraphicPiece &) = delete;
    GraphicPiece &operator=(GraphicPiece &&) = default;

    GraphicPiece(ptr_type &&item, figure_type t, figure_side s)
        : item(std::move(item)), type(t), side(s) {}

    ptr_type item = nullptr;
    figure_type type;
    figure_side side;
};

struct HexAndPiece{
    QGraphicsPolygonItem* hex = nullptr;
    std::optional<GraphicPiece> piece;
    lhc::position position;
};