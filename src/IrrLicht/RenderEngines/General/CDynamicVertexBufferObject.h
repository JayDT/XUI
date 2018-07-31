#ifndef __CDYNAMIC_VERTEX_BUFFER_OBJECT_H__
#define __CDYNAMIC_VERTEX_BUFFER_OBJECT_H__

#include "IVertexBuffer.h"
#include "SMaterial.h"
#include "IMeshBuffer.h"
#include "IDynamicMeshBuffer.h"

namespace irr
{
    namespace scene
    {
        struct CDynamicVertexBufferObject;

        template <class T>
        class CDynamicVerticies : public irr::scene::IVertexBuffer
        {
        public:
            irr::core::array<T> Vertices;
            irr::scene::E_HARDWARE_MAPPING MappingHint : 8;
            uint8 ChangedID;

            explicit CDynamicVerticies() : ChangedID(1), MappingHint(irr::scene::EHM_NEVER)
            {
            }

            virtual ~CDynamicVerticies()
            {
            }

            virtual u32 stride() const { return sizeof(T); }
            virtual u32 size() const { return Vertices.size(); }

            virtual irr::video::S3DVertex* getBaseVertexPointer()
            {
                return Vertices.pointer();
            }

            virtual void push_back(const irr::video::S3DVertex &element)
            {
                Vertices.push_back((T&)element);
            }

            virtual void operator ()(const u32 index, irr::video::S3DVertex const& element) const
            {
                (T&)Vertices[index] = (T&)element;
            }

            virtual irr::video::S3DVertex& operator [](const u32 index) const
            {
                return (T&)Vertices[index];
            }

            virtual irr::video::S3DVertex& getLast()
            {
                return (irr::video::S3DVertex&)Vertices.getLast();
            }

            virtual void set_used(u32 usedNow)
            {
                Vertices.set_used(usedNow);
            }

            virtual void reallocate(u32 new_size)
            {
                Vertices.reallocate(new_size);
            }

            virtual u32 allocated_size() const
            {
                return Vertices.allocated_size();
            }

            virtual irr::video::S3DVertex* pointer() { return Vertices.pointer(); }
            virtual irr::video::E_VERTEX_TYPE getType() const 
            { 
                static T empty;
                return empty.getType();
            }

            //! get the current hardware mapping hint
            virtual irr::scene::E_HARDWARE_MAPPING getHardwareMappingHint() const
            {
                return MappingHint;
            }

            //! set the hardware mapping hint, for driver
            virtual void setHardwareMappingHint(irr::scene::E_HARDWARE_MAPPING NewMappingHint)
            {
                MappingHint = NewMappingHint;
            }

            //! flags the mesh as changed, reloads hardware buffers
            virtual void setDirty()
            {
                ++ChangedID;
            }

            //! Get the currently used ID for identification of changes.
            /** This shouldn't be used for anything outside the VideoDriver. */
            virtual u32 getChangedID() const { return ChangedID; }

            virtual void setType(irr::video::E_VERTEX_TYPE vertexType)
            {
                throw std::runtime_error("not implement yet!");
            }

            virtual void* getData() { return Vertices.pointer(); }
        };

        struct CDynamicVertexSubBuffer
        {
            CDynamicVertexSubBuffer()
            {
                m_indicesStart = 0;
                m_indicesCount = 0;
                m_vectiesStart = 0;
                m_vectiesEnd = 0;
                Material.m_textureRefMgr = &TextureRefMgr;
                m_isEnabled = true;
            }

            CDynamicVertexSubBuffer(u32 istart, u32 icount, u32 vstart, u32 vend)
            {
                m_indicesStart = istart;
                m_indicesCount = icount;
                m_vectiesStart = vstart;
                m_vectiesEnd = vend;
                Material.m_textureRefMgr = &TextureRefMgr;
                m_isEnabled = true;
            }

            ~CDynamicVertexSubBuffer()
            {
            }

            //! Recalculate bounding box
            virtual void recalculateBoundingBox(CDynamicVertexBufferObject*);

            // Draw Range Elements
            u32 m_indicesStart;
            u32 m_indicesCount;
            u32 m_vectiesStart;
            u32 m_vectiesEnd;
            bool m_isEnabled;

            irr::core::aabbox3d<float> BoundingBox;
            irr::video::SMaterial Material;
            irr::video::STextureStageCache TextureRefMgr;
        };

        struct IRRLICHT_API CDynamicVertexBufferObject : public irr::scene::IDynamicMeshBuffer
        {
            CDynamicVertexBufferObject() : m_isEnabled(true), VertexBuffer(nullptr), IndexBuffer(nullptr), m_gpuProgram(nullptr), m_gpuProgramId(-1), InstanceBuffer(nullptr), Material(new irr::video::SMaterial), ActiveSubBuffer(0), m_needBoundRecalulate(true)
            {
            }

            virtual ~CDynamicVertexBufferObject()
            {
                assert(getReferenceCount() == 0);

                if (SubBuffer.empty())
                {
                    if (Material)
                        delete Material;
                }

                for (u32 i = 0; i != SubBuffer.size(); ++i)
                    delete SubBuffer[i];

                setVertexBuffer(nullptr);
                setIndexBuffer(nullptr);
            }

            virtual irr::scene::IVertexBuffer& getVertexBuffer() const
            {
                return *VertexBuffer;
            }

            virtual irr::scene::IIndexBuffer& getIndexBuffer() const
            {
                return *IndexBuffer;
            }

            virtual void setVertexBuffer(irr::scene::IVertexBuffer *newVertexBuffer)
            {
                if (newVertexBuffer)
                    newVertexBuffer->grab();
                if (VertexBuffer)
                    VertexBuffer->drop();

                VertexBuffer = newVertexBuffer;
            }

            virtual void setIndexBuffer(irr::scene::IIndexBuffer *newIndexBuffer)
            {
                if (newIndexBuffer)
                    newIndexBuffer->grab();
                if (IndexBuffer)
                    IndexBuffer->drop();

                IndexBuffer = newIndexBuffer;
            }

            CDynamicVertexSubBuffer const* GetSubBufferDesc(int sid)
            {
                return !SubBuffer.empty() ? SubBuffer[sid] : nullptr;
            }

            //! Get Material of this buffer.
            virtual const irr::video::SMaterial& getMaterial() const
            {
                return (!SubBuffer.empty() ? SubBuffer[ActiveSubBuffer]->Material : *Material);
            }

            //! Get Material of this buffer.
            virtual irr::video::SMaterial& getMaterial()
            {
                return (!SubBuffer.empty() ? SubBuffer[ActiveSubBuffer]->Material : *Material);
            }

            //! Get Material of this buffer.
            virtual irr::video::SMaterial& getMaterialBySubBufferId(int sid) _IRR_OVERRIDE_
            {
                return (!SubBuffer.empty() ? SubBuffer[sid]->Material : *Material);
            }

            //! Get bounding box
            virtual const irr::core::aabbox3d<float>& getBoundingBox() const
            {
                return BoundingBox;
            }

            //! Set bounding box
            virtual void setBoundingBox(const irr::core::aabbox3df& box)
            {
                BoundingBox = box;
            }

            //! Get bounding box
            virtual const irr::core::aabbox3d<float>& getSubBoundingBox(int sid) const
            {
                return (!SubBuffer.empty() ? SubBuffer[sid]->BoundingBox : BoundingBox);
            }

            //! Recalculate bounding box
            virtual void recalculateBoundingBox()
            {
                if ( m_needBoundRecalulate )
                {
                    if ( !getVertexBuffer().size() )
                        BoundingBox.reset(0, 0, 0);
                    else
                    {
                        BoundingBox.reset(getVertexBuffer()[0].Pos);
                        for ( u32 i = 1; i < getVertexBuffer().size(); ++i )
                            BoundingBox.addInternalPoint(getVertexBuffer()[i].Pos);

                        for (u32 i = 0; i != SubBuffer.size(); ++i)
                            SubBuffer[i]->recalculateBoundingBox(this);
                    }
                }
            }

            virtual void CreateBuffers() {}
            virtual void UpdateBuffers() {}

            virtual void InitSubBuffers(u16 count)
            {
                if (!count)
                    return;

                if (Material)
                    delete Material;
                Material = nullptr;
                SubBuffer.reallocate(count);
            }

            virtual void AddSubBuffer(u16 istart, u16 icount, u16 vstart, u16 vcount)
            {
                SubBuffer.push_back(new CDynamicVertexSubBuffer(istart, icount, vstart, vcount));
            }

            virtual void SetActiveSubBuffer(u16 sid) 
            {
                if (SubBuffer.size() <= sid)
                    return;

                m_needBoundRecalulate = true;
                ActiveSubBuffer = sid;
                Material = &SubBuffer[sid]->Material;
            }

            virtual u32 GetSubBufferCount() const
            {
                return SubBuffer.size();
            }

            virtual s32 GetVertexRangeStart() const { return !SubBuffer.empty() ? SubBuffer[ActiveSubBuffer]->m_vectiesStart : 0; }
            virtual s32 GetVertexRangeEnd()   const { return !SubBuffer.empty() ? SubBuffer[ActiveSubBuffer]->m_vectiesEnd : getVertexCount(); }
            virtual s32 GetIndexRangeStart()  const { return !SubBuffer.empty() ? SubBuffer[ActiveSubBuffer]->m_indicesStart : 0; }
            virtual s32 GetIndexRangeCount()  const { return !SubBuffer.empty() ? SubBuffer[ActiveSubBuffer]->m_indicesCount : getIndexCount(); }

            virtual s32 GetVertexStride() { return VertexBuffer ? VertexBuffer->stride() : 0; }

            virtual irr::video::IShader* GetGPUProgram() const;
            virtual void SetGPUProgram(irr::video::IShader* gpuProgram) override;
            virtual void SetGPUProgram(irr::video::IVideoDriver* driver, s32 gpuProgramId) override
            {
                Driver = driver;
                m_gpuProgramId = gpuProgramId;
            }

            virtual irr::video::IShaderDataBuffer* GetHWInstanceBuffer() { return InstanceBuffer; }
            virtual void SetHWInstanceBuffer(irr::video::IShaderDataBuffer* instanceBuffer);

            virtual void boundingBoxNeedsRecalculated(void)
            {
                m_needBoundRecalulate = true;
            }

            irr::video::SMaterial* Material;
            irr::video::IShaderDataBuffer* InstanceBuffer;
            irr::core::aabbox3d<float> BoundingBox;

            virtual void EnableSubBuffer(bool on, u16 sid)
            {
                if (SubBuffer.size() <= sid)
                {
                    m_isEnabled = on;
                    return;
                }

                SubBuffer[sid]->m_isEnabled = on;
            }
            virtual bool IsAvailableSubBuffer(u16 sid)
            {
                if (SubBuffer.size() <= sid)
                    return m_isEnabled;

                return SubBuffer[sid]->m_isEnabled;
            }

        private:
            irr::core::array<CDynamicVertexSubBuffer*> SubBuffer;
            irr::scene::IVertexBuffer* VertexBuffer;
            irr::scene::IIndexBuffer * IndexBuffer;
            union
            {
                mutable irr::video::IShader* m_gpuProgram;
                irr::video::IVideoDriver* Driver;
            };
            s32 m_gpuProgramId;
            u16 ActiveSubBuffer;
            bool m_isEnabled : 1;
            bool m_needBoundRecalulate : 1;
        };
    }
}
#endif // __CDYNAMIC_VERTEX_BUFFER_OBJECT_H__