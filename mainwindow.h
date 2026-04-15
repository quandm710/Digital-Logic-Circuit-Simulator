#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsScene>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    bool getWiringMode() { return isWiringMode; }

public slots:
    void keyPressEvent(QKeyEvent *event);
    void on_componentList_itemPressed(QListWidgetItem *item);

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionRun_triggered();

    void on_actionStop_triggered();

    void on_actionConfig_triggered();

private:
    bool isWiringMode = false;
    void setupComponentList(); // Hàm bổ trợ
    void addNewTab(const QString &title = "New Circuit");
    Ui::MainWindow *ui;
    QGraphicsScene* getCurrentScene();
};
#endif // MAINWINDOW_H
