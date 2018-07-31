#pragma once

namespace irr
{
    namespace video
    {

#define _MAX_BOUND_VERTEX_BUFFERS 16

/** Maximum number of color surfaces that can be attached to a multi render target. */
#define _MAX_MULTIPLE_RENDER_TARGETS 8
#define _FORCE_SINGLETHREADED_RENDERING 0

/** Maximum number of individual GPU queues, per type. */
#define _MAX_QUEUES_PER_TYPE 8

/** Maximum number of hardware devices usable at once. */
#define _MAX_DEVICES 2U

/** Maximum number of devices one resource can exist at the same time. */
#define _MAX_LINKED_DEVICES 2U

/** Maximum number of GPU queues that may exist at once. */
#define _MAX_UNIQUE_QUEUES _MAX_QUEUES_PER_TYPE * GpuQueueType::GQT_COUNT // Must fit within 4 bytes

/** Types of GPU queues. */
        enum GpuQueueType
        {
            /**
            * Queue used for rendering. Allows the use of draw commands, but also all commands supported by compute
            * or upload buffers.
            */
            GQT_GRAPHICS,
            /** Discrete queue used for compute operations. Allows the use of dispatch and upload commands. */
            GQT_COMPUTE,
            /** Queue used for memory transfer operations only. No rendering or compute dispatch allowed. */
            GQT_UPLOAD,
            GQT_COUNT // Keep at end
        };

        /**
        * Bits that map to a specific surface of a render target. Combine the bits to generate a mask that references
        * only specific render target surfaces.
        */
        enum RenderSurfaceMaskBits
        {
            RT_NONE = 0,
            RT_COLOR0 = 1 << 0,
            RT_COLOR1 = 1 << 1,
            RT_COLOR2 = 1 << 2,
            RT_COLOR3 = 1 << 3,
            RT_COLOR4 = 1 << 4,
            RT_COLOR5 = 1 << 5,
            RT_COLOR6 = 1 << 6,
            RT_COLOR7 = 1 << 7,
            RT_DEPTH = 1 << 30,
            RT_STENCIL = 1 << 31,
            RT_DEPTH_STENCIL = (1 << 30) | (1 << 31),
            RT_ALL = 0xFF
        };

        /** Combineable set of bits that describe a set of physical GPU's. */
        enum GpuDeviceFlags
        {
            /**
            * Use the default set of devices. This may be the primary device or multiple devices. Cannot be used together with
            * other device flags.
            */
            GDF_DEFAULT = 0,
            /** Use only the primary GPU. */
            GDF_PRIMARY = 0x01,
            /** Use the second GPU. */
            GDF_GPU2 = 0x02,
            /** Use the third GPU. */
            GDF_GPU3 = 0x04,
            /** Use the fourth GPU. */
            GDF_GPU4 = 0x08,
            /** Use the fifth GPU. */
            GDF_GPU5 = 0x10
        };
    }
}