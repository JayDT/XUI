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

#include "StdAfx.h"
#include "BaseTypes.h"

#include <iostream>
#include <limits>
#include <regex>
#include <locale>

bool System::Bool::Parse(const std::string & value, std::locale const& loc)
{
    bool result;
    System::String s(value);
    std::istringstream is(s.ToLower());
    is >> std::boolalpha >> result;
    return result;
}

bool System::Bool::TryParse(const std::string & value, bool & result, std::locale const& loc)
{
    static const std::regex filter(R"(^([Tt][Rr][Uu][Ee]|[Ff][Aa][Ll][Ss][Ee]|1|0)$)", std::regex_constants::optimize | std::regex_constants::ECMAScript);
    if (!value.empty() && std::regex_match(value, filter))
    {
        result = Bool::Parse(value);
        return true;
    }
    return false;
}

std::string System::Bool::ToString() const
{
    return std::to_string(value);
}

int System::Int32::Parse(const std::string & value, std::locale const& loc)
{
    return std::stoi(value);
}

bool System::Int32::TryParse(const std::string & value, int & result, std::locale const& loc)
{
    if (!value.empty() && std::find_if(value.begin(),
        value.end(), [&loc](char c) { return !std::isdigit(c, loc); }) == value.end())
    {
        result = Int32::Parse(value);
        return true;
    }
    return false;
}

std::string System::Int32::ToString() const
{
    return std::to_string(value);
}

long long System::Int64::Parse(const std::string & value, std::locale const & loc)
{
    return std::stoll(value);
}

bool System::Int64::TryParse(const std::string & value, long long & result, std::locale const & loc)
{
    if (!value.empty() && std::find_if(value.begin(),
        value.end(), [&loc](char c) { return !std::isdigit(c, loc); }) == value.end())
    {
        result = Int32::Parse(value);
        return true;
    }
    return false;
}

std::string System::Int64::ToString() const
{
    return std::to_string(value);
}

double System::Double::Parse(const std::string & str, std::locale const & loc)
{
    std::stringstream ss(str);
    ss.imbue(loc);
    double value;
    ss >> value;
    return value;
}

bool System::Double::TryParse(const std::string & str, double & result, std::locale const & loc)
{
    if (str.empty())
        return false;

    std::stringstream ss(str);
    ss.imbue(loc);
    ss >> result;
    return !std::isnan(result) && !std::isfinite(result);
}

float System::Float::Parse(const std::string & str, std::locale const & loc)
{
    std::stringstream ss(str);
    ss.imbue(loc);
    double value;
    ss >> value;
    return value;
}

bool System::Float::TryParse(const std::string & str, float & result, std::locale const & loc)
{
    if (str.empty())
        return false;

    std::stringstream ss(str);
    ss.imbue(loc);
    ss >> result;
    return !std::isnan(result) && !std::isfinite(result);
}
