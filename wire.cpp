#include "wire.h"
#include "logicgateitem.h"
#include <QPainterPath>

WireItem::WireItem(PinItem* start, PinItem* end, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), m_startPin(start), m_endPin(end) {

    setPen(QPen(Qt::blue, 2)); // Dây màu xanh chuyên nghiệp
    setZValue(-1);             // Cho dây nằm dưới Pin
    setAcceptedMouseButtons(Qt::NoButton); // Quan trọng: Dây không được cướp chuột của Pin
    updatePosition();
}

void WireItem::updatePosition() {
    QPointF p1 = m_startPin->scenePos();
    QPointF p2 = m_endPin->scenePos();

    QPainterPath path;
    path.moveTo(p1);

    // Tính điểm trung gian để bo góc vuông
    qreal midX = (p1.x() + p2.x()) / 2;

    // Vẽ: từ p1 → midX,p1.y → midX,p2.y → p2
    path.lineTo(midX, p1.y());
    path.lineTo(midX, p2.y());
    path.lineTo(p2);

    setPath(path);
}
void WireItem::transmit() {
    if (!m_startPin || !m_endPin) return;

    // Lấy giá trị từ startPin truyền sang endPin
    bool valStart = m_startPin->value();
    m_endPin->setValue(valStart);

    // Đồng bộ ngược lại: lấy giá trị từ endPin truyền sang startPin
    bool valEnd = m_endPin->value();
    m_startPin->setValue(valEnd);
}