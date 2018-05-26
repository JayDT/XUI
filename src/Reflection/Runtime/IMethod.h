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

#include "Reflection/Runtime/ArgumentConfig.h"
#include "Reflection/Runtime/IMemberInfo.h"
#include "Reflection/Runtime/InvokableConfig.h"
#include <mutex>

namespace System::Reflection
{
    inline namespace meta
    {
        struct MethodDesc;
        class Variant;

        struct TC_METARUNTIME_API IMethod : IMemberInfo
        {
            virtual ~IMethod() {}

            bool IsPublic() const { return GetAccessLevel() == CXXAccessSpecifier::Public; }
            bool IsProtected() const { return GetAccessLevel() == CXXAccessSpecifier::Protected; }
            bool IsPrivate() const { return GetAccessLevel() == CXXAccessSpecifier::Private; }
            virtual bool IsStatic() const = 0;

            virtual bool CanCall(const ArgHashStorage & signature) const = 0;
			virtual bool CanCall(const InvokableSignature &signature) const { throw System::NotSupportedException(); }
            virtual bool IsValid(void) const = 0;

            virtual MethodDesc const& GetMethodInfo() const = 0;

            virtual Variant InternalInvoke(Variant &instance, ArgumentList &arguments) const = 0;

            template<typename ...Args>
            Variant Invoke(Variant &instance, Args &&...args) const;
        };

        template<typename ...Args>
        inline const System::Reflection::meta::InvokableSignature& GetInvokableSignature(Args && ...args)
        {
            static std::mutex __lock;
            static System::Reflection::meta::InvokableSignature signatureTypes;
            static bool initial = true;

            if (initial)
            {
                __lock.lock();
                if (initial)
                {
                    TypeUnpacker<Args...>::Apply(signatureTypes);
                    initial = false;
                }
                __lock.unlock();
            }

            return signatureTypes;
        }

        template<typename ...Args>
        inline Variant IMethod::Invoke(Variant & instance, Args && ...args) const
        {
            //static Type::List signatureTypes;
            //static bool initial = true;
            //
            //if (initial)
            //{
            //    TypeUnpacker<Args...>::Apply(signatureTypes);
            //    initial = false;
            //}

            System::Reflection::meta::InvokableSignature& signatureTypes = const_cast<System::Reflection::meta::InvokableSignature&>(GetInvokableSignature(std::forward<Args>(args)...));

            ArgumentList arguments;
            ArgUnpacker().Apply(arguments, signatureTypes, 0, std::forward<Args>(args)...);

            auto ret = InternalInvoke(instance, arguments);

            ArgUnpacker().Free<int(sizeof...(Args)-1)>(arguments);

            return ret;
        }
    }
}