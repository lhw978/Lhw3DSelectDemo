#pragma once

struct LeftMouse /*鼠标的结构体*/
{
    bool IsPress;
    int x;
    int y;

    LeftMouse()
    {
        IsPress = false;
        x = 0;
        y = 0;
    }
};
