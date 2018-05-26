#include "Precompiled.h"
#include "FieldSetterBase.h"
#include "Field.h"
#include "Argument.h"

using namespace System::Reflection;
using namespace System::Reflection::meta;

Type const & System::Reflection::meta::FieldSetterBase::DeclaringType(void) const
{
    return m_declaringField ? m_declaringField->DeclaringType() : Type::Invalid();
}

const char * System::Reflection::meta::FieldSetterBase::GetName(void) const
{
    return m_declaringField ? m_declaringField->GetName() : "";
}

CXXAccessSpecifier System::Reflection::meta::FieldSetterBase::GetAccessLevel() const
{
    return m_declaringField ? m_declaringField->GetAccessLevel() : CXXAccessSpecifier::Public;
}

bool System::Reflection::meta::FieldSetterBase::CanCall(const ArgHashStorage & signature) const
{
    return signature.size() == 1;
}

bool System::Reflection::meta::FieldSetterBase::IsValid(void) const
{
    return true;
}

MethodDesc const & System::Reflection::meta::FieldSetterBase::GetMethodInfo() const
{
    static MethodDesc default(
        0, 
        DeclaringType(),
        m_declaringField ? *m_declaringField->GetFieldType() : Type::Invalid(),
        MethodDesc::MethodType::NONE,
        {});
    return default;
}

Variant System::Reflection::meta::FieldSetterBase::InternalInvoke(Variant & instance, ArgumentList & arguments) const
{
    Variant value(arguments[0]->GetPtr());
    const_cast<FieldSetterBase*>(this)->SetValue(m_declaringField->GetUID(), instance, value);
    return Variant::Empty;
}

uint32_t System::Reflection::meta::FieldSetterBase::GetUID() const
{
    return m_declaringField ? m_declaringField->GetUID() : 0;
}
