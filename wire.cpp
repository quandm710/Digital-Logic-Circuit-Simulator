#include "wire.h"
#include "logicgateitem.h"
#include <QPainterPath>
#include <QGraphicsScene>
WireItem::WireItem(PinItem* start, PinItem* end, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), m_startPin(start), m_endPin(end) {
    setPen(QPen(Qt::blue, 2));
    setZValue(-1);
    setAcceptedMouseButtons(Qt::NoButton);
    updatePosition();
}
void WireItem::updatePosition() {
    if (!m_startPin || !m_endPin) return;
    QPointF p1 = m_startPin->mapToScene(m_startPin->boundingRect().center());
    QPointF p2 = m_endPin->mapToScene(m_endPin->boundingRect().center());
    QPainterPath path;
    path.moveTo(p1);
    // 1. Phân loại dây dựa trên chân Input đích (trên hay dưới)
    bool isUpperPin = (p2.y() < m_endPin->parentItem()->mapToScene(m_endPin->parentItem()->boundingRect().center()).y());
    // 2. THIẾT LẬP KHOẢNG CÁCH TÁCH BIỆT (Tạo làn đường riêng)
    // Sợi dây dưới sẽ thoát xa hơn và gập rộng hơn sợi dây trên
    qreal exitOffset = isUpperPin ? 20 : 45;    // Chênh lệch 25px ở điểm thoát ngang
    qreal bypassMargin = isUpperPin ? 30 : 55;  // Chênh lệch 25px ở độ rộng vòng né
    qreal frontMargin = isUpperPin ? 25 : 40;   // So le điểm gập cuối trước khi vào Input
    // Kiểm tra vị trí đích
    if (p2.x() > p1.x() + 50) {
        // --- TRƯỜNG HỢP CỔNG ĐÍCH BÊN PHẢI ---
        qreal xBreak = p1.x() + exitOffset;

        path.lineTo(xBreak, p1.y());
        path.lineTo(xBreak, p2.y());
        path.lineTo(p2.x(), p2.y());
    }
    else {
        // --- TRƯỜNG HỢP CỔNG ĐÍCH BÊN TRÁI (Đi vòng ziczac) ---
        // Bước 1: Thoát ngang (Sợi dưới sẽ dài hơn sợi trên rõ rệt)
        qreal xExit = p1.x() + exitOffset;
        path.lineTo(xExit, p1.y());
        // Bước 2: Gập dọc (Sợi trên gập xuống, sợi dưới gập lên)
        // Dùng bypassMargin khác nhau để tạo 2 làn đường song song
        qreal yBypass = isUpperPin ? (p1.y() + bypassMargin) : (p1.y() - bypassMargin);
        path.lineTo(xExit, yBypass);
        // Bước 3: Kéo ngang sang trái (Điểm dừng xWait cũng so le)
        qreal xWait = p2.x() - frontMargin;
        path.lineTo(xWait, yBypass);
        // Bước 4: Gập dọc về cao độ chân đích
        path.lineTo(xWait, p2.y());
        // Bước 5: Nối thẳng vào chân Input
        path.lineTo(p2.x(), p2.y());
    }
    setPath(path);
}
void WireItem::transmit() {
    if (!m_startPin || !m_endPin) return;
    bool valStart = m_startPin->value();
    m_endPin->setValue(valStart);
    setPen(QPen(valStart ? Qt::red : Qt::blue, 2));
}
WireItem::~WireItem() {
    // Trước khi bị xóa, báo cho 2 đầu Pin gỡ mình ra khỏi danh sách quản lý
    if (m_startPin) m_startPin->removeWire(this);
    if (m_endPin) m_endPin->removeWire(this);
}
