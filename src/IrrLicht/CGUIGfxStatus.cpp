// GUI Class that draw a image gauge (like a diablo life gauge)
// 2010 by Christian Clavet
//
#include "CGUIGfxStatus.h"
#include "IGUIElement.h"

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIFont.h"
#include "IVideoDriver.h"
#include "rect.h"

namespace irr
{
namespace gui
{

//! constructor
	CGUIGfxStatus::CGUIGfxStatus(IGUIEnvironment* environment, IGUIElement* parent, const core::rect<s32>& rectangle, s32 id)
		: IGUIElement(EGUIET_IMAGE, environment ,parent ,id ,rectangle)

{
	driver = environment->getVideoDriver();
	image = NULL;

	rect = rectangle;
	currentvalue=1;
	maxvalue=1;
	full=true;
	rightportion=false;
	leftportion=false;

}

CGUIGfxStatus::~CGUIGfxStatus()
{
}

void CGUIGfxStatus::draw()
{
	if (image)
	{

		this->getParent()->getAbsolutePosition();
		//Redefenition seem not needed on MSVC
		//core::rect<s32> myRect (s32 x, s32 y, s32 w, s32 h);
		core::rect<s32> parent = this->getParent()->getAbsolutePosition();
		core::rect<s32> destination = this->myRect(parent.UpperLeftCorner.X+rect.UpperLeftCorner.X,
			parent.UpperLeftCorner.Y+rect.UpperLeftCorner.Y,
			rect.getWidth(),rect.getHeight());

		// Circle life bar, the graphic destination position
		f32 base = ((f32)currentvalue * 100.0f)/(f32)maxvalue; // Convert the value in %
		f32 imageheight = image->getSize().Height / 100.0f;
		f32 finalbase = base*imageheight;

		s32 baseX = destination.UpperLeftCorner.X;
		s32 clipX = 0;
		if (rightportion)
		{
			baseX = destination.UpperLeftCorner.X + image->getSize().Width/2;
			clipX = image->getSize().Width;
		}

		if (leftportion)
		{
			baseX = destination.UpperLeftCorner.X;
			clipX = image->getSize().Width/2;
		}

		if (full)

		{
			baseX = destination.UpperLeftCorner.X;
			clipX = image->getSize().Width;
		}

		core::rect<s32> clip = this->myRect(baseX,destination.LowerRightCorner.Y-(s32)finalbase,clipX,(s32)finalbase);
		driver->draw2DImage(image,destination,this->myRect(0,0,image->getSize().Height,image->getSize().Width),&clip,0,true);
		IGUIElement::draw();
	}
}

void CGUIGfxStatus::setCurrentValue(irr::s32 value)
{
	currentvalue = value;
}

void CGUIGfxStatus::setMaxValue(irr::s32 value)
{
	maxvalue=value;
}

void CGUIGfxStatus::setImage(video::ITexture * setimage)
{
	image = setimage;
}

void CGUIGfxStatus::ViewHalfRight()
{
	full=false;
	rightportion=true;
	leftportion=false;
}

void CGUIGfxStatus::ViewHalfLeft()
{
	full=false;
	rightportion=false;
	leftportion=true;
}

void CGUIGfxStatus::ViewFull()
{
	full=true;
	rightportion=false;
	leftportion=false;
}

core::rect<s32> CGUIGfxStatus::myRect(s32 x, s32 y, s32 w, s32 h)
{
    return core::rect<s32>(x,y,x+w,y+h);
}

}
}
