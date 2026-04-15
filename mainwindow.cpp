#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logicgateitem.h"
#include <QGraphicsView>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QPoint>
#include <QPointF>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tabWidget->setTabsClosable(true);

    addNewTab("Untitled 1");

    setupComponentList();

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
        // Có thể thêm bước hỏi "Bạn có muốn lưu không?" ở đây
        QWidget* w = ui->tabWidget->widget(index);
        ui->tabWidget->removeTab(index);
        delete w; // Xóa để giải phóng bộ nhớ
    });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupComponentList() {
    QStringList gates = {"AND Gate", "OR Gate", "NAND Gate", "NOR Gate", "EXOR Gate", "EXNOR Gate", "NOT Gate"};
    ui->componentList->addItems(gates);
}

void MainWindow::on_componentList_itemPressed(QListWidgetItem *item) {
    // Lấy scene của tab hiện tại
    QGraphicsScene* s = getCurrentScene();
    if (!s) {
        qDebug() << "Không tìm thấy Scene hiện tại!";
        return;
    }

    QString text = item->text();
    LogicGateItem* gate = nullptr;

    if (text == "AND Gate") gate = new LogicGateItem(LogicGateItem::AND);
    else if (text == "OR Gate") gate = new LogicGateItem(LogicGateItem::OR);
    else if (text == "NAND Gate") gate = new LogicGateItem(LogicGateItem::NAND);
    else if (text == "NOR Gate") gate = new LogicGateItem(LogicGateItem::NOR);
    else if (text == "EXOR Gate") gate = new LogicGateItem(LogicGateItem::EXOR);
    else if (text == "EXNOR Gate") gate = new LogicGateItem(LogicGateItem::EXNOR);
    else gate = new LogicGateItem(LogicGateItem::NOT);

    try {
        if (gate) {
            s->addItem(gate); // Thêm vào scene hiện tại
            gate->setPos(0, 0);
        }
    } catch (...) {
        qDebug() << "Lỗi khi tạo cổng logic!";
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W || event->text().toLower() == "w" || event->text() == "ư") {
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
        QGraphicsScene* s = getCurrentScene();
        QList<QGraphicsItem*> selected = s->selectedItems();
        for (QGraphicsItem* item : selected) {
            s->removeItem(item);
            delete item;
        }
    }
    QMainWindow::keyPressEvent(event);
}

// --- SCENE ---
// Hàm lấy Scene của Tab đang mở
QGraphicsScene* MainWindow::getCurrentScene() {
    QWidget* currentTab = ui->tabWidget->currentWidget();
    if (!currentTab) return nullptr;

    // Tìm QtabWidget trong Tab hiện tại
    QGraphicsView* view = currentTab->findChild<QGraphicsView*>();
    return view ? view->scene() : nullptr;
}
void MainWindow::addNewTab(const QString &title) {
    // 1. Tạo View và Scene riêng cho Tab này
    QGraphicsView* view = new QGraphicsView();
    QGraphicsScene* newScene = new QGraphicsScene(this);

    newScene->setSceneRect(-1000, -1000, 2000, 2000);
    newScene->setBackgroundBrush(QBrush(QColor(245, 245, 245)));

    view->setScene(newScene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->centerOn(0, 0);

    // 2. Thêm vào TabWidget
    int index = ui->tabWidget->addTab(view, title);
    ui->tabWidget->setCurrentIndex(index);
}
// --- FILE MENU ---
void MainWindow::on_actionNew_triggered() {
    addNewTab("Untitled " + QString::number(ui->tabWidget->count() + 1));
}
void MainWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Circuit", "", "*.logic");
    if (fileName.isEmpty()) return;

    // Tạo Tab mới với tên là tên file
    addNewTab(QFileInfo(fileName).fileName());
    QGraphicsScene* s = getCurrentScene();

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList tokens = line.split(" ");
            if (tokens.size() >= 4 && tokens[0] == "GATE") {
                LogicGateItem::GateType type = static_cast<LogicGateItem::GateType>(tokens[1].toInt());
                LogicGateItem* gate = new LogicGateItem(type);
                gate->setPos(tokens[2].toDouble(), tokens[3].toDouble());
                s->addItem(gate);
            }
        }
        file.close();
    }
}
void MainWindow::on_actionSave_triggered() {
    QGraphicsScene* s = getCurrentScene();
    if (!s) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save Circuit", "", "*.logic");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // Người 4: Duyệt qua các item để lưu tọa độ
        for (QGraphicsItem* item : s->items()) {
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
    QGraphicsScene* s = getCurrentScene();
    if (!s) return;
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) s->setBackgroundBrush(color);
}

// --- SIMULATION ---
void MainWindow::on_actionRun_triggered() {
    QGraphicsScene* s = getCurrentScene();
    if (!s) return;

    s->setBackgroundBrush(QBrush(QColor(240, 255, 240))); // Màu xanh nhạt báo hiệu đang chạy
    ui->statusbar->showMessage("Simulation is running...");

}

void MainWindow::on_actionStop_triggered() {
    // Trả lại màu nền cũ
    QGraphicsScene* s = getCurrentScene();
    if (!s) return;
    s->setBackgroundBrush(QBrush(Qt::white));
    ui->statusbar->showMessage("Simulation stopped.");
}