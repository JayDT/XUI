/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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
#ifndef __XAMLCPP_TYPECONVERTER_PROVIDER_H__
#define __XAMLCPP_TYPECONVERTER_PROVIDER_H__

#include <System/System.h>
#include "Interfaces/ITypeConverterProvider.h"
#include "Interfaces/ITypeConverter.h"

namespace XamlCPP::Core::ReflEx
{
    class TypeConverterProvider : public Interfaces::ITypeConverterProvider
    {
    public:
        TypeConverterProvider();
		virtual ~TypeConverterProvider() {}

		Interfaces::ITypeConverter *GetTypeConverter(std::size_t typehash) override;

		const std::unordered_map<std::size_t, std::shared_ptr<Interfaces::ITypeConverter>>& GetConverters() const;

        void Add(Core::TypeConverterRegistration const& item) override;

        std::vector<Core::TypeConverterRegistration> getTypeConverters() override;

    private:
        void RegisterBuiltIn();

        void Register(std::size_t typehash, std::shared_ptr<Interfaces::ITypeConverter> const& converter);

        static bool IsNullable(System::Reflection::Type const& type);

    };

}
#endif //!__XAMLCPP_TYPECONVERTER_PROVIDER_H__