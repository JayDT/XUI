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

#ifndef _XUI_MEDIA_BRUSH_H__
#define _XUI_MEDIA_BRUSH_H__

#include "Interfaces/IBrush.h"
#include "Core/Media/Color_t.h"

namespace XUI::Core::Media
{
    class TC_XUI_API Brush : public Interfaces::IBrush
    {
    public:
        static Core::Dependency::RefDependencyProperty OpacityProperty;

        Brush() {}
        virtual ~Brush() {}

        PROPERTY_T(double, Opacity);
        double GetOpacity() const override;
        void SetOpacity(double);

        virtual Interfaces::BurshTypes GetTypeId() const override { return Interfaces::BurshTypes::BURSH; }

        /// <summary>
        /// Parses a brush string.
        /// </summary>
        /// <param name="s">The brush string.</param>
        /// <returns>The <see cref="Color"/>.</returns>
        static std::shared_ptr<IBrush> Parse(std::string const& s);
    };

    class TC_XUI_API SolidColorBrush : public Brush
    {
    public:
        static Core::Dependency::RefDependencyProperty ColorProperty;

        SolidColorBrush() {}
        SolidColorBrush(XUI::Core::Media::Color const& c);
        virtual ~SolidColorBrush() {}

        virtual Interfaces::BurshTypes GetTypeId() const override { return Interfaces::BurshTypes::SOLID_COLOR; }

        PROPERTY_T(XUI::Core::Media::Color, Color)
        XUI::Core::Media::Color GetColor() const;
        void SetColor(XUI::Core::Media::Color const&);

        bool operator==(SolidColorBrush const& rhs) const
        {
            return GetColor() == rhs.GetColor() && GetOpacity() == rhs.GetOpacity();
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(SolidColorBrush))
                return false;

            return *this == *(SolidColorBrush*)(variant.ToPointer());
        }
    };
}

#endif //!_XUI_MEDIA_COLOR_H__