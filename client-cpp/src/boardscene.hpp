#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <types.hpp>
#include "graphicpeace.hpp"

class BoardSceneWidget : public QWidget {
    Q_OBJECT
public:
    BoardSceneWidget(QWidget* parent = nullptr);

    void setAllPeaces(std::vector<lhc::protocol::payload::peace> const& initialPeaces);

private:
    static QPolygonF createHexagon(double x, double y);

    QBrush choiceBrushColor(Color const&);
    void initAllCells();

    void setPeace(figure_type const&, lhc::position const&, figure_side const&);
    QGraphicsPolygonItem* getCellAt(lhc::position const&);
    GraphicPeace::ptr_type getPeaceGraphicItem(figure_type const&, figure_side const&);

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    std::optional<std::array<QGraphicsPolygonItem*,91>> allCells;
    std::vector<GraphicPeace> allPeaces;
    static constexpr std::uint8_t hex_size = 40;
};