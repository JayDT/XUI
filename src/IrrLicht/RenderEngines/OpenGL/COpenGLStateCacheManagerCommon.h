#ifndef __GLStateCacheManagerCommon_H__
#define __GLStateCacheManagerCommon_H__

#include <unordered_map>
#include <array>
#include <irrMath.h>

namespace irr
{
    namespace video
    {
        enum class OGLStateEntries : u8
        {
            OGL_TEXTURE_2D = 0,
            OGL_ALPHA_TEST,
            OGL_DEPTH_TEST,
            OGL_STENCIL_TEST,
            OGL_CULL_FACE,
            OGL_BLEND,
            OGL_POLYGON_OFFSET_LINE,
            OGL_POLYGON_OFFSET_POINT,
            OGL_POLYGON_OFFSET_FILL,
            OGL_SAMPLE_ALPHA_TO_COVERAGE,
            OGL_MULTISAMPLE_ARB,
            OGL_LINE_SMOOTH,
            OGL_POINT_SMOOTH,
            OGL_CLIP_PLANE0,
            OGL_CLIP_PLANE1,
            OGL_CLIP_PLANE2,
            OGL_CLIP_PLANE3,
            OGL_CLIP_PLANE4,
            OGL_CLIP_PLANE5,
            OGL_MAX
        };

        class COpenGLStateCacheManagerCommon
        {
        protected:
            typedef std::unordered_map<u32, u32> BindBufferMap;
            typedef std::unordered_map<u32, int> TexParameteriMap;
            typedef std::unordered_map<u32, float> TexParameterfMap;

             /// Array of each OpenGL feature that is enabled i.e. blending, depth test, etc.
            std::array<std::pair<u16/*GL State*/, bool /*enabled*/>, u8(OGLStateEntries::OGL_MAX)> mEnableVector;
            /// Stores the current clear colour
            float mClearColour[4];
            /// Stores the current depth clearing colour
            float mClearDepth;
            /// Stores the current colour write mask
            u8 mColourMask[4];
            /// Stores the current depth write mask
            u8 mDepthMask;
            /// Stores the current stencil mask
            u32 mStencilMask;
            /// Viewport origin and size
            int mViewport[4];
            /// A map of different buffer types and the currently bound buffer for each type
            BindBufferMap mActiveBufferMap;
            /// Stores the current face culling setting
            u32 mCullFace;
            /// Stores the current depth test function
            u32 mDepthFunc;
            /// Stores the current blend equation
            u32 mBlendEquationRGB;
            u32 mBlendEquationAlpha;
            /// Stores the currently active texture unit
            size_t mActiveTextureUnit;
        public:
            virtual ~COpenGLStateCacheManagerCommon() {}

            void getViewport(int* array) {
                for (int i = 0; i < 4; ++i)
                    array[i] = mViewport[i];
            }

            /** Gets the current colour mask setting.
             @return An array containing the mask in RGBA order.
             */
            u8* getColourMask() { return mColourMask; }

            /** Gets the current depth mask setting.
             @return The current depth mask.
             */
            u8 getDepthMask() const { return mDepthMask; }

            /** Gets the current stencil mask.
             @return The stencil mask.
             */
            u32 getStencilMask(void) const { return mStencilMask; }
        };
    }
}

#endif
