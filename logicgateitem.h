#ifndef LOGICGATEITEM_H
#define LOGICGATEITEM_H
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <vector>
#include <QBrush>    // Thêm dòng này để xử lý lỗi QBrush
#include <QPen>      // Thêm dòng này để xử lý lỗi QPen

using namespace std;

// Lớp đại diện cho điểm nối (Của Người 1 và Người 4 phối hợp)
class PinItem : public QGraphicsEllipseItem {
public:
    PinItem(bool isInput, QGraphicsItem* parent = nullptr)
        : QGraphicsEllipseItem(-5, -5, 10, 10, parent), m_isInput(isInput) {
        setBrush(Qt::white);
        setPen(QPen(Qt::black, 1));
        // Cho phép nhận sự kiện để Người 4 xử lý nối dây sau này
        setAcceptHoverEvents(true);
    }
    bool isInput() const { return m_isInput; }
private:
    bool m_isInput;
};

// Lớp Cổng Logic
class LogicGateItem : public QGraphicsRectItem {
public:
    enum GateType { AND, OR, NAND, NOR, EXOR, EXNOR , NOT};
    LogicGateItem(GateType type, QGraphicsItem* parent = nullptr);

    // Dành cho Người 4: Lấy danh sách các Pin để vẽ dây
    vector<PinItem*> inputPins() { return m_inputs; }
    PinItem* outputPin() { return m_output; }

protected:
    // Cập nhật vị trí dây nối khi cổng di chuyển
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    GateType m_type;
    vector<PinItem*> m_inputs;
    PinItem* m_output;
};

#endif // LOGICGATEITEM_H
