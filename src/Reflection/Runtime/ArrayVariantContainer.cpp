#include "Precompiled.h"
#include "ArrayVariantContainer.h"

using namespace System::Reflection::meta;

ArrayVariantContainer::ArrayVariantContainer(Type const* elementType, ArrayWrapper const& array)
    : m_array(array)
    , m_elementType(elementType)
{

}

System::Reflection::meta::ArrayVariantContainer::~ArrayVariantContainer()
{
}

Type const& ArrayVariantContainer::GetType(void) const
{
    return *m_elementType;
}


void *ArrayVariantContainer::GetPtr(void) const
{
    return m_array.GetPtr();
}


int ArrayVariantContainer::ToInt(void) const
{
    return int();
}


bool ArrayVariantContainer::ToBool(void) const
{
    return bool();
}


float ArrayVariantContainer::ToFloat(void) const
{
    return float();
}


double ArrayVariantContainer::ToDouble(void) const
{
    return double();
}


std::string ArrayVariantContainer::ToString(void) const
{
    return std::string();
}


bool ArrayVariantContainer::IsArray(void) const
{
    return true;
}


ArrayWrapper* ArrayVariantContainer::GetArray(void) const
{
    return const_cast<ArrayWrapper*>(&m_array);
}


VariantBase *ArrayVariantContainer::Clone(void) const
{
    return nullptr;
}

bool System::Reflection::meta::ArrayVariantContainer::Equal(VariantBase const & variant) const
{
    if (!variant || GetType() != variant.GetType())
        return false;

    return GetPtr() == variant.GetPtr();
}
