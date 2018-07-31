// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_NULL_SHADER_H_INCLUDED__
#define __C_VIDEO_NULL_SHADER_H_INCLUDED__

#include "IVideoDriver.h"

namespace irr
{
    namespace video
    {
        class IHardwareBuffer;
        struct IShader;
        struct VertexDeclaration;

        enum E_ShaderTypes
        {
            EST_VERTEX_SHADER,
            EST_FRAGMENT_SHADER,
            EST_GEOMETRY_SHADER,
            EST_HULL_SHADER,
            EST_DOMAIN_SHADER,
            EST_COMPUTE_SHADER,
            EST_TESSELLATION_SHADER,
            EST_HIGH_LEVEL_SHADER,
        };

        enum E_ShaderVersion
        {
            ESV_GLSL_ASM,
            ESV_GLSL_HIGH_LEVEL,
            ESV_HLSL_HIGH_LEVEL
        };

#define INVALID_UNIFORM_LOCATION 0xffffffff
#define INVALID_OGL_VALUE 0xffffffff

        enum E_ShaderVariableType
        {
            ESVT_UNKNOWN,
            ESVT_ATTRIBUTE,
            ESVT_UNIFORM,
            ESVT_CONSTANT,
            ESVT_INPUT_STREAM
        };

        struct ShaderVariableDescriptor
        {
            ShaderVariableDescriptor()
            {
                m_location = 0;
                m_size = 0;
                m_length = 0;
                m_type = 0;
                m_class = 0;
                m_divisor = -1;
                m_shaderIndex = 0;
                m_variableType = 0;
                //m_basicVariableLocation = EGVAT_NONE;
                //m_name = "<unknow>";
            }

            s32 m_location;
            u16 m_class;
            u16 m_size;
            u16 m_length;
            u8 m_type;
            u8 m_shaderIndex;
            u32 m_variableType;
            s32 m_divisor;
            //E_VERTEX_ELEMENT_SEMANTIC m_basicVariableLocation : 8;
            std::string m_name;
        };

        struct IShaderDataBufferElement
        {
            struct ShaderDataBufferElementExepction : public std::exception
            {
                ShaderDataBufferElementExepction(const char* message) : std::exception(message)
                {

                }
            };

            ShaderVariableDescriptor const* m_description;

            explicit IShaderDataBufferElement(ShaderVariableDescriptor const* description) : m_description(description)
            {
            }

            virtual ~IShaderDataBufferElement()
            {

            }

            ShaderVariableDescriptor const* getDescription() const { return m_description; }
            virtual u16 getShaderLocation() const { return m_description->m_location; }
            virtual void* getShaderValues() const = 0;
            virtual u32 getShaderValueCount() const = 0;
            virtual size_t getValueSizeOf() const = 0;
            virtual bool isChanged() const = 0;
            virtual void setDirty() = 0;
            virtual void setUpdated() = 0;
        };

        template<typename T>
        struct ShaderDataBufferElementObject : public IShaderDataBufferElement
        {
            T m_values;
            //u8* m_memoryBlock;

            bool m_changed : 1;

            explicit ShaderDataBufferElementObject(ShaderVariableDescriptor const* description) : IShaderDataBufferElement(description), m_changed(true), m_values(T())
            {
                //m_memoryBlock = &description->DataBuffer[constantDecl.offset];
            }

            virtual bool isChanged() const { return m_changed; }
            virtual void setDirty() { m_changed = true; }
            virtual void setUpdated() { m_changed = false; }

            void setShaderValues(T const& data)
            {
                if (m_values != data)
                {
                    m_values = data;
                    m_changed = true;
                }
            }

            ShaderDataBufferElementObject<T>* operator=(T const& data)
            {
                setShaderValues(data);
                return (ShaderDataBufferElementObject<T>*)this;
            }

            //T& GetValueRef() { return (T&)m_memoryBlock; }
            virtual void* getShaderValues() const { return (void*)&m_values; }
            virtual u32 getShaderValueCount() const { return 1; }
            virtual size_t getValueSizeOf() const { return sizeof(T); }
        };

        template<typename T>
        struct ShaderDataBufferElementArray : public IShaderDataBufferElement
        {
            std::vector<T> m_values;
            bool m_changed : 1;

            explicit ShaderDataBufferElementArray(ShaderVariableDescriptor const* description) : IShaderDataBufferElement(description), m_changed(true)
            {
            }

            // TODO: implement later or set manual
            virtual bool isChanged() const { return m_changed; }
            virtual void setDirty() { m_changed = true; }
            virtual void setUpdated() { m_changed = false; }

            void ReAllocBuffer(int size)
            {
                m_values.reserve(size);
            }

            void ResetShaderValues()
            {
                m_changed = true;
                m_values.resize(0);
            }

            void PushShaderValues(T const& data)
            {
                m_changed = true;
                m_values.push_back(data);
            }

            void MoveShaderValue(T const&& data)
            {
                m_changed = true;
                m_values.push_back(data);
            }

            T& Emplace()
            {
                m_changed = true;
                m_values.emplace_back();
                return m_values.back();
            }

            ShaderDataBufferElementObject<T>* operator+=(T const& data)
            {
                PushShaderValues(data);
                return (ShaderDataBufferElementObject<T>*)this;
            }

            virtual void* getShaderValues() const { return (void*)m_values.data(); }
            virtual u32 getShaderValueCount() const { return m_values.size(); }
            virtual size_t getValueSizeOf() const { return sizeof(T); }
        };

        struct IRRLICHT_API IShaderDataBuffer
        {
            enum E_UPDATE_TYPE
            {
                EUT_PER_FRAME,
                EUT_PER_FRAME_PER_MESH,
                EUT_PER_FRAME_PER_MATERIAL,
                EUT_MAX_VALUE,
            };

            enum E_UPDATE_FLAGS
            {
                EUF_COMMIT_VALUES = 0x0000001,
                EUF_CUSTOM_INPUT_VALUES = 0x0000002,
            };

            typedef core::array<IShaderDataBufferElement*> BufferDataArray;

            BufferDataArray m_bufferDataArray;
            u32 mChangedID;
            E_UPDATE_TYPE m_updateType : 8;

            explicit IShaderDataBuffer(IShaderDataBuffer::E_UPDATE_TYPE updateType) : mChangedID(1), m_updateType(updateType)
            {

            }

            virtual ~IShaderDataBuffer()
            {
                Invalidate();
            }

            void Invalidate()
            {
                for (u32 i = 0; i != m_bufferDataArray.size(); ++i)
                    delete m_bufferDataArray[i];
                m_bufferDataArray.clear();
            }

            void AddBufferElement(IShaderDataBufferElement* databuf)
            {
                m_bufferDataArray.push_back(databuf);
            }

            bool isChanged() const
            {
                for (u32 i = 0; i != m_bufferDataArray.size(); ++i)
                    if (m_bufferDataArray[i]->isChanged())
                        return true;
                return false;
            }

            virtual void Clear() {}
            virtual bool AddDataFromSceneNode(scene::ISceneNode*, irr::scene::IMesh*) { return true; }

            E_UPDATE_TYPE getUpdateType() const { return m_updateType; }

            virtual void setDirty();

            virtual void InitializeFormShader(IShader* gpuProgram, void* Descriptor) = 0;
            virtual void CommitBuffer(IShader* gpuProgram, IHardwareBuffer* buffer = nullptr);
            virtual void UpdateBuffer(IShader* gpuProgram, irr::scene::IMeshBuffer* meshBuffer, irr::scene::IMesh* mesh, irr::scene::ISceneNode* node, u32 updateFlags) = 0;
            virtual u32 getInstanceCount() { return 0; }

            //! Get the currently used ID for identification of changes.
            /** This shouldn't be used for anything outside the VideoDriver. */
            virtual u32 getChangedID() const { return mChangedID; }
        };

        struct  IRRLICHT_API  IShader
        {
            typedef core::array<ShaderVariableDescriptor> ShaderVariableDescArray;

            core::array<IShaderDataBuffer*> mBuffers[IShaderDataBuffer::EUT_MAX_VALUE];
            video::IVideoDriver* mContext;
            u8 mContextType;
            E_ShaderTypes mShaderType;
            bool mBinded : 1;

            ShaderVariableDescArray m_shaderVariableDescArray;
            //s16 m_shaderVariableIndexTable[EGVAT_MAX_VALUE];

            explicit IShader(video::IVideoDriver* context, E_ShaderTypes type);
            virtual ~IShader();

            static bool readFile(const char* pFileName, std::string& outFile);
            u8 getContextType() const;
            E_ShaderTypes getShaderType() const { return mShaderType; }

            video::IVideoDriver* getVideoDriver() { return mContext; }

            void addDataBuffer(IShaderDataBuffer* buffer, void* Descriptor)
            {
                buffer->InitializeFormShader(this, Descriptor);
                mBuffers[buffer->getUpdateType()].push_back(buffer);
            }

            //void LinkShaderVariable(const char* name, E_VERTEX_ELEMENT_SEMANTIC basicVariableLocation = EGVAT_NONE, u32* rid = nullptr);

            virtual void AddShaderVariable(ShaderVariableDescriptor* ui)
            {
                m_shaderVariableDescArray.push_back(*ui);
            }

            virtual E_ShaderVersion getShaderVersion() const = 0;
            virtual void bind() = 0;
            virtual void unbind() = 0;
            virtual void destroy() = 0;
            virtual void Init() = 0;

            virtual void CreateAttributeExtensions(video::IHardwareBuffer* hwBuff) {}
            void UpdateValues(IShaderDataBuffer::E_UPDATE_TYPE updateType, scene::IMeshBuffer* meshBuffer, scene::IMesh* mesh, scene::ISceneNode* node, u32 updateFlags)
            {
                if (mBuffers[updateType].empty())
                    return;

                for (u32 i = 0; i != mBuffers[updateType].size(); ++i)
                    mBuffers[updateType][i]->UpdateBuffer(this, meshBuffer, mesh, node, updateFlags);

                if (updateFlags & video::IShaderDataBuffer::EUF_COMMIT_VALUES)
                    CommitValues(updateType);
            }

            void CommitValues(IShaderDataBuffer::E_UPDATE_TYPE updateType)
            {
                if (mBuffers[updateType].empty())
                    return;

                for (u32 i = 0; i != mBuffers[updateType].size(); ++i)
                    mBuffers[updateType][i]->CommitBuffer(this);
            }

            void setDirty()
            {
                for (u32 ib = 0; ib != IShaderDataBuffer::EUT_MAX_VALUE; ++ib)
                {
                    if (mBuffers[ib].empty())
                        return;

                    for (u32 i = 0; i != mBuffers[ib].size(); ++i)
                        mBuffers[ib][i]->setDirty();
                }
            }

            virtual short GetUniformLocationByCacheIdx(short id) { return -1; }
            //virtual ShaderVariableDescriptor const* GetGPUVariableDesc(E_VERTEX_ELEMENT_SEMANTIC type) const;
            virtual s32 getGPUProgramAttribLocation(E_VERTEX_ELEMENT_SEMANTIC type) const;
            virtual ShaderVariableDescriptor const* GetGPUVariableDesc(const char* name, u32* rid = nullptr) const;
            virtual int getVariableLocation(const char* name);
            virtual int getProgramParam(int param) { return -1; }
        };
    }
}

#endif // !__C_VIDEO_NULL_SHADER_H_INCLUDED__