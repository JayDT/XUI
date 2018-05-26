// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUISpriteBank.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "ITexture.h"
#include "IHardwareBuffer.h"

namespace irr
{
namespace gui
{

CGUISpriteBank::CGUISpriteBank(IGUIEnvironment* env) :
	Environment(env), Driver(0)
{
	#ifdef _DEBUG
	setDebugName("CGUISpriteBank");
	#endif

	if (Environment)
	{
		Driver = Environment->getVideoDriver();
		if (Driver)
			Driver->grab();
	}
}


CGUISpriteBank::~CGUISpriteBank()
{
	// drop textures
	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i])
			Textures[i]->drop();

	// drop video driver
	if (Driver)
		Driver->drop();
}


core::array< core::rect<s32> >& CGUISpriteBank::getPositions()
{
	return Rectangles;
}


core::array< SGUISprite >& CGUISpriteBank::getSprites()
{
	return Sprites;
}

core::array<core::rect<s32>>& CGUISpriteBank::getRectangles()
{
    return Rectangles;
}


u32 CGUISpriteBank::getTextureCount() const
{
	return Textures.size();
}


video::ITexture* CGUISpriteBank::getTexture(u32 index) const
{
	if (index < Textures.size())
		return Textures[index];
	else
		return 0;
}


void CGUISpriteBank::addTexture(video::ITexture* texture)
{
	if (texture)
		texture->grab();

	Textures.push_back(texture);
}


void CGUISpriteBank::setTexture(u32 index, video::ITexture* texture)
{
	while (index >= Textures.size())
		Textures.push_back(0);

	if (texture)
		texture->grab();

	if (Textures[index])
		Textures[index]->drop();

	Textures[index] = texture;
}


//! clear everything
void CGUISpriteBank::clear()
{
	// drop textures
	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i])
			Textures[i]->drop();
	Textures.clear();
	Sprites.clear();
	Rectangles.clear();
}

//! Add the texture and use it for a single non-animated sprite.
s32 CGUISpriteBank::addTextureAsSprite(video::ITexture* texture)
{
	if ( !texture )
		return -1;

	addTexture(texture);
	u32 textureIndex = getTextureCount() - 1;

	u32 rectangleIndex = Rectangles.size();
	Rectangles.push_back( core::rect<s32>(0,0, texture->getOriginalSize().Width, texture->getOriginalSize().Height) );

	SGUISprite sprite;
	sprite.frameTime = 0;

	SGUISpriteFrame frame;
	frame.textureNumber = textureIndex;
	frame.rectNumber = rectangleIndex;
	sprite.Frames.push_back( frame );

	Sprites.push_back( sprite );

	return Sprites.size() - 1;
}

//! draws a sprite in 2d with scale and color
void CGUISpriteBank::draw2DSprite(u32 index, const core::position2di& pos,
		const core::rect<s32>* clip, const video::SColor& color,
		u32 starttime, u32 currenttime, bool loop, bool center)
{
	if (index >= Sprites.size() || Sprites[index].Frames.empty() )
		return;

	// work out frame number
	u32 frame = 0;
	if (Sprites[index].frameTime)
	{
		u32 f = ((currenttime - starttime) / Sprites[index].frameTime);
		if (loop)
			frame = f % Sprites[index].Frames.size();
		else
			frame = (f >= Sprites[index].Frames.size()) ? Sprites[index].Frames.size()-1 : f;
	}

	const video::ITexture* tex = Textures[Sprites[index].Frames[frame].textureNumber];
	if (!tex)
		return;

	const u32 rn = Sprites[index].Frames[frame].rectNumber;
	if (rn >= Rectangles.size())
		return;

	const core::rect<s32>& r = Rectangles[rn];

	if (center)
	{
		core::position2di p = pos;
		p -= r.getSize() / 2;
		Driver->draw2DImage(tex, p, r, clip, color, true);
	}
	else
	{
		Driver->draw2DImage(tex, pos, r, clip, color, true);
	}
}

bool CGUISpriteBank::CacheGeometry(scene::IMeshBuffer& mb,
    const core::array<u32>& indices, 
    const core::array<core::position2di>& pos,
    const core::rect<s32>* clipRect,
    const video::SColor& color,
    u32 starttime, u32 currenttime,
    bool loop, bool center)
{
    const irr::u32 drawCount = core::min_<u32>(indices.size(), pos.size());
    const core::dimension2d<u32>& renderTargetSize = Driver->getCurrentRenderTargetSize();

    if (Textures.empty())
        return false;

    scene::CMeshBufferEx<video::S3DVertex>& meshBuffer = (scene::CMeshBufferEx<video::S3DVertex>&)mb;
    meshBuffer.Vertices.reallocate(drawCount * 4);
    meshBuffer.Indices.reallocate(drawCount * 6);

    meshBuffer.Vertices.set_used(0);
    meshBuffer.Indices.set_used(0);

    int textureId = 0;
    //int vertexStart = 0;
    //int indexStart = 0;

    for (u32 i = 0; i < drawCount; i++)
    {
        const u32 index = indices[i];

        if (index >= Sprites.size() || Sprites[index].Frames.empty())
            continue;

        // work out frame number
        u32 frame = 0;
        if (Sprites[index].frameTime)
        {
            u32 f = ((currenttime - starttime) / Sprites[index].frameTime);
            if (loop)
                frame = f % Sprites[index].Frames.size();
            else
                frame = (f >= Sprites[index].Frames.size()) ? Sprites[index].Frames.size() - 1 : f;
        }

        const u32 texNum = Sprites[index].Frames[frame].textureNumber;

        const u32 rn = Sprites[index].Frames[frame].rectNumber;
        if (rn >= Rectangles.size())
            return false;

        const core::rect<s32>& r = Rectangles[rn];

        core::position2d<s32> targetPos = pos[i];

        if (center)
        {
            core::position2di p = pos[i];
            p -= r.getSize() / 2;

            targetPos = p;
        }

        core::position2d<s32> sourcePos = r.UpperLeftCorner;
        // This needs to be signed as it may go negative.
        core::dimension2d<s32> sourceSize(r.getSize());

        if (clipRect)
        {
            if (targetPos.X < clipRect->UpperLeftCorner.X)
            {
                sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
                if (sourceSize.Width <= 0)
                    continue;

                sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
                targetPos.X = clipRect->UpperLeftCorner.X;
            }

            if (targetPos.X + (s32)sourceSize.Width > clipRect->LowerRightCorner.X)
            {
                sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
                if (sourceSize.Width <= 0)
                    continue;
            }

            if (targetPos.Y < clipRect->UpperLeftCorner.Y)
            {
                sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
                if (sourceSize.Height <= 0)
                    continue;

                sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
                targetPos.Y = clipRect->UpperLeftCorner.Y;
            }

            if (targetPos.Y + (s32)sourceSize.Height > clipRect->LowerRightCorner.Y)
            {
                sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
                if (sourceSize.Height <= 0)
                    continue;
            }
        }

        // clip these coordinates

        if (targetPos.X<0)
        {
            sourceSize.Width += targetPos.X;
            if (sourceSize.Width <= 0)
                continue;

            sourcePos.X -= targetPos.X;
            targetPos.X = 0;
        }

        if (targetPos.X + sourceSize.Width >(s32)renderTargetSize.Width)
        {
            sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
            if (sourceSize.Width <= 0)
                continue;
        }

        if (targetPos.Y<0)
        {
            sourceSize.Height += targetPos.Y;
            if (sourceSize.Height <= 0)
                continue;

            sourcePos.Y -= targetPos.Y;
            targetPos.Y = 0;
        }

        if (targetPos.Y + sourceSize.Height >(s32)renderTargetSize.Height)
        {
            sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
            if (sourceSize.Height <= 0)
                continue;
        }

        // ok, we've clipped everything.
        // now draw it.

        core::rect<f32> tcoords;
        tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)) / Textures[texNum]->getOriginalSize().Width;
        tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)) / Textures[texNum]->getOriginalSize().Height;
        tcoords.LowerRightCorner.X = tcoords.UpperLeftCorner.X + ((f32)(sourceSize.Width) / Textures[texNum]->getOriginalSize().Width);
        tcoords.LowerRightCorner.Y = tcoords.UpperLeftCorner.Y + ((f32)(sourceSize.Height) / Textures[texNum]->getOriginalSize().Height);

        const core::rect<s32> poss(targetPos, sourceSize);

        meshBuffer.Vertices.push_back(video::S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));
        meshBuffer.Vertices.push_back(video::S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));
        meshBuffer.Vertices.push_back(video::S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));
        meshBuffer.Vertices.push_back(video::S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));

        const u32 curPos = meshBuffer.Vertices.size() - 4;
        meshBuffer.Indices.push_back(0 + curPos);
        meshBuffer.Indices.push_back(1 + curPos);
        meshBuffer.Indices.push_back(2 + curPos);

        meshBuffer.Indices.push_back(0 + curPos);
        meshBuffer.Indices.push_back(2 + curPos);
        meshBuffer.Indices.push_back(3 + curPos);
    }

    meshBuffer.setDirty();
    meshBuffer.getMaterial().setTexture(0, Textures[textureId]);
    if (Textures[textureId])
        meshBuffer.getMaterial().MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
    else if (color.getAlpha()<255)
        meshBuffer.getMaterial().MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;

    if (!meshBuffer.GetHWBuffer())
    {
        Driver->CreateHardwareBuffer(&meshBuffer);
        meshBuffer.GetHWBuffer()->SetManualBind(true);
    }
    return true;
}


void CGUISpriteBank::draw2DSpriteBatch(	const core::array<u32>& indices,
										const core::array<core::position2di>& pos,
										const core::rect<s32>* clip,
										const video::SColor& color,
										u32 starttime, u32 currenttime,
										bool loop, bool center)
{
	const irr::u32 drawCount = core::min_<u32>(indices.size(), pos.size());

	if( Textures.empty() )
		return;
	core::array<SDrawBatch> drawBatches(Textures.size());
	for(u32 i = 0;i < Textures.size();i++)
	{
		drawBatches.push_back(SDrawBatch());
		drawBatches[i].positions.reallocate(drawCount);
		drawBatches[i].sourceRects.reallocate(drawCount);
	}

	for(u32 i = 0;i < drawCount;i++)
	{
		const u32 index = indices[i];

		if (index >= Sprites.size() || Sprites[index].Frames.empty() )
			continue;

		// work out frame number
		u32 frame = 0;
		if (Sprites[index].frameTime)
		{
			u32 f = ((currenttime - starttime) / Sprites[index].frameTime);
			if (loop)
				frame = f % Sprites[index].Frames.size();
			else
				frame = (f >= Sprites[index].Frames.size()) ? Sprites[index].Frames.size()-1 : f;
		}

		const u32 texNum = Sprites[index].Frames[frame].textureNumber;

		SDrawBatch& currentBatch = drawBatches[texNum];

		const u32 rn = Sprites[index].Frames[frame].rectNumber;
		if (rn >= Rectangles.size())
			return;

		const core::rect<s32>& r = Rectangles[rn];

		if (center)
		{
			core::position2di p = pos[i];
			p -= r.getSize() / 2;

			currentBatch.positions.push_back(p);
			currentBatch.sourceRects.push_back(r);
		}
		else
		{
			currentBatch.positions.push_back(pos[i]);
			currentBatch.sourceRects.push_back(r);
		}
	}

	for(u32 i = 0;i < drawBatches.size();i++)
	{
		if(!drawBatches[i].positions.empty() && !drawBatches[i].sourceRects.empty())
			Driver->draw2DImageBatch(Textures[i], drawBatches[i].positions,
				drawBatches[i].sourceRects, clip, color, true);
	}
}

} // namespace gui
} // namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

