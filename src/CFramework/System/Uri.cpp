#include "Uri.h"

System::URI::URI(std::string const& uriString, bool absolute)
{
    Parse(uriString, absolute);
}

void System::URI::Create(std::string const & uriString, bool absolute)
{
    if (Validate(uriString, absolute) != UriValidationError::Ok)
        return;

    UriExpressionParser parser(false, absolute, m_descriptor);
    parser.Parse(uriString);
}

System::UriValidationError System::URI::Validate(std::string const & uriString, bool absolute)
{
    // ToDo: implement this
    return UriValidationError::Ok;
}

void System::URI::Parse(std::string const & uriString, bool absolute)
{
    Create(uriString, absolute);
}

bool System::UriExpressionParser::IsLetter(char ch) const
{
    return ((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A'));
}

bool System::UriExpressionParser::IsValidIdentiferChar(char ch) const
{
    return expr[pos] == '_' || IsLetter(expr[pos]);
}

bool System::UriExpressionParser::Parse(std::string const & expression, State state)
{
    pos = 0;
    expr = expression.c_str();
    length = expression.size();

    while (expr[pos] != '\0' && state != State::End && state != State::EndWithFailed)
    {
        switch (state)
        {
            case State::Schema:
                state = ParseSchema();
                break;

            case State::Path:
                state = ParsePath();
                break;

            case State::Port:
                state = ParsePort();
                break;

            case State::Host:
                state = ParseHost();
                break;

            case State::User:
                state = ParseUser();
                break;

            case State::Authority:
                state = ParseAuthority();
                break;

            case State::Localpath:
                state = ParseLocalpath();
                break;
        }
    }

    return state == State::End;
}

System::UriExpressionParser::State System::UriExpressionParser::ParseSchema()
{
    std::string ret;
    while (expr[pos] != ':' && IsLetter(expr[pos]))
        ret += expr[pos++];

    if (expr[pos++] != ':' || expr[pos++] != '/' || expr[pos++] != '/')
        return State::EndWithFailed;

    desc.m_scheme = ret;

    return State::Authority;
}

System::UriExpressionParser::State System::UriExpressionParser::ParseHost()
{
    return State();
}

System::UriExpressionParser::State System::UriExpressionParser::ParsePath()
{
    return State();
}

System::UriExpressionParser::State System::UriExpressionParser::ParseUser()
{
    // dont use
    return State();
}

System::UriExpressionParser::State System::UriExpressionParser::ParsePort()
{
    // dont use
    return State();
}

System::UriExpressionParser::State System::UriExpressionParser::ParseAuthority()
{
    int atPos = -1;
    int colonPos = -1;
    int colonPos2 = -1;

    std::string ret;
    ret.reserve(32);
    while (expr[pos] != '/' && (IsLetter(expr[pos]) || isdigit(expr[pos]) ||
           IsCharInSeqvence(expr[pos], ':', '.', '[', ']', '(', ')', '!', '$', '#', '^', '_', ',', '@')))
    {
        if (expr[pos] == '@')
            atPos = ret.size();

        if (expr[pos] == ':')
        {
            if (colonPos == -1)
                colonPos = ret.size();
            else
                colonPos2 = ret.size();
        }

        ret += expr[pos++];
    }

    if (atPos != -1)
    {
        if (colonPos != -1 && colonPos < atPos)
        {
            desc.m_user = ret.substr(0, colonPos);
            desc.m_pass = ret.substr(colonPos + 1, atPos - colonPos - 1);
        }
        else
        {
            desc.m_user = ret.substr(0, atPos);
        }

        ret = ret.substr(atPos + 1);

        if (colonPos2 != -1)
        {
            colonPos2 = ret.find(':');
            desc.m_port = atoi(ret.substr(colonPos2 + 1).c_str());

            ret = ret.substr(0, colonPos2);
        }

    }
    else if (colonPos != -1)
    {
        desc.m_port = atoi(ret.substr(colonPos + 1).c_str());

        if (desc.m_port)
            ret = ret.substr(0, colonPos);
    }

    desc.m_path.Authority = ret;

    return State::Localpath;
}

System::UriExpressionParser::State System::UriExpressionParser::ParseLocalpath()
{
    int delimiter = -1;
    int delimiter2 = -1;

    std::string ret;
    ret.reserve(32);
    while ((IsLetter(expr[pos]) || isdigit(expr[pos]) ||
           IsCharInSeqvence(expr[pos], ':', '.', '[', ']', '(', ')', '!', '$', '#', '^', '_', ',', '/', ';', '?', '=')))
    {
        if (expr[pos] == ';')
        {
            if (delimiter == -1)
                delimiter = ret.size();
            else
                delimiter2 = ret.size();
        }

        ret += expr[pos++];
    }

    if (delimiter != -1)
    {
        desc.m_path.Assembly = ret.substr(0, delimiter);

        if (delimiter2 != -1)
        {
            desc.m_path.Version = ret.substr(delimiter + 1, delimiter2 - delimiter - 1);
            ret = ret.substr(delimiter2 + 1);
        }
        else
        {
            ret = ret.substr(delimiter + 1);
        }
   }

    desc.m_path.LocalPath = ret;
    return State::End;
}

void System::UriExpressionParser::SkipWhitespace()
{
    while (expr[pos] != '\0' && isspace(expr[pos]))
    {
        ++pos;
    }
}

bool System::UriExpressionParser::TakeIf(char c)
{
    if (expr[pos] != '\0' && expr[pos] == c)
    {
        ++pos;
        return true;
    }
    else
    {
        return false;
    }
}