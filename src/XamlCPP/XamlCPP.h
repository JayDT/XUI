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

#ifndef __XAMLCPP_DEFAULT_PARSER_H__
#define __XAMLCPP_DEFAULT_PARSER_H__

#include <memory>
#include "standard/Platform/Define.h"
#include "standard/BasicPrimities.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Interfaces/IDeferredLoader.h"
#include "Interfaces/ILoader.h"
#include "Interfaces/IParser.h"
#include "Interfaces/IAssemblers.h"
#include "Interfaces/IXmlReader.h"
#include "Core/Parser/PhaseParserKit.h"
#include "standard/component_service.h"

#include <exception>

namespace XamlCPP
{
    class TC_XAMLCPP_API Xaml final
    {
    private:
        static std::shared_ptr<Interfaces::ILoader> xmlLoader;
        static std::shared_ptr<Interfaces::IRuntimeTypeSource> runtimeTypeSource;

    public:

        static System::ServiceContainer* UIServiceContext;

        static std::shared_ptr<Interfaces::IRuntimeTypeSource> const GetRuntimeTypeSource() { return runtimeTypeSource; }

        static std::shared_ptr<Interfaces::ILoader> DefaultLoader(System::ServiceContainer* serviceContext);
        static std::shared_ptr<Interfaces::ILoader> CreateLoaderContext();
        static bool IsUnsetValue(System::Reflection::Variant const&);
    };
}

#endif // !__XAMLCPP_DEFAULT_PARSER_H__
