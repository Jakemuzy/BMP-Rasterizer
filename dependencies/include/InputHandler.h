//  Only want the windows INPUT none of the other bloat
#define WIN32_LEAN_AND_MEAN    
#define NOMINMAX               
#define NOUSER                 
#define NOGDI                  
#define NOCRYPT                
#define NOSOUND                
#define NOWINOFFSETS           
#define NOCOMM                 
#define NOKERNEL               
#define NOSERVICE              
#define NOMCX                  
#include <windows.h>        //  Sorry everyone this is only for windows rn >_<

enum operating_system {
    WINDOWS, UNIX, MAC
};

struct inputs {
    bool firstMouseMovement = true;
    bool w = false, a = false, s = false, d = false, esc = false;

    long lastX = 0, lastY = 0;
    POINT cursor = {0, 0};
};

class InputHandler {
private:
    inputs currentInputs;
    operating_system os;
public:
    InputHandler(operating_system _os)
    {
        //  Different logic for different os
    }

    //  Windows
    void listen(){
        if (GetAsyncKeyState('W') & 0x8000)
            currentInputs.w = true;
        else 
            currentInputs.w = false;

        if (GetAsyncKeyState('A') & 0x8000)
            currentInputs.a = true;
        else 
            currentInputs.a = false;

        if (GetAsyncKeyState('S') & 0x8000)
            currentInputs.s = true;
        else 
            currentInputs.s = false;
            
        if (GetAsyncKeyState('D') & 0x8000)
            currentInputs.d = true;
        else 
            currentInputs.d = false;

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            currentInputs.esc = true;
        else
            currentInputs.esc = false;

        //  Mouse Pos
        currentInputs.lastX = currentInputs.cursor.x;
        currentInputs.lastY = currentInputs.cursor.y;
        if (GetCursorPos(&currentInputs.cursor))
        {
            if (currentInputs.firstMouseMovement)
                currentInputs.firstMouseMovement = false;
        }
    }
    
    inputs getInputs(){
        return currentInputs;
    }

};

