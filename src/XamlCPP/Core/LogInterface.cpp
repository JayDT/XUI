/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "LogInterface.h"

XamlCPP::Platform::ILogHandler* XamlCPP::Platform::ILog::_handle = nullptr;
XamlCPP::Platform::LogEventLevel XamlCPP::Platform::ILog::_logLevelLimit = XamlCPP::Platform::LogEventLevel::Error;

void XamlCPP::Platform::ILog::Log(LogEventLevel level, std::string const & area, System::Reflection::meta::Variant const * source, const char * messageTemplate, va_list args)
{
    if (_handle)
        _handle->Log(level, area, source, messageTemplate, args);
}

void XamlCPP::Platform::ILog::SetLogHandle(ILogHandler * handle)
{
    _handle = handle;
}

XamlCPP::Platform::LogEventLevel XamlCPP::Platform::ILog::GetLogLimit()
{
    return _logLevelLimit;
}

void XamlCPP::Platform::ILog::Verbose(std::string const & area, System::Reflection::meta::Variant const * source, const char * messageTemplate, ...)
{
    if (_logLevelLimit < LogEventLevel::Verbose)
        return;

    va_list arguments;
    va_start(arguments, messageTemplate);
    Log(LogEventLevel::Verbose, area, source, messageTemplate, arguments);
    va_end(arguments);
}

void XamlCPP::Platform::ILog::Debug(std::string const & area, System::Reflection::meta::Variant const * source, const char * messageTemplate, ...)
{
    if (_logLevelLimit < LogEventLevel::Debug)
        return;

    va_list arguments;
    va_start(arguments, messageTemplate);
    Log(LogEventLevel::Debug, area, source, messageTemplate, arguments);
    va_end(arguments);
}

void XamlCPP::Platform::ILog::Information(std::string const & area, System::Reflection::meta::Variant const * source, const char * messageTemplate, ...)
{
    if (_logLevelLimit < LogEventLevel::Information)
        return;

    va_list arguments;
    va_start(arguments, messageTemplate);
    Log(LogEventLevel::Information, area, source, messageTemplate, arguments);
    va_end(arguments);
}

void XamlCPP::Platform::ILog::Warning(std::string const & area, System::Reflection::meta::Variant const * source, const char * messageTemplate, ...)
{
    if (_logLevelLimit < LogEventLevel::Warning)
        return;

    va_list arguments;
    va_start(arguments, messageTemplate);
    Log(LogEventLevel::Warning, area, source, messageTemplate, arguments);
    va_end(arguments);
}

void XamlCPP::Platform::ILog::Error(std::string const & area, System::Reflection::meta::Variant const * source, const char * messageTemplate, ...)
{
    if (_logLevelLimit < LogEventLevel::Error)
        return;

    va_list arguments;
    va_start(arguments, messageTemplate);
    Log(LogEventLevel::Error, area, source, messageTemplate, arguments);
    va_end(arguments);
}

void XamlCPP::Platform::ILog::Fatal(std::string const & area, System::Reflection::meta::Variant const * source, const char * messageTemplate, ...)
{
    if (_logLevelLimit < LogEventLevel::Fatal)
        return;

    va_list arguments;
    va_start(arguments, messageTemplate);
    Log(LogEventLevel::Fatal, area, source, messageTemplate, arguments);
    va_end(arguments);
}
