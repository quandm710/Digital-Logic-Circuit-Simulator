#include "wire.h"
#include "logicgateitem.h"
#include <QPainterPath>
#include <QGraphicsScene>
#include <QKeyEvent>
WireItem::WireItem(PinItem* start, PinItem* end, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), m_startPin(start), m_endPin(end)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setZValue(-1);
    // Đảm bảo dây cập nhật vị trí ngay khi vừa tạo ra
    updatePosition();
}
void WireItem::updatePosition() {
    if (!m_startPin || !m_endPin) return;

    // --- 1. XÁC ĐỊNH AI LÀ OUTPUT (NGUỒN), AI LÀ INPUT (ĐÍCH) ---
    // Dù bạn kéo từ đâu, ta luôn quy ước pSrc là chân Output, pDst là chân Input
    PinItem* srcPin = m_startPin->isInput() ? m_endPin : m_startPin;
    PinItem* dstPin = m_startPin->isInput() ? m_startPin : m_endPin;

    QPointF pSrc = srcPin->mapToScene(srcPin->boundingRect().center());
    QPointF pDst = dstPin->mapToScene(dstPin->boundingRect().center());

    QGraphicsItem* srcGate = srcPin->parentItem();
    QGraphicsItem* dstGate = dstPin->parentItem();
    QRectF srcRect = srcGate->mapRectToScene(srcGate->boundingRect());
    QRectF dstRect = dstGate->mapRectToScene(dstGate->boundingRect());

    QPainterPath path;
    path.moveTo(pSrc);

    // Phân làn để tránh dây đè nhau (giữ nguyên logic của Khôi)
    int laneIdx = (reinterpret_cast<quintptr>(srcPin) / 16) % 6;
    qreal offset = laneIdx * 8;

    // --- 2. TÍNH TOÁN CÁC ĐIỂM GẬP ---

    // Điểm thoát ra khỏi Output (luôn nằm bên phải cổng nguồn)
    qreal xExit = srcRect.right() + 15 + offset;

    // Khoảng cách an toàn trước khi vào Input (luôn nằm bên trái cổng đích)
    qreal xEntry = dstRect.left() - 15 - offset;

    // KIỂM TRA TRƯỜNG HỢP ƯU TIÊN: ZIC-ZAC 3 ĐOẠN
    // Điều kiện: Chân Output nằm bên trái chân Input và không quá sát nhau
    if (xExit < xEntry) {
        qreal xMid = (xExit + xEntry) / 2; // Điểm gập giữa khoảng không
        path.lineTo(xExit, pSrc.y());
        path.lineTo(xExit, pDst.y()); // Gập dọc ngay sau khi thoát cổng
        path.lineTo(pDst.x(), pDst.y());
    }
    // TRƯỜNG HỢP KHÓ: ĐÍCH NẰM SAU LƯNG HOẶC QUÁ SÁT (VÒNG TRÁNH)
    else {
        // Bước 1: Thoát ra bên phải cổng nguồn
        path.lineTo(xExit, pSrc.y());

        // Bước 2: Chọn hướng vòng (Lên trên hoặc Xuống dưới cổng)
        qreal yBypass;
        bool goAbove = (pSrc.y() < pDst.y()); // Tùy vị trí tương đối mà chọn đường vòng ngắn nhất

        if (goAbove) {
            yBypass = qMin(srcRect.top(), dstRect.top()) - 20 - offset;
        } else {
            yBypass = qMax(srcRect.bottom(), dstRect.bottom()) + 20 + offset;
        }

        path.lineTo(xExit, yBypass);         // Gập lên/xuống hành lang
        path.lineTo(xEntry, yBypass);        // Chạy ngang qua mặt/sau lưng cổng
        path.lineTo(xEntry, pDst.y());       // Gập về cao độ đích
        path.lineTo(pDst.x(), pDst.y());     // Nối vào chân Input
    }

    setPath(path);
    setZValue(-1);
}
void WireItem::transmit() {
    if (!m_startPin || !m_endPin) return;

    // Xác định đúng chân nguồn (Output) và chân đích (Input)
    // Tín hiệu phải đi từ Output sang Input bất kể bạn kéo dây từ đâu
    PinItem* source = m_startPin->isInput() ? m_endPin : m_startPin;
    PinItem* destination = m_startPin->isInput() ? m_startPin : m_endPin;

    // Chỉ truyền nếu source thực sự là Output và destination thực sự là Input
    // (Tránh trường hợp nối Input-Input hoặc Output-Output nếu logic cho phép)
    if (!source->isInput() && destination->isInput()) {
        bool val = source->value();
        destination->setValue(val);

        // Cập nhật màu sắc dây: Đỏ cho mức cao (1), Xanh cho mức thấp (0)
        setPen(QPen(val ? Qt::red : Qt::blue, 2));
    }
}
WireItem::~WireItem() {
    // Trước khi bị xóa, báo cho 2 đầu Pin gỡ mình ra khỏi danh sách quản lý
    if (m_startPin) m_startPin->removeWire(this);
    if (m_endPin) m_endPin->removeWire(this);
}
void WireItem::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        removeSelf();
    } else {
        QGraphicsPathItem::keyPressEvent(event);
    }
}
void WireItem::removeSelf() {
    // Báo cho các Pin gỡ dây này ra khỏi danh sách quản lý của chúng
    if (m_startPin) m_startPin->removeWire(this);
    if (m_endPin) m_endPin->removeWire(this);

    if (scene()) {
        scene()->removeItem(this);
        delete this;
    }
}