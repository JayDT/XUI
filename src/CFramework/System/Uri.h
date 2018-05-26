/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __CF_URI_H__
#define __CF_URI_H__

namespace System
{
    enum class UriValidationError
    {
        Ok = 0,
        Malformed = 1,
        UnsupportedScheme = 2
    };

    struct URIDesc
    {
        struct PathInfo
        {
            std::string Authority;
            std::string LocalPath;
            std::string Assembly;
            std::string Version;

            std::string Original() const { return Authority + LocalPath; }
        };

        std::string m_host;
        PathInfo m_path;
        std::string m_scheme;
        std::string m_user;
        std::string m_pass;
        uint16 m_port;
    };

    //Note: Fast implement very very greedy
    class TC_CFRAMEWORK_API URI
    {
        void Create(std::string const& uriString, bool absolute);

        static UriValidationError Validate(std::string const& uriString, bool absolute);

        URIDesc m_descriptor;

    public:
        URI() {}
        URI(std::string const& uriString, bool absolute);

        void Parse(std::string const& uriString, bool absolute);

        URIDesc const& GetDescriptor() const { return m_descriptor; }
    };

    struct TC_CFRAMEWORK_API UriExpressionParser
    {
        enum State
        {
            Schema,
            Host,
            User,
            Port,
            Path,
            Authority,
            Localpath,
            EndWithFailed,
            End
        };

        bool EnableValidation = false;
        bool Absolute = false;
        size_t pos = 0;
        size_t length = 0;
        const char* expr = nullptr;
        URIDesc& desc;

        static constexpr const char* SchemePostfix = "://";
        static constexpr const char ForwardSlash = '/';
        static constexpr const char Quote = '\'';
        static constexpr const char OpenCurly = '{';
        static constexpr const char CloseCurly = '}';
        static constexpr const char Comma = ',';
        static constexpr const char Colon = ':';
        static constexpr const char EqualSign = '=';

        UriExpressionParser(bool enableValidation, bool absolute, URIDesc& d)
            : EnableValidation(enableValidation)
            , Absolute(absolute)
            , desc(d)
        { }

        bool Parse(std::string const& expression, State state = State::Schema);

        std::string GetRemainingString()
        {
            return std::string(&expr[pos]);
        }

    private:

        State ParseSchema();

        State ParseHost();

        State ParsePath();

        State ParseUser();

        State ParsePort();

        State ParseAuthority();

        State ParseLocalpath();

        void SkipWhitespace();

        bool IsLetter(char ch) const;

        bool IsValidIdentiferChar(char ch) const;

        bool IsCharInSeqvence(char t, char c)
        {
            return t == c;
        }

        template<typename... ARGS>
        bool IsCharInSeqvence(char t, char c, ARGS... args)
        {
            return IsCharInSeqvence(t, c) || IsCharInSeqvence(t, args...);
        }

        bool ParseChar(char c)
        {
            return expr[pos] != '\0' && TakeIf(c);
        }

        template<typename... ARGS>
        bool ParseChar(char c, ARGS... args)
        {
            return ParseChar(c) || ParseChar(args...);
        }

        bool TakeIf(char c);
    };

}

#endif // !__RS_RESOURCE_BUILDER_H__
