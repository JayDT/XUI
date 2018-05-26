#include "StdAfx.h"
#include "filesystem.h"

#ifdef _WIN32
#include "direct.h"
#else
#include <sys/stat.h>
#define ERROR_PATH_NOT_FOUND ERROR_FILE_NOT_FOUND
#endif

void System::FileSystem::GetFileList(boost::filesystem::path const & path, TFileList & result_set, bool recurisve, const boost::regex * filter)
{
    namespace fs = boost::filesystem;

    fs::directory_iterator end_iter;
    if (fs::exists(path) && fs::is_directory(path))
    {
        for (fs::directory_iterator dir_iter(path); dir_iter != end_iter; ++dir_iter)
        {
            if (recurisve && fs::is_directory(dir_iter->status()))
            {
                GetFileList(*dir_iter, result_set, recurisve);
            }
            else if (fs::is_regular_file(dir_iter->status()))
            {
                boost::smatch what;
                if (!filter || boost::regex_match(dir_iter->path().filename().string(), what, *filter))
                    result_set.push_back(*dir_iter);
            }
        }
    }
}

void System::FileSystem::CreateDir(boost::filesystem::path const & path)
{
    //            if (chdir(path.generic_string().c_str()) == 0)
    //            {
    //                chdir("../");
    //                return;
    //            }
    //#ifdef _WIN32
    //            _mkdir(path.generic_string().c_str());
    //#else
    //            mkdir(path.generic_string().c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // 0777
    //#endif
    namespace fs = boost::filesystem;
    if (fs::exists(path))
        return;

    if (!fs::create_directory(path))
        throw std::runtime_error("Unable to create directory" + path.string());
}

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string System::Platform::GetLastErrorAsString(DWORD errorMessageID)
{
#ifdef WIN32
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
#else
    return "none";
#endif
}
