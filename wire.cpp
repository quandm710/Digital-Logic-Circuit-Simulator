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
    // 1. Lấy thông tin vị trí và vùng bao quanh linh kiện
    QPointF p1 = m_startPin->mapToScene(m_startPin->boundingRect().center());
    QPointF p2 = m_endPin->mapToScene(m_endPin->boundingRect().center());
    QGraphicsItem* startGate = m_startPin->parentItem();
    QGraphicsItem* endGate = m_endPin->parentItem();
    QRectF startRect = startGate->mapRectToScene(startGate->boundingRect());
    QRectF endRect = endGate->mapRectToScene(endGate->boundingRect());
    QPainterPath path;
    path.moveTo(p1);
    // --- HỆ THỐNG PHÂN LÀN (LANE) ---
    // Dùng địa chỉ pin để tạo độ lệch 8px, 16px... giúp dây không bao giờ trùng nhau
    int laneIdx = (reinterpret_cast<quintptr>(m_startPin) / 16) % 6;
    qreal offset = laneIdx * 8;
    // Kiểm tra loại kết nối
    bool isInputToInput = (m_startPin->isInput() && m_endPin->isInput());
    qreal deltaX = p2.x() - p1.x();
    bool isUpperSource = (p1.y() < p2.y());
    if (isInputToInput) {
        // TRƯỜNG HỢP A: INPUT NỐI INPUT (Đi hình chữ U bên trái)
        // Điểm gập bên trái: Lấy mép trái nhất của 2 cổng rồi lùi thêm một đoạn
        qreal xLeft = qMin(startRect.left(), endRect.left()) - 20 - offset;

        path.lineTo(xLeft, p1.y());
        path.lineTo(xLeft, p2.y());
        path.lineTo(p2.x(), p2.y());
    }
    else {
        // TRƯỜNG HỢP B: OUTPUT NỐI INPUT (Hoặc ngược lại)
        // Điểm thoát an toàn khỏi cổng nguồn (phía bên phải)
        qreal xExit = startRect.right() + 20 + offset;
        // Kiểm tra xem đường đi từ xExit đến p2 có bị chặn không
        // Chặn khi: Đích nằm bên trái HOẶC Đích nằm bên phải nhưng bị thân cổng đích che cao độ
        bool isPathBlocked = (p2.x() < xExit + 20) ||
                             (p1.y() > endRect.top() - 5 && p1.y() < endRect.bottom() + 5);

        if (!isPathBlocked) {
            // --- 1. ZICZAC 3 ĐOẠN (Đường thoáng - Ưu tiên của Khôi) ---
            path.lineTo(xExit, p1.y());
            path.lineTo(xExit, p2.y());
            path.lineTo(p2.x(), p2.y());
        }
        else {
            // --- 2. VÒNG TRÁNH 5 BƯỚC (Khi bị chặn hoặc đích nằm bên trái) ---
            // Bước 1: Thoát ngang
            path.lineTo(xExit, p1.y());
            // Bước 2: Gập dọc lên/xuống hành lang an toàn
            qreal yBypass;
            if (isUpperSource) {
                yBypass = qMin(startRect.top(), endRect.top()) - 25 - offset;
            } else {
                yBypass = qMax(startRect.bottom(), endRect.bottom()) + 25 + offset;
            }
            path.lineTo(xExit, yBypass);
            // Bước 3: Chạy ngang lùi về phía sau (né cổng)
            qreal xFarLeft = qMin(startRect.left(), endRect.left()) - 35 - offset;
            path.lineTo(xFarLeft, yBypass);
            // Bước 4: Gập dọc về cao độ chân đích
            path.lineTo(xFarLeft, p2.y());
            // Bước 5: Nối vào chân đích
            path.lineTo(p2.x(), p2.y());
        }
    }
    setPath(path);
    setZValue(-1); // Dây luôn nằm dưới cổng
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
