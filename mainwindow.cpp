#include "mainwindow.h"
#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsView>
#include <QMessageBox>
#include <QPoint>
#include <QPointF>
#include <QVBoxLayout>
#include "./ui_mainwindow.h"
#include "logicgateitem.h"
#include "wire.h"
#include <QApplication>
#include <QStack>
#include <QMap>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabsClosable(true);

    addNewTab("Untitled 1");

    setupComponentList();

    QShortcut *undoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(undoShortcut, &QShortcut::activated, this, &MainWindow::undo);

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTabCloseRequested(int index)
{
    QWidget *tabToClose = ui->tabWidget->widget(index);
    if (!tabToClose)
        return;

    bool isDirty = tabToClose->property("isDirty").toBool();

    if (isDirty) {
        QString tabName = ui->tabWidget->tabText(index);
        if (tabName.endsWith("*"))
            tabName.chop(1);

        QMessageBox::StandardButton resBtn = QMessageBox::question(
            this,
            "Xác nhận đóng",
            QString("Mạch '%1' đã thay đổi. Bạn có muốn lưu trước khi đóng không?").arg(tabName),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);

        if (resBtn == QMessageBox::Save) {
            on_actionSave_triggered();
        } else if (resBtn == QMessageBox::Discard) {
            // Không lưu, thực hiện đóng (không làm gì thêm, chỉ để trôi xuống lệnh removeTab)
        } else {
            // TRƯỜNG HỢP CANCEL (Hủy bỏ)
            return; // Thoát hàm NGAY LẬP TỨC, không chạy xuống lệnh removeTab bên dưới
        }
    }
    ui->tabWidget->removeTab(index);
    tabToClose->deleteLater(); // Dùng deleteLater() an toàn hơn delete trực tiếp trong Qt
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool hasDirtyTabs = false;
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        if (ui->tabWidget->widget(i)->property("isDirty").toBool()) {
            hasDirtyTabs = true;
            break;
        }
    }

    if (hasDirtyTabs) {
        QMessageBox::StandardButton resBtn = QMessageBox::question(
            this,
            "Thoát phần mềm",
            "Có tab chưa lưu. Bạn có chắc chắn muốn thoát không?\nMọi thay đổi chưa lưu sẽ bị mất.",
            QMessageBox::No | QMessageBox::Yes,
            QMessageBox::No);
        if (resBtn != QMessageBox::Yes) {
            event->ignore(); // Ngăn việc đóng ứng dụng
            return;
        }
    }
    event->accept(); // Cho phép đóng
}

void MainWindow::setDocumentDirty(bool dirty)
{
    isDirty = dirty;
    int index = ui->tabWidget->currentIndex();
    if (index == -1)
        return;

    QWidget *currentTab = ui->tabWidget->widget(index);
    currentTab->setProperty("isDirty", dirty);

    QString title = ui->tabWidget->tabText(index);
    if (dirty && !title.endsWith("*")) {
        ui->tabWidget->setTabText(index, title + "*");
    } else if (!dirty && title.endsWith("*")) {
        title.chop(1); // Bỏ dấu *
        ui->tabWidget->setTabText(index, title);
    }
}

void MainWindow::setupComponentList()
{
    QStringList gates
        = {"AND Gate", "OR Gate", "NAND Gate", "NOR Gate", "EXOR Gate", "EXNOR Gate", "NOT Gate"};
    ui->componentList->addItems(gates);
}

void MainWindow::on_componentList_itemPressed(QListWidgetItem *item)
{
    if (QApplication::mouseButtons() != Qt::LeftButton) {
        return;
    }
    // Lấy scene của tab hiện tại
    QGraphicsScene *s = getCurrentScene();
    if (!s) {
        qDebug() << "Không tìm thấy Scene hiện tại!";
        return;
    }

    QString text = item->text();
    LogicGateItem *gate = nullptr;

    if (text == "AND Gate")
        gate = new LogicGateItem(LogicGateItem::AND);
    else if (text == "OR Gate")
        gate = new LogicGateItem(LogicGateItem::OR);
    else if (text == "NAND Gate")
        gate = new LogicGateItem(LogicGateItem::NAND);
    else if (text == "NOR Gate")
        gate = new LogicGateItem(LogicGateItem::NOR);
    else if (text == "EXOR Gate")
        gate = new LogicGateItem(LogicGateItem::EXOR);
    else if (text == "EXNOR Gate")
        gate = new LogicGateItem(LogicGateItem::EXNOR);
    else if (text == "NOT Gate")
        gate = new LogicGateItem(LogicGateItem::NOT);

    try {
        if (gate) {
            saveStateForUndo();
            s->addItem(gate); // Thêm vào scene hiện tại
            if (!s->views().isEmpty()) {
                QGraphicsView *currentView = s->views().first();
                // Tính toán vị trí tâm của view và chuyển sang tọa độ scene
                QPointF centerPos = currentView->mapToScene(currentView->viewport()->width() / 2,
                                                            currentView->viewport()->height() / 2);
                gate->setPos(centerPos);
                setDocumentDirty(true);
            } else {
                gate->setPos(0, 0);
                setDocumentDirty(true);
            }
        }
    } catch (...) {
        qDebug() << "Lỗi khi tạo cổng logic!";
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_W || event->text().toLower() == "w" || event->text() == "ư") {
        isWiringMode = !isWiringMode; // Đảo trạng thái

        if (isWiringMode) {
            ui->statusbar->showMessage("Chế độ: NỐI DÂY (Nhấn W để tắt)", 2000);
            setCursor(Qt::CrossCursor); // Đổi chuột thành hình chữ thập
        } else {
            ui->statusbar->showMessage("Chế độ: TƯƠNG TÁC (Nhấn W để nối dây)", 2000);
            setCursor(Qt::ArrowCursor); // Trở về chuột bình thường
        }
        return;
    }
    if (event->key() == Qt::Key_Delete) {
        saveStateForUndo();
        QGraphicsScene *s = getCurrentScene();
        if (s) {
            QList<QGraphicsItem *> selected = s->selectedItems();
            for (QGraphicsItem *item : selected) {
                s->removeItem(item);
                delete item;
            }
        }
        event->accept();
    }
    QMainWindow::keyPressEvent(event);
}

// --- SCENE ---
// Hàm lấy Scene của Tab đang mở
QGraphicsScene *MainWindow::getCurrentScene()
{
    QWidget *currentTab = ui->tabWidget->currentWidget();
    if (!currentTab)
        return nullptr;

    // Tìm QtabWidget trong Tab hiện tại
    QGraphicsView *view = currentTab->findChild<QGraphicsView *>();

    if (view && view->scene()) {
        return view->scene();
    }

    qDebug() << "Lỗi: Không tìm thấy QGraphicsView hoặc Scene trong Tab!";
    return nullptr;
}
void MainWindow::addNewTab(const QString &title)
{
    // 1. Tạo View và Scene riêng cho Tab này
    QWidget *tabPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tabPage);
    layout->setContentsMargins(0, 0, 0, 0);

    QGraphicsView *view = new QGraphicsView(tabPage);
    QGraphicsScene *newScene = new QGraphicsScene(this);

    newScene->setSceneRect(-1000, -1000, 2000, 2000);
    newScene->setBackgroundBrush(QBrush(QColor(245, 245, 245)));

    view->setScene(newScene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->centerOn(0, 0);

    layout->addWidget(view);

    // 2. Thêm vào TabWidget
    int index = ui->tabWidget->addTab(tabPage, title);
    ui->tabWidget->setCurrentIndex(index);
}
// --- FILE MENU ---
void MainWindow::on_actionNew_triggered()
{
    addNewTab("Untitled " + QString::number(ui->tabWidget->count() + 1));
}
void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Circuit", "", "*.logic");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    // Tạo tab mới để load nội dung
    addNewTab(QFileInfo(fileName).fileName());
    QGraphicsScene *s = getCurrentScene();

    QTextStream in(&file);
    QMap<int, LogicGateItem *> idToGate;
    QString section = "";

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        if (line.startsWith("[")) {
            section = line;
            continue;
        }
        if (line == "[WIRES]") {
            section = "WIRES";
            continue;
        }

        QStringList tokens = line.split(" ");

        if (section == "[GATES]" && tokens.size() >= 4) {
            QStringList parts = line.split(" ");
            if (parts.size() >= 5) {
                int id = parts[0].toInt();
                int typeInt = parts[1].toInt();
                qreal x = parts[2].toDouble();
                qreal y = parts[3].toDouble();
                bool savedValue = (parts[4].toInt() == 1);

                LogicGateItem *gate = new LogicGateItem(
                    static_cast<LogicGateItem::GateType>(typeInt));
                gate->setPos(x, y);

                // Gán giá trị đọc được vào chân Output của cổng
                if (gate->getOutputPin()) {
                    gate->getOutputPin()->setValue(savedValue);
                }
                gate->compute();

                s->addItem(gate);
                idToGate[id] = gate;
            }
        } else if (section == "[WIRES]" && tokens.size() >= 2) {
            QStringList parts = line.split(" ");
            if (parts.size() == 2) {
                int startId = tokens[0].toInt();
                int endId = tokens[1].toInt();

                LogicGateItem *startGate = idToGate[startId];
                LogicGateItem *endGate = idToGate[endId];

                PinItem *outPin = startGate->getOutputPin();
                PinItem *inPin = endGate->getInputPin(0);

                if (outPin && inPin) {
                    WireItem *wire = new WireItem(outPin, inPin);
                    s->addItem(wire);
                    outPin->addWire(wire);
                    inPin->addWire(wire);

                    wire->updatePosition();
                    wire->transmit();

                    endGate->compute();
                }
            }
        }
    }
    file.close();
    setDocumentDirty(false);
    ui->statusbar->showMessage("Đã mở file thành công!", 2000);
}
void MainWindow::on_actionSave_triggered()
{
    QGraphicsScene *s = getCurrentScene();
    if (!s)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save Circuit", "", "*.logic");
    if (fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);
    int currentIndex = ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(currentIndex, fileInfo.fileName());

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // 1. Lưu các Cổng (Gates)
        QMap<LogicGateItem *, int> gateToId;
        int idCounter = 0;

        out << "[GATES]\n";
        for (QGraphicsItem *item : s->items()) {
            LogicGateItem *gate = dynamic_cast<LogicGateItem *>(item);
            if (gate) {
                gateToId[gate] = idCounter;
                out << idCounter << " " << (int) gate->getGateType() << " " << gate->scenePos().x()
                    << " " << gate->scenePos().y() << " " << (gate->getOutputValue() ? 1 : 0)
                    << "\n";
                idCounter++;
            }
        }
        // 2. Lưu các Dây nối (Wires)
        out << "[WIRES]\n";
        for (QGraphicsItem *item : s->items()) {
            WireItem *wire = dynamic_cast<WireItem *>(item);
            if (wire) {
                LogicGateItem *startGate = dynamic_cast<LogicGateItem *>(
                    wire->getStartPin()->parentItem());
                LogicGateItem *endGate = dynamic_cast<LogicGateItem *>(
                    wire->getEndPin()->parentItem());
                // Lưu tọa độ điểm đầu và điểm cuối của dây
                if (startGate && endGate && gateToId.contains(startGate)
                    && gateToId.contains(endGate)) {
                    out << gateToId[startGate] << " " << gateToId[endGate] << "\n";
                }
            }
        }
        file.close();
        setDocumentDirty(false);
        ui->statusbar->showMessage("Đã lưu thành công!", 2000);
    } else {
        ui->statusbar->showMessage("Lưu không thành công!", 2000);
    }
}

// --- CONFIG ---
void MainWindow::on_actionConfig_triggered()
{
    QGraphicsScene *s = getCurrentScene();
    if (!s)
        return;
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        s->setBackgroundBrush(color);
    }
}
QString MainWindow::serializeScene(QGraphicsScene *s)
{
    QString result;
    QTextStream out(&result);

    QMap<LogicGateItem *, int> gateToId;
    int idCounter = 0;

    out << "[GATES]\n";
    for (QGraphicsItem *item : s->items()) {
        LogicGateItem *gate = dynamic_cast<LogicGateItem *>(item);
        if (gate) {
            gateToId[gate] = idCounter;
            out << idCounter << " " << (int)gate->getGateType() << " " 
                << gate->scenePos().x() << " " << gate->scenePos().y() << " " 
                << (gate->getOutputValue() ? 1 : 0) << "\n";
            idCounter++;
        }
    }

    out << "[WIRES]\n";
    for (QGraphicsItem *item : s->items()) {
        WireItem *wire = dynamic_cast<WireItem *>(item);
        if (wire) {
            LogicGateItem *startGate = dynamic_cast<LogicGateItem *>(wire->getStartPin()->parentItem());
            LogicGateItem *endGate = dynamic_cast<LogicGateItem *>(wire->getEndPin()->parentItem());
            if (startGate && endGate && gateToId.contains(startGate) && gateToId.contains(endGate)) {
                out << gateToId[startGate] << " " << gateToId[endGate] << "\n";
            }
        }
    }
    return result;
}
void MainWindow::deserializeScene(QGraphicsScene *s, const QString &data)
{
    s->clear(); // Xóa sạch scene hiện tại
    
    QTextStream in(const_cast<QString*>(&data), QIODevice::ReadOnly);
    QMap<int, LogicGateItem *> idToGate;
    QString section = "";

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        if (line.startsWith("[")) {
            section = line;
            continue;
        }

        QStringList tokens = line.split(" ");
        if (section == "[GATES]" && tokens.size() >= 5) {
            int id = tokens[0].toInt();
            int typeInt = tokens[1].toInt();
            qreal x = tokens[2].toDouble();
            qreal y = tokens[3].toDouble();
            bool savedValue = (tokens[4].toInt() == 1);

            LogicGateItem *gate = new LogicGateItem(static_cast<LogicGateItem::GateType>(typeInt));
            gate->setPos(x, y);
            if (gate->getOutputPin()) gate->getOutputPin()->setValue(savedValue);
            gate->compute();

            s->addItem(gate);
            idToGate[id] = gate;
        } else if (section == "[WIRES]" && tokens.size() >= 2) {
            int startId = tokens[0].toInt();
            int endId = tokens[1].toInt();

            LogicGateItem *startGate = idToGate.value(startId);
            LogicGateItem *endGate = idToGate.value(endId);

            if (startGate && endGate) {
                PinItem *outPin = startGate->getOutputPin();
                PinItem *inPin = endGate->getInputPin(0); 
                // Lưu ý: Code cũ của bạn auto lấy input 0, nếu cổng có 2 input thì bạn cần nâng cấp hàm Save/Open để lưu index của Pin nữa nhé.

                if (outPin && inPin) {
                    WireItem *wire = new WireItem(outPin, inPin);
                    s->addItem(wire);
                    outPin->addWire(wire);
                    inPin->addWire(wire);
                    wire->updatePosition();
                    wire->transmit();
                    endGate->compute();
                }
            }
        }
    }
}void MainWindow::saveStateForUndo()
{
    QGraphicsScene *s = getCurrentScene();
    if (!s) return;

    QString currentState = serializeScene(s);
    m_undoStacks[s].push(currentState);
}

void MainWindow::undo()
{
    QGraphicsScene *s = getCurrentScene();
    if (!s || !m_undoStacks.contains(s) || m_undoStacks[s].isEmpty()) {
        ui->statusbar->showMessage("Không có hành động nào để Undo", 2000);
        return;
    }

    QString previousState = m_undoStacks[s].pop();
    
    deserializeScene(s, previousState);
    
    ui->statusbar->showMessage("Đã Undo", 2000);
    setDocumentDirty(true);
}