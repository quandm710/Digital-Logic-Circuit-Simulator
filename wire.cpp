#include "wire.h"
#include "logicgateitem.h"
#include <QGraphicsLineItem>


WireItem::WireItem(PinItem* start, PinItem* end, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), m_startPin(start), m_endPin(end) {

    setZValue(-1); // Cho dây nằm dưới Pin và Cổng
    setAcceptedMouseButtons(Qt::NoButton); // Dây không nhận sự kiện chuột
    setPen(QPen(Qt::blue, 2));
    updatePosition();
}
void WireItem::transmit() {
    if (!m_startPin || !m_endPin) return;

    // Quy tắc: Luôn truyền từ chân OUTPUT sang chân INPUT
    PinItem* source = (!m_startPin->isInput()) ? m_startPin : m_endPin;
    PinItem* target = (m_startPin->isInput()) ? m_startPin : m_endPin;

    // Kiểm tra xem có đúng 1 đầu ra 1 đầu vào không
    if (source->isInput() == false && target->isInput() == true) {
        target->setValue(source->value());

        // Sau khi chân Input của cổng đích nhận giá trị mới, bảo cổng đó tính toán
        if (auto gate = dynamic_cast<LogicGateItem*>(target->parentItem())) {
            gate->compute();
        }
    }
}
void WireItem::updatePosition() {
    if (m_startPin && m_endPin) {
        setLine(QLineF(m_startPin->scenePos(), m_endPin->scenePos()));
    }
}
