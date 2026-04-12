#ifndef WIRE_H
#define WIRE_H
#include <QGraphicsPathItem>
#include "logicgateitem.h"

class PinItem;
class Wire : public QGraphicsPathItem {
public:
    Wire(PinItem* start, PinItem* end) : m_start(start), m_end(end) {
        setPen(QPen(Qt::darkGreen, 2));
        updatePath();
    }

    // RAII: Khi Wire bị xóa, các tham chiếu liên quan phải được dọn dẹp
    void updatePath() {
        QPainterPath path;
        path.moveTo(m_start->scenePos());
        path.lineTo(m_end->scenePos());
        setPath(path);
    }

private:
    PinItem *m_start, *m_end;
};
class WireItem : public QGraphicsLineItem {
public:
    WireItem(PinItem* start, PinItem* end, QGraphicsItem* parent = nullptr);
    void updatePosition(); // Cập nhật khi cổng di chuyển
    void transmit();       // Truyền giá trị từ start sang end
private:
    PinItem *m_startPin;
    PinItem *m_endPin;
};
#endif // WIRE_H
