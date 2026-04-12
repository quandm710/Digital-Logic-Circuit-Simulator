#include "wire.h"
#include "logicgateitem.h"
#include <QGraphicsLineItem>


WireItem::WireItem(PinItem* start, PinItem* end, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), m_startPin(start), m_endPin(end) {
    setPen(QPen(Qt::blue, 2));
    updatePosition();
}
void WireItem::transmit() {
    if (m_startPin && m_endPin) {
        m_endPin->setValue(m_startPin->value());
        // Sau khi truyền, bảo cổng đích tính toán lại
        if (auto gate = dynamic_cast<LogicGateItem*>(m_endPin->parentItem())) {
            gate->compute();
        }
    }
}
void WireItem::updatePosition() {
    if (m_startPin && m_endPin) {
        setLine(QLineF(m_startPin->scenePos(), m_endPin->scenePos()));
    }
}
