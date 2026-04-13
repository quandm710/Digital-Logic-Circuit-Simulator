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
        // Bảo tất cả các chân Pin cập nhật lại vị trí dây nối
        for (auto pin : m_inputs) pin->updateConnectedWires();
        if (m_output) m_output->updateConnectedWires();
    }
    return QGraphicsRectItem::itemChange(change, value);
}
void PinItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // Đổi giá trị nếu là chân Input
    if (m_isInput) {
        setValue(!m_value);
        if (auto gate = dynamic_cast<LogicGateItem*>(parentItem())) {
            gate->compute();
        }
    }

    // Tạo dây tạm với ZValue thấp để không đè lên các Pin khác
    m_tempWire = new QGraphicsLineItem();
    m_tempWire->setPen(QPen(Qt::blue, 1, Qt::DashLine));
    m_tempWire->setLine(QLineF(scenePos(), event->scenePos()));
    m_tempWire->setZValue(-1); // Cho nằm dưới các cổng
    scene()->addItem(m_tempWire);

    this->grabMouse();
    event->accept();
}
void PinItem::updateConnectedWires() {
    for (WireItem* wire : m_connectedWires) {
        wire->updatePosition();
    }
}
void PinItem::notifyWires() {
    for (auto wire : m_connectedWires) {
        if (wire) wire->transmit();
    }
}
void PinItem::setValue(bool v) {
    m_value = v;
    setBrush(v ? Qt::red : Qt::white);
    if (m_label) {
        m_label->setPlainText(v ? "1" : "0");
        m_label->setDefaultTextColor(v ? Qt::white : Qt::black);
        centerLabel();
    }

    // Nếu là chân Output, phải báo cho các dây đang nối với nó truyền tin đi
    if (!m_isInput) {
        notifyWires();
    }

    update(); // Bắt buộc phải có để cập nhật giao diện
}
void PinItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_tempWire) {
        // Cập nhật điểm kết thúc của dây tạm thời theo vị trí chuột
        m_tempWire->setLine(QLineF(scenePos(), event->scenePos()));
        event->accept();
    } else {
        QGraphicsEllipseItem::mouseMoveEvent(event);
    }
}

void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    ungrabMouse();

    if (m_tempWire) {
        m_tempWire->hide(); // Ẩn dây tạm để không tự quét trúng nó

        // CÁCH SỬA: Tạo vùng quét 16x16 pixel xung quanh điểm nhả chuột
        // Kích thước 16x16 giúp việc "bắt dính" (snap) dễ hơn rất nhiều
        QRectF scanArea(0, 0, 16, 16);
        scanArea.moveCenter(event->scenePos());

        // Lấy tất cả item trong vùng quét
        QList<QGraphicsItem*> items = scene()->items(scanArea);
        PinItem* targetPin = nullptr;

        for (QGraphicsItem* item : items) {
            targetPin = dynamic_cast<PinItem*>(item);

            // Điều kiện nối:
            // 1. Là Pin và không phải chính nó
            // 2. Khác loại (In nối với Out)
            // 3. Không nằm trên cùng một cổng logic (Tránh nối tắt trong 1 cổng)
            if (targetPin && targetPin != this &&
                targetPin->isInput() != this->isInput() &&
                targetPin->parentItem() != this->parentItem()) {
                break;
            }
            targetPin = nullptr;
        }

        if (targetPin) {
            // Kiểm tra xem đã có dây nối giữa 2 pin này chưa (tránh nối đè)
            WireItem* wire = new WireItem(this, targetPin);
            scene()->addItem(wire);
            this->addWire(wire);
            targetPin->addWire(wire);
            wire->transmit();
        }

        scene()->removeItem(m_tempWire);
        delete m_tempWire;
        m_tempWire = nullptr;
    }

    QGraphicsEllipseItem::mouseReleaseEvent(event);
}