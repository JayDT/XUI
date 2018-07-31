#ifndef __C_VIDEO_OPEN_GL_CACHE_STATE_H_INCLUDED__
#define __C_VIDEO_OPEN_GL_CACHE_STATE_H_INCLUDED__

#include "COpenGLStateCacheManagerCommon.h"
#include "COpenGLSupport.h"
#include "SMaterial.h"

#include <set>

namespace irr
{
    namespace video
    {
        class COpenGLStateCacheManager : public COpenGLStateCacheManagerCommon
        {
        protected:
            struct TextureUnitParams
            {
                TexParameteriMap mTexParameteriMap;
            };

            typedef std::unordered_map<GLuint, TextureUnitParams> TexUnitsMap;

            /* These variables are used for caching OpenGL state.
             They are cached because state changes can be quite expensive,
             which is especially important on mobile or embedded systems.
             */

             /// Stores textures currently bound to each texture stage
            std::unordered_map <GLenum, GLuint> mBoundTextures;

            struct TexGenParams
            {
                std::set<GLenum> mEnabled;
            };
            /// Stores the currently enabled texcoord generation types per texture unit
            std::unordered_map <GLenum, TexGenParams> mTextureCoordGen;

            /// A map of texture parameters for each texture unit
            TexUnitsMap mTexUnitsMap;
            /// Stores the current polygon rendering mode
            GLenum mPolygonMode;
            /// Stores the current blend source function
            GLenum mBlendFuncSource;
            /// Stores the current blend destination function
            GLenum mBlendFuncDest;
            GLenum mBlendFuncAlphaSource;
            GLenum mBlendFuncAlphaDest;

            /// Stores the last bound texture id
            GLsizei mLastBoundTexRange;
            GLuint mLastBoundTexID[MATERIAL_MAX_TEXTURES];
            GLuint mLastBoundSamplerID[MATERIAL_MAX_TEXTURES];
            GLuint mSamplers[MATERIAL_MAX_TEXTURES];

            GLenum mShadeModel;

            GLfloat mAmbient[4];
            GLfloat mDiffuse[4];
            GLfloat mSpecular[4];
            GLfloat mEmissive[4];
            GLfloat mLightAmbient[4];
            GLfloat mShininess;

            GLfloat mPointAttenuation[3];
            GLfloat mPointSize;
            GLfloat mPointSizeMin;
            GLfloat mPointSizeMax;
        public:
            COpenGLStateCacheManager(void);

            /// See GLStateCacheManager.initializeCache.
            void initializeCache();

            /** Clears all cached values
            */
            void clearCache();

            /** Bind an OpenGL buffer of any type.
             @param target The buffer target.
             @param buffer The buffer ID.
             @param force Optional parameter to force an update.
             */
            void bindGLBuffer(GLenum target, GLuint buffer, bool force = false);

            /** Delete an OpenGL buffer of any type.
             @param target The buffer target.
             @param buffer The buffer ID.
             */
            void deleteGLBuffer(GLenum target, GLuint buffer);

            /** Bind an OpenGL texture of any type.
             @param target The texture target.
             @param texture The texture ID.
             */
            bool bindGLTexture(GLuint unit, GLuint texture, bool queued = false);
            bool flushGLTextureBind(GLsizei size);
            bool bindGLSampler(GLuint unit, GLuint sampler, bool queued = false);

            GLuint getSampler(GLuint unit);

            /** Invalidates the state associated with a particular texture ID.
             @param texture The texture ID.
             */
            void invalidateStateForTexture(GLuint texture);

            /** Sets an integer parameter value per texture target.
             @param target The texture target.
             @param pname The parameter name.
             @param param The parameter value.
             */
            void setTexParameteri(GLuint unit, GLenum pname, GLint param);

            /** Activate an OpenGL texture unit.
             @param unit The texture unit to activate.
             @return Whether or not the texture unit was successfully activated.
             */
            bool activateGLTextureUnit(size_t unit);

            /** Sets the current blend equation setting.
             @param eq The blend equation to use.
             */
            void setBlendEquation(GLenum eq);

            /// Set the blend equation for RGB and alpha separately.
            void setBlendEquation(GLenum eqRGB, GLenum eqA);

            /** Sets the blending function.
             @param source The blend mode for the source.
             @param dest The blend mode for the destination
             */
            void setBlendFunc(GLenum ColorSource, GLenum ColorDest, GLenum AlphaSource, GLenum AlphaDest);

            void setShadeModel(GLenum model);

            void setLightAmbient(GLfloat r, GLfloat g, GLfloat b);

            /** Sets the current depth mask setting.
             @param mask The depth mask to use.
             */
            void setDepthMask(GLboolean mask);

            /** Gets the current depth test function.
             @return The current depth test function.
             */
            GLenum getDepthFunc(void) const { return mDepthFunc; }

            /** Sets the current depth test function.
             @param func The depth test function to use.
             */
            void setDepthFunc(GLenum func);

            /** Gets the clear depth in the range from [0..1].
             @return The current clearing depth.
             */
            GLclampf getClearDepth(void) const { return mClearDepth; }

            /** Sets the clear depth in the range from [0..1].
             @param depth The clear depth to use.
             */
            void setClearDepth(GLclampf depth);

            /** Sets the color to clear to.
             @param red The red component.
             @param green The green component.
             @param blue The blue component.
             @param alpha The alpha component.
             */
            void setClearColour(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);


            /** Sets the current colour mask.
             @param red The red component.
             @param green The green component.
             @param blue The blue component.
             @param alpha The alpha component.
             */
            void setColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

            /** Sets the stencil mask.
             @param mask The stencil mask to use
             */
            void setStencilMask(GLuint mask);

            /** Enables a piece of OpenGL functionality.
             @param flag The function to enable.
             */
            void setEnabled(GLenum flag, bool enabled);
            void setEnabled(OGLStateEntries flag, bool enabled);

            /** Gets the current polygon rendering mode, fill, wireframe, points, etc.
             @return The current polygon rendering mode.
             */
            GLenum getPolygonMode(void) const { return mPolygonMode; }

            /** Sets the current polygon rendering mode.
             @param mode The polygon mode to use.
             */
            void setPolygonMode(GLenum mode);

            /** Sets the face culling mode.
             @return The current face culling mode
             */
            GLenum getCullFace(void) const { return mCullFace; }

            /** Sets the face culling setting.
             @param face The face culling mode to use.
             */
            void setCullFace(GLenum face);

            /// Enable the specified texture coordinate generation option for the currently active texture unit
            void enableTextureCoordGen(GLenum type);
            /// Disable the specified texture coordinate generation option for the currently active texture unit
            void disableTextureCoordGen(GLenum type);

            // Set material lighting parameters
            void setMaterialAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
            void setMaterialDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
            void setMaterialEmissive(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
            void setMaterialSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
            void setMaterialShininess(GLfloat shininess);
            void setPointSize(GLfloat size);
            void setPointParameters(GLfloat* attenuation, float minSize, float maxSize);

            void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);

        };
    }
}

#endif //! __C_VIDEO_OPEN_GL_CACHE_STATE_H_INCLUDED__
