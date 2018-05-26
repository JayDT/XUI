#include "stdafx.h"
#include "Log.h"

using namespace System;

Log * System::Log::Instance()
{
    static Log m_instance;
    return &m_instance;
}

Log::Log(): logfile(nullptr), dberLogfile(nullptr), m_colored(true), m_includeTime(true)
{
    Initialize();
}

Log::~Log()
{
    if (logfile != NULL)
        fclose(logfile);
    logfile = NULL;

    if (dberLogfile != NULL)
        fclose(dberLogfile);
    dberLogfile = NULL;
}

const int LogType_count = int(LogError) + 1;

#ifdef WIN32
HANDLE hConsole = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
#endif

static Color LastColor = Color(0);
void Log::SetColor(bool stdout_stream, Color color, std::string& unix_result)
{

    if (LastColor == color)
        return;

#if PLATFORM == PLATFORM_WINDOWS

    static WORD WinColorFG[Color_count] =
    {
        0,                                                  // BLACK
        FOREGROUND_RED,                                     // RED
        FOREGROUND_GREEN,                                   // GREEN
        FOREGROUND_RED | FOREGROUND_GREEN,                  // BROWN
        FOREGROUND_BLUE,                                    // BLUE
        FOREGROUND_RED | FOREGROUND_BLUE,// MAGENTA
        FOREGROUND_GREEN | FOREGROUND_BLUE,                 // CYAN
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,// WHITE
                                                            // YELLOW
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        // RED_BOLD
        FOREGROUND_RED | FOREGROUND_INTENSITY,
        // GREEN_BOLD
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,             // BLUE_BOLD
                                                            // MAGENTA_BOLD
        FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        // CYAN_BOLD
        FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        // WHITE_BOLD
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    };

    //HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE /*: STD_ERROR_HANDLE*/ ); //stdout_stream ?
    SetConsoleTextAttribute(hConsole, LastColor = (Color)WinColorFG[color]);
#else

    enum ANSITextAttr
    {
        TA_NORMAL = 0,
        TA_BOLD = 1,
        TA_BLINK = 5,
        TA_REVERSE = 7
    };

    enum ANSIFgTextAttr
    {
        FG_BLACK = 30, FG_RED, FG_GREEN, FG_BROWN, FG_BLUE,
        FG_MAGENTA, FG_CYAN, FG_WHITE, FG_YELLOW
    };

    enum ANSIBgTextAttr
    {
        BG_BLACK = 40, BG_RED, BG_GREEN, BG_BROWN, BG_BLUE,
        BG_MAGENTA, BG_CYAN, BG_WHITE
    };

    static uint8 UnixColorFG[Color_count] =
    {
        FG_BLACK,                                           // BLACK
        FG_RED,                                             // RED
        FG_GREEN,                                           // GREEN
        FG_BROWN,                                           // BROWN
        FG_BLUE,                                            // BLUE
        FG_MAGENTA,                                         // MAGENTA
        FG_CYAN,                                            // CYAN
        FG_WHITE,                                           // WHITE
        FG_YELLOW,                                          // YELLOW
        FG_RED,                                             // LRED
        FG_GREEN,                                           // LGREEN
        FG_BLUE,                                            // LBLUE
        FG_MAGENTA,                                         // LMAGENTA
        FG_CYAN,                                            // LCYAN
        FG_WHITE                                            // LWHITE
    };
    char temp[64];
    sprintf(&temp[0], "\x1b[%d%sm", UnixColorFG[color], (color >= YELLOW&&color<Color_count ? ";1" : ""));
    unix_result.append(temp);
#endif
}

void Log::InitColors(const std::string& str)
{
    if (str.empty())
    {
        m_colored = false;
        return;
    }

    int color[4];

    System::String ss(str);
    auto colorStr = ss.Split(' ');

    for (int i = 0; i < LogType_count; ++i)
    {
        if (i < colorStr.size())
            color[i] = atoi(colorStr[i].c_str());
        else
            color[i] = 0;

        if (color[i] >= Color_count)
            color[i] = 0;
    }

    for (int i = 0; i < LogType_count; ++i)
        m_colors[i] = Color(color[i]);

    m_colored = true;
}

void Log::ResetColor(bool stdout_stream, std::string& unix_result)
{
#if PLATFORM == PLATFORM_WINDOWS
    //HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE );
    //SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED );
    //unix_result.append("");
#else
    char temp[16];
    snprintf(&temp[0], 16, "\x1b[0m");
    unix_result.append(temp);
#endif
}

void Log::outTimestamp(std::ostringstream& outFileStream)
{
    tm aTm;
    time_t t = time(NULL);
    tm* tm = localtime(&t);
    aTm = *tm;
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char temp[512];
    sprintf(temp, "%-4d-%02d-%02d %02d:%02d:%02d ", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
    outFileStream << temp;
}

void Log::outTime(std::string& result)
{
    tm aTm;
    time_t t = time(NULL);
    tm* tm = localtime(&t);
    aTm = *tm;
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char temp[512];
    sprintf(&temp[0], "%02d:%02d:%02d ", aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
    result.append(temp);
}

std::string Log::GetTimestampStr()
{
    tm aTm;
    time_t t = time(NULL);
    tm* tm = localtime(&t);
    aTm = *tm;
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char buf[128];
    snprintf(buf, 20, "%04d-%02d-%02d_%02d-%02d-%02d", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
    return std::string(buf);
}

FILE* Log::openLogFile(char const *configFileName, char const *configTimeStampFlag, char const *mode)
{
    std::string logfn = configFileName;
    if (logfn.empty())
        return NULL;

    //if (configTimeStampFlag && sConfig.GetBoolDefault(configTimeStampFlag, false))
    {
        size_t dot_pos = logfn.find_last_of(".");
        if (dot_pos != logfn.npos)
            logfn.insert(dot_pos, m_logsTimestamp);
        else
            logfn += m_logsTimestamp;
    }

    return fopen((m_logsDir + logfn).c_str(), mode);
}

void Log::Initialize()
{
    /// Common log files data
    m_logsDir = "Logs";
    if (!m_logsDir.empty())
    {
        if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
            m_logsDir.append("/");
    }

    m_logsTimestamp = "_" + GetTimestampStr();

    /// Open specific log files
    logfile = openLogFile("Server.log", nullptr, "w");
    dberLogfile = openLogFile("DBError.log", nullptr, "w");

    // Main log file settings
    m_includeTime = true;
    InitColors("13 7 11 9");
}


void Log::WriteToConsole(const char * str, va_list& ap, short logType, bool is_inline)
{
    if (!str)
        return;

    short colorId = logType;
    bool show = logType != LogDBError;

    std::string result;
    if (m_colored)
        SetColor(true, m_colors[colorId], result);
    char temp[4096];
    vsnprintf(&temp[0], 4096, str, ap);

    if (show)
    {
        if (m_includeTime)
            outTime(result);

        result.append(temp);

        // flush buffer
        if (!OnConsoleWrite.empty())
        {
            ConsoleLogEventArg arg(&result, logType);
            OnConsoleWrite(this, arg);
        }

#ifdef WIN32
        if (!is_inline)
            result.append("\r\n");

        WriteConsole(hConsole, result.c_str(), result.size(), NULL, NULL);
#else
        if (!is_inline)
            std::cout << result.c_str() << '\n'; //Use of std::endl in place of '\n', encouraged by some sources, may significantly degrade output performance. (5000 times)
        else
            std::cout << result.c_str();
#endif
    }

    // TODO:
    // use stderr

    if (logfile)
    {
        std::ostringstream _stream;
        outTimestamp(_stream);
        switch (logType)
        {
            case LogDBError:
            case LogError:
                _stream << "ERROR:";
                break;
        }

        _stream << temp;
        if (!is_inline)
            _stream << "\n";

        fputs(_stream.str().c_str(), logfile);
        fflush(logfile);
    }
    switch (logType)
    {
        case LogDBError:
        {
            if (dberLogfile)
            {
                std::ostringstream _stream;
                outTimestamp(_stream);
                switch (logType)
                {
                    case LogDBError:
                    case LogError:
                        _stream << "ERROR:";
                        break;
                }

                _stream << temp;
                if (!is_inline)
                    _stream << "\n";

                fputs(_stream.str().c_str(), dberLogfile);
                fflush(dberLogfile);
            }
            break;
        }
    };
}

void Log::WriteLine(const char * str, ...)
{
    if (!str)
        return;

    va_list ap;
    va_start(ap, str);
    WriteToConsole(str, ap, LogNormal);
    va_end(ap);
}

void Log::WriteErrorLine(const char * err, ...)
{
    if (!err)
        return;

    va_list ap;
    va_start(ap, err);
    WriteToConsole(err, ap, LogError);
    va_end(ap);
}

void Log::WriteErrorDbLine(const char * err, ...)
{
    if (!err)
        return;

    va_list ap;
    va_start(ap, err);
    WriteToConsole(err, ap, LogDBError);
    va_end(ap);
}

void Log::WriteDebugLine(const char * str, ...)
{
    if (!str)
        return;

    va_list ap;
    va_start(ap, str);
    WriteToConsole(str, ap, LogDebug);
    va_end(ap);
}

void Log::WriteDetailLine(const char * str, ...)
{
    if (!str)
        return;

    va_list ap;
    va_start(ap, str);
    WriteToConsole(str, ap, LogDetails);
    va_end(ap);
}