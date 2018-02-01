#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#include <string>
#include <ctime>
#include <sstream>
#include <fstream>

//Helper.h has general purpose functions that will be used throughout the project
namespace Helper
{

    //this function will convert any variable type to a string
    //that's why it's a template so it can accept many different types of variables
    template <class T>
    std::string ToString(const T &);

    struct DateTime
    {
        DateTime()
        {
            time_t ms;
             //places the system time into the variable ms by passing in the address of ms
            time(&ms);
            //localtime() converts the "raw" time data in ms to local time
            //struct tm is part of the ctime library and contains the day, month, year, hour etc of local time
            struct tm *info = localtime(&ms);

            D=info->tm_mday; //day
            m=info->tm_mon +1; //month +1 since counting starts at 0
            y=1900 + info->tm_year; // year
            M=info->tm_min; //minutes
            H=info->tm_hour; //hours
            S=info->tm_sec; //seconds
        }

        DateTime(int D, int m, int y, int H, int M, int S) : D(D), m(m), y(y), H(H), M(M), S(S) {}
        DateTime(int D, int m, int y) : D(D), m(m), y(y), H(0), M(0), S(0) {}

        DateTime Now() const
        {
            return DateTime();
        }

        int D, m, y, H, M, S;

        std::string GetDateString() const
        {
            //returns Date in the DD.mm.YYYY format
            return std::string(D < 10 ? "0" : "") + ToString(D) + std::string(M < 10 ? ".0" : ".") + ToString(m) + "." + ToString(y);

        }

        std::string GetTimeString(const std::string &sep = ":") const
        {
            //returns time in the following format HH:MM:SS
            return std::string(H < 10 ? "0" : "") + ToString(H) + sep +
                   std::string(M < 10 ? "0" : "") + ToString(M) + sep +
                   std::string(S < 10 ? sep : "") + ToString(S);

        }

        std::string GetDateTimeString(const std::string &sep = ":") const
        {
            return GetDateString() + " " + GetTimeString(sep);
        }


    };

    //converts different types to string
    template <class T>
    std::string ToString(const T &e)
    {
        std::ostringstream s;
        s << e;
        return s.str();
    }

    //writes to the log
    void WriteAppLog(const std::string &s)
    {
        std::ofstream file("AppLog.txt", std::ios::app);
        file << "[" << Helper::DateTime().GetDateTimeString() << "]" <<
        "\n" << s << std::endl << "\n";

        file.close();
    }

}


#endif // HELPER_H_INCLUDED
