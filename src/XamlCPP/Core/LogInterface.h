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

#ifndef __XAMLCPP_LOG_H__
#define __XAMLCPP_LOG_H__

#include "standard/Platform/Define.h"
#include "Reflection/Runtime/Variant.h"

namespace XamlCPP::Platform
{
    /// <summary>
    /// Specifies the meaning and relative importance of a log event.
    /// </summary>
    enum class LogEventLevel
    {
        Verbose,
        Debug,
        Information,
        Warning,
        Error,
        Fatal
    };

    /// <summary>
    /// Specifies the area in which a log event occurred.
    /// </summary>
    struct LogArea
    {
        /// <summary>
        /// The log event comes from the property system.
        /// </summary>
        static constexpr const char* const Property = "Property";

        /// <summary>
        /// The log event comes from the binding system.
        /// </summary>
        static constexpr const char* const Binding = "Binding";

        /// <summary>
        /// The log event comes from the visual system.
        /// </summary>
        static constexpr const char* const Visual = "Visual";

        /// <summary>
        /// The log event comes from the layout system.
        /// </summary>
        static constexpr const char* const Layout = "Layout";

        /// <summary>
        /// The log event comes from the control system.
        /// </summary>
        static constexpr const char* const Control = "Control";
    };

    struct ILogHandler
    {
        virtual void Log(LogEventLevel level, std::string const& area, System::Reflection::meta::Variant const* source, const char* messageTemplate, va_list args) = 0;
    };


    struct TC_XAMLCPP_API ILog
    {
    private:
        static ILogHandler* _handle;
        static LogEventLevel _logLevelLimit;

        static void Log(
            LogEventLevel level,
            std::string const& area,
            System::Reflection::meta::Variant const* source,
            const char* messageTemplate,
            va_list args);

    public:

        static void SetLogHandle(ILogHandler*);
        static LogEventLevel GetLogLimit();

        static void Verbose(
            std::string const& area,
            System::Reflection::meta::Variant const* source,
            const char* messageTemplate, 
            ...) ATTR_PRINTF(5, 6);

        static void Debug(
            std::string const& area,
            System::Reflection::meta::Variant const* source,
            const char* messageTemplate,
            ...) ATTR_PRINTF(5, 6);

        static void Information(
            std::string const& area,
            System::Reflection::meta::Variant const* source,
            const char* messageTemplate,
            ...) ATTR_PRINTF(5, 6);

        static void Warning(
            std::string const& area,
            System::Reflection::meta::Variant const* source,
            const char* messageTemplate,
            ...) ATTR_PRINTF(5, 6);

        static void Error(
            std::string const& area,
            System::Reflection::meta::Variant const* source,
            const char* messageTemplate,
            ...) ATTR_PRINTF(5, 6);

        static void Fatal(
            std::string const& area,
            System::Reflection::meta::Variant const* source,
            const char* messageTemplate,
            ...) ATTR_PRINTF(5, 6);
    };
}

#define XamlLogVerbose(...)                                                                  \
    if (XamlCPP::Platform::ILog::GetLogLimit() >= XamlCPP::Platform::LogEventLevel::Verbose) \
        XamlCPP::Platform::ILog::Verbose(__VA_ARGS__);

#endif // !__XAMLCPP_LOG_H__
