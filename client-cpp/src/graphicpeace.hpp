#include "types.hpp"
#include <qgraphicsitem.h>


struct GraphicPeace{
    using ptr_type = std::unique_ptr<QGraphicsPixmapItem,std::function<void(QGraphicsPixmapItem*)>>;

    GraphicPeace(const GraphicPeace &) = delete;
    GraphicPeace(GraphicPeace &&) = default;
    GraphicPeace &operator=(const GraphicPeace &) = delete;
    GraphicPeace &operator=(GraphicPeace &&) = default;

    GraphicPeace(ptr_type &&item, figure_type t, figure_side s)
        : item(std::move(item)), type(t), side(s) {}

    ptr_type item = nullptr;
    figure_type type;
    figure_side side;
};

struct HexAndPeace{
    QGraphicsPolygonItem* hex = nullptr;
    std::optional<GraphicPeace> peace;
    lhc::position position;
};