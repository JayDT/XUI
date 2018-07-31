#include "COpenGLStateCacheManager.h"
#include "COpenGLDriver.h"

using namespace irr;
using namespace irr::video;

COpenGLStateCacheManager::COpenGLStateCacheManager(void)
{
    clearCache();
}

void COpenGLStateCacheManager::initializeCache()
{
    glBlendEquation(GL_FUNC_ADD);

    if (GLEW_VERSION_2_0)
    {
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    }
    else if (GLEW_EXT_blend_equation_separate)
    {
        glBlendEquationSeparateEXT(GL_FUNC_ADD, GL_FUNC_ADD);
    }

    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ZERO);

    glCullFace(mCullFace);

    glDepthFunc(mDepthFunc);

    glDepthMask(mDepthMask);

    glStencilMask(mStencilMask);

    glClearDepth(mClearDepth);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

    glBindRenderbufferEXT(GL_RENDERBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);

    glClearColor(mClearColour[0], mClearColour[1], mClearColour[2], mClearColour[3]);

    glColorMask(mColourMask[0], mColourMask[1], mColourMask[2], mColourMask[3]);

    glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
}

void COpenGLStateCacheManager::clearCache()
{
    mEnableVector[u8(OGLStateEntries::OGL_TEXTURE_2D)]                  = std::make_pair(GL_TEXTURE_2D, false);
    mEnableVector[u8(OGLStateEntries::OGL_ALPHA_TEST)]                  = std::make_pair(GL_ALPHA_TEST, false);
    mEnableVector[u8(OGLStateEntries::OGL_DEPTH_TEST)]                  = std::make_pair(GL_DEPTH_TEST, false);
    mEnableVector[u8(OGLStateEntries::OGL_STENCIL_TEST)]                = std::make_pair(GL_STENCIL_TEST, false);
    mEnableVector[u8(OGLStateEntries::OGL_CULL_FACE)]                   = std::make_pair(GL_CULL_FACE, false);
    mEnableVector[u8(OGLStateEntries::OGL_BLEND)]                       = std::make_pair(GL_BLEND, false);
    mEnableVector[u8(OGLStateEntries::OGL_POLYGON_OFFSET_LINE)]         = std::make_pair(GL_POLYGON_OFFSET_LINE, false);
    mEnableVector[u8(OGLStateEntries::OGL_POLYGON_OFFSET_POINT)]        = std::make_pair(GL_POLYGON_OFFSET_POINT, false);
    mEnableVector[u8(OGLStateEntries::OGL_POLYGON_OFFSET_FILL)]         = std::make_pair(GL_POLYGON_OFFSET_FILL, false);
    mEnableVector[u8(OGLStateEntries::OGL_SAMPLE_ALPHA_TO_COVERAGE)]    = std::make_pair(GL_SAMPLE_ALPHA_TO_COVERAGE, false);
    mEnableVector[u8(OGLStateEntries::OGL_MULTISAMPLE_ARB)]             = std::make_pair(GL_MULTISAMPLE_ARB, false);
    mEnableVector[u8(OGLStateEntries::OGL_LINE_SMOOTH)]                 = std::make_pair(GL_LINE_SMOOTH, false);
    mEnableVector[u8(OGLStateEntries::OGL_POINT_SMOOTH)]                = std::make_pair(GL_POINT_SMOOTH, false);
    mEnableVector[u8(OGLStateEntries::OGL_CLIP_PLANE0)]                 = std::make_pair(GL_CLIP_PLANE0, false);
    mEnableVector[u8(OGLStateEntries::OGL_CLIP_PLANE1)]                 = std::make_pair(GL_CLIP_PLANE1, false);
    mEnableVector[u8(OGLStateEntries::OGL_CLIP_PLANE2)]                 = std::make_pair(GL_CLIP_PLANE2, false);
    mEnableVector[u8(OGLStateEntries::OGL_CLIP_PLANE3)]                 = std::make_pair(GL_CLIP_PLANE3, false);
    mEnableVector[u8(OGLStateEntries::OGL_CLIP_PLANE4)]                 = std::make_pair(GL_CLIP_PLANE4, false);
    mEnableVector[u8(OGLStateEntries::OGL_CLIP_PLANE5)]                 = std::make_pair(GL_CLIP_PLANE5, false);

    mDepthMask = GL_TRUE;
    mBlendEquationRGB = GL_FUNC_ADD;
    mBlendEquationAlpha = GL_FUNC_ADD;
    mCullFace = GL_BACK;
    mDepthFunc = GL_LESS;
    mStencilMask = 0xFFFFFFFF;
    mActiveTextureUnit = 0;
    mClearDepth = 1.0f;
    mLastBoundTexRange = 0;
    memset(mLastBoundTexID, 0, sizeof(mLastBoundTexID));
    memset(mLastBoundSamplerID, 0, sizeof(mLastBoundSamplerID));
    memset(mSamplers, 0, sizeof(mSamplers));
    mShininess = 0.0f;
    mPolygonMode = GL_FILL;
    mShadeModel = GL_SMOOTH;

    // Initialize our cache variables and also the GL so that the
    // stored values match the GL state
    mBlendFuncSource = GL_ONE;
    mBlendFuncDest = GL_ZERO;
    mBlendFuncAlphaSource = GL_ZERO;
    mBlendFuncAlphaDest = GL_ZERO;

    mClearColour[0] = mClearColour[1] = mClearColour[2] = mClearColour[3] = 0.0f;
    mColourMask[0] = mColourMask[1] = mColourMask[2] = mColourMask[3] = GL_TRUE;

#ifdef _ENABLE_STATE_CACHE
    mEnableVector.reserve(25);
    mEnableVector.clear();
#endif
    mActiveBufferMap.clear();
    mTexUnitsMap.clear();
    mTextureCoordGen.clear();

    mViewport[0] = 0.0f;
    mViewport[1] = 0.0f;
    mViewport[2] = 0.0f;
    mViewport[3] = 0.0f;

    mAmbient[0] = 0.2f;
    mAmbient[1] = 0.2f;
    mAmbient[2] = 0.2f;
    mAmbient[3] = 1.0f;

    mDiffuse[0] = 0.8f;
    mDiffuse[1] = 0.8f;
    mDiffuse[2] = 0.8f;
    mDiffuse[3] = 1.0f;

    mSpecular[0] = 0.0f;
    mSpecular[1] = 0.0f;
    mSpecular[2] = 0.0f;
    mSpecular[3] = 1.0f;

    mEmissive[0] = 0.0f;
    mEmissive[1] = 0.0f;
    mEmissive[2] = 0.0f;
    mEmissive[3] = 1.0f;

    mLightAmbient[0] = 0.2f;
    mLightAmbient[1] = 0.2f;
    mLightAmbient[2] = 0.2f;
    mLightAmbient[3] = 1.0f;

    mPointSize = 1.0f;
    mPointSizeMin = 1.0f;
    mPointSizeMax = 1.0f;
    mPointAttenuation[0] = 1.0f;
    mPointAttenuation[1] = 0.0f;
    mPointAttenuation[2] = 0.0f;
}

void COpenGLStateCacheManager::bindGLBuffer(GLenum target, GLuint buffer, bool force)
{
#ifdef _ENABLE_STATE_CACHE
    bool update = false;
    BindBufferMap::iterator i = mActiveBufferMap.find(target);
    if (i == mActiveBufferMap.end())
    {
        // Haven't cached this state yet.  Insert it into the map
        mActiveBufferMap.insert(BindBufferMap::value_type(target, buffer));
        update = true;
    }
    else if ((*i).second != buffer || force) // Update the cached value if needed
    {
        (*i).second = buffer;
        update = true;
    }

    // Update GL
    if (update)
#endif
    {
        if (target == GL_FRAMEBUFFER)
        {
            WPAssert(false);
        }
        else if (target == GL_RENDERBUFFER)
        {
            glBindRenderbufferEXT(target, buffer);
        }
        else
        {
            glBindBuffer(target, buffer);
        }
    }

}

void COpenGLStateCacheManager::deleteGLBuffer(GLenum target, GLuint buffer)
{
    // Buffer name 0 is reserved and we should never try to delete it
    if (buffer == 0)
        return;

    if (target == GL_FRAMEBUFFER)
    {
        glDeleteFramebuffers(1, &buffer);
    }
    else if (target == GL_RENDERBUFFER)
    {
        glDeleteRenderbuffers(1, &buffer);
    }
    else
    {
        glDeleteBuffers(1, &buffer);
    }

#ifdef _ENABLE_STATE_CACHE
    BindBufferMap::iterator i = mActiveBufferMap.find(target);

    if (i != mActiveBufferMap.end() && ((*i).second == buffer))
    {
        // Currently bound buffer is being deleted, update the cached value to 0,
        // which it likely the buffer that will be bound by the driver.
        // An update will be forced next time we try to bind on this target.
        (*i).second = 0;
    }
#endif
}

void COpenGLStateCacheManager::invalidateStateForTexture(GLuint texture)
{
#ifdef _ENABLE_STATE_CACHE
    mTexUnitsMap.erase(texture);
#endif
}

// TODO: Store as high/low bits of a GLuint, use vector instead of map for TexParameteriMap
void COpenGLStateCacheManager::setTexParameteri(GLenum target, GLenum pname, GLint param)
{
#ifdef _ENABLE_STATE_CACHE
    // Check if we have a map entry for this texture id. If not, create a blank one and insert it.
    TexUnitsMap::iterator it = mTexUnitsMap.find(mLastBoundTexID);
    if (it == mTexUnitsMap.end())
    {
        TextureUnitParams unit;
        mTexUnitsMap[mLastBoundTexID] = unit;

        // Update the iterator
        it = mTexUnitsMap.find(mLastBoundTexID);
    }

    // Get a local copy of the parameter map and search for this parameter
    TexParameteriMap &myMap = (*it).second.mTexParameteriMap;
    TexParameteriMap::iterator i = myMap.find(pname);

    if (i == myMap.end())
    {
        // Haven't cached this state yet.  Insert it into the map
        myMap.insert(TexParameteriMap::value_type(pname, param));

        // Update GL
        glTexParameteri(target, pname, param);
    }
    else
    {
        // Update the cached value if needed
        if ((*i).second != param)
        {
            (*i).second = param;

            // Update GL
            glTexParameteri(target, pname, param);
        }
    }
#else
    glTexParameteri(target, pname, param);
#endif
}

bool COpenGLStateCacheManager::bindGLTexture(GLuint unit, GLuint texture, bool queued)
{
    //if (texture)
    //    mLastBoundTexRange = std::max(mLastBoundTexRange + 1, GLsizei(unit));

    if (unit >= MATERIAL_MAX_TEXTURES || mLastBoundTexID[unit] == texture)
        return false;
    
    mLastBoundTexID[unit] = texture;

    if (!queued)
    {
        // Update GL 4.5
        glBindTextureUnit(unit, texture);

        //activateGLTextureUnit(unit);
        //glBindTexture(GL_TEXTURE_2D, texture);
    }
    return true;
}

bool irr::video::COpenGLStateCacheManager::flushGLTextureBind(GLsizei size)
{
    glBindTextures(0, size, mLastBoundTexID);
    //glBindSamplers(0, size, mLastBoundSamplerID);
    mLastBoundTexRange = 0;
    return true;
}

bool irr::video::COpenGLStateCacheManager::bindGLSampler(GLuint unit, GLuint sampler, bool queued)
{
    //if (sampler)
    //    mLastBoundTexRange = std::max(mLastBoundTexRange + 1, GLsizei(unit));

    if (unit >= MATERIAL_MAX_TEXTURES || mLastBoundSamplerID[unit] == sampler)
        return false;

    mLastBoundSamplerID[unit] = sampler;

    if (!queued)
    {
        // Update GL
        glBindSampler(unit, sampler);
    }
    return true;
}

GLuint irr::video::COpenGLStateCacheManager::getSampler(GLuint unit)
{
    // ToDo: rework D11 and OpenGL Sampler handle
    if (!mSamplers[unit])
    {
        glGenSamplers(1, &mSamplers[unit]);
    }
    return mSamplers[unit];
}

bool COpenGLStateCacheManager::activateGLTextureUnit(size_t unit)
{
    if (mActiveTextureUnit == unit)
        return true;

    //if (unit >= Root::getSingleton().getRenderSystem()->getCapabilities()->getNumTextureUnits())
    //    return false;

    glActiveTexture(GL_TEXTURE0 + unit);
    mActiveTextureUnit = unit;
    return true;
}

void COpenGLStateCacheManager::setBlendFunc(GLenum ColorSource, GLenum ColorDest, GLenum AlphaSource, GLenum AlphaDest)
{
    if (mBlendFuncSource != ColorSource || mBlendFuncDest != ColorDest || mBlendFuncAlphaSource != AlphaSource || mBlendFuncAlphaDest != AlphaDest)
    {
        mBlendFuncAlphaSource   = AlphaSource;
        mBlendFuncAlphaDest     = AlphaDest;
        mBlendFuncSource        = ColorSource;
        mBlendFuncDest          = ColorDest;

        glBlendFuncSeparate(ColorSource, ColorDest, AlphaSource, AlphaDest);
    }
}

void COpenGLStateCacheManager::setDepthMask(GLboolean mask)
{
    if (mDepthMask != mask)
    {
        mDepthMask = mask;

        glDepthMask(mask);
    }
}

void COpenGLStateCacheManager::setDepthFunc(GLenum func)
{
    if (mDepthFunc != func)
    {
        mDepthFunc = func;

        glDepthFunc(func);
    }
}

void COpenGLStateCacheManager::setClearDepth(GLclampf depth)
{
    if (mClearDepth != depth)
    {
        mClearDepth = depth;

        glClearDepth(depth);
    }
}

void COpenGLStateCacheManager::setClearColour(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    if ((mClearColour[0] != red) ||
        (mClearColour[1] != green) ||
        (mClearColour[2] != blue) ||
        (mClearColour[3] != alpha))
    {
        mClearColour[0] = red;
        mClearColour[1] = green;
        mClearColour[2] = blue;
        mClearColour[3] = alpha;

        glClearColor(mClearColour[0], mClearColour[1], mClearColour[2], mClearColour[3]);
    }
}

void COpenGLStateCacheManager::setColourMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    if ((mColourMask[0] != red) ||
        (mColourMask[1] != green) ||
        (mColourMask[2] != blue) ||
        (mColourMask[3] != alpha))
    {
        mColourMask[0] = red;
        mColourMask[1] = green;
        mColourMask[2] = blue;
        mColourMask[3] = alpha;

        glColorMask(mColourMask[0], mColourMask[1], mColourMask[2], mColourMask[3]);
    }
}

void COpenGLStateCacheManager::setStencilMask(GLuint mask)
{
    if (mStencilMask != mask)
    {
        mStencilMask = mask;

        glStencilMask(mask);
    }
}

void COpenGLStateCacheManager::setEnabled(GLenum flag, bool enabled)
{
    //auto& itState = mEnableVector[flag];

    if (!enabled)
    {
        //if (itState)
        {
            //itState = false;
            glDisable(flag);
        }
    }
    else //if (!itState)
    {
        //itState = true;
        glEnable(flag);
    }
}

void irr::video::COpenGLStateCacheManager::setEnabled(OGLStateEntries flag, bool enabled)
{
    if (!enabled)
    {
        if (mEnableVector[u8(flag)].second)
        {
            mEnableVector[u8(flag)].second = false;
            glDisable(mEnableVector[u8(flag)].first);
        }
    }
    else if (!mEnableVector[u8(flag)].second)
    {
        mEnableVector[u8(flag)].second = true;
        glEnable(mEnableVector[u8(flag)].first);
    }
}

void COpenGLStateCacheManager::setViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
#ifdef _ENABLE_STATE_CACHE
    if ((mViewport[0] != x) ||
        (mViewport[1] != y) ||
        (mViewport[2] != width) ||
        (mViewport[3] != height))
#endif
    {
        mViewport[0] = x;
        mViewport[1] = y;
        mViewport[2] = width;
        mViewport[3] = height;
        glViewport(x, y, width, height);
    }
}

void COpenGLStateCacheManager::setCullFace(GLenum face)
{
    if (mCullFace != face)
    {
        mCullFace = face;

        glCullFace(face);
    }
}

void COpenGLStateCacheManager::setBlendEquation(GLenum eq)
{
    if (mBlendEquationRGB != eq || mBlendEquationAlpha != eq)
    {
        mBlendEquationRGB = eq;
        mBlendEquationAlpha = eq;

        if (GLEW_VERSION_1_4 || GLEW_ARB_imaging)
        {
            glBlendEquation(eq);
        }
        else if (GLEW_EXT_blend_minmax && (eq == GL_MIN || eq == GL_MAX))
        {
            glBlendEquationEXT(eq);
        }
    }
}

void COpenGLStateCacheManager::setBlendEquation(GLenum eqRGB, GLenum eqAlpha)
{
    if (mBlendEquationRGB != eqRGB || mBlendEquationAlpha != eqAlpha)
    {
        mBlendEquationRGB = eqRGB;
        mBlendEquationAlpha = eqAlpha;

        if (GLEW_VERSION_2_0)
        {
            glBlendEquationSeparate(eqRGB, eqAlpha);
        }
        else if (GLEW_EXT_blend_equation_separate)
        {
            glBlendEquationSeparateEXT(eqRGB, eqAlpha);
        }
    }
}

void COpenGLStateCacheManager::setMaterialDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    if ((mDiffuse[0] != r) ||
        (mDiffuse[1] != g) ||
        (mDiffuse[2] != b) ||
        (mDiffuse[3] != a))
    {
        mDiffuse[0] = r;
        mDiffuse[1] = g;
        mDiffuse[2] = b;
        mDiffuse[3] = a;

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &mDiffuse[0]);
    }
}

void COpenGLStateCacheManager::setMaterialAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    if ((mAmbient[0] != r) ||
        (mAmbient[1] != g) ||
        (mAmbient[2] != b) ||
        (mAmbient[3] != a))
    {
        mAmbient[0] = r;
        mAmbient[1] = g;
        mAmbient[2] = b;
        mAmbient[3] = a;

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &mAmbient[0]);
    }
}

void COpenGLStateCacheManager::setMaterialEmissive(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    if ((mEmissive[0] != r) ||
        (mEmissive[1] != g) ||
        (mEmissive[2] != b) ||
        (mEmissive[3] != a))
    {
        mEmissive[0] = r;
        mEmissive[1] = g;
        mEmissive[2] = b;
        mEmissive[3] = a;

        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &mEmissive[0]);
    }
}

void COpenGLStateCacheManager::setMaterialSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    if ((mSpecular[0] != r) ||
        (mSpecular[1] != g) ||
        (mSpecular[2] != b) ||
        (mSpecular[3] != a))
    {
        mSpecular[0] = r;
        mSpecular[1] = g;
        mSpecular[2] = b;
        mSpecular[3] = a;

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &mSpecular[0]);
    }
}

void COpenGLStateCacheManager::setMaterialShininess(GLfloat shininess)
{
    if (mShininess != shininess)
    {
        mShininess = shininess;
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
    }
}

void COpenGLStateCacheManager::setPolygonMode(GLenum mode)
{
    if (mPolygonMode != mode)
    {
        mPolygonMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
    }
}

void COpenGLStateCacheManager::setShadeModel(GLenum model)
{
    if (mShadeModel != model)
    {
        mShadeModel = model;
        glShadeModel(model);
    }
}

void COpenGLStateCacheManager::setLightAmbient(GLfloat r, GLfloat g, GLfloat b)
{
    if ((mLightAmbient[0] != r) ||
        (mLightAmbient[1] != g) ||
        (mLightAmbient[2] != b))
    {
        mLightAmbient[0] = r;
        mLightAmbient[1] = g;
        mLightAmbient[2] = b;

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &mLightAmbient[0]);
    }
}

void COpenGLStateCacheManager::setPointSize(GLfloat size)
{
    if (mPointSize != size)
    {
        mPointSize = size;
        glPointSize(mPointSize);
    }
}

void COpenGLStateCacheManager::setPointParameters(GLfloat *attenuation, float minSize, float maxSize)
{
    //if (minSize != mPointSizeMin)
    //{
    //    mPointSizeMin = minSize;
    //    const ::RenderSystemCapabilities* caps = dynamic_cast<GLRenderSystem*>(Root::getSingleton().getRenderSystem())->getCapabilities();
    //    if (caps->hasCapability(RSC_POINT_EXTENDED_PARAMETERS))
    //        glPointParameterf(GL_POINT_SIZE_MIN, mPointSizeMin);
    //}
    //if (maxSize != mPointSizeMax)
    //{
    //    mPointSizeMax = maxSize;
    //    const ::RenderSystemCapabilities* caps = dynamic_cast<GLRenderSystem*>(Root::getSingleton().getRenderSystem())->getCapabilities();
    //    if (caps->hasCapability(RSC_POINT_EXTENDED_PARAMETERS))
    //        glPointParameterf(GL_POINT_SIZE_MAX, mPointSizeMax);
    //}
    //if (attenuation[0] != mPointAttenuation[0] || attenuation[1] != mPointAttenuation[1] || attenuation[2] != mPointAttenuation[2])
    //{
    //    mPointAttenuation[0] = attenuation[0];
    //    mPointAttenuation[1] = attenuation[1];
    //    mPointAttenuation[2] = attenuation[2];
    //    const ::RenderSystemCapabilities* caps = dynamic_cast<GLRenderSystem*>(Root::getSingleton().getRenderSystem())->getCapabilities();
    //    if (caps->hasCapability(RSC_POINT_EXTENDED_PARAMETERS))
    //        glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, &mPointAttenuation[0]);
    //}
}

void COpenGLStateCacheManager::enableTextureCoordGen(GLenum type)
{
#ifdef _ENABLE_STATE_CACHE
    std::unordered_map<GLenum, TexGenParams>::iterator it = mTextureCoordGen.find(mActiveTextureUnit);
    if (it == mTextureCoordGen.end())
    {
        glEnable(type);
        mTextureCoordGen[mActiveTextureUnit].mEnabled.insert(type);
    }
    else
    {
        if (it->second.mEnabled.find(type) == it->second.mEnabled.end())
        {
            glEnable(type);
            it->second.mEnabled.insert(type);
        }
    }
#else
    glEnable(type);
#endif
}

void COpenGLStateCacheManager::disableTextureCoordGen(GLenum type)
{
#ifdef _ENABLE_STATE_CACHE
    std::unordered_map<GLenum, TexGenParams>::iterator it = mTextureCoordGen.find(mActiveTextureUnit);
    if (it != mTextureCoordGen.end())
    {
        std::set<GLenum>::iterator found = it->second.mEnabled.find(type);
        if (found != it->second.mEnabled.end())
        {
            glDisable(type);
            it->second.mEnabled.erase(found);
        }
    }
#else
    glDisable(type);
#endif
}
