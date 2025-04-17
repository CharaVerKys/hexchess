#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <types.hpp>

class BoardSceneWidget : public QWidget {
    Q_OBJECT
public:
    BoardSceneWidget(QWidget* parent = nullptr);

private:
    static QPolygonF createHexagon(double x, double y);

    QBrush choiceBrushColor(Color const&);
    void initAllCells();

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    std::optional<std::array<QGraphicsPolygonItem*,91>> allCells;
    std::vector<QGraphicsPixmapItem*> allPeaces;
    static constexpr std::uint8_t hex_size = 40;
};