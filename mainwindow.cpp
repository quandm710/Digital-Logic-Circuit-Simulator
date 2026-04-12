#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logicgateitem.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QPoint>
#include <QPointF>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(-1000, -1000, 2000, 2000);

    ui->graphicsView->setScene(scene);
    // Khử răng cưa và tối ưu hóa vẽ
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::TextAntialiasing);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    // Giúp việc kéo thả mượt hơn
    ui->graphicsView->setOptimizationFlag(QGraphicsView::IndirectPainting);

    scene->setBackgroundBrush(QBrush(QColor(245, 245, 245)));
    setupComponentList();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupComponentList() {
    QStringList gates = {"AND Gate", "OR Gate", "NAND Gate", "NOR Gate", "EXOR Gate", "EXNOR Gate", "NOT Gate"};
    ui->componentList->addItems(gates);
}

void MainWindow::on_componentList_itemPressed(QListWidgetItem *item) {
    QString text = item->text();
    LogicGateItem::GateType type;

    if (text == "AND Gate") type = LogicGateItem::AND;
    else if (text == "OR Gate") type = LogicGateItem::OR;
    else if (text == "NAND Gate") type = LogicGateItem::NAND;
    else if (text == "NOR Gate") type = LogicGateItem::NOR;
    else if (text == "EXOR Gate") type = LogicGateItem::EXOR;
    else if (text == "EXNOR Gate") type = LogicGateItem::EXNOR;
    else type = LogicGateItem::NOT;

    try {
        auto gate = new LogicGateItem(type);
        gate->setPos(0,0); // Tạo tại tâm tọa độ
        scene->addItem(gate);
        scene->clearSelection();
        gate->setSelected(true);
        scene->update();
    } catch (...) {
        qDebug() << "Lỗi khi tạo cổng logic!";
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W) {
        isWiringMode = !isWiringMode; // Đảo trạng thái

        if (isWiringMode) {
            ui->statusbar->showMessage("Chế độ: NỐI DÂY (Nhấn W để tắt)", 2000);
            setCursor(Qt::CrossCursor); // Đổi chuột thành hình chữ thập
        } else {
            ui->statusbar->showMessage("Chế độ: TƯƠNG TÁC (Nhấn W để nối dây)", 2000);
            setCursor(Qt::ArrowCursor); // Trở về chuột bình thường
        }
    }
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        QList<QGraphicsItem*> selected = scene->selectedItems();
        for (QGraphicsItem* item : selected) {
            scene->removeItem(item);
            delete item;
        }
    }
    QMainWindow::keyPressEvent(event);
}

// --- FILE MENU ---
void MainWindow::on_actionNew_triggered() {
    scene->clear();
}

void MainWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Circuit", "", "*.logic");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QStringList tokens = in.readLine().split(" ");
            if (tokens[0] == "GATE") {
                auto gate = new LogicGateItem((LogicGateItem::GateType)tokens[1].toInt());
                gate->setPos(tokens[2].toDouble(), tokens[3].toDouble());
                scene->addItem(gate);
            }
        }
        file.close();
    }
}

void MainWindow::on_actionSave_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Circuit", "", "*.logic");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // Người 4: Duyệt qua các item để lưu tọa độ
        for (QGraphicsItem* item : scene->items()) {
            LogicGateItem* gate = dynamic_cast<LogicGateItem*>(item);
            if (gate) {
                out << "GATE " << gate->getGateType() << " "
                    << gate->x() << " " << gate->y() << "\n";
            }
        }
        file.close();
    }
}

// --- CONFIG ---
void MainWindow::on_actionConfig_triggered() {
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) scene->setBackgroundBrush(color);
}

// --- SIMULATION ---
void MainWindow::on_actionRun_triggered() {
    // Đổi màu nền sang xanh nhẹ để báo hiệu đang chạy
    scene->setBackgroundBrush(QBrush(QColor(240, 255, 240)));
    ui->statusbar->showMessage("Simulation is running...");
    // Gọi thread mô phỏng của bạn ở đây
}

void MainWindow::on_actionStop_triggered() {
    // Trả lại màu nền cũ
    scene->setBackgroundBrush(QBrush(Qt::white));
    ui->statusbar->showMessage("Simulation stopped.");
}