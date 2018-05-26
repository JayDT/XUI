// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MESH_H_INCLUDED__
#define __I_MESH_H_INCLUDED__

#include "IReferenceCounted.h"
#include "SMaterial.h"
#include "EHardwareBufferFlags.h"

namespace irr
{

namespace video
{
    struct SOccQuery;
}

namespace scene
{
    enum E_ANIMATED_MESH_TYPE : unsigned char;
    class IAnimatedMesh;
    class IMeshBuffer;

	//! Class which holds the geometry of an object.
	/** An IMesh is nothing more than a collection of some mesh buffers
	(IMeshBuffer). SMesh is a simple implementation of an IMesh.
	A mesh is usually added to an IMeshSceneNode in order to be rendered.
	*/
    class IRRLICHT_API IMesh : public virtual IReferenceCounted
    {
    public:

        virtual ~IMesh() {}

        //! Get the amount of mesh buffers.
        /** \return Amount of mesh buffers (IMeshBuffer) in this mesh. */
        virtual u32 getMeshBufferCount() const = 0;

        //! Get pointer to a mesh buffer.
        /** \param nr: Zero based index of the mesh buffer. The maximum value is
        getMeshBufferCount() - 1;
        \return Pointer to the mesh buffer or 0 if there is no such
        mesh buffer. */
        virtual IMeshBuffer* getMeshBuffer(u32 nr) const = 0;

        //! Get pointer to a mesh buffer which fits a material
        /** \param material: material to search for
        \return Pointer to the mesh buffer or 0 if there is no such
        mesh buffer. */
        virtual IMeshBuffer* getMeshBuffer(const video::SMaterial &material) const = 0;

        virtual u32 getOcculisionMeshCount() const { return 0; }
        virtual IMesh* getOcculisionMesh(u32 id) const { return nullptr; }

        //! Get an axis aligned bounding box of the mesh.
        /** \return Bounding box of this mesh. */
        virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;

        //! Set user-defined axis aligned bounding box
        /** \param box New bounding box to use for the mesh. */
        virtual void setBoundingBox(const core::aabbox3df& box) = 0;

        //! Recalculates the bounding box.
        virtual void recalculateBoundingBox()  {}

        //! Sets a flag of all contained materials to a new value.
        /** \param flag: Flag to set in all materials.
        \param newvalue: New value to set in all materials. */
        virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue) = 0;

        //! Set the hardware mapping hint
        /** This methods allows to define optimization hints for the
        hardware. This enables, e.g., the use of hardware buffers on
        pltforms that support this feature. This can lead to noticeable
        performance gains. */
        virtual void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) = 0;

        //! Flag the meshbuffer as changed, reloads hardware buffers
        /** This method has to be called every time the vertices or
        indices have changed. Otherwise, changes won't be updated
        on the GPU in the next render cycle. */
        virtual void setDirty(E_BUFFER_TYPE buffer = EBT_VERTEX_AND_INDEX) = 0;

        //! updates the bounding box
        virtual void updateBoundingBox(void) {}

        virtual E_ANIMATED_MESH_TYPE getMeshTypeID() const { return (E_ANIMATED_MESH_TYPE)-1; }

        virtual IAnimatedMesh* toIAnimateMesh() { return nullptr; }

        virtual void setInstanceMode(bool on) {}
        virtual bool IsInstanceModeAvailable() { return true; }

        virtual const char* getName() const { return "<unknown>"; }

        virtual void CreateBuffer()
        {

        }
    };

} // end namespace scene
} // end namespace irr

#endif

