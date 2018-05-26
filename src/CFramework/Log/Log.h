#ifndef _CONSOLE_H__
#define _CONSOLE_H__

#include "standard/events.h"
#include <mutex>

namespace System
{
    enum LogType
    {
        LogNormal = 0,
        LogDetails,
        LogDebug,
        LogError,
        LogDBError,
    };

    enum Color
    {
        BLACK,
        RED,
        GREEN,
        BROWN,
        BLUE,
        MAGENTA,
        CYAN,
        GREY,
        YELLOW,
        LRED,
        LGREEN,
        LBLUE,
        LMAGENTA,
        LCYAN,
        WHITE
    };

    const int Color_count = int(WHITE) + 1;

    struct ConsoleLogEventArg : public System::Events::EventArg
    {
        ConsoleLogEventArg() : i_message(NULL), i_logType(-1)
        {
        }

        explicit ConsoleLogEventArg(std::string* message, short logType) : i_message(message), i_logType(logType)
        {
        }
        virtual ~ConsoleLogEventArg()
        {
        }

        std::string* i_message;
        short i_logType;
    };

    class TC_CFRAMEWORK_API Log
    {
    public:
        FILE* openLogFile(char const *configFileName, char const *configTimeStampFlag, char const *mode);
        FILE* logfile;
        FILE* dberLogfile;

    public:
        bool m_colored;
        bool m_includeTime;

        std::string m_logsDir;
        std::string m_logsTimestamp;
        Color m_colors[4];

        System::Events::EventHandlerThreadSafe< ConsoleLogEventArg, std::mutex > OnConsoleWrite;

        static Log* Instance();

        Log();
        ~Log();

        void Initialize();
        void InitColors(const std::string& init_str);

        void SetColor(bool stdout_stream, Color color, std::string& unix_result);
        void ResetColor(bool stdout_stream, std::string& unix_result);
        void outTime(std::string& result);
        static void outTimestamp(std::ostringstream& outFileStream);
        static std::string GetTimestampStr();

        void WriteToConsole(const char * str, va_list& ap, short logType, bool is_inline = false);
        void WriteLine(const char * str, ...)      ATTR_PRINTF(2, 3);
        void WriteErrorLine(const char * err, ...)       ATTR_PRINTF(2, 3);
        void WriteErrorDbLine(const char * str, ...)     ATTR_PRINTF(2, 3);
        void WriteDebugLine(const char * str, ...)     ATTR_PRINTF(2, 3);
        void WriteDetailLine(const char * str, ...)     ATTR_PRINTF(2, 3);
    };

    inline Log* Console()
    {
        return Log::Instance();
    }
}

#endif //_CONSOLE_H__