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

#ifndef __XUI_TRANSFORMED_BOUNDS_H__
#define __XUI_TRANSFORMED_BOUNDS_H__

#include "Core/Media/Vector_t.h"

namespace XUI::Core::Media
{
    class TC_XUI_API TransformedBounds : public System::Reflection::Object
    {
    private:

        std::vector<Vector> m_vertex;

    public:

        TransformedBounds() {}

        TransformedBounds(const std::vector<Vector>& pts, bool reverse = false);
        TransformedBounds(const std::vector<Vector>&& pts, bool reverse = false);

        inline int VertexSize() const { return m_vertex.size(); }
        inline const Vector& GetVertex(int index) const { return m_vertex[index]; }
        bool Contains(const Vector& p, bool reverse = false) const;
    };
}

#endif //!__XUI_TRANSFORMED_BOUNDS_H__