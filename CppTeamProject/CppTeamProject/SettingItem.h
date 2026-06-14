#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

class SettingItem
{
public:
    SettingItem(const string& name, const vector<string>& options, int defaultIdx = 0);

    string GetName() const { return m_name; }
    string GetCurrentOption() const { return m_options[m_currentIdx]; }
    int GetCurrentIndex() const { return m_currentIdx; }

    void SelectNext();
    void SelectPrev();

private:
    string m_name;
    vector<string> m_options;
    int m_currentIdx;
};
