#include "MethodBase.h"

#include "LanguageTypes/Class.h"
#include "LanguageTypes/Method.h"

#include "MetaDataConfig.h"
#include "MetaUtils.h"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

MethodBase::MethodBase(const Cursor & cursor, const Namespace & currentNamespace, Class * parent)
    : LanguageType(cursor, currentNamespace)
    , Invokable(cursor)
    , m_isTemplate(parent ? parent->IsTemplate() : false)
    , m_isStatic(cursor.IsStatic())
    , m_isVirtual(cursor.IsVirtual())
    , m_isPureVirtual(cursor.isPureVirtual())
    , m_isFunctionTemplate(cursor.GetKind() == CXCursorKind::CXCursor_FunctionTemplate)
    , m_parent(parent)
    , m_name(cursor.GetSpelling())
    , m_isReferenceReturnType(false)
{
    m_displayName = cursor.GetSpelling();

    switch (m_returnTypeInfo.GetKind())
    {
        case CXType_Pointer:
            break;
        case CXType_LValueReference:
        case CXType_RValueReference:
            m_isReferenceReturnType = true;
            break;
    }
}

void MethodBase::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    LanguageType::MetaInit(cursor, currentNamespace);
    Invokable::InitMeta(cursor, m_parent ? m_parent->IsTemplate() : m_isTemplate, (LanguageType*)(this));

    if (m_isFunctionTemplate)
    {
        auto TemplateTypes = m_metaData.GetAttribute(CXCursorKind::CXCursor_TemplateTypeParameter);
        for (auto iT = TemplateTypes.first; iT != TemplateTypes.second; ++iT)
        {
            m_templateTypeNames.push_back(iT->second.first);
        }
    }
}

Mustache::Data<std::string> MethodBase::CompileStandardTemplate(Mustache::Data<std::string>& data, const ReflectionParser * context) const
{

    data["internalID"] = std::to_string(++internalID);
    data["name"] = m_name;
    data["AccessLevel"] = std::to_string(int(m_accessModifier));

    data["isVoidReturn"] = utils::TemplateBool(m_returnType == "void");
    data["isReferenceReturnType"] = utils::TemplateBool(m_isReferenceReturnType);

    if (m_parent)
        data["parentQualifiedName"] =  m_parent->m_qualifiedName;
    data["returnType"] = m_returnType;

    data["qualifiedSignature"] = getQualifiedSignature("");
    data["typedQualifiedSignature"] = getQualifiedSignature("Signature");

    data["isTemplate"] = utils::TemplateBool(m_isFunctionTemplate);
    data["isVirtual"] = utils::TemplateBool(m_isVirtual);
    data["isReflObject"] = utils::TemplateBool(m_parent && m_parent->IsReflObject());
    data["isDynamicCast"] = utils::TemplateBool(m_parent && m_parent->HasVirtualBase());
    data["instanceBody"] = context->LoadTemplatePartial(kPartialInstanceDefination);

    // TODO: Build template
    std::string invoke_args;
    std::string signature_args;
    for (int i = 0; i != m_signature.size(); ++i)
    {
        if (i)
        {
            invoke_args += " ,";
            signature_args += " ,";
        }

        bool hasVirtualBase = false;
        bool isReflObject = false;
        bool isPointer = m_arguments[i]->IsPointer();
        auto ArgTypeClass = m_arguments[i]->FindClassLangType();  //GetDeclarationType()->GetKind() != CXTypeKind::CXType_Invalid ? m_arguments[i]->GetDeclarationType()->GetClassLangType() : m_arguments[i]->GetClassLangType();
        if (ArgTypeClass && ArgTypeClass->IsTemplate() && ArgTypeClass->GetDisplayName() == "shared_ptr")
        {
            data["isTypeReflPointer"] = utils::TemplateBool(true);
            for (auto defClass : ArgTypeClass->m_definations)
            {
                if (m_arguments[i]->GetQualifiedName() == defClass->m_qualifiedName)
                {
                    // Special case for std::shared_ptr
                    if (!defClass->m_templateTypes.empty())
                    {
                        auto templateArgClass = defClass->m_templateTypes[0]->GetClassLangType().get();

                        if (!templateArgClass)
                        {
                            auto hash = defClass->m_templateTypes[0]->GetDeclarationHash();
                            if (defClass->m_templateTypes[0]->GetElaboratedType())
                                hash = defClass->m_templateTypes[0]->GetElaboratedType()->GetDeclarationHash();

                            templateArgClass = ReflectionModulParser::Instance->GetClassDesc(hash);
                        }

                        if (templateArgClass)
                        {
                            // Note: This class not part of assembly but required init for correct typeinfo
                            if (!templateArgClass->IsPreparedForAssembly())
                                templateArgClass->PrepareForAssembly();
                            isReflObject = templateArgClass->IsReflObject();
                            hasVirtualBase = templateArgClass->HasVirtualBase();
                            break;
                        }
                    }
                }
            }

            if (!isPointer && isReflObject)
            {
                if (hasVirtualBase)
                    invoke_args += "std::dynamic_pointer_cast";
                else
                    invoke_args += "std::static_pointer_cast";

                invoke_args += (boost::format("<System::Reflection::meta_traits::RemoveSharedPtr<%2%>::type>(arguments[%1%]->ToSharedObject())") % i % m_signature[i]).str();
            }
            else
                invoke_args += (boost::format("*static_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->GetPtr())") % i % m_signature[i]).str();
        }
        else
        {
            if (m_arguments[i]->GetElaboratedType())
            {
                Class* klass = ReflectionModulParser::Instance->GetClassDesc(m_arguments[i]->GetElaboratedType()->GetDeclarationHash());
                if (klass)
                {
                    // Note: This class not part of assembly but required init for correct typeinfo
                    if (!klass->IsPreparedForAssembly())
                        klass->PrepareForAssembly();
                    isReflObject = klass->IsReflObject();
                    hasVirtualBase = klass->HasVirtualBase();
                }
            }

            if (isReflObject)
            {
                if (hasVirtualBase)
                {
                    if (isPointer)
                        invoke_args += (boost::format("dynamic_cast<std::remove_reference<%2%>::type>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                    else
                        invoke_args += (boost::format("*dynamic_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                }
                else
                {
                    if (isPointer)
                        invoke_args += (boost::format("static_cast<std::remove_reference<%2%>::type>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                    else
                        invoke_args += (boost::format("*static_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                }
            }
            else
                invoke_args += (boost::format("*static_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->GetPtr())") % i % m_signature[i]).str();
        }
        signature_args += (boost::format("typeid(%1%).hash_code()") % m_signature[i]).str();
    }

    data["invoke_args"] = invoke_args;
    data["signature_args"] = signature_args;

    m_metaData.CompileTemplateData(data, context);

    return data;
}

bool MethodBase::ShouldCompile(void) const
{
    return !m_isFunctionTemplate;
}

bool MethodBase::isAccessible(void) const
{
    return !m_metaData.GetFlag(native_attribute::Disable);
}
