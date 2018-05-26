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

#pragma once

#include "Cursor.h"
#include "Namespace.h"

#include "Wrappers/Mustache.h"
#include <boost/filesystem/path.hpp>

namespace utils
{
    void ToString(const CXString &str, std::string &output);

    Mustache::Data<std::string>::Type TemplateBool(bool value);

    Namespace GetNamespaceParent(Cursor const& cursor);

    std::string GetQualifiedName(const CursorType &type, const CursorType &nstype);
    std::string GetQualifiedName(const CursorType &type);
    std::string GetUnqualifiedName(const CursorType &type, std::string* frontQualifiers = nullptr);

    std::string GetQualifiedName(
        const std::string &displayName, 
        const Namespace &currentNamespace
    );

    std::string GetQualifiedName(
        const Cursor &cursor, 
        const Namespace &currentNamespace
    );

    void LoadText(const std::string &filename, std::string &output);
    void WriteText(const std::string &filename, const std::string &text, bool writeIfChecksumDif);

    boost::filesystem::path MakeRelativePath(
        const boost::filesystem::path &from, 
        const boost::filesystem::path &to
    );

    void FatalError(const std::string &error);

    template<typename A, typename B>
    bool RangeEqual(A startA, A endA, B startB, B endB);
}

#include "MetaUtils.hpp"