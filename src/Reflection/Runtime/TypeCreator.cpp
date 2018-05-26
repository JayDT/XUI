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

#include "Precompiled.h"

#include "TypeCreator.h"

using namespace System::Reflection;

Variant TypeCreator::Create(const Type& type, std::vector<System::Reflection::Variant> const& args)
{
    ArgHashStorage signature;

    for (auto const& any : args)
        signature.emplace_back(any.GetType()->GetHashCode());

    Type const* _type = &type;
    if (_type->IsPointer() && _type->ElaborateType)
        _type = type.ElaborateType;

    ArgumentList arguments;
    for (auto const& any : args)
    {
        arguments.push_back(new Argument(any));
    }

    auto &constructor = _type->GetConstructor(signature);

    auto ret = constructor.InvokeVariadic(arguments);

    for (auto& arg : arguments)
    {
        delete arg;
    }

    return ret;
}

std::shared_ptr<Object> TypeCreator::CreateShared(const Type & type, std::vector<System::Reflection::Variant> const & args)
{
	ArgHashStorage signature;

    for (auto const& any : args)
        signature.emplace_back(any.GetType()->GetHashCode());

    Type const* _type = &type;
    if (_type->IsPointer() && _type->ElaborateType)
        _type = type.ElaborateType;

    ArgumentList arguments;
    for (auto const& any : args)
    {
        arguments.push_back(new Argument(any));
    }

    auto &constructor = _type->GetConstructor(signature);
    if (!constructor.IsValid())
        return nullptr;

    auto ret = _type->__class_shared_contructor_impl(constructor.GetUID(), std::move(arguments));

    for (auto& arg : arguments)
    {
        delete arg;
    }

    return ret;
}

Variant TypeCreator::CreateEx(const Type& type, std::vector<System::Reflection::Variant> const& args)
{
	InvokableSignature signature;

	for (auto const& any : args)
		signature.emplace_back(any.GetType());

	Type const* _type = &type;
	if (_type->IsPointer() && _type->ElaborateType)
		_type = type.ElaborateType;

	ArgumentList arguments;
	for (auto const& any : args)
	{
		arguments.push_back(new Argument(any));
	}

	auto &constructor = _type->GetConstructor(signature);
	if (!constructor.IsValid())
		return nullptr;

	auto ret = constructor.InvokeVariadic(arguments);

	for (auto& arg : arguments)
	{
		delete arg;
	}

	return ret;
}

std::shared_ptr<Object> TypeCreator::CreateSharedEx(const Type& type,
	std::vector<System::Reflection::Variant> const& args)
{
	InvokableSignature signature;

	for (auto const& any : args)
		signature.emplace_back(any.GetType());

	Type const* _type = &type;
	if (_type->IsPointer() && _type->ElaborateType)
		_type = type.ElaborateType;

	ArgumentList arguments;
	for (auto const& any : args)
	{
		arguments.push_back(new Argument(any));
	}

	auto &constructor = _type->GetConstructor(signature);
	if (!constructor.IsValid())
		return nullptr;

	auto ret = _type->__class_shared_contructor_impl(constructor.GetUID(), std::move(arguments));

	for (auto& arg : arguments)
	{
		delete arg;
	}

	return ret;
}

//Variant TypeCreator::CreateVariadic(const Type &type, const ArgumentList &arguments)
//{
//    InvokableSignature signature;
//    
//    for (auto itr = arguments.begin(); itr != arguments.end(); ++itr)
//        signature.emplace_back( &(*itr)->GetType( ) );
//    
//    auto &constructor = type.GetConstructor( signature );
//    
//    return constructor.InvokeVariadic( const_cast<ArgumentList &>(arguments) );
//}
//
//Variant TypeCreator::CreateDynamicVariadic(const Type &type, const ArgumentList &arguments)
//{
//    InvokableSignature signature;
//    
//    for (auto &argument : arguments)
//        signature.emplace_back( &argument->GetType( ) );
//    
//    auto &constructor = type.GetDynamicConstructor( signature );
//    
//    return constructor.InvokeVariadic( const_cast<ArgumentList &>(arguments) );
//}
