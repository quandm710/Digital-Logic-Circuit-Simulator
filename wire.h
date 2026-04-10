#ifndef WIRE_H
#define WIRE_H
#include <QGraphicsPathItem>
#include "LogicGateItem.h"

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
#endif // WIRE_H
