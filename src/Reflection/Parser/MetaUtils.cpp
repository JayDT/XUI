/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
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

#include "Precompiled.h"

#include "MetaUtils.h"

#include <boost/algorithm/string/join.hpp>

#include <fstream>
#include <exception>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace utils
{
    void ToString(const CXString &str, std::string &output)
    {
        if (!str.data)
            return;

        auto cstr = clang_getCString( str );

        output = cstr;

        clang_disposeString( str );
    }

    Mustache::Data<std::string>::Type TemplateBool(bool value)
    {
        return value ? Mustache::Data<std::string>::Type::True : Mustache::Data<std::string>::Type::False;
    }

    std::string GetQualifiedName(const CursorType &type, const CursorType &nstype)
    {
        if (type.GetKind() != CXType_Typedef && type.GetKind() != CXType_Unexposed && type.GetKind() != CXType_Elaborated)
            return type.GetDisplayName();

        auto declaration = nstype.GetDeclaration();
        if (declaration.GetKind() == CXType_Invalid)
            return nstype.GetDisplayName();

        auto parent = declaration.GetLexicalParent();

        Namespace parentNamespace;

        // walk up to the root namespace
        while (parent.GetKind() == CXCursorKind::CXCursor_Namespace)
        {
            parentNamespace.emplace(parentNamespace.begin(), parent.GetDisplayName());

            parent = parent.GetLexicalParent();
        }

        // add the display name as the end of the namespace
        parentNamespace.emplace_back(
            GetUnqualifiedName(type)
        );

        return boost::join(parentNamespace, "::");
    }

    std::string GetQualifiedName(const CursorType &type)
    {
        if (type.GetKind() != CXType_Typedef && type.GetKind() != CXType_Unexposed && type.GetKind() != CXType_Elaborated &&
            type.GetKind() != CXType_Pointer && type.GetKind() != CXType_LValueReference && type.GetKind() != CXType_RValueReference)
            return type.GetDisplayName();

        CursorType declType = type;
        while (true)
        {
            switch (declType.GetKind())
            {
                case CXType_Pointer:
                case CXType_LValueReference:
                case CXType_RValueReference:
                {
                    declType = declType.GetElaboratedType();
                    continue;
                }
            }

            break;
        }

        auto declaration = declType.GetDeclaration( );
        if (declaration.GetKind() == CXType_Invalid)
            return declType.GetDisplayName();

        auto parent = declaration.GetLexicalParent( );

        Namespace parentNamespace;

        // walk up to the root namespace
        while (parent.GetKind( ) == CXCursorKind::CXCursor_Namespace)
        {
            parentNamespace.emplace( parentNamespace.begin( ), parent.GetDisplayName( ) );

            parent = parent.GetLexicalParent( );
        }

        std::string frontQualifiers;
		System::String name = GetUnqualifiedName(type, &frontQualifiers);

		if (type.GetTemplateArgumentCount() > 0)
		{
			auto templateTypes = type.GetTemplateArguments();
			for (CursorType& tType : templateTypes)
			{
				std::string type_name = tType.GetDisplayName();
				if (type_name.empty())
					continue;

				std::string qtype_name = GetQualifiedName(tType);
				if (type_name == qtype_name)
					continue;

				size_t pos;
				if ((pos = name.find(type_name)) != std::string::npos)
					name.replace(pos, type_name.size(), qtype_name);
			}
		}
        // add the display name as the end of the namespace
        parentNamespace.emplace_back(
            name
        );

        if (!frontQualifiers.empty())
            return frontQualifiers + boost::join( parentNamespace, "::" );
        return boost::join(parentNamespace, "::");
    }

    std::string GetUnqualifiedName(const CursorType &type, std::string* frontQualifiers /*= nullptr*/)
    {
        if (type.GetKind() != CXType_Typedef && type.GetKind() != CXType_Unexposed && type.GetKind() != CXType_Elaborated &&
            type.GetKind() != CXType_Pointer && type.GetKind() != CXType_LValueReference && type.GetKind() != CXType_RValueReference)
            return type.GetDisplayName();

        CursorType declType = type;
        while (true)
        {
            switch (declType.GetKind())
            {
                case CXType_Pointer:
                case CXType_LValueReference:
                case CXType_RValueReference:
                {
                    declType = declType.GetElaboratedType();
                    continue;
                }
            }

            break;
        }

        //if (type.GetKind() != CXType_Typedef && type.GetKind() != CXType_Unexposed && type.GetKind() != CXType_Elaborated)
        //    return type.GetDisplayName();

        std::string displayname = type.GetDisplayName();
        size_t lasttag = displayname.find_last_of(':');

        //NOTE: trim not interestring prefix attributes
        //size_t unusedAttrib = displayname.find_first_of(' ');
        //size_t templateStart = displayname.find_first_of('<');
        //if (unusedAttrib != std::string::npos && (templateStart == std::string::npos || unusedAttrib < templateStart) && lasttag > unusedAttrib && displayname[unusedAttrib + 1] != '*' && displayname[unusedAttrib + 1] != '&' && displayname[unusedAttrib + 1] != 0)
        //    displayname = displayname.substr(unusedAttrib + 1, displayname.size() - unusedAttrib - 1);


        // ToDo: hax for the win (very boring)
        size_t constpos = displayname.find("const ");
        size_t volatilepos = constpos == std::string::npos ? displayname.find("volatile ") : std::string::npos;

        if (constpos != std::string::npos || volatilepos != std::string::npos)
        {
            size_t templateStart = displayname.find_first_of('<');
            if (templateStart == std::string::npos)
            {
                System::String _displayname(displayname);

                constpos = _displayname.find("const ");
                if (constpos != std::string::npos && constpos == 0)
                {
                    _displayname = _displayname.Replace("const ", "");
                    if (frontQualifiers && constpos != std::string::npos && constpos == 0)
                        *frontQualifiers += "const ";
                }

                volatilepos = _displayname.find("volatile ");
                if (volatilepos != std::string::npos && volatilepos == 0)
                {
                    _displayname = _displayname.Replace("volatile ", "");
                    if (frontQualifiers)
                        *frontQualifiers += "volatile ";
                }

                displayname = _displayname;
            }
            else
            {
                System::String _displayname(displayname.substr(0, templateStart));
                std::string postpart = displayname.substr(templateStart, displayname.size() - templateStart);

                constpos = _displayname.find("const ");
                if (constpos != std::string::npos && constpos == 0)
                {
                    _displayname = _displayname.Replace("const ", "");
                    if (frontQualifiers && constpos != std::string::npos && constpos == 0)
                        *frontQualifiers += "const ";
                }

                volatilepos = _displayname.find("volatile ");
                if (volatilepos != std::string::npos && volatilepos == 0)
                {
                    _displayname = _displayname.Replace("volatile ", "");
                    if (frontQualifiers)
                        *frontQualifiers += "volatile ";
                }

                displayname = _displayname + postpart;
            }
        }

        //size_t unusedAttrib = displayname.find("const ");
        //size_t templateStart = displayname.find_first_of('<');
        //if (unusedAttrib != std::string::npos && (templateStart == std::string::npos || unusedAttrib < templateStart) && lasttag > unusedAttrib)
        //    displayname = displayname.substr(unusedAttrib + 1, displayname.size() - unusedAttrib - 1);


        if (lasttag != std::string::npos)
        {
            auto declaration = declType.GetDeclaration();
            if (declaration.GetKind() == CXType_Invalid)
                return type.GetDisplayName();

            auto parent = declaration.GetLexicalParent();

            Namespace parentNamespace;
            while (parent.GetKind() == CXCursorKind::CXCursor_Namespace)
            {
                parentNamespace.emplace(parentNamespace.begin(), parent.GetDisplayName());

                parent = parent.GetLexicalParent();
            }

            std::string parentStr = boost::join(parentNamespace, "::") + "::";

            for (auto itr = parentNamespace.begin(); itr != parentNamespace.end(); ++itr)
            {
                std::string parentStr = *itr;
                if (std::equal(displayname.begin(), displayname.begin() + parentStr.size(), parentStr.begin()))
                    displayname = displayname.substr(parentStr.size() + 2, displayname.size() - parentStr.size() - 2);
            }
        }
        return displayname;
    }

    Namespace GetNamespaceParent(Cursor const& cursor)
    {
        Namespace parentNamespace;

        auto parent = cursor.GetLexicalParent();

        // walk up to the root namespace
        while (parent.GetKind() == CXCursorKind::CXCursor_Namespace)
        {
            parentNamespace.emplace(parentNamespace.begin(), parent.GetDisplayName());

            parent = parent.GetLexicalParent();
        }

        return parentNamespace;
    }

    std::string GetQualifiedName(
        const std::string &displayName, 
        const Namespace &currentNamespace
    )
    {
        auto name = boost::join( currentNamespace, "::" );

        if (!currentNamespace.empty( ))
            name += "::";

        name += displayName;

        return name;
    }

    std::string GetQualifiedName(
        const Cursor &cursor, 
        const Namespace &currentNamespace
    )
    {
        return GetQualifiedName( cursor.GetSpelling( ), currentNamespace );
    }

    void LoadText(const std::string &filename, std::string &output)
    {
        std::ifstream input( filename );

        if (!input)
        {
            std::stringstream error;

            error << "Unable to open file \"" 
                  << filename 
                  << "\" for reading." << std::endl;
            error << strerror( errno );

            throw std::runtime_error( error.str( ) );
        }

        input.seekg( 0, std::ios::end );

        output.reserve(
            static_cast<std::string::size_type>( input.tellg( ) )
        );

        input.seekg( 0, std::ios::beg );

        output.assign(
            // disambiguate function declaration
            (std::istreambuf_iterator<char>( input )),
            std::istreambuf_iterator<char>( ) 
        );

        input.close( );
    }

    std::hash<uint32_t> hasher;

    uint32_t ChecksumFile(std::ifstream& file)
    {
        uint32_t seed = 0;
        uint32_t dword = 0;

        while (file.read(reinterpret_cast<char*>(&dword), sizeof(dword)) && file.gcount() == 4)
            seed ^= hasher(dword) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }

    uint32_t ChecksumDiff(std::ifstream& file, const std::string &text)
    {
        uint32_t seed = 0, fseed = 0;
        size_t i = 0;
    
        //for (; (i + 4) < text.size(); i += 4)
        //{
        //    uint32_t before = seed;
        //    uint32_t dword = *(uint32_t*)&text[i];
        //    seed ^= hasher(dword) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        //
        //    uint32_t fbefore = fseed;
        //    uint32_t fdword = 0;
        //    if (file.read(reinterpret_cast<char*>(&fdword), sizeof(fdword)))
        //        fseed ^= hasher(fdword) + 0x9e3779b9 + (fseed << 6) + (fseed >> 2);
        //
        //    if (seed != fseed)
        //        return 0;
        //}

        uint32_t fdword = 0;
        while (file.read(reinterpret_cast<char*>(&fdword), sizeof(fdword)) && file.gcount() == 4)
        {
            uint32_t fbefore = fseed;
            fseed ^= hasher(fdword) + 0x9e3779b9 + (fseed << 6) + (fseed >> 2);

            if ((i + 4) <= text.size())
            {
                uint32_t before = seed;
                uint32_t dword = *(uint32_t*)&text[i];
                seed ^= hasher(dword) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                i += 4;
            }
            else
                return seed;

            if (seed != fseed)
                return 0;
        }

        return seed;
    }

    uint32_t ChecksumString(const std::string &text)
    {
        uint32_t seed = 0;
        size_t i = 0;

        for (; (i + 4) <= text.size(); i += 4)
        {
            uint32_t dword = *(uint32_t*)&text[i];
            seed ^= hasher(dword) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }

    void WriteText(const std::string &filename, const std::string &text, bool writeIfChecksumDif)
    {
        std::ifstream outFileForRead( filename );

        if (outFileForRead)
        {
            uint32_t stringHash = ChecksumString(text);
            uint32_t fileHash = ChecksumFile(outFileForRead);
            outFileForRead.close();

            if (stringHash == fileHash)
                return;

            //outFileForRead = std::ifstream(filename);
            //if (ChecksumDiff(outFileForRead, text))
            //{
            //    outFileForRead.close();
            //    return;
            //}
            //outFileForRead.close();
        }

        std::ofstream output( filename );

        if (!output)
        {
            std::stringstream error;

            error << "Unable to open file \"" 
                  << filename << "\" for writing." 
                  << std::endl;
            error << strerror( errno );

            throw std::runtime_error( error.str( ) );
        }

        output << text;

        output.close( );
    }

    boost::filesystem::path MakeRelativePath(const boost::filesystem::path &from, const boost::filesystem::path &to)
    {
        // Start at the root path and while they are the same then do nothing then when they first
        // diverge take the remainder of the two path and replace the entire from path with ".."
        // segments.
        auto itFrom = from.begin( );
        auto itTo = to.begin( );

        // Loop through both
        while (itFrom != from.end( ) && itTo != to.end( ) && (*itTo) == (*itFrom))
        {
            ++itTo;
            ++itFrom;
        }

        boost::filesystem::path finalPath;

        while (itFrom != from.end( ))
        {
            finalPath /= "..";

            ++itFrom;
        }

        while (itTo != to.end( ))
        {
            finalPath /= *itTo;

            ++itTo;
        }

        return finalPath;
    }

    void FatalError(const std::string &error)
    {
        std::cerr << "Error: " << error << std::endl;

        exit( EXIT_FAILURE );
    }
}