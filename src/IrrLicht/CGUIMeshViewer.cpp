// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIMeshViewer.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IrrlichtDevice.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IAnimatedMesh.h"
#include "IMesh.h"
#include "os.h"
#include "IGUISkin.h"
#include "IHardwareBuffer.h"
#include "G3D\Matrix4.h"

namespace irr
{

namespace gui
{


//! constructor
CGUIMeshViewer::CGUIMeshViewer(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIMeshViewer(environment, parent, id, rectangle)
, Mesh(0)
, Device(nullptr)
, MouseHovered(false)
, GuiFocused(false)
{
    #ifdef _DEBUG
    setDebugName("CGUIMeshViewer");
    #endif

    viewMatrix.setTranslation(core::vector3df(0, 0, 25));
}


//! destructor
CGUIMeshViewer::~CGUIMeshViewer()
{
    if (Mesh)
        Mesh->drop();
}


//! sets the mesh to be shown
void CGUIMeshViewer::setMesh(scene::IMesh* mesh)
{
    if (mesh)
        mesh->grab();
    if (Mesh)
        Mesh->drop();

    Mesh = mesh;

    if (Mesh)
        Mesh->CreateBuffer();

    /* This might be used for proper transformation etc.
    core::vector3df center(0.0f,0.0f,0.0f);
    core::aabbox3d<f32> box;

    box = Mesh->getMesh(0)->getBoundingBox();
    center = (box.MaxEdge + box.MinEdge) / 2;
    */
}


//! Gets the displayed mesh
scene::IMesh* CGUIMeshViewer::getMesh() const
{
    return Mesh;
}


//! sets the material
void CGUIMeshViewer::setMaterial(const video::SMaterial& material)
{
    Material = material;
}


//! gets the material
const video::SMaterial& CGUIMeshViewer::getMaterial() const
{
    return Material;
}


//! called if an event happened.
bool CGUIMeshViewer::OnEvent(const SEvent& event)
{
    if ( !isEnabled() )
        return IGUIElement::OnEvent(event);

    switch ( event.EventType )
    {
        case EET_KEY_INPUT_EVENT:
        {
            if ( !MouseHovered || !GuiFocused )
                break;

            if ( event.KeyInput.PressedDown &&
                (event.KeyInput.Key == KEY_NUMPAD7 || event.KeyInput.Key == KEY_NUMPAD9) )
            {
                //G3D::Matrix4& m = (G3D::Matrix4&)viewMatrix;
                //core::vector3df rotation;
                //m.upper3x3().toEulerAnglesZYX(rotation.X, rotation.Y, rotation.Z);

                auto rotation = viewMatrix.getRotationDegrees();
                if ( rotation.X )
                    rotation.Y += (event.KeyInput.Key == KEY_NUMPAD7 ? -5 : 5);
                else
                    rotation.Y += (event.KeyInput.Key == KEY_NUMPAD7 ? 5 : -5);

                viewMatrix.setRotationDegrees(rotation);
            }

            if ( event.KeyInput.PressedDown &&
                (event.KeyInput.Key == KEY_MULTIPLY || event.KeyInput.Key == KEY_DIVIDE) )
            {
                //G3D::Matrix4& m = (G3D::Matrix4&)viewMatrix;
                //core::vector3df rotation;
                //m.upper3x3().toEulerAnglesZYX(rotation.X, rotation.Y, rotation.Z);

                auto scale = WorldViewMatrix.getScale();
                auto rotation = WorldViewMatrix.getRotationDegrees();
                //if ( rotation.X )
                //    rotation.Z += (event.KeyInput.Key == KEY_MULTIPLY ? -5 : 5);
                //else
                    rotation.X += (event.KeyInput.Key == KEY_MULTIPLY ? 5 : -5);

                core::matrix4 m;
                m.setScale(scale);
                WorldViewMatrix.setRotationDegrees(rotation);
                WorldViewMatrix *= m;
            }

            if ( event.KeyInput.PressedDown &&
                (event.KeyInput.Key == KEY_NUMPAD1 || event.KeyInput.Key == KEY_NUMPAD3) )
            {
                //G3D::Matrix4& m = (G3D::Matrix4&)viewMatrix;
                //core::vector3df rotation;
                //m.upper3x3().toEulerAnglesZYX(rotation.X, rotation.Y, rotation.Z);

                auto scale = WorldViewMatrix.getScale();
                auto rotation = WorldViewMatrix.getRotationDegrees();
                //if ( rotation.X )
                //    rotation.X += (event.KeyInput.Key == KEY_NUMPAD1 ? -5 : 5);
                //else
                    rotation.Z += (event.KeyInput.Key == KEY_NUMPAD1 ? 30 : -30);

                core::matrix4 m;
                m.setScale(scale);
                WorldViewMatrix.setRotationDegrees(rotation);
                WorldViewMatrix *= m;
            }

            if ( event.KeyInput.PressedDown &&
                (event.KeyInput.Key == KEY_ADD || event.KeyInput.Key == KEY_SUBTRACT) )
            {
                auto rotation = WorldViewMatrix.getRotationDegrees();
                auto scale = WorldViewMatrix.getScale();

                scale *= (event.KeyInput.Key == KEY_ADD ? 1.2f : (1.f / 1.2f));

                core::matrix4 m;
                m.setRotationDegrees(rotation);
                WorldViewMatrix.setScale(scale);
                WorldViewMatrix *= m;
            }

            if ( event.KeyInput.PressedDown &&
                (event.KeyInput.Key == KEY_UP || event.KeyInput.Key == KEY_DOWN) )
            {
                auto position = viewMatrix.getTranslation();
                position.Y += (event.KeyInput.Key == KEY_UP ? 1 : -1);

                viewMatrix.setTranslation(position);
            }

            if ( event.KeyInput.PressedDown &&
                (event.KeyInput.Key == KEY_LEFT || event.KeyInput.Key == KEY_RIGHT) )
            {
                auto position = viewMatrix.getTranslation();
                position.X += (event.KeyInput.Key == KEY_LEFT ? 1 : -1);

                viewMatrix.setTranslation(position);
            }
            break;
        }
        case EET_GUI_EVENT:
            if ( event.GUIEvent.Caller == this )
            {
                if ( event.GUIEvent.EventType == EGET_ELEMENT_HOVERED || event.GUIEvent.EventType == EGET_ELEMENT_LEFT )
                {
                    MouseHovered = event.GUIEvent.EventType == EGET_ELEMENT_HOVERED;
                }

                if ( event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED || event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST )
                {
                    GuiFocused = event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED;
                }
            }
            break;
        case EET_MOUSE_INPUT_EVENT:
        {
            if (!MouseHovered || !GuiFocused)
                break;

            switch (event.MouseInput.Event)
            {
                case EMIE_MOUSE_WHEEL:
                {
                    auto rotation = WorldViewMatrix.getRotationDegrees();
                    auto scale = WorldViewMatrix.getScale();

                    scale *= event.MouseInput.Wheel > 0 ? (1.f + event.MouseInput.Wheel / 4.f) : 1.f / (1.f + fabs(event.MouseInput.Wheel / 4.f));

                    core::matrix4 m;
                    m.setRotationDegrees(rotation);
                    WorldViewMatrix.setScale(scale);
                    WorldViewMatrix *= m;
                    break;
                }
                case EMIE_LMOUSE_PRESSED_DOWN:
                {
                    MouseLastPosition.set(event.MouseInput.X, event.MouseInput.Y);
                    return true;
                }
                case EMIE_MMOUSE_PRESSED_DOWN:
                {
                    MouseLastPosition.set(event.MouseInput.X, event.MouseInput.Y);
                    return true;
                }
                case EMIE_RMOUSE_PRESSED_DOWN:
                {
                    MouseLastPosition.set(event.MouseInput.X, event.MouseInput.Y);
                    return true;
                }
                case EMIE_MOUSE_MOVED:
                {
                    if (event.MouseInput.isLeftPressed())
                    {
                        auto diffX = MouseLastPosition.X - event.MouseInput.X;
                        auto rotation = viewMatrix.getRotationDegrees();
                        if (rotation.X)
                            rotation.Y += -diffX / 2.f;
                        else
                            rotation.Y += diffX / 2.f;

                        viewMatrix.setRotationDegrees(rotation);
                    }

                    if (event.MouseInput.isMiddlePressed())
                    {
                        auto diffX = event.MouseInput.X - MouseLastPosition.X;
                        auto diffY = MouseLastPosition.Y - event.MouseInput.Y;
                        auto position = viewMatrix.getTranslation();

                        position.Y += diffY / 10.f; // Height
                        position.X += diffX / 10.f; // Width

                        viewMatrix.setTranslation(position);
                    }

                    if (event.MouseInput.isRightPressed())
                    {
                        auto scale = WorldViewMatrix.getScale();

                        auto diffX = event.MouseInput.X - MouseLastPosition.X;
                        auto diffY = MouseLastPosition.Y - event.MouseInput.Y;

                        auto rotation = WorldViewMatrix.getRotationDegrees();
                        rotation.X += diffX / 2.f;
                        rotation.Z += diffY / 2.f;

                        core::matrix4 m;
                        m.setScale(scale);
                        WorldViewMatrix.setRotationDegrees(rotation);
                        WorldViewMatrix *= m;
                    }

                    MouseLastPosition.set(event.MouseInput.X, event.MouseInput.Y);
                    //Device->getCursorControl()
                    //if (reset)
                    //{
                    //    // Force a reset.
                    //    CursorControl->setPosition(0.5f, 0.5f);
                    //    CenterCursor = CursorControl->getRelativePosition();
                    //    CursorPos = CenterCursor;
                    //}
                    return true;
                    break;
                }
            }
            break;
        }
        default:
            break;
    }

    return false;
}


//! draws the element and its children
void CGUIMeshViewer::draw()
{
    if (!IsVisible)
        return;

    IGUISkin* skin = Environment->getSkin();
    video::IVideoDriver* driver = Environment->getVideoDriver();
    core::rect<s32> viewPort = AbsoluteRect;
    viewPort.LowerRightCorner.X -= 1;
    viewPort.LowerRightCorner.Y -= 1;
    viewPort.UpperLeftCorner.X += 1;
    viewPort.UpperLeftCorner.Y += 1;

    viewPort.clipAgainst(AbsoluteClippingRect);

    // draw the frame

    core::rect<s32> frameRect(AbsoluteRect);
    frameRect.LowerRightCorner.Y = frameRect.UpperLeftCorner.Y + 1;
    skin->draw2DRectangle(this, skin->getColor(EGDC_3D_SHADOW), frameRect, &AbsoluteClippingRect);

    frameRect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
    frameRect.LowerRightCorner.X = frameRect.UpperLeftCorner.X + 1;
    skin->draw2DRectangle(this, skin->getColor(EGDC_3D_SHADOW), frameRect, &AbsoluteClippingRect);

    frameRect = AbsoluteRect;
    frameRect.UpperLeftCorner.X = frameRect.LowerRightCorner.X - 1;
    skin->draw2DRectangle(this, skin->getColor(EGDC_3D_HIGH_LIGHT), frameRect, &AbsoluteClippingRect);

    frameRect = AbsoluteRect;
    frameRect.UpperLeftCorner.Y = AbsoluteRect.LowerRightCorner.Y - 1;
    skin->draw2DRectangle(this, skin->getColor(EGDC_3D_HIGH_LIGHT), frameRect, &AbsoluteClippingRect);

    // draw the mesh

    if (Mesh && Device )
    {
        //if (Mesh->toIAnimateMesh() && Mesh->toIAnimateMesh()->OnInvoke)
        //    Mesh->toIAnimateMesh()->OnInvoke();

        //TODO: if outside of screen, dont draw.
        // - why is the absolute clipping rect not already the screen?

        auto CameraControl = Device->getSceneManager()->getActiveCamera();

        core::rect<s32> oldViewPort = driver->getViewPort();

        driver->setViewPort(viewPort);
        driver->clearZBuffer(); //setRenderTarget(nullptr);

        driver->setTransform(video::ETS_PROJECTION, CameraControl->getProjectionMatrix());
        driver->setTransform(video::ETS_VIEW, viewMatrix);
        driver->setTransform(video::ETS_WORLD, WorldViewMatrix);

        driver->setMaterial(Material);

        u32 frame = 0;
        //if(Mesh->getFrameCount())
        //	frame = (os::Timer::getTime()/20)%Mesh->getFrameCount();
        scene::IMesh* m = Mesh;
        Mesh->setInstanceMode(false);
        if (Mesh->toIAnimateMesh())
        {
            scene::IMesh* _m = Mesh->toIAnimateMesh()->getMesh(os::Timer::getTime());
            if (_m)
                m = _m;
        }

        for (u32 i=0; i<m->getMeshBufferCount(); ++i)
        {
            scene::IMeshBuffer* mb = m->getMeshBuffer(i);
            if ( !mb )
                continue;

            u32 subNum = mb->GetSubBufferCount() ? mb->GetSubBufferCount() : 1;
            for ( u32 i = 0; i < subNum; ++i )
            {
                if (mb->GetSubBufferCount() && !mb->IsAvailableSubBuffer(i))
                    continue;

                mb->SetActiveSubBuffer(i);
                video::SMaterial m = mb->getMaterial();
                m.Lighting = false;
                m.FogEnable = false;
                //m.ZBuffer = true;
                //m.ZWriteEnable = true;
                driver->setMaterial(m);
                driver->drawMeshBuffer(mb, getMesh(), nullptr);
                //drawVertexPrimitiveList(mb->getVertices(),
                //	mb->getVertexCount(), mb->getIndices(),
                //	mb->getIndexCount()/ 3, mb->getVertexType(),
                //	scene::EPT_TRIANGLES, mb->getIndexType());
            }

            if ( mb->GetHWBuffer() )
                mb->GetHWBuffer()->Unbind();
        }

        if (Mesh->toIAnimateMesh())
            Mesh->toIAnimateMesh()->RenderAttachements(WorldViewMatrix);

        Mesh->setInstanceMode(true);
        driver->setViewPort(oldViewPort);
    }

    IGUIElement::draw();
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

