#include "InputHandler.h"
#include<Windows.h>
#include "MoveCommand.h"
#include "Console.h"

InputHandler::InputHandler()
{
    m_vecKeys['W'] = Dir::UP;
    m_vecKeys['w'] = Dir::UP;
    m_vecKeys['S'] = Dir::DOWN;
    m_vecKeys['s'] = Dir::DOWN;
    m_vecKeys['A'] = Dir::LEFT;
    m_vecKeys['a'] = Dir::LEFT;
    m_vecKeys['D'] = Dir::RIGHT;
    m_vecKeys['d'] = Dir::RIGHT;
}

ICommand* InputHandler::HandleInput()
{
    int cur = GetTickCount64();
    for (auto& key : m_vecKeys)
    {
        if (GetKey(key.first))
            return m_commandPool.GetMoveCommand(key.second);
    }

    return nullptr;
}
