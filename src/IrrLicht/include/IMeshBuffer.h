// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MESH_BUFFER_H_INCLUDED__
#define __I_MESH_BUFFER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "SMaterial.h"
#include "aabbox3d.h"
#include "S3DVertex.h"
#include "SVertexIndex.h"
#include "EHardwareBufferFlags.h"
#include "EPrimitiveTypes.h"

#include <mutex>

namespace irr
{
    namespace video
    {
        enum E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE;
        class IHardwareBuffer;
        struct IShader;
        struct IShaderDataBuffer;
    }

    namespace video
    {
        class STextureStageCache : public ITextureRefMgr
        {
            const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
            public:
            STextureStageCache()
            {
                for ( u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i )
                {
                    CurrentTexture[i] = 0;
                }
            }

            ~STextureStageCache()
            {
                clear();
            }

            mutable std::mutex mLock;

            void set(u32 stage, const ITexture* tex)
            {
                if ( stage<MATERIAL_MAX_TEXTURES )
                {
                    mLock.lock();
                    const ITexture* oldTexture = CurrentTexture[stage];
                    if ( tex )
                        tex->grab();
                    CurrentTexture[stage] = tex;
                    if ( oldTexture )
                        oldTexture->drop();
                    mLock.unlock();
                }
            }

            const ITexture* operator[](int stage) const
            {
                std::lock_guard<std::mutex> g(mLock);
                if ( (u32)stage<MATERIAL_MAX_TEXTURES )
                    return CurrentTexture[stage];
                else
                    return 0;
            }

            void remove(const ITexture* tex)
            {
                mLock.lock();
                for ( s32 i = MATERIAL_MAX_TEXTURES - 1; i >= 0; --i )
                {
                    if ( tex && CurrentTexture[i] == tex )
                    {
                        tex->drop();
                        CurrentTexture[i] = 0;
                    }
                }
                mLock.unlock();
            }

            void clear()
            {
                mLock.lock();
                // Drop all the CurrentTexture handles
                for ( u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i )
                {
                    if ( CurrentTexture[i] )
                    {
                        CurrentTexture[i]->drop();
                        CurrentTexture[i] = 0;
                    }
                }
                mLock.unlock();
            }
        };
    }

namespace scene
{
	//! Struct for holding a mesh with a single material.
	/** A part of an IMesh which has the same material on each face of that
	group. Logical groups of an IMesh need not be put into separate mesh
	buffers, but can be. Separately animated parts of the mesh must be put
	into separate mesh buffers.
	Some mesh buffer implementations have limitations on the number of
	vertices the buffer can hold. In that case, logical grouping can help.
	Moreover, the number of vertices should be optimized for the GPU upload,
	which often depends on the type of gfx card. Typial figures are
	1000-10000 vertices per buffer.
	SMeshBuffer is a simple implementation of a MeshBuffer, which supports
	up to 65535 vertices.

	Since meshbuffers are used for drawing, and hence will be exposed
	to the driver, chances are high that they are grab()'ed from somewhere.
	It's therefore required to dynamically allocate meshbuffers which are
	passed to a video driver and only drop the buffer once it's not used in
	the current code block anymore.
	*/
    class IRRLICHT_API IMeshBuffer : public virtual IReferenceCounted
	{
        mutable video::IHardwareBuffer* m_HWBuffer;
        scene::E_PRIMITIVE_TYPE m_renderType : 8;
        bool m_forceVBO : 1;
        bool m_3DMode : 1;
    public:

        IMeshBuffer() : m_renderType(EPT_TRIANGLES), m_HWBuffer(nullptr), m_forceVBO(false), m_3DMode(true) {}
        virtual ~IMeshBuffer() {}

		//! Get the material of this meshbuffer
		/** \return Material of this buffer. */
		virtual video::SMaterial& getMaterial() = 0;

		//! Get the material of this meshbuffer
		/** \return Material of this buffer. */
		virtual const video::SMaterial& getMaterial() const = 0;

		//! Get type of vertex data which is stored in this meshbuffer.
		/** \return Vertex type of this buffer. */
		virtual video::E_VERTEX_TYPE getVertexType() const = 0;

		//! Get access to vertex data. The data is an array of vertices.
		/** Which vertex type is used can be determined by getVertexType().
		\return Pointer to array of vertices. */
		virtual const void* getVertices() const = 0;

		//! Get access to vertex data. The data is an array of vertices.
		/** Which vertex type is used can be determined by getVertexType().
		\return Pointer to array of vertices. */
		virtual void* getVertices() = 0;

		//! Get amount of vertices in meshbuffer.
		/** \return Number of vertices in this buffer. */
		virtual u32 getVertexCount() const = 0;

		//! Get type of index data which is stored in this meshbuffer.
		/** \return Index type of this buffer. */
		virtual video::E_INDEX_TYPE getIndexType() const =0;

		//! Get access to Indices.
		/** \return Pointer to indices array. */
		virtual const u16* getIndices() const = 0;

		//! Get access to Indices.
		/** \return Pointer to indices array. */
		virtual u16* getIndices() = 0;

		//! Get amount of indices in this meshbuffer.
		/** \return Number of indices in this buffer. */
		virtual u32 getIndexCount() const = 0;

		//! Get the axis aligned bounding box of this meshbuffer.
		/** \return Axis aligned bounding box of this buffer. */
		virtual const core::aabbox3df& getBoundingBox() const = 0;

		//! Set axis aligned bounding box
		/** \param box User defined axis aligned bounding box to use
		for this buffer. */
		virtual void setBoundingBox(const core::aabbox3df& box) = 0;

		//! Recalculates the bounding box. Should be called if the mesh changed.
		virtual void recalculateBoundingBox() = 0;

		//! returns position of vertex i
		virtual const core::vector3df& getPosition(u32 i) const = 0;

		//! returns position of vertex i
		virtual core::vector3df& getPosition(u32 i) = 0;

		//! returns normal of vertex i
		virtual const core::vector3df& getNormal(u32 i) const = 0;

		//! returns normal of vertex i
		virtual core::vector3df& getNormal(u32 i) = 0;

		//! returns texture coord of vertex i
		virtual const core::vector2df& getTCoords(u32 i) const = 0;

		//! returns texture coord of vertex i
		virtual core::vector2df& getTCoords(u32 i) = 0;

        virtual video::S3DVertex& operator[](int idx) = 0;

		//! Append the vertices and indices to the current buffer
		/** Only works for compatible vertex types.
		\param vertices Pointer to a vertex array.
		\param numVertices Number of vertices in the array.
		\param indices Pointer to index array.
		\param numIndices Number of indices in array. */
		virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices) = 0;

		//! Append the meshbuffer to the current buffer
		/** Only works for compatible vertex types
		\param other Buffer to append to this one. */
		virtual void append(const IMeshBuffer* const other) = 0;

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const = 0;

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const = 0;

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint( E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX ) = 0;

		//! flags the meshbuffer as changed, reloads hardware buffers
		virtual void setDirty(E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX) = 0;

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Vertex() const = 0;

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Index() const = 0;

        virtual void setRenderPrimitive(scene::E_PRIMITIVE_TYPE pType) { m_renderType = pType; }
        virtual scene::E_PRIMITIVE_TYPE getRenderPrimitive() const { return m_renderType; }

        virtual void CreateBuffers() {}
        virtual void UpdateBuffers() {}

        virtual video::IShader* GetGPUProgram() const { return nullptr; }
        virtual void SetGPUProgram(video::IShader* gpuProgram) { }

        virtual s32 GetVertexRangeStart() const { return 0; }
        virtual s32 GetVertexRangeEnd() const { return getVertexCount(); }
        virtual s32 GetIndexRangeStart() const { return 0; }
        virtual s32 GetIndexRangeCount() const { return getIndexCount(); }

        virtual video::SMaterial& getMaterialBySubBufferId(int sid)
        {
            return getMaterial();
        }

        virtual void EnableSubBuffer(bool on, u16 sid) {}
        virtual bool IsAvailableSubBuffer(u16 sid) { return true; }
        virtual void SetActiveSubBuffer(u16 sid) {}
        virtual void AddSubBuffer(u16 istart, u16 icount, u16 vstart, u16 vcount) {}
        virtual void InitSubBuffers(u16 count) {}
        virtual u32 GetSubBufferCount() const { return 0;  }

        virtual s32 GetVertexStride() { return 0; }

        virtual core::matrix4 const& GetWorldViewMatrix()
        {
            return core::IdentityMatrix;
        }

        virtual const core::aabbox3d<f32>& getSubBoundingBox(int /*sid*/) const
        {
            return getBoundingBox();
        }

        virtual void SetWorldViewMatrix(core::matrix4 const* m) {}
        virtual void boundingBoxNeedsRecalculated(void) { }

        void SetForceVBO(bool b) { m_forceVBO = b; }
        bool IsForceVBO() const { return m_forceVBO; }

        void Set3DMode(bool b) { m_3DMode = b; }
        bool Is3DMode() const { return m_3DMode; }

        video::IHardwareBuffer* GetHWBuffer() const { return m_HWBuffer; }
        void SetHWBuffer(video::IHardwareBuffer* hwb) const { m_HWBuffer = hwb; }

        virtual video::IShaderDataBuffer* GetHWInstanceBuffer() { return nullptr; }
        virtual void SetHWInstanceBuffer(video::IShaderDataBuffer* instanceBuffer) {}
	};

} // end namespace scene
} // end namespace irr

#endif


