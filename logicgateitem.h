#ifndef LOGICGATEITEM_H
#define LOGICGATEITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>
#include <vector>

// Chân cắm (Người 4 dùng để nối dây)
class PinItem : public QGraphicsEllipseItem {
public:
    PinItem(bool isInput, QGraphicsItem* parent = nullptr)
        : QGraphicsEllipseItem(-4, -4, 8, 8, parent), m_isInput(isInput) {
        setBrush(Qt::white);
        setPen(QPen(Qt::black, 1));
    }
    bool isInput() const { return m_isInput; }
private:
    bool m_isInput;
};

// Cổng Logic (Người 1 vẽ - Người 2&3 xử lý logic)
class LogicGateItem : public QGraphicsRectItem {
public:
    enum GateType { AND, OR, NAND, NOR, EXOR, EXNOR, NOT };
    LogicGateItem(GateType type, QGraphicsItem* parent = nullptr);

    GateType getGateType() const { return m_type; }

protected:
    // Tự động cập nhật khi di chuyển (Người 4)
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    GateType m_type;
    std::vector<PinItem*> m_inputs;
    PinItem* m_output;
};

#endif