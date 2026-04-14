#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsPathItem>
#include <QPen>

class PinItem; // Khai báo trước để trình biên dịch không lỗi

class WireItem : public QGraphicsPathItem {
public:
    WireItem(PinItem* start, PinItem* end, QGraphicsItem* parent = nullptr);

    void updatePosition(); // Hàm then chốt: Để dây tự hít vào Pin
    void transmit();       // Hàm then chốt: Để truyền điện từ 0 sang 1

private:
    PinItem *m_startPin;
    PinItem *m_endPin;
};

#endif // WIRE_H