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

signals:
    void clicked(lhc::position, std::optional<figure_type>, std::optional<figure_side>);
public slots:
    void deletePeace(lhc::position, figure_type, figure_side);
    void setPeace(lhc::position, figure_type, figure_side);

private slots:
    void colorizeCell(QGraphicsPolygonItem*);

private:
    HexAndPeace const& findCellByGraphItem(QGraphicsPolygonItem*);
    HexAndPeace const& findCellByGraphItem(QGraphicsPixmapItem*);
    static QPolygonF createHexagon(double x, double y);

    QBrush choiceBrushColor(Color const&);
    void initAllCells();

    bool eventFilter(QObject* obj, QEvent* ev) override;
    void setPeace_private(figure_type const&, lhc::position const&, figure_side const&);
    HexAndPeace& getCellAt(lhc::position const&);
    GraphicPeace::ptr_type getPeaceGraphicItem(figure_type const&, figure_side const&);

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    std::optional<std::array<HexAndPeace,91>> allCells;
    static constexpr std::uint8_t hex_size = 40;
    std::optional<std::pair<QGraphicsPolygonItem*,QBrush>> oldColor;
};