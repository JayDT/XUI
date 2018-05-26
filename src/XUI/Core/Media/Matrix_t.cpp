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
#include "Core/Media/Matrix_t.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace XUI::Core::Media;

namespace glm
{
    template<typename valType, qualifier Q>
    void CreateFromYawPitchRollQuat
    (
    	valType const& pitch,
    	valType const& yaw,
    	valType const& roll,
        tquat<valType, Q>& out
    )
    {
    	vec<3, valType> eulerAngle(pitch * valType(0.5), yaw * valType(0.5), roll * valType(0.5));
    	vec<3, valType> c = glm::cos(eulerAngle);
    	vec<3, valType> s = glm::sin(eulerAngle);
    	
    	out.x = s.x * c.y * c.z + c.x * s.y * s.z;
    	out.y = c.x * s.y * c.z - s.x * c.y * s.z;
    	out.z = c.x * c.y * s.z - s.x * s.y * c.z;
    	out.w = c.x * c.y * c.z + s.x * s.y * s.z;
    }
}

glm::vec2 operator*(glm::vec2 const& position, glm::mat4 const& matrix)
{
    glm::vec2 result;

    result.x = (position.x * matrix[0][0]) + (position.y * matrix[1][0]) + matrix[3][0];
    result.y = (position.x * matrix[0][1]) + (position.y * matrix[1][1]) + matrix[3][1];

    return result;
}

glm::vec3 operator*(glm::vec3 const& position, glm::mat4 const& matrix)
{
    glm::vec3 result;

    result.x = (position.x * matrix[0][0]) + (position.y * matrix[1][0]) + (position.z * matrix[2][0]) + matrix[3][0];
    result.y = (position.x * matrix[0][1]) + (position.y * matrix[1][1]) + (position.z * matrix[2][1]) + matrix[3][1];
    result.z = (position.x * matrix[0][2]) + (position.y * matrix[1][2]) + (position.z * matrix[2][2]) + matrix[3][2];

    return result;
}

glm::vec4 operator*(glm::vec4 const& position, glm::mat4 const& matrix)
{
    glm::vec4 result;

    result.x = (position.x * matrix[0][0]) + (position.y * matrix[1][0]) + (position.z * matrix[2][0]) + matrix[3][0];
    result.y = (position.x * matrix[0][1]) + (position.y * matrix[1][1]) + (position.z * matrix[2][1]) + matrix[3][1];
    result.z = (position.x * matrix[0][2]) + (position.y * matrix[1][2]) + (position.z * matrix[2][2]) + matrix[3][2];
    result.w = (position.x * matrix[0][3]) + (position.y * matrix[1][3]) + (position.z * matrix[2][3]) + matrix[3][3];

    return result;
}

XUI::Core::Media::Rect operator*(XUI::Core::Media::Rect const& rect, XUI::Core::Media::Matrix const& mat)
{
    XUI::Core::Media::Vector points[4];

    points[0] = (glm::vec2&)rect.x0y0() * (glm::mat4&)mat;
    points[1] = (glm::vec2&)rect.x0y1() * (glm::mat4&)mat;
    points[2] = (glm::vec2&)rect.x1y0() * (glm::mat4&)mat;
    points[3] = (glm::vec2&)rect.x1y1() * (glm::mat4&)mat;

    auto left = std::numeric_limits<double>::max();
    auto right = std::numeric_limits<double>::max() * -1;
    auto top = std::numeric_limits<double>::max();
    auto bottom = std::numeric_limits<double>::max() * -1;

    for (auto const& p : points)
    {
        if (p.x < left) left = p.x;
        if (p.x > right) right = p.x;
        if (p.y < top) top = p.y;
        if (p.y > bottom) bottom = p.y;
    }

    return XUI::Core::Media::Rect::xyxy(left, top, right, bottom);
}

glm::mat3x2 operator*(glm::mat3x2 const& m1, glm::mat3x2 const& m2)
{
    glm::mat3x2 out;

    out[0][0] = (m1[0][0] * m2[0][0]) + (m1[0][1] * m2[1][0]);
    out[0][1] = (m1[0][0] * m2[0][1]) + (m1[0][1] * m2[1][1]);
    out[1][0] = (m1[1][0] * m2[0][0]) + (m1[1][1] * m2[1][0]);
    out[1][1] = (m1[1][0] * m2[0][1]) + (m1[1][1] * m2[1][1]);
    out[2][0] = (m1[2][0] * m2[0][0]) + (m1[2][1] * m2[1][0]) + m2[2][0];
    out[2][1] = (m1[2][0] * m2[0][1]) + (m1[2][1] * m2[1][1]) + m2[2][1];

    return out;
}

void Matrix::CreateFromYawPitchRoll(float yaw, float pitch, float roll, Matrix& out)
{
    glm::quat quaternion;
    glm::CreateFromYawPitchRollQuat(yaw, pitch, roll, quaternion);

    out = glm::mat4_cast(quaternion);
}

void XUI::Core::Media::Matrix::CreateTranslation(float x, float y, float z, Matrix & out)
{
    out = glm::translate(out, glm::vec3(x, y, z));
}

void XUI::Core::Media::Matrix::CreateScale(float xScale, float yScale, float zScale, Matrix & out)
{
    out = glm::scale(out, glm::vec3(xScale, yScale, zScale));
}

Matrix const & XUI::Core::Media::Matrix::Identity()
{
    static Matrix ident(glm::mat4(1.0f));
    return ident;
}

Matrix XUI::Core::Media::Matrix::Inverse() const
{
    return glm::inverse(*this);
}

/// <summary>
/// Returns a String representing this matrix instance.
/// </summary>
/// <returns>The string representation.</returns>
std::string Matrix::ToString() const
{
    //return std::string::Format(ci, "{{ {{M11:{0} M12:{1}}} {{M21:{2} M22:{3}}} {{M31:{4} M32:{5}}} }}", M11.ToString(ci), M12.ToString(ci), M21.ToString(ci), M22.ToString(ci), M31.ToString(ci), M32.ToString(ci));
    return "";
}

/// <summary>
/// Parses a <see cref="Matrix"/> string.
/// </summary>
/// <param name="s">The string.</param>
/// <param name="culture">The current culture.</param>
/// <returns>The <see cref="Matrix"/>.</returns>

Matrix XUI::Core::Media::Matrix::Parse(const std::string & s)
{
    return Matrix();
}
