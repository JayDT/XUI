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

#ifndef __XUI_MATRIX_H__
#define __XUI_MATRIX_H__

#include <glm/mat4x4.hpp>

namespace XUI::Core::Media
{
    /// <summary>
    /// Defines a rectangle.
    /// </summary>
    struct TC_XUI_API Matrix : glm::mat4, System::Reflection::Object
    {
        Matrix()
            : glm::mat4(1.0f)
        {}

        Matrix(glm::mat4 const& from)
            : glm::mat4(from)
        {
        }

        Matrix(Matrix const& from)
            : glm::mat4(from)
        {
        }

        static Matrix CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        {
            Matrix out = Identity();
            CreateFromYawPitchRoll(yaw, pitch, roll, out);
            return out;
        }
        static void CreateFromYawPitchRoll(float yaw, float pitch, float roll, Matrix& out);

        static Matrix CreateTranslation(float x, float y, float z)
        {
            Matrix out = Identity();
            CreateTranslation(x, y, z, out);
            return out;
        }

        static void CreateTranslation(float x, float y, float z, Matrix& out);

        static Matrix CreateScale(float x, float y, float z)
        {
            Matrix out = Identity();
            CreateScale(x, y, z, out);
            return out;
        }

        static void CreateScale(float xScale, float yScale, float zScale, Matrix& out);

        static Matrix const& Identity();

        Matrix Inverse() const;

        static glm::mat3x2 CreateTranslation(float x, float y)
        {
            glm::mat3x2 out;
            out[0][0] = 1.0f;
            out[0][1] = 0.0f;
            out[1][0] = 0.0f;
            out[1][1] = 1.0f;
            out[2][0] = x;
            out[2][1] = y;
            return out;
        }

        static glm::mat3x2 ToMat3x2(glm::mat4 const& m1)
        {
            glm::mat3x2 out;
            out[0][0] = m1[0][0];
            out[0][1] = m1[0][1];
            out[1][0] = m1[1][0];
            out[1][1] = m1[1][1];
            out[2][0] = m1[3][0];
            out[2][1] = m1[3][1];
            return out;
        }

        static Core::Media::Matrix From(glm::mat3x2 const& m1)
        {
            auto m11 = m1[0][0];
            auto m12 = m1[0][1];
            auto m13 = 0;
            auto m14 = 0;

            auto m21 = m1[1][0];
            auto m22 = m1[1][1];
            auto m23 = 0;
            auto m24 = 0;

            auto m31 = 0;
            auto m32 = 0;
            auto m33 = 1.0f;
            auto m34 = 0;

            auto m41 = m1[2][0];
            auto m42 = m1[2][1];
            auto m43 = 0;
            auto m44 = 1.0f;

            return glm::mat4(
                m11,
                m12,
                m13,
                m14,
                m21,
                m22,
                m23,
                m24,
                m31,
                m32,
                m33,
                m34,
                m41,
                m42,
                m43,
                m44);
        }

        /// <summary>
        /// Returns a String representing this matrix instance.
        /// </summary>
        /// <returns>The string representation.</returns>
        std::string ToString() const override;

        /// <summary>
        /// Parses a <see cref="Matrix"/> string.
        /// </summary>
        /// <param name="s">The string.</param>
        /// <param name="culture">The current culture.</param>
        /// <returns>The <see cref="Matrix"/>.</returns>
        static Matrix Parse(const std::string &s);

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(Matrix))
                return false;

            return *this == *(Matrix*)(variant.ToPointer());
        }
    };
}


#endif	//#ifndef __XUI_MATRIX_H__
