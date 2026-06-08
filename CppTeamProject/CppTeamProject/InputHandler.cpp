#include "InputHandler.h"
#include<Windows.h>
#include "VelocityCommand.h"
#include "Console.h"

ICommand* InputHandler::HandleInput()
{
    int delta = GetMouseWheelChange();
    GotoXY(6, 6);
    cout << delta;
    if (delta != 0)
        return m_commandPool.GetVelocityCommand(delta);

    return nullptr;
}
