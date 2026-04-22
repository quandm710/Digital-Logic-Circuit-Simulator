#include "logicgateitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include "logicCore.h"
#include "mainwindow.h"
#include "wire.h"
#include <memory>
#include <cmath>

// Constructor của LogicGateItem
LogicGateItem::LogicGateItem(GateType type, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_type(type)
{
    setRect(0, 0, 80, 50);
    setBrush(QBrush(QColor(230, 240, 250)));
    setPen(QPen(Qt::black, 2));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable
             | QGraphicsItem::ItemSendsGeometryChanges);

    // Hiển thị tên cổng tương ứng
    QString name;
    switch (type) {
    case LogicGateItem::AND:
        name = "AND";
        m_core = std::make_unique<AndGate>(); // Kết nối logic cổng AND
        break;
    case LogicGateItem::OR:
        name = "OR";
        m_core = std::make_unique<OrGate>(); // Kết nối logic cổng OR
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
    for (int i = 0; i < inputCount; ++i) {
        PinItem *pin = new PinItem(true, this);
        int yPos = (inputCount == 1) ? 25 : (15 + i * 20);
        pin->setPos(0, yPos);
        m_inputs.push_back(pin);
    }

    m_output = new PinItem(false, this);
    m_output->setPos(80, 25);
}
// Destructor của LogicGateItem
LogicGateItem::~LogicGateItem()
{
    // Lấy tất cả Pin của cổng này
    for (auto pin : m_inputs)
        deleteWireOfPin(pin);
    deleteWireOfPin(m_output);
}
QVariant LogicGateItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        // Mỗi khi cổng di chuyển, bảo các chân Pin hãy kéo dây theo
        for (auto pin : m_inputs)
            pin->updateConnectedWires();
        if (m_output)
            m_output->updateConnectedWires();
        if (scene() && !scene()->views().isEmpty()) {
            // Tìm MainWindow thông qua view và cửa sổ chứa nó
            MainWindow *mainWin = qobject_cast<MainWindow *>(scene()->views().first()->window());
            if (mainWin) {
                mainWin->setDocumentDirty(true);
            }
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}
void LogicGateItem::compute()
{
    if (!m_core || m_inputs.empty())
        return;

    // 2. Thu thập giá trị 0/1 (true/false) từ các chân Pin đầu vào
    std::vector<bool> inputValues;
    for (PinItem *pin : m_inputs) {
        inputValues.push_back(pin->value());
    }

    // 3. Gọi hàm compute của m_core để tính toán dựa trên quy tắc logic
    bool result = m_core->compute(inputValues);

    // 4. Cập nhật kết quả ra chân đầu ra (m_output)
    if (m_output) {
        bool oldValue = m_output->value();
        if (oldValue != result) {
            m_output->setValue(result);

            // Đánh dấu mạch đã thay đổi (Dirty Bit)
            if (scene() && !scene()->views().isEmpty()) {
                MainWindow *mainWin = qobject_cast<MainWindow *>(scene()->views().first()->window());
                if (mainWin)
                    mainWin->setDocumentDirty(true);
            }
        }
    }
}
PinItem *LogicGateItem::getInputPin(int index)
{
    // Giả sử bạn lưu các pin vào một vector m_inputs khi khởi tạo gate
    if (index >= 0 && index < m_inputs.size()) {
        return m_inputs[index];
    }
    return nullptr;
}

PinItem *LogicGateItem::getOutputPin()
{
    return m_output; // Trả về pin đầu ra đã tạo trong constructor
}
bool LogicGateItem::getOutputValue()
{
    if (!m_core || m_inputs.empty())
        return false;

    // Thu thập giá trị hiện tại từ các chân input
    std::vector<bool> inputValues;
    for (PinItem *pin : m_inputs) {
        inputValues.push_back(pin->value());
    }

    // Trả về kết quả tính toán từ core logic
    return m_core->compute(inputValues);
}
// --- PinItem ---
void PinItem::updateConnectedWires()
{
    for (WireItem *wire : m_connectedWires) {
        if (wire)
            wire->updatePosition();
    }
}
void PinItem::notifyWires()
{
    for (auto wire : m_connectedWires) {
        if (wire)
            wire->transmit();
    }
}
void LogicGateItem::deleteWireOfPin(PinItem *pin)
{
    if (!pin)
        return;

    // Lấy danh sách dây thông qua hàm getter vừa tạo
    std::vector<WireItem *> wires = pin->getConnectedWires();

    for (WireItem *wire : wires) {
        if (wire) {
            // Xóa dây khỏi Scene trước khi delete
            if (scene()) {
                scene()->removeItem(wire);
            }
            delete wire;
        }
    }
}
void PinItem::removeWire(WireItem *wire)
{
    auto it = std::find(m_connectedWires.begin(), m_connectedWires.end(), wire);
    if (it != m_connectedWires.end()) {
        m_connectedWires.erase(it);
    }
}
void PinItem::setValue(bool v)
{
    if (m_value == v)
        return; // tránh vòng lặp vô hạn
    m_value = v;

    setBrush(v ? Qt::red : Qt::white);
    if (m_label)
        m_label->setPlainText(v ? "1" : "0");

    if (m_isInput) {
        if (auto gate = dynamic_cast<LogicGateItem *>(parentItem())) {
            gate->compute();
        }
    } else {
        notifyWires(); // truyền tín hiệu qua dây
    }
}
void PinItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_tempWire) {
        // Cập nhật đường kẻ từ tâm Pin đến vị trí chuột hiện tại
        m_tempWire->setLine(QLineF(this->scenePos(), event->scenePos()));
    }
}
void PinItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isInput && !m_connectedWires.empty()) {
        return; // Thoát ra, không cho phép đổi giá trị (0 -> 1)
    }
    // Lấy trạng thái mode từ MainWindow
    MainWindow *mainWin = qobject_cast<MainWindow *>(scene()->views().first()->window());
    bool wiringMode = mainWin ? mainWin->getWiringMode() : false;

    // Chuột trái
    if (event->button() == Qt::LeftButton) {
        // Chế độ nối dây
        if (wiringMode) {
            m_dragStartPosition = scenePos(); // Lấy tâm của Pin làm điểm bắt đầu
            m_tempWire = new QGraphicsLineItem(QLineF(m_dragStartPosition, m_dragStartPosition));

            QPen pen(Qt::blue, 1, Qt::DashLine); // Dây tạm màu xanh nét đứt
            m_tempWire->setPen(pen);
            scene()->addItem(m_tempWire);

            grabMouse();
            event->accept();
        } // Chế độ tương tác
        else {
            if (m_isInput) {
                if (mainWin) mainWin->saveStateForUndo();
                setValue(!m_value); // Đảo giá trị 0/1

                LogicGateItem *parentGate = dynamic_cast<LogicGateItem *>(parentItem());
                if (parentGate) {
                    parentGate->compute();
                }
            }
            event->accept();
        }
    } else {
        event->ignore();
    }
}
void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
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
        QRectF area(event->scenePos() - QPointF(5, 5), QSizeF(10, 10));
        QList<QGraphicsItem *> items = scene()->items(area);

        for (QGraphicsItem *i : items) {
            PinItem *targetPin = dynamic_cast<PinItem *>(i);
            if (targetPin && targetPin != this) {
                // Chỉ cần khác gate, không quan trọng input/output
                if (targetPin->parentItem() != this->parentItem()) {
                    bool exists = false;
                    for (auto w : m_connectedWires) {
                        // Nếu dây đã nối tới targetPin thì bỏ qua
                        if (w->getStartPin() == targetPin || w->getEndPin() == targetPin) {
                            exists = true;
                            break;
                        }
                    }
                    // Tạo dây chính thức
                    if (!exists && targetPin->parentItem() != this->parentItem()) {
                        MainWindow *mainWin = qobject_cast<MainWindow *>(scene()->views().first()->window());
                        if (mainWin) mainWin->saveStateForUndo(); 

                        WireItem *wire = new WireItem(this, targetPin);
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
        if (scene() && !scene()->views().isEmpty()) {
            MainWindow *mainWin = qobject_cast<MainWindow *>(scene()->views().first()->window());
            if (mainWin)
                mainWin->setDocumentDirty(true);
        }
    }
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}