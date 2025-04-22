#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <types.hpp>
#include "graphicpiece.hpp"
#include "protocol.hpp"

class BoardSceneWidget : public QWidget {
    Q_OBJECT
public:
    BoardSceneWidget(QWidget* parent = nullptr);

    void setAllPieces(std::vector<lhc::protocol::payload::piece> const& initialPieces);

signals:
    void clicked(std::optional<lhc::position>, std::optional<figure_type>, std::optional<figure_side>);
public slots:
    void deletePiece(lhc::position, figure_type, figure_side);
    void setPiece(lhc::position, figure_type, figure_side);

private slots:
    void colorizeCell(QGraphicsPolygonItem*);

private:
    HexAndPiece const& findCellByGraphItem(QGraphicsPolygonItem*);
    HexAndPiece const& findCellByGraphItem(QGraphicsPixmapItem*);
    static QPolygonF createHexagon(double x, double y);

    QBrush choiceBrushColor(Color const&);
    void initAllCells();

    bool eventFilter(QObject* obj, QEvent* ev) override;
    void setPiece_private(figure_type const&, lhc::position const&, figure_side const&);
    HexAndPiece& getCellAt(lhc::position const&);
    GraphicPiece::ptr_type getPieceGraphicItem(figure_type const&, figure_side const&);

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    std::optional<std::array<HexAndPiece,91>> allCells;
    static constexpr std::uint8_t hex_size = 40;
    std::optional<std::pair<QGraphicsPolygonItem*,QBrush>> oldColor;
};