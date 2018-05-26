#pragma once
#ifndef __CLIENT_FILESYSTEM_INTERFACE_H__
#define __CLIENT_FILESYSTEM_INTERFACE_H__

#include <standard/Platform/Common.h>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

namespace System
{
    namespace Platform
    {
        //Returns the last Win32 error, in string format. Returns an empty string if there is no error.
        std::string TC_CFRAMEWORK_API GetLastErrorAsString(DWORD errorMessageID);
    }

    namespace FileSystem
    {
        typedef std::vector<boost::filesystem::path> TFileList;
        void TC_CFRAMEWORK_API GetFileList(boost::filesystem::path const& path, TFileList& result_set, bool recurisve, const boost::regex* filter = nullptr);
        void TC_CFRAMEWORK_API CreateDir(boost::filesystem::path const& path);

        inline bool FileExists(char const* fileName)
        {
            namespace fs = boost::filesystem;
            return fs::exists(fileName);
        }
    }
}

#endif //__CLIENT_FILESYSTEM_INTERFACE_H__