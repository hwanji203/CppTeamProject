#include "InputHandler.h"
#include<Windows.h>
#include "MoveCommand.h"

ICommand* InputHandler::HandleInput()
//std::unique_ptr<ICommand> InputHandler::HandleInput()
{
    //int[] arr;
    //// for - each?
    //for (var i in arr)
    //{

    //}
    //int cur = GetTickCount64();
    //for (auto& key : m_vecKeys)
    //{
    //    bool isDown = (GetAsyncKeyState(key.vk) & 0x8000) != 0;
    //    if (isDown)
    //    {
    //        if (!key.isPrevDown)
    //        {
    //            key.isPrevDown = true;
    //            key.prevTime = cur;
    //            //return new MoveCommand(key.dir);
    //            //return std::make_unique<MoveCommand>(key.dir);
    //            return m_commandPool.GetMoveCommand(key.dir);
    //        }
    //        // ²Ú ´©¸£°í
    //        if (cur - key.prevTime >= 200)
    //        {
    //            key.prevTime = cur;
    //            return m_commandPool.GetMoveCommand(key.dir);
    //        }
    //    }
    //    else
    //    {
    //        key.isPrevDown = false;
    //        key.prevTime = 0;
    //    }
    //}
    Sleep(30);
    return nullptr;
}
