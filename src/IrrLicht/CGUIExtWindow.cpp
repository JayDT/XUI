#include "CGUIExtWindow.h"


const s32 FOD_WIDTH = 640;
const s32 FOD_HEIGHT = 400;
const s32 yoffset = 10;

//! constructor
CGUIExtWindow::CGUIExtWindow(const wchar_t* title, IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIElement(EGUIET_WINDOW, environment, parent, id, rectangle), IsDraggable(true), DrawBackground(true), DrawTitlebar(true), IsActive(false),
DrawInsideBorder(true)
{
#ifdef _DEBUG
	IGUIElement::setDebugName("CGUIExtWindow");
#endif

	DragByTitlebar = true;
	collapse = false;
	oldsize_y = 0;
	oldmin = this->MinSize;

	Text = title;
	device=NULL;
	borderwidth=5;
	
	//Keep the old rectangle in memory.
	oldrectangle=rectangle;

	IGUISkin* skin = Environment->getSkin();
	IGUISpriteBank* sprites = 0;
	video::SColor color(255,255,255,255);
	if (skin)
	{
		sprites = skin->getSpriteBank();
		color = skin->getColor(EGDC_WINDOW_SYMBOL);
	}

	s32 buttonw = 42; 
		//Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);
	
	enableright = false;
	enableleft = false;
	enablebottom = false;
	enabletop = false;
	timer1 = 0;
	timer2 = 0;
	expand = false;
	retract = false;
	closehide = false;
	drawTitleBar = true;

	triggered = false;
	dragging = false;

	// Position the close button Windows or Linux style
#ifdef WIN32
	s32 posx = RelativeRect.getWidth() - buttonw - 6;
#else
	s32 posx = 6;
#endif
	//buttonw, this, -1,
	//CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3+buttonw),this, -1 ,
	CloseButton = Environment->addButton(core::rect<s32>(posx, 1, posx + buttonw, 20),this, -1 ,
		L"", L"Close");
	CloseButton->setSubElement(true);
	if (sprites) {
		CloseButton->setSpriteBank(sprites);
		CloseButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_CLOSE), color);
		CloseButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_CLOSE), color);
		//CloseButton->setDrawBorder(false);
	}
#ifdef WIN32
	CloseButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
#else
	CloseButton->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
#endif
	CloseButton->grab();
	oldrectangle = AbsoluteRect;

}



//! destructor
CGUIExtWindow::~CGUIExtWindow()
{
	if (CloseButton)
		CloseButton->drop();

}


//! Returns pointer to the close button
IGUIButton* CGUIExtWindow::getCloseButton() const
{
	return CloseButton;
}

//! called if an event happened.
bool CGUIExtWindow::OnEvent(const SEvent& evt)
{
	if (!isVisible() || !isEnabled())
			return IGUIElement::OnEvent(evt);

	//if (!device->getCursorControl()->getPosition().isBetweenPoints(this->getAbsoluteClippingRect().UpperLeftCorner,this->getAbsoluteClippingRect().LowerRightCorner))
	//	return IGUIElement::OnEvent(evt); // This will only accept events if the pointer is over the button.

	switch(evt.EventType)
	{
		case EET_GUI_EVENT:
		if (evt.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			//dragging = false;
			IsActive = false;
		}
		else
		if (evt.GUIEvent.EventType == EGET_ELEMENT_FOCUSED)
		{
			if (Parent && ((evt.GUIEvent.Caller == this) || isMyChild(evt.GUIEvent.Caller)))
			{
				Parent->bringToFront(this);
				IsActive = true;
			}
			else
			{
				IsActive = false;
			}
		}
		else
		if (evt.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (evt.GUIEvent.Caller == CloseButton)
			{
				if (device->getCursorControl()->getActiveIcon()!= ECURSOR_ICON(0))
					device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0));
				//User can decide if he want to "remove" the class when closing the window or simply hide it.
				if (!closehide)
					remove();
				else 
					this->setVisible(false);

				return true;
			}
	
		}
		
		break;

		case EET_MOUSE_INPUT_EVENT:
		if (evt.MouseInput.Event==EMIE_LMOUSE_PRESSED_DOWN)
		{	
			core::rect<s32> rect = AbsoluteRect; //define a new rect based on the titlebar size (aprx 25pix)
			rect.LowerRightCorner.Y=rect.UpperLeftCorner.Y+25;
			core::vector2d<s32> mousepos; //Current mouse position
			mousepos.X=evt.MouseInput.X;
			mousepos.Y=evt.MouseInput.Y;
			dragging = true;

			if (DragByTitlebar)
			{
				//if (Dragging==false && rect.isPointInside(mousepos))
				{
					DragStart.X = evt.MouseInput.X;
					DragStart.Y = evt.MouseInput.Y;
				}
			} else
				if (!DragByTitlebar)
				{
					DragStart.X = evt.MouseInput.X;
					DragStart.Y = evt.MouseInput.Y;
				}
			break;
		}
		else
			if (evt.MouseInput.Event==EMIE_LMOUSE_DOUBLE_CLICK)
			{
				u32 titleheight = Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH)+4;
				core::rect<s32> rect = AbsoluteRect; //define a new rect based on the titlebar size (aprx 25pix)
				rect.LowerRightCorner.Y=rect.UpperLeftCorner.Y+titleheight;
				core::vector2d<s32> mousepos; //Current mouse position
				mousepos.X=evt.MouseInput.X;
				mousepos.Y=evt.MouseInput.Y;
				
				if (!collapse && DragByTitlebar && rect.isPointInside(mousepos))
				{
					oldmin = this->MinSize;
					this->MinSize.Height=25;
					oldsize_y = AbsoluteRect.LowerRightCorner.Y;
					AbsoluteRect.LowerRightCorner.Y = AbsoluteRect.UpperLeftCorner.Y+titleheight;
					AbsoluteClippingRect = AbsoluteRect;
					DesiredRect = AbsoluteRect;
					this->updateAbsolutePosition();
					collapse=true;
				}
				else
					if (DragByTitlebar && rect.isPointInside(mousepos))
				{
					this->MinSize=oldmin;
					AbsoluteRect.LowerRightCorner.Y = oldsize_y;
					AbsoluteClippingRect = AbsoluteRect;
					DesiredRect = AbsoluteRect;
					this->updateAbsolutePosition();
					collapse=false;
				}

			}
		else
		if (evt.MouseInput.Event==EMIE_LMOUSE_LEFT_UP && dragging)
		{
			// Set back the pointer cursor after the dragging operation
			if (device->getCursorControl()->getActiveIcon()!= ECURSOR_ICON(0))
			{	
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0) );
				triggered = false;
			}
			dragging = false;
			break;
		}
		else
		if (evt.MouseInput.Event==EMIE_MOUSE_MOVED)
		{
			if (!IsDraggable)
					break;

			core::vector2d<s32> mousepos; //Current mouse position
			mousepos.X=evt.MouseInput.X;
			mousepos.Y=evt.MouseInput.Y;

			if (dragging && !stretchbottom && !stretchtop  && !stretchright && !stretchleft)
			{
				// gui window should not be dragged outside its parent
				if (Parent)
					if (mousepos.X < Parent->getAbsolutePosition().UpperLeftCorner.X +1 ||
						mousepos.Y < Parent->getAbsolutePosition().UpperLeftCorner.Y +1 ||
						mousepos.X > Parent->getAbsolutePosition().LowerRightCorner.X -1 ||
						mousepos.Y > Parent->getAbsolutePosition().LowerRightCorner.Y -1)

						break;

				move(core::position2d<s32>(evt.MouseInput.X - DragStart.X, evt.MouseInput.Y - DragStart.Y));
				DragStart = mousepos;				
			}
			break;
		}

		default:
			break;
		
	}
	
	//return true;
	return IGUIElement::OnEvent(evt);
}


//! draws the element and its children
void CGUIExtWindow::draw()
{

	if (!IsVisible)
		return;

	// Update the size of the client rectangle if changed
	ClientRect = core::rect<s32>(0,0, AbsoluteRect.getWidth(), AbsoluteRect.getHeight());

	core::vector2d<s32> mousepos = core::vector2d<s32>(0,0);
	IGUISkin* skin = Environment->getSkin();
	irr::video::IVideoDriver* driver = Environment->getVideoDriver();
	
	// Get the cursor position
	// For this, the device need to be set
	if (device)
		mousepos=device->getCursorControl()->getPosition();

	core::rect<s32> rect = AbsoluteRect;
	
	rect = skin->draw3DWindowBackground(this, drawTitleBar, skin->getColor(IsActive ? EGDC_ACTIVE_BORDER : EGDC_INACTIVE_BORDER),
		rect, &AbsoluteClippingRect); 

	if (Text.size())
	{
#ifdef WIN32
		rect.UpperLeftCorner.X += 10;
#else
		rect.UpperLeftCorner.X += CloseButton->getAbsoluteClippingRect().getWidth()+10;
#endif
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		IGUIFont* font = skin->getFont(EGDF_WINDOW);
		if (font)
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true,
			&AbsoluteClippingRect);
	}

	//Draw inside borders
	if (DrawInsideBorder && !collapse)
	{
		// Inner border as double border lines (better looking, could use an activator so the user could decide to have it or not
		rect.UpperLeftCorner.X=AbsoluteRect.UpperLeftCorner.X+borderwidth;
		rect.UpperLeftCorner.Y=AbsoluteRect.UpperLeftCorner.Y+(skin->getSize(EGDS_WINDOW_BUTTON_WIDTH)+borderwidth+4);
		
		rect.LowerRightCorner.X=AbsoluteRect.LowerRightCorner.X-borderwidth;

		rect.LowerRightCorner.Y=AbsoluteRect.LowerRightCorner.Y-borderwidth;
		skin->draw3DSunkenPane(this,skin->getColor(EGDC_INACTIVE_BORDER),true,true,rect, &AbsoluteClippingRect);
	}

	// Draw cursor changes
	if (!dragging && !collapse)
	{
		drawRef(mousepos);
	}
	
	if (dragging && !collapse)
	{
		//Stretch the window defined on the desired directions
		if (drawStretch(mousepos))
		{
			// Move outside the parent then return
			IGUIElement::draw();
			return;
		}
	}

	if (expand && device && !collapse)
	{
		u32 time = device->getTimer()->getRealTime();
		//2 ms interval
		if ((time-timer1) > 17)
		{
			timer1=time;
			AbsoluteRect.UpperLeftCorner.X-=50;
			if (AbsoluteRect.getWidth()>(irr::s32)this->MaxSize.Width)
			{
				expand=false;
				AbsoluteRect.UpperLeftCorner.X=AbsoluteRect.LowerRightCorner.X-MaxSize.Width;
				if (device->getCursorControl()->getActiveIcon()!= ECURSOR_ICON(0))
					device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0) );
			}
			this->AbsoluteClippingRect=this->AbsoluteRect;
			this->DesiredRect=AbsoluteRect;
			this->updateAbsolutePosition();

		}

	}
	if (retract && device && !collapse)
	{
		
		u32 time = device->getTimer()->getRealTime();
		//2 ms interval
		if ((time-timer1) > 17)
		{
			timer1=time;
			AbsoluteRect.UpperLeftCorner.X+=50;
			if (AbsoluteRect.getWidth()<(irr::s32)this->MinSize.Width)
			{
				retract=false;
				AbsoluteRect.UpperLeftCorner.X=AbsoluteRect.LowerRightCorner.X-MinSize.Width;
				if (device->getCursorControl()->getActiveIcon()!= ECURSOR_ICON(0))
					device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0) );
			}
			this->AbsoluteClippingRect=this->AbsoluteRect;
			this->DesiredRect=AbsoluteRect;
			this->updateAbsolutePosition();

		}

	}

	
	if (!collapse)
		IGUIElement::draw();
	
	// If not streched from any corner then update the reference rectangle
	if (!stretchright && !stretchleft && !stretchbottom && !stretchtop)
		oldrectangle=AbsoluteRect;
}

//Draw the cursor changes or hinting reference for a window stretch
void CGUIExtWindow::drawRef(core::vector2d<s32> mousepos)
{

	if (!device)
		return;

	// Determine if the GUI is being hovered or any of the child of this window
	// if nothing is hovered then there must be NO POINTER changes.
	bool hovered = false;
	if (Environment->getHovered()==this || isMyChild(Environment->getHovered()))
		hovered = true;

		//Hinting and detection for stretch on the right side
	if ((mousepos.X>=AbsoluteRect.LowerRightCorner.X-8) && (mousepos.X<AbsoluteRect.LowerRightCorner.X) &&
			(mousepos.Y<=AbsoluteRect.LowerRightCorner.Y) && (mousepos.Y>=AbsoluteRect.UpperLeftCorner.Y))
		{   
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.UpperLeftCorner.Y+1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.LowerRightCorner.Y-1),video::SColor(255,255,255,0));
			if (enableright && hovered) 
				this->stretchright=true;
		} else
		{
			this->stretchright=false;
		}

		//Hinting and detection for stretch on the left side
		if ((mousepos.X>AbsoluteRect.UpperLeftCorner.X) && (mousepos.X<=AbsoluteRect.UpperLeftCorner.X+8) &&
			(mousepos.Y<=AbsoluteRect.LowerRightCorner.Y) && (mousepos.Y>=AbsoluteRect.UpperLeftCorner.Y))
		{
			if (enableleft && hovered)
				this->stretchleft=true;
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.LowerRightCorner.Y-1),core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.UpperLeftCorner.Y-1),video::SColor(255,255,255,0));
			
		} else
		{
			this->stretchleft=false;
		}

		//Hinting and detection for stretch on the bottom
		if ((mousepos.Y>=AbsoluteRect.LowerRightCorner.Y-8) && (mousepos.Y<AbsoluteRect.LowerRightCorner.Y) &&
			(mousepos.X>=AbsoluteRect.UpperLeftCorner.X) && (mousepos.X<=AbsoluteRect.LowerRightCorner.X))
		{
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.LowerRightCorner.Y-1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.LowerRightCorner.Y-1),video::SColor(255,255,255,0));
			if (enablebottom && hovered)
				this->stretchbottom=true;
		}
		else
			this->stretchbottom=false;

		//Hinting and detection for stretch on the top
		if ((mousepos.Y<=AbsoluteRect.UpperLeftCorner.Y+8) && (mousepos.Y>AbsoluteRect.UpperLeftCorner.Y) &&
			(mousepos.X>=AbsoluteRect.UpperLeftCorner.X) && (mousepos.X<=AbsoluteRect.LowerRightCorner.X))
		{
			//driver->draw2DLine(core::vector2d<s32>(AbsoluteRect.UpperLeftCorner.X+1,AbsoluteRect.UpperLeftCorner.Y+1),core::vector2d<s32>(AbsoluteRect.LowerRightCorner.X-1,AbsoluteRect.UpperLeftCorner.Y+1),video::SColor(255,255,255,0));
			if (enabletop && hovered)
				this->stretchtop=true;
		}
		else
			this->stretchtop=false;


		if (stretchright || stretchleft)
		{
			if (!stretchbottom && !stretchtop)
			{
				triggered = true;
					device->getCursorControl()->setActiveIcon( ECURSOR_ICON(11) );
			}
		}
		if (stretchbottom || stretchtop)
		{
			if (!stretchright && !stretchleft)
			{
				triggered = true;
					device->getCursorControl()->setActiveIcon( ECURSOR_ICON(10) );
			}
		}

		if (stretchbottom && stretchleft)
		{
			triggered = true;
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(8) );
		}

		if (stretchbottom && stretchright)
		{
			triggered = true;
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(9) );
		}

		if (stretchtop && stretchleft)
		{
			triggered = true;
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(9) );
		}

		if (stretchtop && stretchright)
		{
			triggered = true;
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(8) );
		}

		// reset the default cursor if the cursor get out or inside the area (border select)
		if (!stretchright && !stretchleft && !stretchbottom && !stretchtop)
		{
			// Define a rectangle that is bigger than the source rectangle
			core::rect<s32> outsiderect = AbsoluteRect;
			outsiderect.LowerRightCorner.X+=15;
			outsiderect.LowerRightCorner.Y+=15;
			outsiderect.UpperLeftCorner.X-=15;
			outsiderect.UpperLeftCorner.Y-=15;
			
			// Define a rectangle that is smaller than the source rectangle
			core::rect<s32> insiderect = AbsoluteRect;
			insiderect.LowerRightCorner.X-=15;
			insiderect.LowerRightCorner.Y-=15;
			insiderect.UpperLeftCorner.X+=15;
			insiderect.UpperLeftCorner.Y+=15;

			// Cursor changes if the mouse pointer is over another guielement (any other elements including childs)
			if ((Environment->getHovered()!=this) && (device->getCursorControl()->getActiveIcon()!= ECURSOR_ICON(0)) && triggered)
			{
				triggered = false;
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0)); 
				return;
			}

			// Cursor is in the defined area to reset
			if (device->getCursorControl()->getActiveIcon()!= ECURSOR_ICON(0) && outsiderect.isPointInside(mousepos) && !insiderect.isPointInside(mousepos) && triggered)
			{
				triggered = false;
				device->getCursorControl()->setActiveIcon( ECURSOR_ICON(0) );
			}
			
		}
}

//Stretch the window defined on the desired direction
//The windows will stretch but respect the min/max limits
bool CGUIExtWindow::drawStretch(core::vector2d<s32> mousepos)
{
	if (stretchright)
	{
		if (mousepos.X<10 || mousepos.X>this->getParent()->getAbsoluteClippingRect().getWidth()-10)
		return true;

		// Do only the stretching by limiting the stretch to the max/min limits of the rectangle
		if ((mousepos.X-AbsoluteRect.UpperLeftCorner.X)>=(s32)MaxSize.Width && MaxSize.Width!=0)
			AbsoluteRect.LowerRightCorner.X = oldrectangle.UpperLeftCorner.X+MaxSize.Width;
		else
			if ((mousepos.X-AbsoluteRect.UpperLeftCorner.X)<=(s32)MinSize.Width)
				AbsoluteRect.LowerRightCorner.X = oldrectangle.UpperLeftCorner.X+MinSize.Width;
			else
				AbsoluteRect.LowerRightCorner.X=mousepos.X;	
	
	}
	if (stretchleft)
	{
		if (mousepos.X<10 || mousepos.X>this->getParent()->getAbsoluteClippingRect().getWidth()-10)
		return true;

		// Do only the stretching by limiting the stretch to the max/min limits of the rectangle
		if ((oldrectangle.LowerRightCorner.X-mousepos.X)>=(s32)MaxSize.Width && MaxSize.Width!=0)
			AbsoluteRect.UpperLeftCorner.X = oldrectangle.LowerRightCorner.X-MaxSize.Width;
		else 
			if ((oldrectangle.LowerRightCorner.X-mousepos.X)<=(s32)MinSize.Width)
				AbsoluteRect.UpperLeftCorner.X = oldrectangle.LowerRightCorner.X-MinSize.Width;
			else
				AbsoluteRect.UpperLeftCorner.X=mousepos.X;	
			
		
	}
	if (stretchbottom)
	{
		if (mousepos.Y<10 || mousepos.Y>this->getParent()->getAbsoluteClippingRect().getHeight()-10)
		return true;

		// Do only the stretching by limiting the stretch to the max/min limits of the rectangle
		if ((mousepos.X-AbsoluteRect.UpperLeftCorner.Y)>=(s32)MaxSize.Height && MaxSize.Height!=0)
			AbsoluteRect.LowerRightCorner.Y = oldrectangle.UpperLeftCorner.Y+MaxSize.Height;
		else
			if ((mousepos.Y-AbsoluteRect.UpperLeftCorner.Y)<=(s32)MinSize.Height)
				AbsoluteRect.LowerRightCorner.Y = oldrectangle.UpperLeftCorner.Y+MinSize.Height;
			else
				AbsoluteRect.LowerRightCorner.Y=mousepos.Y;	
	}
	if (stretchtop)
	{
		if (mousepos.Y<10 || mousepos.Y>this->getParent()->getAbsoluteClippingRect().getHeight()-10)
		return true;

		// Do only the stretching by limiting the stretch to the max/min limits of the rectangle
		if ((AbsoluteRect.LowerRightCorner.Y-mousepos.Y)>=(s32)MaxSize.Height && MaxSize.Height!=0)
			AbsoluteRect.UpperLeftCorner.Y = oldrectangle.LowerRightCorner.Y-MaxSize.Height;
		else
			if ((AbsoluteRect.LowerRightCorner.Y-mousepos.Y)<=(s32)MinSize.Height)
				AbsoluteRect.UpperLeftCorner.Y = oldrectangle.LowerRightCorner.Y-MinSize.Height;
			else
				AbsoluteRect.UpperLeftCorner.Y=mousepos.Y;	
		
	}

	this->AbsoluteClippingRect=this->AbsoluteRect;
	this->DesiredRect=AbsoluteRect;
	this->updateAbsolutePosition();
	return false;
}

//! Returns the rectangle of the drawable area (without border, without titlebar)
core::rect<s32> CGUIExtWindow::getClientRect()
{
	return ClientRect;
}

// Initialize a expand of the left side of the window
// Will have to be expanded to allow the four direction
// Enums will be needed to define the direction
void CGUIExtWindow::Expand(irr::u16 dir)
{
	timer1 = device->getTimer()->getRealTime();
	expand=true;
}

// Initialize a retract of the left side of the window
// Will have to be expanded to allow the four direction
// Enums will be needed to define the direction
void CGUIExtWindow::Retract(irr::u16 dir)
{
	timer1 = device->getTimer()->getRealTime();
	retract=true;
}

// Give a status of the window (to determine to expand or retract)
bool CGUIExtWindow::Status(irr::u16 dir)
{
	//printf("Asked the pane status\n");
	if ((AbsoluteRect.getWidth()/2)>(irr::s32)MinSize.Width)
		return false; // Should call expand
	else
		return true; // Should call retract
}