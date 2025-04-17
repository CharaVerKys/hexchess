#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <types.hpp>
#include "graphicpeace.hpp"
#include "protocol.hpp"

class BoardSceneWidget : public QWidget {
    Q_OBJECT
public:
    BoardSceneWidget(QWidget* parent = nullptr);

    void setAllPeaces(std::vector<lhc::protocol::payload::peace> const& initialPeaces);

private:
    bool eventFilter(QObject* obj, QEvent* ev) override;
    static QPolygonF createHexagon(double x, double y);

    QBrush choiceBrushColor(Color const&);
    void initAllCells();

    void setPeace(figure_type const&, lhc::position const&, figure_side const&);
    HexAndPeace& getCellAt(lhc::position const&);
    GraphicPeace::ptr_type getPeaceGraphicItem(figure_type const&, figure_side const&);

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    std::optional<std::array<HexAndPeace,91>> allCells;
    static constexpr std::uint8_t hex_size = 40;
};