#pragma once
/*创建鼠标的结构体*/
struct LeftMouse
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
