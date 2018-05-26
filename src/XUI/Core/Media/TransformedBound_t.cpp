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

#include "StdAfx.h"
#include "TransformedBound_t.h"

XUI::Core::Media::TransformedBounds::TransformedBounds(const std::vector<Vector>& pts, bool reverse)
    : m_vertex(pts)
{
}

XUI::Core::Media::TransformedBounds::TransformedBounds(const std::vector<Vector>&& pts, bool reverse)
    : m_vertex(std::move(pts))
{
}

bool XUI::Core::Media::TransformedBounds::Contains(const Vector & p, bool reverse) const
{
    float r = reverse ? -1 : 1;

    for (int i0 = 0; i0 < m_vertex.size(); ++i0)
    {
        int i1 = (i0 + 1) % m_vertex.size();
        const Vector& v0 = m_vertex[i0];
        const Vector& v1 = m_vertex[i1];

        Vector e0 = v0 - p;
        Vector e1 = v1 - p;

        float area = -(e0.x * e1.y - e0.y * e1.x);

        if (area * r < 0)
            return false;
    }

    return true;
}
