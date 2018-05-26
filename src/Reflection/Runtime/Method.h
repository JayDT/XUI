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

#include "Invokable.h"

#include "Variant.h"
#include "Argument.h"

#include "MethodInvoker.h"

#include <vector>
#include <unordered_map>
#include <functional>

namespace System::Reflection
{
    inline namespace meta
    {

        class TC_METARUNTIME_API Method
            : public NamedMethod
        {
            friend class TypeData;

        public:
            Method(void);

            Method(
                const char* name, 
                Type const& returnType,
                MethodInvokerBase* invoker,
                bool isConst,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            virtual ~Method();

            static const Method &Invalid(void);

            Type const& DeclaringType(void) const override;

            bool CanCall(const ArgHashStorage &signature) const override;
            bool CanCall(const InvokableSignature &signature) const override;
            bool IsValid(void) const override;
            bool IsConst(void) const;

            CXXAccessSpecifier GetAccessLevel() const override { return AccessLevel; }

            Variant InternalInvoke(Variant &instance, ArgumentList &arguments) const override;

            MethodDesc const& GetMethodInfo() const override
            {
                return m_invoker->GetMethodInfo(m_UID);
            }

            uint32_t GetUID() const override { return m_UID; }

        private:
            Method(Method const&) = delete;
            Method& operator=(Method const&) = delete;

            bool m_isConst;
            CXXAccessSpecifier AccessLevel : 8;
            uint32_t m_UID;

            Type const& m_classType;
            MethodInvokerBase* m_invoker;
        };
    }
}

#include "Impl/Method.hpp"