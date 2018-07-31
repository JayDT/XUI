// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_NULL_OCCULISION_H_INCLUDED__
#define __C_VIDEO_NULL_OCCULISION_H_INCLUDED__

#include "IVideoDriver.h"
#include "IMesh.h"
#include "ISceneNode.h"

namespace irr
{
    namespace video
    {
        class CommandBuffer;

        struct SOccQuery : public virtual IReferenceCounted
        {
            SOccQuery(bool binary)
                : mActive(false)
                , mBinary(binary)
            {
            }

            virtual ~SOccQuery()
            {
            }

            /**
            * Starts the query. Any draw calls after this call will have any rendered samples counted in the query.
            *
            * @note	Place any commands you want to measure after this call. Call end() when done.
            */
            virtual void begin(CommandBuffer* cb = nullptr) = 0;

            /**
            * Stops the query.
            *
            * @note	Be aware that queries are executed on the GPU and the results will not be immediately available.
            */
            virtual void end(CommandBuffer* cb = nullptr) = 0;

            /** Check if GPU has processed the query. */
            virtual bool isReady() const = 0;

            /**
            * Returns the number of samples that passed the depth and stencil test between query start and end.
            *
            * @note
            * If the query is binary, this will return 0 or 1. 1 meaning one or more samples were rendered, but will not give
            * you the exact count.
            */
            virtual UINT32 getNumSamples() = 0;

            /** Triggered when the query has completed. Argument provided is the number of samples counted by the query. */
            std::function<void(UINT32)> onComplete;

        protected:
            friend class QueryManager;

            /**	Returns true if the has still not been completed by the GPU. */
            bool isActive() const { return mActive; }
            void setActive(bool active) { mActive = active; }

            bool mActive;
            bool mBinary;
        };
    }
}

#endif //!__C_VIDEO_NULL_OCCULISION_H_INCLUDED__