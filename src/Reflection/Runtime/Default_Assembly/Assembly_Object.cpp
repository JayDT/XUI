/* ----------------------------------------------------------------------------
** GENERATED SOURCE FILE
** Do not modify the contents of this file.
**
** System::Reflection Meta Generator v1.0
** --------------------------------------------------------------------------*/

#include "Reflection/Runtime/Default_Assembly/Assembly_Object.h"
#include "Reflection/Runtime/Default_Assembly/Assembly_Modul_Default.h"


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

void System::Reflection::Assembly::AssemblyModulDefault::AllocateModuleFileObject(m::ReflectionDatabase &db)
{
    ///////////////////////////////////////////////////////////////////////////
    // Class Allocation
    ///////////////////////////////////////////////////////////////////////////

    DeclareReflTypeData(Variant, System::Reflection::meta::Variant)
    DeclareReflTypeData(Type, System::Reflection::meta::Type)

    {
        { // Base Type
            m::TypeID id = db.AllocateType( "void", typeid(void) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<void>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<void>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<void>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< void >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<void>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<void>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<void>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<void>::type);                                                               
                Data->native_hash_code = typeid(void).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "void*", typeid(void*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<void*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<void*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<void*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< void* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<void*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<void*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<void*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<void*>::type);                                                               
                    Data->native_hash_code = typeid(void*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const void*", typeid(const void*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const void*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const void*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const void*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const void* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const void*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const void*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const void*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const void*>::type);                                                               
                    Data->native_hash_code = typeid(const void*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "char", typeid(char) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<char>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<char>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<char>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< char >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<char>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<char>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<char>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<char>::type);                                                               
                Data->native_hash_code = typeid(char).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "char*", typeid(char*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<char*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<char*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<char*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< char* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<char*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<char*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<char*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<char*>::type);                                                               
                    Data->native_hash_code = typeid(char*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const char*", typeid(const char*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const char*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const char*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const char*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const char* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const char*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const char*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const char*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const char*>::type);                                                               
                    Data->native_hash_code = typeid(const char*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "unsigned char", typeid(unsigned char) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned char>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned char>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned char>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< unsigned char >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned char>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned char>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned char>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<unsigned char>::type);                                                               
                Data->native_hash_code = typeid(unsigned char).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "unsigned char*", typeid(unsigned char*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned char*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned char*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned char*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< unsigned char* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned char*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned char*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned char*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<unsigned char*>::type);                                                               
                    Data->native_hash_code = typeid(unsigned char*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const unsigned char*", typeid(const unsigned char*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const unsigned char*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const unsigned char*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const unsigned char*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const unsigned char* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const unsigned char*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const unsigned char*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const unsigned char*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const unsigned char*>::type);                                                               
                    Data->native_hash_code = typeid(const unsigned char*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "short", typeid(short) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<short>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<short>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<short>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< short >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<short>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<short>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<short>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<short>::type);                                                               
                Data->native_hash_code = typeid(short).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "short*", typeid(short*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<short*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<short*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<short*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< short* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<short*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<short*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<short*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<short*>::type);                                                               
                    Data->native_hash_code = typeid(short*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const short*", typeid(const short*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const short*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const short*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const short*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const short* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const short*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const short*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const short*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const short*>::type);                                                               
                    Data->native_hash_code = typeid(const short*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "unsigned short", typeid(unsigned short) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned short>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned short>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned short>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< unsigned short >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned short>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned short>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned short>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<unsigned short>::type);                                                               
                Data->native_hash_code = typeid(unsigned short).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "unsigned short*", typeid(unsigned short*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned short*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned short*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned short*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< unsigned short* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned short*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned short*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned short*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<unsigned short*>::type);                                                               
                    Data->native_hash_code = typeid(unsigned short*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const unsigned short*", typeid(const unsigned short*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const unsigned short*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const unsigned short*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const unsigned short*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const unsigned short* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const unsigned short*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const unsigned short*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const unsigned short*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const unsigned short*>::type);                                                               
                    Data->native_hash_code = typeid(const unsigned short*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "int", typeid(int) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<int>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<int>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<int>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< int >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<int>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<int>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<int>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<int>::type);                                                               
                Data->native_hash_code = typeid(int).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "int*", typeid(int*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<int*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<int*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<int*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< int* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<int*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<int*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<int*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<int*>::type);                                                               
                    Data->native_hash_code = typeid(int*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const int*", typeid(const int*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const int*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const int*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const int*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const int* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const int*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const int*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const int*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const int*>::type);                                                               
                    Data->native_hash_code = typeid(const int*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "unsigned int", typeid(unsigned int) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned int>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned int>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned int>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< unsigned int >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned int>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned int>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned int>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<unsigned int>::type);                                                               
                Data->native_hash_code = typeid(unsigned int).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "unsigned int*", typeid(unsigned int*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned int*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned int*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned int*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< unsigned int* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned int*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned int*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned int*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<unsigned int*>::type);                                                               
                    Data->native_hash_code = typeid(unsigned int*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const unsigned int*", typeid(const unsigned int*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const unsigned int*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const unsigned int*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const unsigned int*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const unsigned int* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const unsigned int*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const unsigned int*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const unsigned int*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const unsigned int*>::type);                                                               
                    Data->native_hash_code = typeid(const unsigned int*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "long long", typeid(long long) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<long long>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<long long>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<long long>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< long long >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<long long>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<long long>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<long long>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<long long>::type);                                                               
                Data->native_hash_code = typeid(long long).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "long long*", typeid(long long*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<long long*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<long long*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<long long*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< long long* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<long long*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<long long*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<long long*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<long long*>::type);                                                               
                    Data->native_hash_code = typeid(long long*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const long long*", typeid(const long long*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const long long*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const long long*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const long long*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const long long* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const long long*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const long long*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const long long*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const long long*>::type);                                                               
                    Data->native_hash_code = typeid(const long long*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "unsigned long long", typeid(unsigned long long) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned long long>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned long long>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned long long>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< unsigned long long >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned long long>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned long long>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned long long>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<unsigned long long>::type);                                                               
                Data->native_hash_code = typeid(unsigned long long).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "unsigned long long*", typeid(unsigned long long*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<unsigned long long*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<unsigned long long*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<unsigned long long*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< unsigned long long* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<unsigned long long*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<unsigned long long*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<unsigned long long*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<unsigned long long*>::type);                                                               
                    Data->native_hash_code = typeid(unsigned long long*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const unsigned long long*", typeid(const unsigned long long*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const unsigned long long*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const unsigned long long*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const unsigned long long*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const unsigned long long* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const unsigned long long*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const unsigned long long*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const unsigned long long*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const unsigned long long*>::type);                                                               
                    Data->native_hash_code = typeid(const unsigned long long*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "bool", typeid(bool) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<bool>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<bool>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<bool>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< bool >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<bool>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<bool>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<bool>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<bool>::type);                                                               
                Data->native_hash_code = typeid(bool).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "bool*", typeid(bool*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<bool*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<bool*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<bool*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< bool* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<bool*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<bool*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<bool*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<bool*>::type);                                                               
                    Data->native_hash_code = typeid(bool*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const bool*", typeid(const bool*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const bool*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const bool*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const bool*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const bool* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const bool*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const bool*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const bool*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const bool*>::type);                                                               
                    Data->native_hash_code = typeid(const bool*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "float", typeid(float) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<float>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<float>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<float>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< float >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<float>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<float>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<float>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<float>::type);                                                               
                Data->native_hash_code = typeid(float).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "float*", typeid(float*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<float*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<float*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<float*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< float* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<float*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<float*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<float*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<float*>::type);                                                               
                    Data->native_hash_code = typeid(float*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const float*", typeid(const float*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const float*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const float*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const float*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const float* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const float*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const float*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const float*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const float*>::type);                                                               
                    Data->native_hash_code = typeid(const float*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "double", typeid(double) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<double>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<double>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<double>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< double >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<double>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<double>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<double>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<double>::type);                                                               
                Data->native_hash_code = typeid(double).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "double*", typeid(double*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<double*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<double*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<double*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< double* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<double*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<double*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<double*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<double*>::type);                                                               
                    Data->native_hash_code = typeid(double*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const double*", typeid(const double*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const double*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const double*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const double*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const double* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const double*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const double*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const double*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const double*>::type);                                                               
                    Data->native_hash_code = typeid(const double*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "long double", typeid(long double) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<long double>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<long double>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<long double>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< long double >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<long double>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<long double>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<long double>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<long double>::type);                                                               
                Data->native_hash_code = typeid(long double).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "long double*", typeid(long double*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<long double*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<long double*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<long double*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< long double* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<long double*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<long double*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<long double*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<long double*>::type);                                                               
                    Data->native_hash_code = typeid(long double*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const long double*", typeid(const long double*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const long double*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const long double*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const long double*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const long double* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const long double*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const long double*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const long double*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const long double*>::type);                                                               
                    Data->native_hash_code = typeid(const long double*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "std::string", typeid(std::string) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<std::string>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<std::string>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<std::string>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< std::string >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<std::string>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<std::string>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<std::string>::type >::type >::value;                 
                Data->isString = true;
                Data->size = sizeof(_Get_size_type<std::string>::type);                                                               
                Data->native_hash_code = typeid(std::string).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "std::string*", typeid(std::string*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<std::string*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<std::string*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<std::string*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< std::string* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<std::string*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<std::string*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<std::string*>::type >::type >::value;                 
                    Data->isString = true;
                    Data->size = sizeof(_Get_size_type<std::string*>::type);
                    Data->native_hash_code = typeid(std::string*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const std::string*", typeid(const std::string*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const std::string*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const std::string*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const std::string*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const std::string* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const std::string*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const std::string*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const std::string*>::type >::type >::value;                 
                    Data->isString = true;
                    Data->size = sizeof(_Get_size_type<const std::string*>::type);
                    Data->native_hash_code = typeid(const std::string*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "wchar_t", typeid(wchar_t) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<wchar_t>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<wchar_t>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<wchar_t>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< wchar_t >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<wchar_t>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<wchar_t>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<wchar_t>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<wchar_t>::type);                                                               
                Data->native_hash_code = typeid(wchar_t).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "wchar_t*", typeid(wchar_t*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<wchar_t*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<wchar_t*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<wchar_t*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< wchar_t* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<wchar_t*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<wchar_t*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<wchar_t*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<wchar_t*>::type);                                                               
                    Data->native_hash_code = typeid(wchar_t*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const wchar_t*", typeid(const wchar_t*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const wchar_t*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const wchar_t*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const wchar_t*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const wchar_t* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const wchar_t*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const wchar_t*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const wchar_t*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const wchar_t*>::type);                                                               
                    Data->native_hash_code = typeid(const wchar_t*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "std::wstring", typeid(std::wstring) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<std::wstring>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<std::wstring>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<std::wstring>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< std::wstring >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<std::wstring>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<std::wstring>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<std::wstring>::type >::type >::value;                 
                Data->isString = true;
                Data->size = sizeof(_Get_size_type<std::wstring>::type);
                Data->native_hash_code = typeid(std::wstring).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "std::wstring*", typeid(std::wstring*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<std::wstring*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<std::wstring*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<std::wstring*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< std::wstring* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<std::wstring*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<std::wstring*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<std::wstring*>::type >::type >::value;                 
                    Data->isString = true;
                    Data->size = sizeof(_Get_size_type<std::wstring*>::type);
                    Data->native_hash_code = typeid(std::wstring*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const std::wstring*", typeid(const std::wstring*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const std::wstring*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const std::wstring*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const std::wstring*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const std::wstring* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const std::wstring*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const std::wstring*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const std::wstring*>::type >::type >::value;                 
                    Data->isString = true;
                    Data->size = sizeof(_Get_size_type<const std::wstring*>::type);
                    Data->native_hash_code = typeid(const std::wstring*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "System::Reflection::meta::Object", typeid(System::Reflection::meta::Object) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<System::Reflection::meta::Object>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<System::Reflection::meta::Object>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<System::Reflection::meta::Object>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< System::Reflection::meta::Object >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<System::Reflection::meta::Object>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<System::Reflection::meta::Object>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<System::Reflection::meta::Object>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<System::Reflection::meta::Object>::type);                                                               
                Data->native_hash_code = typeid(System::Reflection::meta::Object).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "System::Reflection::meta::Object*", typeid(System::Reflection::meta::Object*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<System::Reflection::meta::Object*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<System::Reflection::meta::Object*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<System::Reflection::meta::Object*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< System::Reflection::meta::Object* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<System::Reflection::meta::Object*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<System::Reflection::meta::Object*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<System::Reflection::meta::Object*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<System::Reflection::meta::Object*>::type);                                                               
                    Data->native_hash_code = typeid(System::Reflection::meta::Object*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const System::Reflection::meta::Object*", typeid(const System::Reflection::meta::Object*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const System::Reflection::meta::Object*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const System::Reflection::meta::Object*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const System::Reflection::meta::Object*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const System::Reflection::meta::Object* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const System::Reflection::meta::Object*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const System::Reflection::meta::Object*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const System::Reflection::meta::Object*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const System::Reflection::meta::Object*>::type);                                                               
                    Data->native_hash_code = typeid(const System::Reflection::meta::Object*).hash_code();                                                             
                }
            }
        }
    }
    
    {
        { // Base Type
            m::TypeID id = db.AllocateType( "System::Reflection::meta::ISerializable", typeid(System::Reflection::meta::ISerializable) );
            auto &type = db.types[ id ];

            if (id != System::Reflection::meta::InvalidTypeID)
            {
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                                  
                                                                                                                                   
                Data->isTrivialConstruct = std::is_trivially_default_constructible<System::Reflection::meta::ISerializable>::value;                               
                Data->isClass = std::is_class< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable>::type >::type >::value;                   
                Data->isEnum = std::is_enum< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable>::type >::type >::value;                     
                Data->isPointer = std::is_pointer< System::Reflection::meta::ISerializable >::value;                                                              
                Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable>::type >::type >::value;          
                Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable>::type >::type >::value;  
                Data->isSigned = std::is_signed< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable>::type >::type >::value;                 
                Data->size = sizeof(_Get_size_type<System::Reflection::meta::ISerializable>::type);                                                               
                Data->native_hash_code = typeid(System::Reflection::meta::ISerializable).hash_code();                                                             
            }
            { // Pointer Type
                m::TypeID uid = db.AllocateType( "System::Reflection::meta::ISerializable*", typeid(System::Reflection::meta::ISerializable*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<System::Reflection::meta::ISerializable*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< System::Reflection::meta::ISerializable* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<System::Reflection::meta::ISerializable*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<System::Reflection::meta::ISerializable*>::type);                                                               
                    Data->native_hash_code = typeid(System::Reflection::meta::ISerializable*).hash_code();                                                             
                }
            }
            { // Const Pointer Type
                m::TypeID uid = db.AllocateType( "const System::Reflection::meta::ISerializable*", typeid(const System::Reflection::meta::ISerializable*) );
                auto &_type = db.types[ uid ];

                if (uid != System::Reflection::meta::InvalidTypeID)
                {
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                  
                                                                                                                                       
                    Data->isTrivialConstruct = std::is_trivially_default_constructible<const System::Reflection::meta::ISerializable*>::value;                               
                    Data->isClass = std::is_class< std::remove_pointer< std::decay<const System::Reflection::meta::ISerializable*>::type >::type >::value;                   
                    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<const System::Reflection::meta::ISerializable*>::type >::type >::value;                     
                    Data->isPointer = std::is_pointer< const System::Reflection::meta::ISerializable* >::value;                                                              
                    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<const System::Reflection::meta::ISerializable*>::type >::type >::value;          
                    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<const System::Reflection::meta::ISerializable*>::type >::type >::value;  
                    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<const System::Reflection::meta::ISerializable*>::type >::type >::value;                 
                    Data->size = sizeof(_Get_size_type<const System::Reflection::meta::ISerializable*>::type);                                                               
                    Data->native_hash_code = typeid(const System::Reflection::meta::ISerializable*).hash_code();                                                             
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Enum Allocation
    ///////////////////////////////////////////////////////////////////////////
    
}

// Virtual Base Helpers (return adjuter to Refl Object)
intptr_t System::Reflection::Assembly::AssemblyModulDefault::offset_adjustment_Object_impl(size_t classId, m::Variant& from)
{
    switch (classId)
    {
        default:
        case 0: return 0;
        // Properties
        case 63:
        {
            return reinterpret_cast<intptr_t>(static_cast<m::Object*>(from.ToObject())) - reinterpret_cast<intptr_t>(dynamic_cast<System::Reflection::meta::Object*>(from.ToObject()));
        }case 75:
        {
            return reinterpret_cast<intptr_t>(static_cast<m::Object*>(from.ToObject())) - reinterpret_cast<intptr_t>(dynamic_cast<System::Reflection::meta::ISerializable*>(from.ToObject()));
        }
    }
}

// Field Explicit Setter Invokers
void System::Reflection::Assembly::AssemblyModulDefault::field_Setter_Object_impl(size_t fieldId, m::Variant&& obj, m::Variant&& value)
{
    switch (fieldId)
    {
        default:
        case 0: return;
        // Properties
        //case 68:
        //{
        //    ((std::remove_const<const System::Reflection::meta::Type *>::type&)(static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->m_type) = *static_cast<std::remove_reference<const System::Reflection::meta::Type *>::type*>( value.getPtr() );
        //    break;
        //}
    }
}
    //staticField
// Field Explicit Getter Invokers
m::Variant System::Reflection::Assembly::AssemblyModulDefault::field_Getter_Object_impl(size_t fieldId, m::Variant&& obj)
{
    switch (fieldId)
    {
        default:
        case 0: return {};
        // Properties
        //case 68:
        //{
        //    return m::Variant((static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->m_type, ReferenceVariantPolicy);
        //}
    }
}

// FieldDesc infos
m::FieldDesc const& System::Reflection::Assembly::AssemblyModulDefault::fieldinfo_Object_impl(size_t fieldId)
{
    switch (fieldId)
    {
        //case 68:
        //{
        //    static m::FieldDesc f( 
        //        68,
        //        System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
        //        System::Reflection::type_of(typeid(const System::Reflection::meta::Type *)),
        //        m::FieldDesc::FieldType::FIELD,
        //        ((size_t)&reinterpret_cast<char const volatile&>((((System::Reflection::meta::Object*)0)->m_type)))
        //    );
        //    return f;
        //}
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
m::Variant System::Reflection::Assembly::AssemblyModulDefault::invoker_Object_impl(size_t methodId, m::Variant&& obj, const m::ArgumentList&& arguments)
{
    switch (methodId)
    {
        default:
        case 0: return {};
        
        case 5:
        {
            return m::Variant(m::ArrayWrapper(new Array<char>()));
        }
        case 6:
        {
            return m::Variant(new char());
        }case 9:
        {
            return m::Variant(m::ArrayWrapper(new Array<unsigned char>()));
        }
        case 10:
        {
            return m::Variant(new unsigned char());
        }case 13:
        {
            return m::Variant(m::ArrayWrapper(new Array<short>()));
        }
        case 14:
        {
            return m::Variant(new short());
        }case 17:
        {
            return m::Variant(m::ArrayWrapper(new Array<unsigned short>()));
        }
        case 18:
        {
            return m::Variant(new unsigned short());
        }case 21:
        {
            return m::Variant(m::ArrayWrapper(new Array<int>()));
        }
        case 22:
        {
            return m::Variant(new int());
        }case 25:
        {
            return m::Variant(m::ArrayWrapper(new Array<unsigned int>()));
        }
        case 26:
        {
            return m::Variant(new unsigned int());
        }case 29:
        {
            return m::Variant(m::ArrayWrapper(new Array<long long>()));
        }
        case 30:
        {
            return m::Variant(new long long());
        }case 33:
        {
            return m::Variant(m::ArrayWrapper(new Array<unsigned long long>()));
        }
        case 34:
        {
            return m::Variant(new unsigned long long());
        }case 37:
        {
            return m::Variant(m::ArrayWrapper(new Array<bool>()));
        }
        case 38:
        {
            return m::Variant(new bool());
        }case 41:
        {
            return m::Variant(m::ArrayWrapper(new Array<float>()));
        }
        case 42:
        {
            return m::Variant(new float());
        }case 45:
        {
            return m::Variant(m::ArrayWrapper(new Array<double>()));
        }
        case 46:
        {
            return m::Variant(new double());
        }case 49:
        {
            return m::Variant(m::ArrayWrapper(new Array<long double>()));
        }
        case 50:
        {
            return m::Variant(new long double());
        }case 53:
        {
            return m::Variant(m::ArrayWrapper(new Array<std::string>()));
        }
        case 54:
        {
            return m::Variant(new std::string());
        }case 57:
        {
            return m::Variant(m::ArrayWrapper(new Array<wchar_t>()));
        }
        case 58:
        {
            return m::Variant(new wchar_t());
        }case 61:
        {
            return m::Variant(m::ArrayWrapper(new Array<std::wstring>()));
        }
        case 62:
        {
            return m::Variant(new std::wstring());
        }case 69:
        {
            
            return m::Variant(((static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->System::Reflection::meta::Object::GetType)());
            
        }
        case 70:
        {
            
            return m::Variant(((static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->System::Reflection::meta::Object::GetHashCode)());
            
        }
        case 71:
        {
            
            return m::Variant(((static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->System::Reflection::meta::Object::Clone)());
            
        }
        case 72:
        {
            
            return m::Variant(((static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->System::Reflection::meta::Object::ToString)());
            
        }
        case 73:
        {
            
            ((static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->System::Reflection::meta::Object::OnSerialize)(*static_cast<std::remove_reference<json11::Json::object &>::type*>(arguments[0]->GetPtr()));
            return {};
            
        }
        case 74:
        {
            
            ((static_cast<System::Reflection::meta::Object*>(obj.ToObject()))->System::Reflection::meta::Object::OnDeserialize)(*static_cast<std::remove_reference<const json11::Json &>::type*>(arguments[0]->GetPtr()));
            return {};
            
        }
        case 67:
        {
            return m::Variant(new System::Reflection::meta::Object());
        }
        
        case 64:
        {
            (((System::Reflection::meta::Object*)*(System::Reflection::meta::Object**)(obj.toPointer())))->~Object();
            return {};
        }
        case 66:
        {
            return m::Variant(new System::Reflection::meta::Object());
        }case 80:
        {
            
            return m::Variant(((static_cast<System::Reflection::meta::ISerializable*>(obj.ToObject()))->System::Reflection::meta::ISerializable::Clone)());
            
        }
        case 81:
        {
            
            ((static_cast<System::Reflection::meta::ISerializable*>(obj.ToObject()))->System::Reflection::meta::ISerializable::OnSerialize)(*static_cast<std::remove_reference<json11::Json::object &>::type*>(arguments[0]->GetPtr()));
            return {};
            
        }
        case 82:
        {
            
            ((static_cast<System::Reflection::meta::ISerializable*>(obj.ToObject()))->System::Reflection::meta::ISerializable::OnDeserialize)(*static_cast<std::remove_reference<const json11::Json &>::type*>(arguments[0]->GetPtr()));
            return {};
            
        }
        case 79:
        {
            return m::Variant(new System::Reflection::meta::ISerializable());
        }
        
        case 76:
        {
            (((System::Reflection::meta::ISerializable*)*(System::Reflection::meta::ISerializable**)(obj.toPointer())))->~ISerializable();
            return {};
        }
        case 78:
        {
            return m::Variant(new System::Reflection::meta::ISerializable());
        }
    }
}
    
// Methods infos
m::MethodDesc const& System::Reflection::Assembly::AssemblyModulDefault::methodinfo_Object_impl(size_t methodId)
{
    switch (methodId)
    {
        case 5:
        {
            static m::MethodDesc m( 
                5,
                System::Reflection::type_of(typeid(char)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 6:
        {
            static m::MethodDesc m( 
                6,
                System::Reflection::type_of(typeid(char)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 9:
        {
            static m::MethodDesc m( 
                9,
                System::Reflection::type_of(typeid(unsigned char)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 10:
        {
            static m::MethodDesc m( 
                10,
                System::Reflection::type_of(typeid(unsigned char)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 13:
        {
            static m::MethodDesc m( 
                13,
                System::Reflection::type_of(typeid(short)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 14:
        {
            static m::MethodDesc m( 
                14,
                System::Reflection::type_of(typeid(short)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 17:
        {
            static m::MethodDesc m( 
                17,
                System::Reflection::type_of(typeid(unsigned short)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 18:
        {
            static m::MethodDesc m( 
                18,
                System::Reflection::type_of(typeid(unsigned short)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 21:
        {
            static m::MethodDesc m( 
                21,
                System::Reflection::type_of(typeid(int)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 22:
        {
            static m::MethodDesc m( 
                22,
                System::Reflection::type_of(typeid(int)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 25:
        {
            static m::MethodDesc m( 
                25,
                System::Reflection::type_of(typeid(unsigned int)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 26:
        {
            static m::MethodDesc m( 
                26,
                System::Reflection::type_of(typeid(unsigned int)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 29:
        {
            static m::MethodDesc m( 
                29,
                System::Reflection::type_of(typeid(long long)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 30:
        {
            static m::MethodDesc m( 
                30,
                System::Reflection::type_of(typeid(long long)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 33:
        {
            static m::MethodDesc m( 
                33,
                System::Reflection::type_of(typeid(unsigned long long)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 34:
        {
            static m::MethodDesc m( 
                34,
                System::Reflection::type_of(typeid(unsigned long long)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 37:
        {
            static m::MethodDesc m( 
                37,
                System::Reflection::type_of(typeid(bool)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 38:
        {
            static m::MethodDesc m( 
                38,
                System::Reflection::type_of(typeid(bool)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 41:
        {
            static m::MethodDesc m( 
                41,
                System::Reflection::type_of(typeid(float)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 42:
        {
            static m::MethodDesc m( 
                42,
                System::Reflection::type_of(typeid(float)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 45:
        {
            static m::MethodDesc m( 
                45,
                System::Reflection::type_of(typeid(double)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 46:
        {
            static m::MethodDesc m( 
                46,
                System::Reflection::type_of(typeid(double)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 49:
        {
            static m::MethodDesc m( 
                49,
                System::Reflection::type_of(typeid(long double)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 50:
        {
            static m::MethodDesc m( 
                50,
                System::Reflection::type_of(typeid(long double)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 53:
        {
            static m::MethodDesc m( 
                53,
                System::Reflection::type_of(typeid(std::string)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 54:
        {
            static m::MethodDesc m( 
                54,
                System::Reflection::type_of(typeid(std::string)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 57:
        {
            static m::MethodDesc m( 
                57,
                System::Reflection::type_of(typeid(wchar_t)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 58:
        {
            static m::MethodDesc m( 
                58,
                System::Reflection::type_of(typeid(wchar_t)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 61:
        {
            static m::MethodDesc m( 
                61,
                System::Reflection::type_of(typeid(std::wstring)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::ARRAY_CONSTRUCTOR,
                { });

            return m;
        }
        case 62:
        {
            static m::MethodDesc m( 
                62,
                System::Reflection::type_of(typeid(std::wstring)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 69:
        {
            static m::MethodDesc m( 
                69,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                System::Reflection::type_of(typeid(const System::Reflection::meta::Type *)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                {  });

            return m;
        }
        case 70:
        {
            static m::MethodDesc m( 
                70,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                System::Reflection::type_of(typeid(unsigned int)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                {  });

            return m;
        }
        case 71:
        {
            static m::MethodDesc m( 
                71,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                System::Reflection::type_of(typeid(System::Reflection::meta::Object *)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                {  });

            return m;
        }
        case 72:
        {
            static m::MethodDesc m( 
                72,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                System::Reflection::type_of(typeid(int)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                {  });

            return m;
        }
        case 73:
        {
            static m::MethodDesc m( 
                73,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                System::Reflection::type_of(typeid(void)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                { typeid(json11::Json::object &).hash_code() });

            return m;
        }
        case 74:
        {
            static m::MethodDesc m( 
                74,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                System::Reflection::type_of(typeid(void)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                { typeid(const json11::Json &).hash_code() });

            return m;
        }
        case 67:
        {
            static m::MethodDesc m( 
                67,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::DYNAMIC_CONSTRUCTOR,
                {  });

            return m;
        }
        
        case 64:
        {
            static m::MethodDesc m( 
                64,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::DESTRUCTOR,
                { });

            return m;
        }
        case 66:
        {
            static m::MethodDesc m( 
                66,
                System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::CONSTRUCTOR,
                { });

            return m;
        }case 80:
        {
            static m::MethodDesc m( 
                80,
                System::Reflection::type_of(typeid(System::Reflection::meta::ISerializable)),
                System::Reflection::type_of(typeid(System::Reflection::meta::Object *)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                {  });

            return m;
        }
        case 81:
        {
            static m::MethodDesc m( 
                81,
                System::Reflection::type_of(typeid(System::Reflection::meta::ISerializable)),
                System::Reflection::type_of(typeid(void)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                { typeid(int &).hash_code() });

            return m;
        }
        case 82:
        {
            static m::MethodDesc m( 
                82,
                System::Reflection::type_of(typeid(System::Reflection::meta::ISerializable)),
                System::Reflection::type_of(typeid(void)),
                m::MethodDesc::MethodType::CLASS_METHOD,
                { typeid(const json11::Json &).hash_code() });

            return m;
        }
        case 79:
        {
            static m::MethodDesc m( 
                79,
                System::Reflection::type_of(typeid(System::Reflection::meta::ISerializable)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::DYNAMIC_CONSTRUCTOR,
                {  });

            return m;
        }
        
        case 76:
        {
            static m::MethodDesc m( 
                76,
                System::Reflection::type_of(typeid(System::Reflection::meta::ISerializable)),
                m::Type::Invalid( ),
                m::MethodDesc::MethodType::DESTRUCTOR,
                { });

            return m;
        }
        case 78:
        {
            static m::MethodDesc m( 
                78,
                System::Reflection::type_of(typeid(System::Reflection::meta::ISerializable)),
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

void System::Reflection::Assembly::AssemblyModulDefault::DefineModuleFileObject(m::ReflectionDatabase &db)
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

    {
        auto typeID = System::Reflection::typeid_of(typeid(System::Reflection::meta::Variant));

        if (typeID != m::InvalidTypeID && !db.types[typeID]->isDefined)
        {
            auto &type = db.types[typeID];
            type->internalId = 50;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {

            };
            type->name = "Variant";

            type->nameSpace = "System::Reflection::meta";

            // Destructor

            // Dynamic Constructors

            // Properties

            // Fields

            // Static Fields

            // Methods

            // Static Methods

            type->isDefined = true;
        }
    }

    {
        auto typeID = System::Reflection::typeid_of(typeid(System::Reflection::meta::Type));

        if (typeID != m::InvalidTypeID && !db.types[typeID]->isDefined)
        {
            auto &type = db.types[typeID];
            type->internalId = 51;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {

            };
            type->name = "Type";

            type->nameSpace = "System::Reflection::meta";

            // Destructor

            // Dynamic Constructors

            // Properties

            // Fields

            // Static Fields

            // Methods

            // Static Methods

            type->isDefined = true;
        }
    }

    /**
     * void
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( void ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 1;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "void";
            
            type->nameSpace = "";
            
            
            
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 2 );
            
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * char
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( char ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 3;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "char";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 5 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 4 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 6, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * unsigned char
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( unsigned char ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 7;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "unsigned char";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 9 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 8 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 10, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * short
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( short ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 11;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "short";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 13 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 12 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 14, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * unsigned short
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( unsigned short ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 15;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "unsigned short";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 17 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 16 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 18, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * int
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( int ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 19;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "int";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 21 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 20 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 22, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * unsigned int
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( unsigned int ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 23;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "unsigned int";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 25 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 24 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 26, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * long long
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( long long ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 27;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "long long";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 29 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 28 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 30, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * unsigned long long
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( unsigned long long ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 31;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "unsigned long long";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 33 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 32 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 34, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * bool
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( bool ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 35;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "bool";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 37 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 36 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 38, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * float
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( float ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 39;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "float";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 41 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 40 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 42, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * double
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( double ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 43;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "double";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 45 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 44 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 46, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * long double
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( long double ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 47;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "long double";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 49 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 48 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 50, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * std::string
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( std::string ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 51;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "std::string";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 53 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 52 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 54, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * wchar_t
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( wchar_t ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 55;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "wchar_t";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 57 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 56 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 58, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * std::wstring
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( std::wstring ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 59;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "std::wstring";
            
            type->nameSpace = "";
            
            
            // Array Constructor
            type->SetArrayConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 61 );
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 60 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 62, { } );
            // Dynamic Constructors
            
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * System::Reflection::meta::Object
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( System::Reflection::meta::Object ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 63;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "Object";
            
            type->nameSpace = "System::Reflection::meta";
            
            
            
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 64 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 66, { } );
            // Dynamic Constructors
            
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 67, {
                    
                }, m::CXXAccessSpecifier(1)
            );
            // Properties
            
            // Fields
            
            //type->AddField( "m_type", typeid(const System::Reflection::meta::Type *).hash_code(), 68, System::Reflection::type_of(typeid(const System::Reflection::meta::Type *)), System::Reflection::type_of(typeid(System::Reflection::meta::Object)),
            //        AssemblyModulDefault::field_Getter_Object_impl,
            //        AssemblyModulDefault::field_Setter_Object_impl,
            //        &AssemblyModulDefault::fieldinfo_Object_impl, {
            //        
            //    }, m::CXXAccessSpecifier(3)
            //);
            // Static Fields
            
            // Methods
            
            type->AddMethod( "GetType", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 69, {
                    
                }, m::CXXAccessSpecifier(1) );
            type->AddMethod( "GetHashCode", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 70, {
                    
                }, m::CXXAccessSpecifier(1) );
            type->AddMethod( "Clone", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 71, {
                    
                }, m::CXXAccessSpecifier(1) );
            type->AddMethod( "ToString", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 72, {
                    
                }, m::CXXAccessSpecifier(1) );
            type->AddMethod( "OnSerialize", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 73, {
                    
                }, m::CXXAccessSpecifier(1) );
            type->AddMethod( "OnDeserialize", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 74, {
                    
                }, m::CXXAccessSpecifier(1) );
            // Static Methods
            
            type->isDefined = true;
        }
    }
    /**
     * System::Reflection::meta::ISerializable
     */
    {
        auto typeID = System::Reflection::typeid_of(typeid( System::Reflection::meta::ISerializable ));

        if (typeID != m::InvalidTypeID && !db.types[ typeID ]->isDefined)
        {
            auto &type = db.types[ typeID ];
            type->internalId = 75;
            type->__class_object_adjuter_impl = &AssemblyModulDefault::offset_adjustment_Object_impl;
            type->metaData = {
                
            };
            type->name = "ISerializable";
            
            type->nameSpace = "System::Reflection::meta";
            
            {
                m::TypeData::AttachedClass aclass;
                aclass.m_type = &typeof( System::Reflection::meta::Object );
                aclass.m_nativeDescriptor = std::type_index( typeid( System::Reflection::meta::Object ) );
                aclass.m_isVirtual = 0;
                aclass.m_accessLevel = m::CXXAccessSpecifier(0);
                type->AddBaseClasses(db, typeID, aclass);
            }
            
            
            
            // Destructor
            type->SetDestructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 76 );
            // Constructors
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 78, { } );
            // Dynamic Constructors
            
            type->AddConstructor( &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 79, {
                    
                }, m::CXXAccessSpecifier(1)
            );
            // Properties
            
            // Fields
            
            // Static Fields
            
            // Methods
            
            type->AddMethod( "Clone", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 80, {
                    
                }, m::CXXAccessSpecifier(1) );
            type->AddMethod( "OnSerialize", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 81, {
                    
                }, m::CXXAccessSpecifier(1) );
            type->AddMethod( "OnDeserialize", &AssemblyModulDefault::invoker_Object_impl, &AssemblyModulDefault::methodinfo_Object_impl, 82, {
                    
                }, m::CXXAccessSpecifier(1) );
            // Static Methods
            
            type->isDefined = true;
        }
    }
}
