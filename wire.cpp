#include "wire.h"
#include "logicgateitem.h"
#include <QPainterPath>

WireItem::WireItem(PinItem* start, PinItem* end, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), m_startPin(start), m_endPin(end) {

    setPen(QPen(Qt::blue, 2)); // Dây màu xanh chuyên nghiệp
    setZValue(-1.0);             // Cho dây nằm dưới Pin
    setAcceptedMouseButtons(Qt::NoButton); // Quan trọng: Dây không được cướp chuột của Pin
    updatePosition();
}
WireItem::~WireItem() {
    if (m_startPin) m_startPin->removeWire(this);
    if (m_endPin) m_endPin->removeWire(this);
}
void WireItem::updatePosition() {
    if (!m_startPin || !m_endPin) return;

    QPointF p1 = m_startPin->scenePos();
    QPointF p2 = m_endPin->scenePos();

    QPainterPath path;
    path.moveTo(p1);

    // Tính điểm trung gian để bo góc vuông
    qreal margin = 20.0;
    qreal offset = (m_startPin->scenePos().y() - m_endPin->scenePos().y()) * 0.1;
    qreal midX = (p1.x() + p2.x()) / 2;

    // Vẽ dây
    if (p1.x() < p2.x()) {
        // Trường hợp đơn giản: Dây đi tới (không bị cổng che)
        qreal midX = (p1.x() + p2.x()) / 2;
        path.lineTo(midX, p1.y());
        path.lineTo(midX, p2.y());
    }
    else {
        // TRƯỜNG HỢP NÉ CỔNG: Dây phải đi vòng ra sau hoặc vòng lên trên
        qreal escapeX = p1.x() + margin; // Đi ra ngoài một chút
        qreal escapeY;

        // Quyết định đi vòng lên trên hay xuống dưới cổng
        if (p1.y() < p2.y()) escapeY = p1.y() - (margin * 2);
        else escapeY = p1.y() + (margin * 2);

        path.lineTo(escapeX, p1.y());   // Đi ngang ra
        path.lineTo(escapeX, escapeY); // Đi dọc né thân cổng
        path.lineTo(p2.x() - margin, escapeY); // Đi ngang qua mặt cổng
        path.lineTo(p2.x() - margin, p2.y()); // Đi dọc về cao độ đích
    }

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