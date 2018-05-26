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

#include "MetaContainer.h"
#include "Invokable.h"

#include "Variant.h"
#include "Argument.h"

#include "FunctionInvoker.h"

#include <functional>

namespace System::Reflection
{
    inline namespace meta
    {
        class TC_METARUNTIME_API Function
            : public NamedMethod
        {
            friend class TypeData;
            friend class ReflectionDatabase;
        public:
            typedef std::function<Variant(ArgumentList&)> Invoker;

            Function(void);

            Function(
                const char* name,
                Type const& parentType,
                FunctionInvokerBase* invoker,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            virtual ~Function();

            static const Function &Invalid(void);

            Type const& DeclaringType(void) const override;

            bool CanCall(const ArgHashStorage & signature) const override;
            bool IsValid(void) const override;

            CXXAccessSpecifier GetAccessLevel() const override { return AccessLevel; }

            Variant InvokeVariadic(ArgumentList &arguments) const;

            template<typename ...Args>
            Variant Invoke(Args &&...args) const;

            MethodDesc const& GetMethodInfo() const override
            {
                return m_invoker->GetMethodInfo(m_UID);
            }

            // Inherited via NamedMethod
            Variant InternalInvoke(Variant & instance, ArgumentList & arguments) const override final;

            uint32_t GetUID() const override { return m_UID; }
            virtual bool IsStatic() const override;

        private:
            Function(Function const&) = delete;
            Function& operator=(Function const&) = delete;

            CXXAccessSpecifier AccessLevel : 8;
            uint32_t m_UID;

            Type const& m_parentType;

            FunctionInvokerBase* m_invoker;

        };
    }
}

#include "Impl/Function.hpp"