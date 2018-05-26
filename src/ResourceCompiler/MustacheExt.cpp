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

#include <chrono>
#include <fstream>
#include <future>
#include <algorithm>

#include <boost/filesystem.hpp>

#include <Wrappers/Mustache.h>

namespace MustacheExt
{
    void FatalError(const std::string &error)
    {
        std::cerr << "Error: " << error << std::endl;

        exit(EXIT_FAILURE);
    }

    void LoadText(const std::string &filename, std::string &output)
    {
        std::ifstream input(filename);

        if (!input)
        {
            std::stringstream error;

            error << "Unable to open file \""
                << filename
                << "\" for reading." << std::endl;
            error << strerror(errno);

            throw std::runtime_error(error.str());
        }

        input.seekg(0, std::ios::end);

        output.reserve(
            static_cast<std::string::size_type>(input.tellg())
        );

        input.seekg(0, std::ios::beg);

        output.assign(
            // disambiguate function declaration
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>()
        );

        input.close();
    }

    Mustache::Mustache<std::string> LoadTemplate(const std::string & name)
    {
        auto path = boost::filesystem::path("Resources/Templates/");

        path.append(name);

        try
        {
            std::string text;

            LoadText(path.string(), text);

            return text;
        }
        catch (std::exception &e)
        {
            std::stringstream error;

            error << "Unable to load template ";
            error << name << "." << std::endl;
            error << "-- " << e.what();

            FatalError(error.str());
        }

        // this will never happen
        return { "" };
    }

    void WriteText(const std::string &filename, const std::string &text, bool writeIfChecksumDif)
    {
        std::ofstream output(filename);

        if (!output)
        {
            std::stringstream error;

            error << "Unable to open file \""
                << filename << "\" for writing."
                << std::endl;
            error << strerror(errno);

            throw std::runtime_error(error.str());
        }

        output << text;

        output.close();
    }

    boost::filesystem::path MakeRelativePath(const boost::filesystem::path &from, const boost::filesystem::path &to)
    {
        // Start at the root path and while they are the same then do nothing then when they first
        // diverge take the remainder of the two path and replace the entire from path with ".."
        // segments.
        auto itFrom = from.begin();
        auto itTo = to.begin();

        // Loop through both
        while (itFrom != from.end() && itTo != to.end() && (*itTo) == (*itFrom))
        {
            ++itTo;
            ++itFrom;
        }

        boost::filesystem::path finalPath;

        while (itFrom != from.end())
        {
            finalPath /= "..";

            ++itFrom;
        }

        while (itTo != to.end())
        {
            finalPath /= *itTo;

            ++itTo;
        }

        return finalPath;
    }

    boost::filesystem::path change_extension(const boost::filesystem::path & p, const boost::filesystem::path & new_extension)
    {
        boost::filesystem::path new_p(p);
        new_p.replace_extension(new_extension);
        return new_p;
    }
}