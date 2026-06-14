#include "SettingItem.h"

SettingItem::SettingItem(const string& name, const vector<string>& options, int defaultIdx)
    : m_name(name), m_options(options), m_currentIdx(defaultIdx)
{
}

void SettingItem::SelectNext()
{
    m_currentIdx = (m_currentIdx + 1) % (int)m_options.size();
}

void SettingItem::SelectPrev()
{
    m_currentIdx = (m_currentIdx - 1 + (int)m_options.size()) % (int)m_options.size();
}
