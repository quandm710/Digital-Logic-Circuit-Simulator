#include "logicgateitem.h"
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>

LogicGateItem::LogicGateItem(GateType type, QGraphicsItem* parent)
    : QGraphicsRectItem(parent), m_type(type)
{
    setRect(0, 0, 80, 50); // Kích thước cổng
    setBrush(Qt::lightGray);
    setPen(QPen(Qt::black, 2));

    // Cho phép di chuyển bằng chuột
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    // Nhãn tên cổng
    QString name = (type == AND) ? "AND" : (type == OR) ? "OR" : "NOT";
    auto text = new QGraphicsTextItem(name, this);
    text->setPos(20, 15);

    // Vẽ Pins đầu vào (Inputs)
    int inputCount = (type == NOT) ? 1 : 2;
    for(int i = 0; i < inputCount; ++i) {
        auto* pin = new PinItem(true, this); // true = isInput
        // Căn chỉnh vị trí pin bên trái cổng
        int yPos = (inputCount == 1) ? 25 : (15 + i * 20);
        pin->setPos(0, yPos);
        m_inputs.push_back(pin);
    }

    // Vẽ Pin đầu ra (Output)
    m_output = new PinItem(false, this); // false = isOutput
    m_output->setPos(80, 25); // Nằm bên phải cổng
}
QVariant LogicGateItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        // Sau này Người 4 sẽ viết code cập nhật vị trí dây nối ở đây
    }
    return QGraphicsRectItem::itemChange(change, value);
}