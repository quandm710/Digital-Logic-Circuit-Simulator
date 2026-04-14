#ifndef LOGICGATEITEM_H
#define LOGICGATEITEM_H

#include <QFont>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <vector>

class LogicGate;
class WireItem;

// Chân cắm (Người 4 dùng để nối dây)
class PinItem : public QGraphicsEllipseItem {
public:
    PinItem(bool isInput, QGraphicsItem* parent)
        : QGraphicsEllipseItem(-10, -10, 20, 20, parent), m_isInput(isInput)
    {
        // 1. Hình dáng: Tăng kích thước từ 8 lên 12 để dễ "bắt" dây hơn
        setBrush(Qt::white);
        setPen(QPen(Qt::black, 1.5));

        // 2. Độ cao hiển thị (Quan trọng nhất để không bị Gate che khuất)
        setZValue(10);

        // 3. Quyền tương tác: Cho phép chọn và nhận thông báo thay đổi tọa độ
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

        // Chỉ định rõ ràng Pin này nhận chuột trái để kích hoạt mousePressEvent
        setAcceptedMouseButtons(Qt::LeftButton);

        // 4. Thiết lập nhãn hiển thị 0/1
        m_label = new QGraphicsTextItem("0", this);
        QFont font("Arial", 8, QFont::Bold);
        m_label->setFont(font);
        m_label->setDefaultTextColor(Qt::black);

        // QUAN TRỌNG: Cho phép chuột "xuyên qua" nhãn số để chạm vào vòng tròn Pin bên dưới
        m_label->setAcceptedMouseButtons(Qt::NoButton);

        // Đặt nhãn nằm trên Pin một chút để luôn nhìn thấy số
        m_label->setZValue(11);

        // 5. Căn giữa nhãn vào vòng tròn
        centerLabel();
    }
    void centerLabel() {
        if (m_label) {
            QRectF textRect = m_label->boundingRect();
            m_label->setPos(-textRect.width() / 2, -textRect.height() / 2);
        }
    }
    void updateConnectedWires();
    bool isInput() const { return m_isInput; }
    void addWire(WireItem* wire) { m_connectedWires.push_back(wire); }
    void notifyWires();
    void setValue(bool v);
    bool value() const { return m_value; }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    bool m_isInput;
    bool m_value = false;
    std::vector<WireItem*> m_connectedWires;
    QPointF m_dragStartPosition;
    QGraphicsLineItem* m_tempWire = nullptr;
    QGraphicsTextItem* m_label = nullptr; // Nhãn hiển thị 0/1

};

// Cổng Logic (Người 1 vẽ - Người 2&3 xử lý logic)
class LogicGateItem : public QGraphicsRectItem {
public:
    enum GateType { AND, OR, NAND, NOR, EXOR, EXNOR, NOT };
    LogicGateItem(GateType type, QGraphicsItem* parent = nullptr);

    GateType getGateType() const { return m_type; }

    void compute();

protected:
    // Tự động cập nhật khi di chuyển (Người 4)
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    std::unique_ptr<LogicGate> m_core; // Dòng này cực kỳ quan trọng
    GateType m_type;
    std::vector<PinItem*> m_inputs;
    PinItem* m_output;
};



#endif