/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#include "PathMarkupParser.h"
#include "Core/Media/Size_t.h"
#include "Core/Media/Geometries/StreamGeometry.h"
#include <locale>

using namespace XUI;

std::locale loc;

const std::unordered_map<char, Core::Media::Geometries::PathMarkupParser::Command> Commands = std::unordered_map<char, Core::Media::Geometries::PathMarkupParser::Command>
{
    {'F', Core::Media::Geometries::PathMarkupParser::Command::FillRule},
    {'M', Core::Media::Geometries::PathMarkupParser::Command::Move},
    {'L', Core::Media::Geometries::PathMarkupParser::Command::Line},
    {'H', Core::Media::Geometries::PathMarkupParser::Command::HorizontalLine},
    {'V', Core::Media::Geometries::PathMarkupParser::Command::VerticalLine},
    {'Q', Core::Media::Geometries::PathMarkupParser::Command::QuadraticBezierCurve},
    {'T', Core::Media::Geometries::PathMarkupParser::Command::SmoothQuadraticBezierCurve},
    {'C', Core::Media::Geometries::PathMarkupParser::Command::CubicBezierCurve},
    {'S', Core::Media::Geometries::PathMarkupParser::Command::SmoothCubicBezierCurve},
    {'A', Core::Media::Geometries::PathMarkupParser::Command::Arc},
    {'Z', Core::Media::Geometries::PathMarkupParser::Command::Close}
};
const std::unordered_map<char, Interfaces::FillRule> FillRules = std::unordered_map<char, Interfaces::FillRule>
{
    {'0', Interfaces::FillRule::EvenOdd},
    {'1', Interfaces::FillRule::NonZero}
};

Core::Media::Geometries::PathMarkupParser::PathMarkupParser::PathMarkupParser(Interfaces::IStreamGeometryContext *context) : _context(context)
{
}

void Core::Media::Geometries::PathMarkupParser::PathMarkupParser::Parse(const std::string &s)
{
    pos = 0;
    expr = s.c_str();
    length = s.size();

    bool openFigure = false;
    {
        Command command = Command::None;
        Point point;
        bool relative = false;
        bool hasPrevPoint = false;
        Point previousControlPoint;

        while (ReadCommand(command, relative))
        {
            switch (command)
            {
                case Command::FillRule:
                    _context->SetFillRule(ReadFillRule());
                    hasPrevPoint = false;
                    break;

                case Command::Move:
                    if (openFigure)
                        _context->EndFigure(false);

                    point = ReadPoint(point, relative);
                    _context->BeginFigure(point, true);
                    openFigure = true;
                    hasPrevPoint = false;
                    break;

                case Command::Line:
                    point = ReadPoint(point, relative);
                    _context->LineTo(point);
                    hasPrevPoint = false;
                    break;

                case Command::HorizontalLine:
                    if (!relative)
                        point = Point(ReadDouble(), point.y);
                    else
                        point = Point(point.x + ReadDouble(), point.y);

                    _context->LineTo(point);
                    hasPrevPoint = false;
                    break;

                case Command::VerticalLine:
                    if (!relative)
                        point = Point(point.x, ReadDouble());
                    else
                        point = Point(point.x, point.y + ReadDouble());

                    _context->LineTo(point);
                    hasPrevPoint = false;
                    break;

                case Command::QuadraticBezierCurve:
                {
                    Point handle = ReadPoint(point, relative);
                    hasPrevPoint = true;
                    previousControlPoint = handle;
                    ReadSeparator();
                    point = ReadPoint(point, relative);
                    _context->QuadraticBezierTo(handle, point);
                    break;
                }

                case Command::SmoothQuadraticBezierCurve:
                {
                    Point end = ReadPoint(point, relative);

                    if (hasPrevPoint)
                        previousControlPoint = MirrorControlPoint(previousControlPoint, point);

                    _context->QuadraticBezierTo(hasPrevPoint ? previousControlPoint : point, end);
                    point = end;
                    break;
                }

                case Command::CubicBezierCurve:
                {
                    Point point1 = ReadPoint(point, relative);
                    ReadSeparator();
                    Point point2 = ReadPoint(point, relative);
                    previousControlPoint = point2;
                    ReadSeparator();
                    point = ReadPoint(point, relative);
                    _context->CubicBezierTo(point1, point2, point);
                    break;
                }

                case Command::SmoothCubicBezierCurve:
                {
                    Point point2 = ReadPoint(point, relative);
                    ReadSeparator();
                    Point end = ReadPoint(point, relative);

                    if (hasPrevPoint)
                        previousControlPoint = MirrorControlPoint(previousControlPoint, point);

                    _context->CubicBezierTo(hasPrevPoint ? previousControlPoint : point, point2, end);
                    previousControlPoint = point2;
                    point = end;
                    break;
                }

                case Command::Arc:
                {
                    Size size = ReadSize();
                    ReadSeparator();
                    double rotationAngle = ReadDouble();
                    ReadSeparator();
                    bool isLargeArc = ReadBool();
                    ReadSeparator();
                    Interfaces::SweepDirection sweepDirection = ReadBool() ? Interfaces::SweepDirection::Clockwise : Interfaces::SweepDirection::CounterClockwise;
                    ReadSeparator();
                    point = ReadPoint(point, relative);

                    _context->ArcTo(point, size, rotationAngle, isLargeArc, sweepDirection);
                    hasPrevPoint = false;
                    break;
                }

                case Command::Close:
                    _context->EndFigure(true);
                    openFigure = false;
                    hasPrevPoint = false;
                    break;

                default:
                    throw System::NotSupportedException("Unsupported command");
            }
        }

        if (openFigure)
            _context->EndFigure(false);
    }
}

Core::Media::Point Core::Media::Geometries::PathMarkupParser::PathMarkupParser::MirrorControlPoint(Core::Media::Point const& controlPoint, Core::Media::Point const& center)
{
    Point dir = (controlPoint - center);
    return center + -dir;
}

bool Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadCommand(Command &command, bool &relative)
{
    ReadWhitespace();

    if (expr[pos] == '\0')
    {
        return false;
    }
    else
    {
        char c = expr[pos];
        Command next = Command::None;

        auto Commands_iterator = Commands.find(std::toupper(c, loc));
        if (Commands_iterator == Commands.end())
        {
            if ((std::isdigit(c, loc) || c == '.' || c == '+' || c == '-') && (command != Command::None))
                return true;
            throw System::InvalidOperationException(("Unexpected path command '" + std::to_string(c) + "'.").c_str());
        }
        else
        {
            next = Commands_iterator->second;
        }

        command = next;
        relative = std::islower(c, loc);
        ++pos;
        return true;
    }
}

Interfaces::FillRule Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadFillRule()
{
    if (expr[pos] == '\0')
        throw System::InvalidOperationException("Invalid fill rule");

    char c = expr[pos++];
    Interfaces::FillRule rule;

    auto FillRules_iterator = FillRules.find(c);
    if (FillRules_iterator == FillRules.end())
        throw System::InvalidOperationException("Invalid fill rule");
    rule = FillRules_iterator->second;

    return rule;
}

double Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadDouble()
{
    ReadWhitespace();

    // TODO: Handle Infinity, NaN and scientific notation.
    bool readSign = false;
    bool readPoint = false;
    bool readExponent = false;
    size_t endpos = pos;

    std::string doublestr;
    while (expr[pos] != '\0')
    {
        char c = std::toupper(expr[pos], loc);

        if (((c == '+' || c == '-') && !readSign) || (c == '.' && !readPoint) || (c == 'E' && !readExponent) || std::isdigit(c, loc))
        {
            if (endpos != pos && !readExponent && c == '-')
                break;

            doublestr += c;
            ++pos;

            if (!readSign)
                readSign = c == '+' || c == '-';

            if (!readPoint)
                readPoint = c == '.';

            if (c == 'E')
            {
                readSign = false;
                readExponent = true;
            }

            continue;
        }
        break;
    }

    std::stringstream ss(doublestr);
    ss.imbue(loc);
    double d;
    ss >> d;

    return d; // std::stod(doublestr);
}

Core::Media::Point Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadPoint(Core::Media::Point const& current, bool relative)
{
    ReadWhitespace();
    double x = relative ? current.x + ReadDouble() : ReadDouble();
    ReadSeparator();
    double y = relative ? current.y + ReadDouble() : ReadDouble();
    return Point(x, y);
}

Core::Media::Size Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadSize()
{
    ReadWhitespace();
    double x = ReadDouble();
    ReadSeparator();
    double y = ReadDouble();
    return Size(x, y);
}

bool Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadBool()
{
    return ReadDouble() != 0;
}

Core::Media::Point Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadRelativePoint(Point const& lastPoint)
{
    ReadWhitespace();
    double x = ReadDouble();
    ReadSeparator();
    double y = ReadDouble();
    return Point(lastPoint.x + x, lastPoint.y + y);
}

void Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadSeparator()
{
    bool readComma = false;

    while (expr[pos] != '\0')
    {
        char c = expr[pos];

        if (std::isspace(c, loc))
        {
            ++pos;
        }
        else if (c == ',')
        {
            if (readComma)
            {
                throw System::InvalidOperationException("Unexpected ','.");
            }

            readComma = true;
            ++pos;
        }
        else
        {
            break;
        }
    }
}

void Core::Media::Geometries::PathMarkupParser::PathMarkupParser::ReadWhitespace()
{
    while (expr[pos] != '\0' && std::isspace(expr[pos], loc))
    {
        ++pos;
    }
}
