#ifndef LOGICCORE_H
#define LOGICCORE_H
#include <vector>
#include <memory>

using namespace std;
class LogicGate {
public:
    virtual ~LogicGate() = default;
    virtual bool compute(const vector<bool>& inputs) = 0;
};

class AndGate : public LogicGate {
public:
    bool compute(const vector<bool>& inputs) override {
        if (inputs.size() < 2) return false;
        return inputs[0] && inputs[1];
    }
};

// Người 2 & 3 sẽ tiếp tục viết OrGate, NotGate... ở đây
class NandGate : public LogicGate {
public:
    bool compute(const vector<bool>& inputs) override {
        if (inputs.size() < 2) return true;
        return !(inputs[0] && inputs[1]); //phủ định của And
    }
};
class NotGate : public LogicGate {
public:
    bool compute(const vector<bool>& inputs) override {
        if (inputs.empty()) return false;
        return !inputs[0];
    }
};
class OrGate : public LogicGate {
public:
    bool compute(const vector<bool>& inputs) override {
        if (inputs.size() < 2) return false;
        return inputs[0] || inputs[1];// trả về true nếu 1 trong 2 vị trí là true
    }
};
class NorGate : public LogicGate {
public:
    bool compute(const vector<bool>& inputs) override {
        if (inputs.size() < 2) return true;
        return !(inputs[0] || inputs[1]);// phủ định lại với cổng OrGate
    }
};
class ExorGate : public LogicGate {
public:
    bool compute(const vector<bool>& inputs) override {
        if (inputs.size() < 2) return false;
        bool A = inputs[0];
        bool B = inputs[1];
        return (!A && B) || (A && !B); //(NOT A AND B) OR (A AND NOT B)
    }
};
class ExnorGate : public LogicGate {
public:
    bool compute(const vector<bool>& inputs) override {
        if (inputs.size() < 2) return true;
        bool A = inputs[0];
        bool B = inputs[1];
        return (A && B) || (!A && !B); //(A AND B) OR (NOT A AND NOT B)
    }
};
#endif // LOGICCORE_H
