/* ----------------------------------------------------------------------------
** GENERATED SOURCE FILE
** Do not modify the contents of this file.
**
** System::Reflection Meta Generator v1.0
** --------------------------------------------------------------------------*/

#include "Runtime/Default_Assembly/Assembly_MetaProperty.h" 
#include "Runtime/Default_Assembly/Assembly_Modul_Default.h"
 
#include "Reflection/Runtime/Type.h"


#include "Reflection/Runtime/ReflectionDatabase.h"
#include "Reflection/Runtime/ReflectionModule.h"
#include "Reflection/Runtime/MetaProperty.h"
#include "Reflection/Runtime/MethodInvokerBase.h"
#include "Reflection/Runtime/ArrayWrapper.h"
#include <typeinfo>
#include <type_traits>
#include <functional>
#include "Reflection/Runtime/Object.h"


namespace m = System::Reflection::meta;

void System::Reflection::Assembly::AssemblyModulDefault::AllocateModuleFileMetaProperty(m::ReflectionDatabase &db)
{
    ///////////////////////////////////////////////////////////////////////////
    // Class Allocation
    ///////////////////////////////////////////////////////////////////////////
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "System::Reflection::meta::MetaProperty", typeid(System::Reflection::meta::MetaProperty) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<System::Reflection::meta::MetaProperty>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< System::Reflection::meta::MetaProperty >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<System::Reflection::meta::MetaProperty>::type);                                                               
                Data->native_hash_code = typeid(System::Reflection::meta::MetaProperty).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "System::Reflection::meta::MetaProperty*", typeid(System::Reflection::meta::MetaProperty*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<System::Reflection::meta::MetaProperty*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< System::Reflection::meta::MetaProperty* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<System::Reflection::meta::MetaProperty*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<System::Reflection::meta::MetaProperty*>::type);                                                               
                    Data->native_hash_code = typeid(System::Reflection::meta::MetaProperty*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const System::Reflection::meta::MetaProperty*", typeid(const System::Reflection::meta::MetaProperty*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const System::Reflection::meta::MetaProperty*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const System::Reflection::meta::MetaProperty*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const System::Reflection::meta::MetaProperty*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const System::Reflection::meta::MetaProperty* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const System::Reflection::meta::MetaProperty*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const System::Reflection::meta::MetaProperty*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const System::Reflection::meta::MetaProperty*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const System::Reflection::meta::MetaProperty*>::type);                                                               
                    Data->native_hash_code = typeid(const System::Reflection::meta::MetaProperty*).hash_code();                                                             
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Enum Allocation
    ///////////////////////////////////////////////////////////////////////////
    
}

// Virtual Base Helpers (return adjuter to Refl Object)
intptr_t System::Reflection::Assembly::AssemblyModulDefault::offset_adjustment_MetaProperty_impl(size_t classId, m::Variant& from)
{
    switch (classId)
    {
        default:
        case 0: return 0;
        // Properties
        case 83:
        {
            return reinterpret_cast<intptr_t>(static_cast<m::Object*>(from.ToObject())) - reinterpret_cast<intptr_t>(dynamic_cast<System::Reflection::meta::MetaProperty*>(from.ToObject()));
        }
    }
}

// Field Explicit Setter Invokers
void System::Reflection::Assembly::AssemblyModulDefault::field_Setter_MetaProperty_impl(size_t fieldId, m::Variant&& obj, m::Variant&& value)
{
    switch (fieldId)
    {
        default:
        case 0: return;
        // Properties
    }
}
    //staticField
// Field Explicit Getter Invokers
m::Variant System::Reflection::Assembly::AssemblyModulDefault::field_Getter_MetaProperty_impl(size_t fieldId, m::Variant&& obj)
{
    switch (fieldId)
    {
        default:
        case 0: return {};
        // Properties
    }
}

// FieldDesc infos
m::FieldDesc const& System::Reflection::Assembly::AssemblyModulDefault::fieldinfo_MetaProperty_impl(size_t fieldId)
{
    switch (fieldId)
    {
        default:
        case 0:
        {
            static m::FieldDesc invalid(
                0,
                System::Reflection::type_of(typeid(void)),
                System::Reflection::type_of(typeid(void)),
                m::FieldDesc::FieldType::NONE,
                0);

            return invalid;
        }
    }
}

// Methods Invokers
m::Variant System::Reflection::Assembly::AssemblyModulDefault::invoker_MetaProperty_impl(size_t methodId, m::Variant&& obj, const m::ArgumentList&& arguments)
{
    switch (methodId)
    {
        default:
        case 0: return {};
        case 87:
        {
            return m::Variant(new System::Reflection::meta::MetaProperty());
        }
        
        case 84:
        {
            (((System::Reflection::meta::MetaProperty*)*(System::Reflection::meta::MetaProperty**)(obj.toPointer())))->~MetaProperty();
            return {};
        }
        case 86:
        {
            return m::Variant(new System::Reflection::meta::MetaProperty());
        }
    }
}
    
// Methods infos
m::MethodDesc const& System::Reflection::Assembly::AssemblyModulDefault::methodinfo_MetaProperty_impl(size_t methodId)
{
    switch (methodId)
    {
        case 87:
        {
            static m::MethodDesc m( 
                87,
                System::Reflection::type_of(typeid(System::Reflection::meta::MetaProperty)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::DYNAMIC_CONSTRUCTOR,
                {  });

            return m;
        }
        
        case 84:
        {
            static m::MethodDesc m( 
                84,
                System::Reflection::type_of(typeid(System::Reflection::meta::MetaProperty)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::DESTRUCTOR,
                { });

            return m;
        }
        case 86:
        {
            static m::MethodDesc m( 
                86,
                System::Reflection::type_of(typeid(System::Reflection::meta::MetaProperty)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }
        case 0:
        default:
        {
            static m::MethodDesc invalid(
                0,
                System::Reflection::type_of(typeid(void)),
                System::Reflection::type_of(typeid(void)),
                m::MethodDesc::MethodType::NONE,
                { });

            return invalid;
        }
    }
}

void System::Reflection::Assembly::AssemblyModulDefault::DefineModuleFileMetaProperty(m::ReflectionDatabase &db)
{
    ///////////////////////////////////////////////////////////////////////////
    // Global Definitions
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////
    // Global Functions
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////
    // Enum Definitions
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////
    // Class Definitions
    ///////////////////////////////////////////////////////////////////////////
    
    /**
     * System::Reflection::meta::MetaProperty
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( System::Reflection::meta::MetaProperty ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 83;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_MetaProperty_impl;
            type->metaData = {
                
            };
            type->name = "MetaProperty";
            
            type->nameSpace = "System::Reflection::meta";
            
            {
                m::TypeData::AttachedClass aclass;
                aclass.m_type = &typeof( System::Reflection::meta::ISerializable );
                aclass.m_nativeDescriptor = std::type_index( typeid( System::Reflection::meta::ISerializable ) );
                aclass.m_isVirtual = 0;
                aclass.m_accessLevel = m::CXXAccessSpecifier(0);
                type->AddBaseClasses(db, typeID, aclass);
            }
            
            
            
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_MetaProperty_impl, &AssemblyModulDefault::methodinfo_MetaProperty_impl, 84 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_MetaProperty_impl, &AssemblyModulDefault::methodinfo_MetaProperty_impl, 86, { } );
            // Dynamic Constructors
            
            type->AddConstructor( &AssemblyModulDefault::invoker_MetaProperty_impl, &AssemblyModulDefault::methodinfo_MetaProperty_impl, 87, {
                    
                }, m::CXXAccessSpecifier(1)
            );
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
}
