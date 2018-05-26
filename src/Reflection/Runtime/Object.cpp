#include "Reflection/Runtime/Object.h"
#include "Reflection/Runtime/ReflectionModule.h"
#include "TypeConfig.h"
#include "Type.h"
#include "TypeCreator.h"

#include "json11.hpp"

using namespace System::Reflection::meta;

Type const* Object::GetType(void) const
{
    if (!this)
        return &Type::Invalid();
    //if (!m_type)
    //    m_type = &typeinstanceof(this);
    //return m_type;
    return &typeinstanceof(this);
}

size_t System::Reflection::meta::Object::GetHashCode() const
{
    return std::hash<size_t>()(reinterpret_cast<uintptr_t>(this));
}

Object * Object::Clone(void) const 
{
    return TypeCreator::Create(*GetType()).ToObject();
}

Object * ISerializable::Clone(void) const
{
    Variant var = TypeCreator::Create(*GetType());
    if (!var.GetBase())
        throw "invalid operation";

    Object* clone = var.ToObject();
    {
        json11::Json::object mirror;
        OnSerialize(mirror);
        clone->OnDeserialize(mirror);
    }
    return clone;
}

void ISerializable::OnSerialize(json11::Json::object & output) const
{
    output = GetType()->SerializeJson(Variant(static_cast<Object const*>(this), static_cast<variant_policy::NoCopy*>(nullptr))).object_items();
}

void ISerializable::OnDeserialize(const json11::Json & input)
{
    GetType()->DeserializeJson(Variant(static_cast<Object const*>(this), static_cast<variant_policy::NoCopy*>(nullptr)), input);
}

std::string System::Reflection::meta::Object::ToString() const 
{
    return GetType()->GetFullName();
}

bool System::Reflection::meta::Object::Equal(VariantBase const & variant) const
{
    if (!variant || !variant.ToObject() || this != variant.ToObject())
        return false;

    //if (*variant.ToObject()->GetType() != *GetType())
    //    return false;

    return true;
}

bool System::Reflection::meta::Object::Is(Type const& TargetType)
{
    if (!this)
        return false;

    const Type* selfType = GetType();

    // Todo virtual parent detect
    return (TargetType.IsAssignableFrom(*selfType) || selfType->DerivesFrom(TargetType));
}
