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

#endif // LOGICCORE_H
