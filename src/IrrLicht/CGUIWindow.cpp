// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIWindow.h"
#include <include/IForm.h>
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIButton.h"
#include "IGUIFont.h"
#include "IGUIFontBitmap.h"
#include "IrrlichtDevice.h"
#include <include/IGUITabControl.h>

namespace irr
{
namespace gui
{

//! constructor
CGUIWindow::CGUIWindow(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIWindow(environment, parent, id, rectangle)
, OriginalWindowSize(rectangle)
, Dragging(false)
, IsDraggable(true)
, DrawBackground(true)
, DrawTitlebar(true)
, IsActive(false)
, OnResizing(false)
, IsResizeable(true)
, isDocked(false)
, isCanDockHide(false)
, OnResizeMode(0)
, DockInfo(nullptr)
{
    #ifdef _DEBUG
    setDebugName("CGUIWindow");
    #endif

    IGUISkin* skin = 0;
    if (environment)
        skin = environment->getSkin();

    CurrentIconColor = video::SColor(255,255,255,255);

    s32 buttonw = 15;
    if (skin)
    {
        buttonw = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);
    }
    s32 posx = RelativeRect.getWidth() - buttonw - 4;

    CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1,
        L"", skin ? skin->getDefaultText(EGDT_WINDOW_CLOSE) : L"Close" );
    CloseButton->setSubElement(true);
    CloseButton->setTabStop(false);
    CloseButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
    posx -= buttonw + 2;

    RestoreButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1,
        L"", skin ? skin->getDefaultText(EGDT_WINDOW_RESTORE) : L"Restore" );
    RestoreButton->setVisible(true);
    RestoreButton->setSubElement(true);
    RestoreButton->setTabStop(false);
    RestoreButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
    posx -= buttonw + 2;

    MinButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1,
        L"", skin ? skin->getDefaultText(EGDT_WINDOW_MINIMIZE) : L"Minimize" );
    MinButton->setVisible(true);
    MinButton->setSubElement(true);
    MinButton->setTabStop(false);
    MinButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

    PinButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1,
        L"", L"Pin");
    PinButton->setVisible(false);
    PinButton->setSubElement(true);
    PinButton->setTabStop(false);
    PinButton->setSubElement(true);
    PinButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

    PinButton->setSpriteBank(skin->getSpriteBank());
    PinButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(EGDC_WINDOW_SYMBOL));
    PinButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(EGDC_WINDOW_SYMBOL));

    s32 th = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);
    ResizeButton = environment->addButton(core::rect<s32>(rectangle.getWidth() - (th + 1), rectangle.getHeight() - (th + 1), rectangle.getWidth() - 1, rectangle.getHeight() - 1), this);
    ResizeButton->setDrawBorder(false);
    ResizeButton->setEnabled(false);
    ResizeButton->setSpriteBank(skin->getSpriteBank());
    ResizeButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_RESIZE), skin->getColor(EGDC_WINDOW_SYMBOL));
    ResizeButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_RESIZE), skin->getColor(EGDC_WINDOW_SYMBOL));
    ResizeButton->grab();
    ResizeButton->setSubElement(true);
    ResizeButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

    MinButton->grab();
    RestoreButton->grab();
    CloseButton->grab();
    PinButton->grab();

    // this element is a tab group
    setTabGroup(true);
    setTabStop(true);
    setTabOrder(-1);

    refreshSprites();
    updateClientRect();
}


//! destructor
CGUIWindow::~CGUIWindow()
{
    if (MinButton)
        MinButton->drop();

    if (RestoreButton)
        RestoreButton->drop();

    if (CloseButton)
        CloseButton->drop();

    if (PinButton)
        PinButton->drop();
}

void CGUIWindow::refreshSprites()
{
    if (!Environment)
        return;
    IGUISkin* skin  = Environment->getSkin();
    if ( !skin )
        return;

    IGUISpriteBank* sprites = skin->getSpriteBank();
    if ( !sprites )
        return;

    CurrentIconColor = skin->getColor(isEnabled() ? EGDC_WINDOW_SYMBOL : EGDC_GRAY_WINDOW_SYMBOL);

    if (sprites)
    {
        CloseButton->setSpriteBank(sprites);
        CloseButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_CLOSE), CurrentIconColor);
        CloseButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_CLOSE), CurrentIconColor);

        RestoreButton->setSpriteBank(sprites);
        RestoreButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_RESTORE), CurrentIconColor);
        RestoreButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_RESTORE), CurrentIconColor);

        MinButton->setSpriteBank(sprites);
        MinButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_MINIMIZE), CurrentIconColor);
        MinButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_MINIMIZE), CurrentIconColor);
    }
}

void CGUIWindow::setDockState(bool on, DockedElementInfo* dockInfo)
{
    isDocked = on;

    if (on)
        DockInfo = dockInfo;
    else
        DockInfo = nullptr;

    MinButton->setVisible(!on);
    PinButton->setVisible(on);
}

void CGUIWindow::DoDragging()
{    
    DragStart = Environment->GetDevice()->getCursorControl()->getPosition();

    isCanBeDockedToTabControl = (getElementFromType(EGUIET_TAB_CONTROL) || getElementFromType(EGUIET_TAB));

    Environment->showDockIcons(IsDraggable && IsDockable());
    Dragging = IsDraggable;
    Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZEALL);
    if (Parent)
        Parent->bringToFront(this);
}

    //! called if an event happened.
bool CGUIWindow::OnEvent(const SEvent& event)
{
    if (GetParentForm() && GetParentForm()->GetEventReceiver())
    {
        if (GetParentForm()->GetEventReceiver()->OnEvent(event))
            return true;
    }

    if (isEnabled())
    {
        switch(event.EventType)
        {
        case EET_GUI_EVENT:
            if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
            {
                if (IsDocked() && IsCanDockHide() && !isMyChild(Environment->getHovered()) && Environment->getHovered() != this)
                {
                    setVisible(false);
                }

                Dragging = false;
                IsActive = false;
            }
            else if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED)
            {
                if (Parent && ((event.GUIEvent.Caller == this) || isMyChild(event.GUIEvent.Caller)))
                {
                    Parent->bringToFront(this);
                    IsActive = true;
                }
                else
                {
                    IsActive = false;
                }
            }
            else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
            {
                if (event.GUIEvent.Caller == CloseButton)
                {
                    if (Parent)
                    {
                        // send close event to parent
                        SEvent e;
                        e.EventType = EET_GUI_EVENT;
                        e.GUIEvent.Caller = this;
                        e.GUIEvent.Element = 0;
                        e.GUIEvent.EventType = EGET_ELEMENT_CLOSED;

                        GUIEventArg arg;
                        arg.eData = e;
                        if (OnClose)
                            OnClose(this, arg);

                        // if the event was not absorbed
                        if (!arg.eHandled && !Parent->OnEvent(e))
                            remove();

                        return true;
                    }
                    else
                    {
                        GUIEventArg arg;
                        if (OnClose)
                            OnClose(this, arg);

                        // if the event was not absorbed
                        if (!arg.eHandled)
                            remove();
                        return true;
                    }
                }

                if (event.GUIEvent.Caller == MinButton)
                {
                    auto rect = getRelativePosition();
                    if (!isMinimalized)
                    {
                        isMinimalized = true;
                        OriginalWindowSize = rect;
                        IGUISkin* skin = Environment->getSkin();
                        rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;
                        setRelativePosition(rect);
                        RemoveActionModeFlags(EAMF_RESIZE_ALL);
                    }
                    else
                    {
                        isMinimalized = false;
                        AddActionModeFlags(EAMF_RESIZE_ALL);
                        rect.LowerRightCorner.Y += OriginalWindowSize.LowerRightCorner.Y - OriginalWindowSize.UpperLeftCorner.Y;
                        setRelativePosition(rect);
                    }
                }

                if (event.GUIEvent.Caller == PinButton)
                {
                    CanDockHide(!IsCanDockHide());
                    // return if we have no skin.
                    IGUISkin *skin = Environment->getSkin();
                    if (!skin)
                        break;

                    if (IsCanDockHide())
                    {
                        PinButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_LEFT), skin->getColor(EGDC_WINDOW_SYMBOL));
                        PinButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_LEFT), skin->getColor(EGDC_WINDOW_SYMBOL));
                    }
                    else
                    {
                        PinButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(EGDC_WINDOW_SYMBOL));
                        PinButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_CURSOR_DOWN), skin->getColor(EGDC_WINDOW_SYMBOL));
                    }

                    GUIEventArg arg;
                    arg.eData = event;
                    if (OnValueChanged)
                        OnValueChanged(this, arg);
                }
            }
            break;
        case EET_MOUSE_INPUT_EVENT:
            switch(event.MouseInput.Event)
            {
            case EMIE_LMOUSE_PRESSED_DOWN:
            {
                DragStart.X = event.MouseInput.X;
                DragStart.Y = event.MouseInput.Y;

                IGUISkin* skin = Environment->getSkin();
                core::rect<s32> rect = AbsoluteRect;
                // title bar area
                rect.UpperLeftCorner.X += 2;
                rect.UpperLeftCorner.Y += 2;
                rect.LowerRightCorner.X -= 2;
                rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 2;

                if (rect.isPointInside(DragStart))
                {
                    DoDragging();
                }
                else if (IsResizeable)
                {
                    auto& rect = getAbsolutePosition();

                    // TOP
                    if (HasActionModeFlags(EAMF_RESIZE_T) && abs(event.MouseInput.Y - rect.UpperLeftCorner.Y) < 4
                        && event.MouseInput.X > rect.UpperLeftCorner.X
                        && event.MouseInput.X < rect.LowerRightCorner.X)
                    {
                        OnResizing = true;
                        OnResizeMode = EAMF_RESIZE_T;
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENS);
                    }

                    // BOTTOM
                    if (HasActionModeFlags(EAMF_RESIZE_B) && abs(event.MouseInput.Y - rect.LowerRightCorner.Y) < 4
                        && event.MouseInput.X > rect.UpperLeftCorner.X
                        && event.MouseInput.X < rect.LowerRightCorner.X)
                    {
                        OnResizing = true;
                        OnResizeMode = EAMF_RESIZE_B;
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENS);
                    }

                    // LEFT
                    if (HasActionModeFlags(EAMF_RESIZE_L) && abs(event.MouseInput.X - rect.UpperLeftCorner.X) < 4
                        && event.MouseInput.Y > rect.UpperLeftCorner.Y
                        && event.MouseInput.Y < rect.LowerRightCorner.Y)
                    {
                        OnResizing = true;
                        OnResizeMode = EAMF_RESIZE_L;
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZEWE);
                    }

                    // RIGHT
                    if (HasActionModeFlags(EAMF_RESIZE_R) && abs(event.MouseInput.X - rect.LowerRightCorner.X) < 4
                        && event.MouseInput.Y > rect.UpperLeftCorner.Y
                        && event.MouseInput.Y < rect.LowerRightCorner.Y)
                    {
                        OnResizing = true;
                        OnResizeMode = EAMF_RESIZE_R;
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZEWE);
                    }

                    auto mousepoint = core::vector2di(event.MouseInput.X, event.MouseInput.Y);
                    auto diff = (mousepoint - rect.LowerRightCorner).getLength();
                    if (HasActionModeFlags(EAMF_RESIZE_BR) && diff < 5)
                    {
                        OnResizing = true;
                        OnResizeMode = EAMF_RESIZE_BR;
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENWSE);
                    }

                    diff = (mousepoint - rect.UpperLeftCorner).getLength();
                    if (HasActionModeFlags(EAMF_RESIZE_TL) && diff < 5)
                    {
                        OnResizing = true;
                        OnResizeMode = EAMF_RESIZE_TL;
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENWSE);
                    }

                    if (OnResizing)
                    {
                        auto rrect = getRelativePosition();
                        ResizingRect = rrect;
                    }
                }

                if (Parent)
                    Parent->bringToFront(this);
                return true;
            }
            case EMIE_LMOUSE_LEFT_UP:
            {
                bool changed = Dragging || OnResizing;
                if (Dragging)
                {
                    auto point = core::position2di(event.MouseInput.X, event.MouseInput.Y);

                    auto targetElement = Environment->getRootGUIElement()->getElementFromPoint(point, this);

                    if (isCanBeDockedToTabControl
                        && targetElement
                        && targetElement
                        && targetElement->getType() == EGUIET_TAB_CONTROL
                        && targetElement != this
                        && !isMyChild(targetElement))
                    {
                        auto DockedElement = getElementFromType(EGUIET_TAB_CONTROL);
                        if (!DockedElement)
                            DockedElement = getElementFromType(EGUIET_TAB);

                        if (DockedElement)
                        {
                            IGUITabControl* dockcontrol = (IGUITabControl*)targetElement;
                            if (DockedElement->getType() == EGUIET_TAB_CONTROL)
                            {

                            }
                            else if (DockedElement->getType() == EGUIET_TAB)
                            {
                                auto GrabTab = (IGUITab*)DockedElement;
                                GrabTab->grab();
                                if (dockcontrol->insertTabByPostion(core::vector2di(event.MouseInput.X, event.MouseInput.Y), GrabTab))
                                {
                                    GrabTab->drop();
                                    remove();
                                    return false;
                                }
                                GrabTab->drop();
                            }
                        }
                    }

                    Environment->DockByPoint(DragStart, this);
                    Environment->showDockIcons(false);
                    Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_NORMAL);
                }

                if (OnResizing)
                {
                    setRelativePosition(ResizingRect);
                    updateAbsolutePosition();
                    OriginalWindowSize = getRelativePosition();
                    Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_NORMAL);
                }

                Dragging = false;
                OnResizing = false;

                if (changed)
                {
                    GUIEventArg arg;
                    arg.eData = event;
                    if (OnValueChanged)
                        OnValueChanged(this, arg);
                }
                return true;
            }
            case EMIE_MOUSE_MOVED:
            {
                if (!event.MouseInput.isLeftPressed())
                {
                    if (OnResizing)
                    {
                        setRelativePosition(ResizingRect);
                        updateAbsolutePosition();
                        OriginalWindowSize = getRelativePosition();
                    }

                    Environment->showDockIcons(false);
                    Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_NORMAL);

                    Dragging = false;
                    OnResizing = false;
                }

                if (Dragging)
                {
                    auto point = core::position2di(event.MouseInput.X, event.MouseInput.Y);

                    // gui window should not be dragged outside its parent
                    if (Parent && !Parent->getAbsolutePosition().isPointInside(point))
                        return true;

                    auto diff = point - DragStart;
                    if (IsDocked())
                    {
                        if (diff.getLength() < 10)
                            return true;
                    }
                    move(diff);
                    if (IsDocked())
                    {
                        _IRR_DEBUG_BREAK_IF(!getParent());
                        getParent()->DockGUIElement(EWD_UNDOCKED, this);
                    }
                    DragStart.X = event.MouseInput.X;
                    DragStart.Y = event.MouseInput.Y;
                    return true;
                }
                else if (OnResizing)
                {
                    switch (OnResizeMode)
                    {
                    case EAMF_RESIZE_T:
                        ResizingRect.UpperLeftCorner.Y = event.MouseInput.Y;
                        break;
                    case EAMF_RESIZE_B:
                        ResizingRect.LowerRightCorner.Y = event.MouseInput.Y;
                        break;
                    case EAMF_RESIZE_L:
                        ResizingRect.UpperLeftCorner.X = event.MouseInput.X;
                        break;
                    case EAMF_RESIZE_R:
                        ResizingRect.LowerRightCorner.X = event.MouseInput.X;
                        break;
                    case EAMF_RESIZE_TL:
                        ResizingRect.UpperLeftCorner.X = event.MouseInput.X;
                        ResizingRect.UpperLeftCorner.Y = event.MouseInput.Y;
                        break;
                    case EAMF_RESIZE_TR:
                        ResizingRect.UpperLeftCorner.Y = event.MouseInput.Y;
                        ResizingRect.LowerRightCorner.X = event.MouseInput.X;
                        break;
                    case EAMF_RESIZE_BL:
                        ResizingRect.UpperLeftCorner.X = event.MouseInput.X;
                        ResizingRect.LowerRightCorner.Y = event.MouseInput.Y;
                        break;
                    case EAMF_RESIZE_BR:
                        ResizingRect.LowerRightCorner.X = event.MouseInput.X;
                        ResizingRect.LowerRightCorner.Y = event.MouseInput.Y;
                        break;
                    }
                    //DragStart.X = event.MouseInput.X;
                    //DragStart.Y = event.MouseInput.Y;
                    //ResizingRect.LowerRightCorner = DragStart;
                }
                else
                {
                    auto& rect = getAbsolutePosition();

                    auto mousepoint = core::vector2di(event.MouseInput.X, event.MouseInput.Y);
                    if (HasActionModeFlags(EAMF_RESIZE_BR) && (mousepoint - rect.LowerRightCorner).getLength() < 5)
                    {
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENWSE);
                    }
                    else if (HasActionModeFlags(EAMF_RESIZE_TL) && (mousepoint - rect.UpperLeftCorner).getLength() < 5)
                    {
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENWSE);
                    }
                    // TOP
                    else if (HasActionModeFlags(EAMF_RESIZE_T) && abs(event.MouseInput.Y - rect.UpperLeftCorner.Y) < 4
                        && event.MouseInput.X > rect.UpperLeftCorner.X 
                        && event.MouseInput.X < rect.LowerRightCorner.X)
                    {
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENS);
                    }

                    // BOTTOM
                    else if (HasActionModeFlags(EAMF_RESIZE_B) && abs(event.MouseInput.Y - rect.LowerRightCorner.Y) < 4
                        && event.MouseInput.X > rect.UpperLeftCorner.X
                        && event.MouseInput.X < rect.LowerRightCorner.X)
                    {
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZENS);
                    }

                    // LEFT
                    else if (HasActionModeFlags(EAMF_RESIZE_L) && abs(event.MouseInput.X - rect.UpperLeftCorner.X) < 4
                        && event.MouseInput.Y > rect.UpperLeftCorner.Y
                        && event.MouseInput.Y < rect.LowerRightCorner.Y)
                    {
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZEWE);
                    }

                    // RIGHT
                    else if (HasActionModeFlags(EAMF_RESIZE_R) && abs(event.MouseInput.X - rect.LowerRightCorner.X) < 4
                        && event.MouseInput.Y > rect.UpperLeftCorner.Y
                        && event.MouseInput.Y < rect.LowerRightCorner.Y)
                    {
                        Environment->GetDevice()->getCursorControl()->setActiveIcon(ECI_SIZEWE);
                    }
                }
                break;
            }
            default:
                break;
            }
        default:
            break;
        }
    }

    return IGUIElement::OnEvent(event);
}


//! Updates the absolute position.
void CGUIWindow::updateAbsolutePosition()
{
    IGUIElement::updateAbsolutePosition();
}


//! draws the element and its children
void CGUIWindow::draw()
{
    if (IsVisible)
    {
        IGUISkin* skin = Environment->getSkin();


        // update each time because the skin is allowed to change this always.
        updateClientRect();

        if ( CurrentIconColor != skin->getColor(isEnabled() ? EGDC_WINDOW_SYMBOL : EGDC_GRAY_WINDOW_SYMBOL) )
            refreshSprites();

        core::rect<s32> rect = AbsoluteRect;

        // draw body fast
        if (DrawBackground)
        {
            rect = skin->draw3DWindowBackground(this, DrawTitlebar,
                    skin->getColor(IsActive ? EGDC_ACTIVE_BORDER : EGDC_INACTIVE_BORDER),
                    AbsoluteRect, &AbsoluteClippingRect);

            if (DrawTitlebar && Text.size())
            {
                rect.UpperLeftCorner.X += skin->getSize(EGDS_TITLEBARTEXT_DISTANCE_X);
                rect.UpperLeftCorner.Y += skin->getSize(EGDS_TITLEBARTEXT_DISTANCE_Y);
                rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

                IGUIFont* font = skin->getFont(EGDF_WINDOW);
                if (font)
                {
                    font->draw(Text.c_str(), rect,
                            skin->getColor(IsActive ? EGDC_ACTIVE_CAPTION:EGDC_INACTIVE_CAPTION),
                            false, true, &AbsoluteClippingRect);
                }
            }
        }

        if (OnResizing)
        {
            auto color = skin->getColor(IsActive ? EGDC_ACTIVE_BORDER : EGDC_INACTIVE_BORDER);
            color.setAlpha(50);
            skin->draw2DRectangle(this, color, ResizingRect);
        }
    }

    IGUIElement::draw();
}


//! Returns pointer to the close button
IGUIButton* CGUIWindow::getCloseButton() const
{
    return CloseButton;
}


//! Returns pointer to the minimize button
IGUIButton* CGUIWindow::getMinimizeButton() const
{
    return MinButton;
}


//! Returns pointer to the maximize button
IGUIButton* CGUIWindow::getMaximizeButton() const
{
    return RestoreButton;
}


//! Returns true if the window is draggable, false if not
bool CGUIWindow::isDraggable() const
{
    return IsDraggable;
}


//! Sets whether the window is draggable
void CGUIWindow::setDraggable(bool draggable)
{
    IsDraggable = draggable;

    if (Dragging && !IsDraggable)
        Dragging = false;
}


//! Set if the window background will be drawn
void CGUIWindow::setDrawBackground(bool draw)
{
    DrawBackground = draw;

    if (MinButton)
        MinButton->setVisible(draw);

    if (RestoreButton)
        RestoreButton->setVisible(draw);

    if (CloseButton)
        CloseButton->setVisible(draw);
}


//! Get if the window background will be drawn
bool CGUIWindow::getDrawBackground() const
{
    return DrawBackground;
}


//! Set if the window titlebar will be drawn
void CGUIWindow::setDrawTitlebar(bool draw)
{
    DrawTitlebar = draw;
}


//! Get if the window titlebar will be drawn
bool CGUIWindow::getDrawTitlebar() const
{
    return DrawTitlebar;
}


void CGUIWindow::updateClientRect()
{
    if (! DrawBackground )
    {
        ClientRect = core::rect<s32>(0,0, AbsoluteRect.getWidth(), AbsoluteRect.getHeight());
        return;
    }
    IGUISkin* skin = Environment->getSkin();
    skin->draw3DWindowBackground(this, DrawTitlebar,
            skin->getColor(IsActive ? EGDC_ACTIVE_BORDER : EGDC_INACTIVE_BORDER),
            AbsoluteRect, &AbsoluteClippingRect, &ClientRect);
    ClientRect -= AbsoluteRect.UpperLeftCorner;
}


//! Returns the rectangle of the drawable area (without border, without titlebar and without scrollbars)
core::rect<s32> CGUIWindow::getClientRect() const
{
    return ClientRect;
}


//! Writes attributes of the element.
void CGUIWindow::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
    IGUIWindow::serializeAttributes(out,options);

    out->addBool("IsDraggable", IsDraggable);
    out->addBool("DrawBackground", DrawBackground);
    out->addBool("DrawTitlebar", DrawTitlebar);

    // Currently we can't just serialize attributes of sub-elements.
    // To do this we either
    // a) allow further serialization after attribute serialiation (second function, callback or event)
    // b) add an IGUIElement attribute
    // c) extend the attribute system to allow attributes to have sub-attributes
    // We just serialize the most important info for now until we can do one of the above solutions.
    out->addBool("IsCloseVisible", CloseButton->isVisible());
    out->addBool("IsMinVisible", MinButton->isVisible());
    out->addBool("IsRestoreVisible", RestoreButton->isVisible());
}


//! Reads attributes of the element
void CGUIWindow::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
    IGUIWindow::deserializeAttributes(in,options);

    Dragging = false;
    IsActive = false;
    IsDraggable = in->getAttributeAsBool("IsDraggable");
    DrawBackground = in->getAttributeAsBool("DrawBackground");
    DrawTitlebar = in->getAttributeAsBool("DrawTitlebar");

    CloseButton->setVisible(in->getAttributeAsBool("IsCloseVisible"));
    MinButton->setVisible(in->getAttributeAsBool("IsMinVisible"));
    RestoreButton->setVisible(in->getAttributeAsBool("IsRestoreVisible"));

    updateClientRect();
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

