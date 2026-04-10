#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logicgateitem.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QKeyEvent>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFocusPolicy(Qt::StrongFocus);

    // Tạo Scene (Không gian chứa vật thể)
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing); // Làm mượt nét vẽ
    setupComponentList();
}

// Hàm xử lý khi nhấn nút thêm cổng AND trên UI
void MainWindow::on_btnAddAnd_clicked() {
    auto gate = new LogicGateItem(LogicGateItem::AND);
    gate->setPos(50, 50); // Vị trí mặc định khi mới tạo
    if (scene) {
        scene->addItem(gate);
        scene->update();
    }
}

void MainWindow::setupComponentList() {
    ui->componentList->addItem("AND Gate");
    ui->componentList->addItem("OR Gate");
    ui->componentList->addItem("NAND Gate");
    ui->componentList->addItem("NOR Gate");
    ui->componentList->addItem("NOT Gate");
    ui->componentList->addItem("EXOR Gate");
    ui->componentList->addItem("EXNOR Gate");
}

void MainWindow::on_componentList_itemPressed(QListWidgetItem *item) {
    LogicGateItem::GateType type;
    QString text = item->text(); // Lấy tên cổng

    // Kiểm tra chính xác tên đã add trong setupComponentList
    if (text == "AND Gate") type = LogicGateItem::AND;
    else if (text == "OR Gate") type = LogicGateItem::OR;
    else if (text == "NAND Gate") type = LogicGateItem::NAND;
    else if (text == "NOR Gate") type = LogicGateItem::NOR;
    else if (text == "EXOR Gate") type = LogicGateItem::EXOR;
    else if (text == "EXNOR Gate") type = LogicGateItem::EXNOR;
    else type = LogicGateItem::NOT;

    // Tạo cổng ngay lập tức khi nhấn (trước khi drag)
    auto gate = new LogicGateItem(type);
    gate->setPos(50, 50);
    scene->addItem(gate);

    // Sau đó mới thực hiện Drag & Drop nếu cần
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(text);
    drag->setMimeData(mimeData);
    drag->exec();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        // Lấy danh sách các cổng đang được chọn trên Workspace
        QList<QGraphicsItem*> selectedItems = scene->selectedItems();
        for (QGraphicsItem* item : selectedItems) {
            scene->removeItem(item);
            delete item; // Giải phóng bộ nhớ
        }
    }
    QMainWindow::keyPressEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}
