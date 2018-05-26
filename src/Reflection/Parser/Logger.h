#pragma once

#include "Cursor.h"
#include <string>

//
// Format and log, named and tagged text
//
#define LOG(name, tag, ...)                     \
{                                               \
    printf(__VA_ARGS__);                        \
}

struct Status
{
    static Status Warn(const std::string& message)
    {
        Status status;
        status.messages = message;
        return status;
    }

    static Status JoinWarn(const Status& older, const std::string& message)
    {
        if (older.IsSilentFail())
            return older;

        // Add the message before concatenating with the older ones
        Status status;
        status.messages = message + "; " + older.messages;
        return status;
    }

    static Status SilentFail()
    {
        Status status;
        status.messages = "SILENT FAIL";
        return status;
    }

    bool HasWarnings() const
    {
        return messages != "";
    }

    bool IsSilentFail() const
    {
        return messages == "SILENT FAIL";
    }

    void Print(Cursor location, const std::string& message)
    {
        // Don't do anything on a silent fail
        if (IsSilentFail())
            return;

        // Get text for source location
        unsigned line, column, offset;
        std::string filename = location.GetSourceFileInfo(line, column, offset);

        // Print immediate warning
        LOG(warnings, INFO, "%s(%d) : warning - %s; %s\n", filename.c_str(), line, message.c_str(), messages.c_str());
    }

    std::string messages;
};
