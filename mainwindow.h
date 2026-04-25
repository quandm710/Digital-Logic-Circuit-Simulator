#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    bool getWiringMode() { return isWiringMode; }
    void setDocumentDirty(bool dirty); // Hàm cập nhật trạng thái UI
    void onTabCloseRequested(int index);
    void closeEvent(QCloseEvent *event);
    void saveStateForUndo();

public slots:
    void keyPressEvent(QKeyEvent *event);
    void on_componentList_itemPressed(QListWidgetItem *item);

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionConfig_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();
    
    void on_actionZoomReset_triggered();

private:
    bool isWiringMode = false;
    void setupComponentList(); // Hàm bổ trợ
    void addNewTab(const QString &title = "New Circuit");
    Ui::MainWindow *ui;
    QGraphicsScene *getCurrentScene();
    bool isDirty = false;
    QMap<QGraphicsScene*, QStack<QString>> m_undoStacks;
    QMap<QGraphicsScene*, QStack<QString>> m_redoStacks;
    QString serializeScene(QGraphicsScene *s);
    void deserializeScene(QGraphicsScene *s, const QString &data);
    QGraphicsView *getCurrentView();
};
#endif // MAINWINDOW_H
