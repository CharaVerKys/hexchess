
#include "protocol.hpp"
#include <qgraphicsitem.h>


struct GraphicPeace{
    using ptr_type = std::unique_ptr<QGraphicsPixmapItem,std::function<void(QGraphicsPixmapItem*)>>;

    GraphicPeace(const GraphicPeace &) = delete;
    GraphicPeace(GraphicPeace &&) = default;
    GraphicPeace &operator=(const GraphicPeace &) = delete;
    GraphicPeace &operator=(GraphicPeace &&) = default;

    GraphicPeace(ptr_type &&item, figure_type t, lhc::position p, figure_side s)
        : item(std::move(item)), type(t), position(p), side(s) {}

    ptr_type item = nullptr;
    figure_type type;
    lhc::position position;
    figure_side side;
};