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

#include "Variant.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class TypeCreator
        {
        public:
            /** @brief Instantiates an instance of the given type with the given
             *         constructor signature. NOTE: it is much faster to cache
             *         the appropriate constructor first, then call
             *         Invoke( ) manually.
             *  @param arguments List of arguments to forward to the
             *                   type constructor.
             *  @return Variant representing the newly created type instance.
             */
            //static Variant CreateVariadic(const Type &type, const ArgumentList &arguments);

            /** @brief Same as CreateVariadic( ), except it uses the dynamic
             *         constructor and returns the class pointer type.
             *  @param arguments List of arguments to forward to the
             *                   type constructor.
             *  @return Variant representing a pointer to the newly
             *          created type instance. NOTE: client is responsible for
             *          memory management, either through type.Destroy( ) or
             *          directly calling the underlying pointer's deconstructor
             */
            //static Variant CreateDynamicVariadic(const Type &type, const ArgumentList &arguments);

            /** @brief Instantiates an instance of the type with the given
             *         constructor signature. NOTE: it is much faster to cache
             *         the appropriate constructor first, then call
             *         Invoke( ) manually.
             *  @param arguments List of arguments to forward to the
             *                   type constructor.
             *  @return Variant representing the newly created type instance.
             */
            template<typename ...Args>
            static Variant Create(const Type &type, Args &&...args);

            template<typename ...Args>
            static std::shared_ptr<Object> CreateShared(const Type &type, Args &&...args);


            /** @brief Same as Create( ), except it uses the dynamic
              *         constructor and returns the class pointer type.
              * @param arguments List of arguments to forward to the
              *                   type constructor.
              * @return Variant representing a pointer to the newly
              *          created type instance. NOTE: client is responsible for
              *          memory management, either through type.Destroy( ) or
              *          directly calling the underlying pointer's deconstructor
              */
            template<typename ...Args>
            static Variant CreateDynamic(const Type &type, Args &&...args);

            static Variant TC_METARUNTIME_API Create(const Type& type, std::vector<System::Reflection::Variant> const& args);
            static std::shared_ptr<Object> TC_METARUNTIME_API CreateShared(const Type& type, std::vector<System::Reflection::Variant> const& args);

			static Variant TC_METARUNTIME_API CreateEx(const Type& type, std::vector<System::Reflection::Variant> const& args);
			static std::shared_ptr<Object> TC_METARUNTIME_API CreateSharedEx(const Type& type, std::vector<System::Reflection::Variant> const& args);
		};
    }
}

#include "Impl/TypeCreator.hpp"