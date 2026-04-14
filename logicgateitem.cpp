#include "logicgateitem.h"
#include "logicCore.h"
#include "mainwindow.h"
#include "wire.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include<memory>

LogicGateItem::LogicGateItem(GateType type, QGraphicsItem* parent)
    : QGraphicsRectItem(parent), m_type(type)
{
    setRect(0, 0, 80, 50);
    setBrush(QBrush(QColor(230, 240, 250)));
    setPen(QPen(Qt::black, 2));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);

    // Hiển thị tên cổng tương ứng
    QString name;
    switch (type) {
    case LogicGateItem::AND:
        name = "AND";
        m_core = std::make_unique<AndGate>(); // Kết nối logic cổng AND
        break;
    case LogicGateItem::OR:
        name = "OR";
        m_core = std::make_unique<OrGate>();  // Kết nối logic cổng OR
        break;
    case LogicGateItem::NAND:
        name = "NAND";
        m_core = std::make_unique<NandGate>();
        break;
    case LogicGateItem::NOR:
        name = "NOR";
        m_core = std::make_unique<NorGate>();
        break;
    case LogicGateItem::EXOR:
        name = "EXOR";
        m_core = std::make_unique<ExorGate>();
        break;
    case LogicGateItem::EXNOR:
        name = "EXNOR";
        m_core = std::make_unique<ExnorGate>();
        break;
    case LogicGateItem::NOT:
        name = "NOT";
        m_core = std::make_unique<NotGate>();
        break;
    }

    auto text = new QGraphicsTextItem(name, this);
    // 1. Tạo Font chữ đậm (Bold)
    QFont font = text->font();
    font.setBold(true);
    font.setPointSize(10); // Có thể tăng kích thước nếu muốn
    text->setFont(font);

    // 2. Đổi màu chữ sang xanh đậm (Dark Blue) cho dễ nhìn
    text->setDefaultTextColor(QColor(0, 51, 102));

    // 3. Căn giữa chữ vào giữa cổng 80x50
    QRectF textRect = text->boundingRect();
    text->setPos((80 - textRect.width()) / 2, (50 - textRect.height()) / 2);
    int inputCount = (type == NOT) ? 1 : 2;
    for(int i = 0; i < inputCount; ++i) {
        PinItem* pin = new PinItem(true, this);
        int yPos = (inputCount == 1) ? 25 : (15 + i * 20);
        pin->setPos(0, yPos);
        m_inputs.push_back(pin);
    }

    m_output = new PinItem(false, this);
    m_output->setPos(80, 25);
}
void LogicGateItem::compute() {
    // 1. Kiểm tra an toàn: nếu chưa có bộ não hoặc chân cắm thì bỏ qua
    if (!m_core || m_inputs.empty()) return;

    // 2. Thu thập giá trị 0/1 (true/false) từ các chân Pin đầu vào
    std::vector<bool> inputValues;
    for (PinItem* pin : m_inputs) {
        inputValues.push_back(pin->value());
    }

    // 3. Gọi hàm compute của m_core để tính toán dựa trên quy tắc logic
    bool result = m_core->compute(inputValues);

    // 4. Cập nhật kết quả ra chân đầu ra (m_output)
    if (m_output) {
        m_output->setValue(result); // Chân đầu ra sẽ đổi màu nếu kết quả thay đổi
    }
}

QVariant LogicGateItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        // Mỗi khi cổng di chuyển, bảo các chân Pin hãy kéo dây theo
        for (auto pin : m_inputs) pin->updateConnectedWires();
        if (m_output) m_output->updateConnectedWires();
    }
    return QGraphicsRectItem::itemChange(change, value);
}
void PinItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // Nếu chuột phải: đảo tín hiệu 0/1 cho pin input
    if (event->button() == Qt::RightButton && m_isInput) {
        setValue(!m_value); // đảo giá trị
        event->accept();
        return;
    }

    // Nếu chuột trái: kéo dây như cũ
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    m_dragStartPosition = event->scenePos();
    m_tempWire = new QGraphicsLineItem(QLineF(m_dragStartPosition, m_dragStartPosition));

    QPen pen(Qt::black, 1, Qt::DashLine);
    m_tempWire->setPen(pen);
    scene()->addItem(m_tempWire);

    grabMouse();
    event->accept();
}
void PinItem::updateConnectedWires() {
    for (WireItem* wire : m_connectedWires) {
        if (wire) wire->updatePosition();
    }
}
void PinItem::notifyWires() {
    for (auto wire : m_connectedWires) {
        if (wire) wire->transmit();
    }
}
void PinItem::setValue(bool v) {
    if (m_value == v) return; // tránh vòng lặp vô hạn
    m_value = v;

    setBrush(v ? Qt::red : Qt::white);
    if (m_label) m_label->setPlainText(v ? "1" : "0");

    if (m_isInput) {
        if (auto gate = dynamic_cast<LogicGateItem*>(parentItem())) {
            gate->compute();
        }
    } else {
        notifyWires(); // truyền tín hiệu qua dây
    }
}
void PinItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_tempWire) {
        // Cập nhật đường kẻ từ tâm Pin đến vị trí chuột hiện tại
        m_tempWire->setLine(QLineF(this->scenePos(), event->scenePos()));
    }
}
void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        QGraphicsEllipseItem::mouseReleaseEvent(event);
        return;
    }

    ungrabMouse();

    if (m_tempWire) {
        // Xóa dây tạm trước khi kiểm tra
        scene()->removeItem(m_tempWire);
        delete m_tempWire;
        m_tempWire = nullptr;

        // Lấy tất cả item trong vùng nhỏ quanh chuột
        QRectF area(event->scenePos() - QPointF(5,5), QSizeF(10,10));
        QList<QGraphicsItem*> items = scene()->items(area);

        for (QGraphicsItem* i : items) {
            PinItem* targetPin = dynamic_cast<PinItem*>(i);
            if (targetPin && targetPin != this) {
                // Chỉ cần khác gate, không quan trọng input/output
                if (targetPin->parentItem() != this->parentItem()) {
                    // Tạo dây chính thức
                    WireItem* wire = new WireItem(this, targetPin);
                    scene()->addItem(wire);

                    this->addWire(wire);
                    targetPin->addWire(wire);

                    wire->updatePosition();
                    wire->transmit();
                    break; // kết nối xong thì thoát vòng lặp
                }
            }
        }
    }

    QGraphicsEllipseItem::mouseReleaseEvent(event);
}