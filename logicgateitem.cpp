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
    // Lấy trạng thái từ MainWindow
    MainWindow* mainWin = qobject_cast<MainWindow*>(scene()->views().first()->window());
    bool wiring = mainWin ? mainWin->getWiringMode() : false;

    if (wiring) {
        event->accept();
        // --- LOGIC NỐI DÂY ---
        // Bắt đầu tạo dây nối tạm thời (Preview)
        m_tempWire = new QGraphicsLineItem();
        m_tempWire->setPen(QPen(Qt::blue, 1, Qt::DashLine));
        m_tempWire->setLine(QLineF(scenePos(), event->scenePos()));
        scene()->addItem(m_tempWire);
        this->grabMouse();
    } else {
        // --- LOGIC ĐỔI TÍN HIỆU 0/1 ---
        if (m_isInput) {
            setValue(!m_value);
            if (auto gate = dynamic_cast<LogicGateItem*>(parentItem())) {
                gate->compute();
            }
        }
    }
    QGraphicsEllipseItem::mousePressEvent(event);
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
    // Đổi màu: Đỏ là mức 1, Trắng là mức 0
    setBrush(v ? Qt::red : Qt::white);
    if (m_label) {
        m_label->setPlainText(v ? "1" : "0");
        // Đổi màu chữ cho dễ nhìn trên nền đỏ/trắng
        m_label->setDefaultTextColor(v ? Qt::white : Qt::black);
        centerLabel();
    }
    update();
    notifyWires();
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
    if (m_tempWire) {
        event->accept();
        // 1. Tìm item tại vị trí thả chuột
        QGraphicsItem* item = scene()->itemAt(event->scenePos(), QTransform());
        PinItem* targetPin = dynamic_cast<PinItem*>(item);

        // 2. Kiểm tra điều kiện kết nối:
        // - Phải là một Pin khác
        // - Một cái là Input, một cái là Output (không nối Input-Input)
        if (targetPin && targetPin != this && targetPin->isInput() != this->isInput()) {

            // Tạo dây nối chính thức
            WireItem* wire = new WireItem(this, targetPin);
            scene()->addItem(wire);

            // Lưu dây vào danh sách quản lý của cả 2 Pin
            this->addWire(wire);
            targetPin->addWire(wire);

            // Truyền tín hiệu ngay lập tức sau khi nối
            wire->transmit();
        }

        // 3. Xóa dây tạm thời
        scene()->removeItem(m_tempWire);
        delete m_tempWire;
        m_tempWire = nullptr;
        event->accept();
    } else {
        QGraphicsEllipseItem::mouseReleaseEvent(event);
    }
}
