#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H
#include <QThread>

class SimulationThread : public QThread {
    Q_OBJECT
    void run() override {
        while (!isInterruptionRequested()) {
            // 1. Lấy dữ liệu từ các cổng đầu vào (Input Sources)
            // 2. Duyệt qua danh sách Wire để truyền tín hiệu
            // 3. Gọi hàm compute() của Người 2 & 3
            // 4. Cập nhật màu sắc cổng trên UI (Sử dụng Signal/Slot)

            msleep(100); // Giới hạn tốc độ mô phỏng 10 FPS
        }
    }
};
#endif // SIMULATIONTHREAD_H
