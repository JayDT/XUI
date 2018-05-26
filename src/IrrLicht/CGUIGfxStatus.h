// GUI Class that draw a image gauge (like a diablo life gauge)
// 2010 by Christian Clavet
//
#ifndef __C_GUI_GFX_STATUS_H_INCLUDED__
#define __C_GUI_GFX_STATUS_H_INCLUDED__

#include "IrrlichtDevice.h"
#include "IGUIElement.h"

namespace irr
{
namespace gui
{
	class CGUIGfxStatus : public IGUIElement
	{
	public:
		//! constructor
		CGUIGfxStatus(IGUIEnvironment* environment, IGUIElement* parent, const core::rect<s32>& rectangle, s32 id);
		//! destructor
		virtual ~CGUIGfxStatus();
		virtual void draw();
		void setImage (video::ITexture* setimage);
		void setMaxValue (s32 value);
		void setCurrentValue (s32 value);
		void ViewHalfRight();
		void ViewHalfLeft();
		void ViewFull();

	private:
		core::rect<s32> rect;
		video::IVideoDriver * driver;
		video::ITexture* image;

		s32 currentvalue;
		s32 maxvalue;

		bool full, rightportion, leftportion;

		core::rect<s32> myRect(s32 x, s32 y, s32 w, s32 h);
	};
}
}
#endif
