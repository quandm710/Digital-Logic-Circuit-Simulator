#ifndef LOGICCORE_H
#define LOGICCORE_H
#include <memory>
#include <vector>

using namespace std;
class LogicGate
{
protected:
    vector<bool> m_lastInputs;
    bool m_output = false;

public:
    virtual ~LogicGate() = default;

    virtual bool compute(const vector<bool> &inputs) = 0;

    virtual bool getOutput() const { return m_output; }
    virtual void setInput(int index, bool value)
    {
        if (index >= 0 && index < (int) m_lastInputs.size()) {
            m_lastInputs[index] = value;
        }
    }
};

// --- LOGIC GATES ---
class AndGate : public LogicGate
{
public:
    bool compute(const vector<bool> &inputs) override
    {
        if (inputs.size() < 2)
            return false;
        m_output = inputs[0] && inputs[1];
        return m_output;
    }
};

class OrGate : public LogicGate
{
public:
    bool compute(const vector<bool> &inputs) override
    {
        if (inputs.size() < 2)
            return false;
        m_output = inputs[0] || inputs[1];
        return m_output;
    }
};

class NandGate : public LogicGate
{
public:
    bool compute(const vector<bool> &inputs) override
    {
        if (inputs.size() < 2)
            return true;
        m_output = !(inputs[0] && inputs[1]);
        return m_output;
    }
};

class NorGate : public LogicGate
{
public:
    bool compute(const vector<bool> &inputs) override
    {
        if (inputs.size() < 2)
            return true;
        m_output = !(inputs[0] || inputs[1]);
        return m_output;
    }
};

class NotGate : public LogicGate
{
public:
    bool compute(const vector<bool> &inputs) override
    {
        if (inputs.empty())
            return false;
        m_output = !inputs[0];
        return m_output;
    }
};

class ExorGate : public LogicGate
{
public:
    bool compute(const vector<bool> &inputs) override
    {
        if (inputs.size() < 2)
            return false;
        // XOR: true nếu 2 đầu vào khác nhau
        m_output = (inputs[0] != inputs[1]);
        return m_output;
    }
};

class ExnorGate : public LogicGate
{
public:
    bool compute(const vector<bool> &inputs) override
    {
        if (inputs.size() < 2)
            return true;
        // XNOR: true nếu 2 đầu vào giống nhau
        m_output = (inputs[0] == inputs[1]);
        return m_output;
    }
};
#endif // LOGICCORE_H
