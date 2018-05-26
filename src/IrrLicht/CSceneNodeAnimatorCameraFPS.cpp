// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorCameraFPS.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "Keycodes.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"
#include "ISceneNodeAnimatorCollisionResponse.h"
#include <math.h>
#include <include/IrrlichtDevice.h>
#include <include/ISceneCollisionManager.h>
#include "G3D\Ray.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorCameraFPS::CSceneNodeAnimatorCameraFPS(gui::ICursorControl* cursorControl,
        f32 rotateSpeed, f32 moveSpeed, f32 jumpSpeed,
        SKeyMap* keyMapArray, u32 keyMapSize, bool noVerticalMovement, bool invertY)
: CursorControl(cursorControl), MaxVerticalAngle(88.0f),
    MoveSpeed(moveSpeed), RotateSpeed(rotateSpeed), JumpSpeed(jumpSpeed),
    MouseYDirection(invertY ? -1.0f : 1.0f), direction(0.f), zdirection(-30.f), distanceToTarget(10.f), MoveNode(nullptr),
    LastAnimationTime(0), finishMove(false), firstUpdate(true), firstInput(true), NoVerticalMovement(noVerticalMovement), isLeftButtonPressed(false), isHandleEvents(false)
{
    #ifdef _DEBUG
    setDebugName("CCameraSceneNodeAnimatorFPS");
    #endif

    if (CursorControl)
        CursorControl->grab();

    allKeysUp();

    // create key map
    if (!keyMapArray || !keyMapSize)
    {
        // create default key map
        KeyMap.push_back(SKeyMap(EKA_MOVE_FORWARD,  irr::KEY_UP));
        KeyMap.push_back(SKeyMap(EKA_MOVE_BACKWARD, irr::KEY_DOWN));
        KeyMap.push_back(SKeyMap(EKA_STRAFE_LEFT,   irr::KEY_LEFT));
        KeyMap.push_back(SKeyMap(EKA_STRAFE_RIGHT,  irr::KEY_RIGHT));
        KeyMap.push_back(SKeyMap(EKA_JUMP_UP,       irr::KEY_SPACE));
        KeyMap.push_back(SKeyMap(EKA_CROUCH,        irr::KEY_KEY_X));
        
    }
    else
    {
        // create custom key map
        setKeyMap(keyMapArray, keyMapSize);
    }
}


//! destructor
CSceneNodeAnimatorCameraFPS::~CSceneNodeAnimatorCameraFPS()
{
    if (CursorControl)
        CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addFPSCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CSceneNodeAnimatorCameraFPS::OnEvent(const SEvent& evt)
{
    switch (evt.EventType)
    {
        case EET_KEY_INPUT_EVENT:
            for (u32 i = 0; i < KeyMap.size(); ++i)
            {
                if (KeyMap[i].KeyCode == evt.KeyInput.Key)
                {
                    CursorKeys[KeyMap[i].Action] = evt.KeyInput.PressedDown;
                    return true;
                }
            }
            break;

        case EET_MOUSE_INPUT_EVENT:
        {
            isLeftButtonPressed = evt.MouseInput.isLeftPressed();
            switch (evt.MouseInput.Event)
            {
                case EMIE_MOUSE_WHEEL:
                {
                    distanceToTarget += evt.MouseInput.Wheel / 4.f;
                    firstUpdate = true;
                    break;
                }
                case EMIE_MOUSE_MOVED:
                {
                    CursorPos = CursorControl->getRelativePosition();
                    return true;
                }
                case EMIE_LMOUSE_PRESSED_DOWN:
                {
                    return true;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return false;
}

void CSceneNodeAnimatorCameraFPS::animateNode(ISceneNode* node, u32 timeMs)
{
    if (!node || node->getType() != ESNT_CAMERA)
        return;

    ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

    if (firstUpdate)
    {
        camera->updateAbsolutePosition();
        if (CursorControl )
        {
            CursorControl->setPosition(0.5f, 0.5f);
            CursorPos = CenterCursor = CursorControl->getRelativePosition();
        }

        LastAnimationTime = timeMs;

        firstUpdate = false;
    }

    // If the camera isn't the active camera, and receiving input, then don't process it.
    else if(!camera->isInputReceiverEnabled())
    {
        if (!CursorControl->isVisible())
            CursorControl->setVisible(true);
        if (isHandleEvents)
            isHandleEvents = false;
        if (!firstInput)
            firstInput = true;
        if (isLeftButtonPressed)
            isLeftButtonPressed = false;

        LastAnimationTime = timeMs;

        if (finishMove)
        {
            finishMove = false;

            scene::ISceneNode* playerNode = getMoveSceneNode();
            if (playerNode)
            {
                core::vector3df playerPos = playerNode->getAbsolutePosition();
                core::vector3df playerRot = playerNode->getRotation();

                playerRot.X = 0;

                //playerNode->OnMove(this, playerPos, playerRot, 1);
                playerNode->OnIdle(this, playerPos, playerRot, 0);
            }
        }
        return;
    }

    finishMove = true;

    if (CursorControl->isVisible())
        CursorControl->setVisible(false);

    if (!isHandleEvents)
        isHandleEvents = true;

    if ( firstInput )
    {
        allKeysUp();
        firstInput = false;
    }

    scene::ISceneManager * smgr = camera->getSceneManager();
    if(smgr && smgr->getActiveCamera() != camera)
        return;

    // get time
    f32 timeDiff = std::min(1000.f, (f32) ( timeMs - LastAnimationTime ));
    LastAnimationTime = timeMs;

    // Update rotation
    core::vector3df relativeRotation;

    if (CursorControl)
    {
        if (CursorPos != CenterCursor)
        {
            relativeRotation.Y -= (0.5f - CursorPos.X) * RotateSpeed;
            relativeRotation.X -= (0.5f - CursorPos.Y) * RotateSpeed * MouseYDirection;

            // Do the fix as normal, special case below
            // reset cursor position to the centre of the window.
            CursorControl->setPosition(0.5f, 0.5f);
            CenterCursor = CursorControl->getRelativePosition();

            // needed to avoid problems when the event receiver is disabled
            CursorPos = CenterCursor;
        }

        // Special case, mouse is whipped outside of window before it can update.
        video::IVideoDriver* driver = smgr->getVideoDriver();
        core::vector2d<u32> mousepos(u32(CursorControl->getPosition().X), u32(CursorControl->getPosition().Y));
        core::rect<u32> screenRect(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height);

        // Only if we are moving outside quickly.
        bool reset = !screenRect.isPointInside(mousepos);

        if(reset)
        {
            // Force a reset.
            CursorControl->setPosition(0.5f, 0.5f);
            CenterCursor = CursorControl->getRelativePosition();
            CursorPos = CenterCursor;
        }
    }

    if (!camera->isFirstPerson())
    {
        core::vector3df cameraPos;

        direction  += relativeRotation.Y;
        zdirection -= relativeRotation.X;

        if (zdirection <-MaxVerticalAngle)
            zdirection = -MaxVerticalAngle;
        else if (zdirection > MaxVerticalAngle)
            zdirection = MaxVerticalAngle;

        float YTOffset = std::min(5.f, distanceToTarget * 0.4f);

        scene::ISceneNode* playerNode = getMoveSceneNode();
        core::vector3df playerPos = playerNode->getAbsolutePosition();
        core::vector3df playerRot = playerNode->getRotation();

        cameraPos.X = -(cos(zdirection * irr::core::PI / 180.0f) * cos(direction * irr::core::PI / 180.0f) * distanceToTarget);
        cameraPos.Y = -(sin(zdirection * core::PI / 180.0f) * distanceToTarget);
        cameraPos.Z = cos(zdirection * irr::core::PI / 180.0f) * sin(direction * irr::core::PI / 180.0f) * distanceToTarget;

        // set target
        core::vector3df target = playerPos; //YTOffset
        target.Y += YTOffset;
        target -= camera->getAbsolutePosition();
        relativeRotation += target.getHorizontalAngle();

        target.set(0, 0, core::max_(1.f, playerPos.getLength()));
        core::vector3df movedir = target;

        // For jumping, we find the collision response animator attached to our camera
        // and if it's not falling, we tell it to jump.
        if (CursorKeys[EKA_JUMP_UP])
        {
            playerNode->OnJumpOrCrouch(this, true);
            if (NoVerticalMovement)
            {
                // start jump
                CursorKeys[EKA_JUMP_UP] = false;
            }
            else
            {
                relativeRotation.X = -MaxVerticalAngle;
                if (!isLeftButtonPressed && !CursorKeys[EKA_MOVE_FORWARD] && !CursorKeys[EKA_MOVE_BACKWARD])
                    relativeRotation.Y = 0;
                else
                    relativeRotation.X *= 0.5f;
            }
        }

        if (CursorKeys[EKA_CROUCH])
        {
            playerNode->OnJumpOrCrouch(this, false);
            if (NoVerticalMovement)
            {
                // start crouch
                CursorKeys[EKA_CROUCH] = false;
            }
            else
            {
                relativeRotation.X = MaxVerticalAngle;
                if (!isLeftButtonPressed && !CursorKeys[EKA_MOVE_FORWARD] && !CursorKeys[EKA_MOVE_BACKWARD])
                    relativeRotation.Y = 0;
                else
                    relativeRotation.X *= 0.5f;
            }
        }


        core::matrix4 mat;
        mat.setRotationDegrees(core::vector3df(relativeRotation.X, relativeRotation.Y, 0));
        mat.transformVect(target);

        if (NoVerticalMovement)
        {
            mat.setRotationDegrees(core::vector3df(0, relativeRotation.Y, 0));
            mat.transformVect(movedir);
        }
        else
        {
            movedir = target;
        }

        movedir.normalize();

        // strafing
        core::vector3df strafevect = target;
        strafevect = strafevect.crossProduct(camera->getUpVector());

        if (NoVerticalMovement)
            strafevect.Y = 0.0f;

        strafevect.normalize();

        bool isOthogonal = true;
        float orient = 360 - direction; //core::radToDeg(atan2(strafevect.X, strafevect.Z));

        u32 moveflags = 0;
        if (CursorKeys[EKA_JUMP_UP] || CursorKeys[EKA_CROUCH])
        {
            if (NoVerticalMovement)
            {
                
            }
            else
            {
                float speed = (CursorKeys[EKA_MOVE_BACKWARD] ? playerNode->GetFloatData(7/*ERFD_FLIGHT_SPEED_BACK*/) : playerNode->GetFloatData(6/*ERFD_FLIGHT_SPEED*/)) * 0.001f;
                  if (speed == 0.f)
                    speed = MoveSpeed;

                if (NoVerticalMovement)
                    moveflags |= (1 << EKA_JUMP_UP);
                else
                    moveflags |= (1 << 7);
                if (isLeftButtonPressed || CursorKeys[EKA_MOVE_FORWARD] || CursorKeys[EKA_JUMP_UP] || CursorKeys[EKA_CROUCH])
                    moveflags |= (1 << EKA_MOVE_FORWARD);
                else if (CursorKeys[EKA_MOVE_BACKWARD])
                    moveflags |= (1 << EKA_MOVE_BACKWARD);
                playerPos += movedir * timeDiff * speed;
                isOthogonal = false;
            }
        }
        else if (isLeftButtonPressed || CursorKeys[EKA_MOVE_FORWARD] || CursorKeys[EKA_JUMP_UP] || CursorKeys[EKA_CROUCH])
        {
            float speed = (NoVerticalMovement ? playerNode->GetFloatData(EKA_MOVE_FORWARD) : playerNode->GetFloatData(6/*ERFD_FLIGHT_SPEED*/)) * 0.001f;
            if (speed == 0.f)
                speed = MoveSpeed;

            moveflags |= (1 << EKA_MOVE_FORWARD);
            playerPos += movedir * timeDiff * speed;
            isOthogonal = false;
        }
        else if (CursorKeys[EKA_MOVE_BACKWARD])
        {
            float speed = (NoVerticalMovement ? playerNode->GetFloatData(EKA_MOVE_BACKWARD) : playerNode->GetFloatData(7/*ERFD_FLIGHT_SPEED_BACK*/)) * 0.001f;
            if (speed == 0.f)
                speed = MoveSpeed;

            moveflags |= (1 << EKA_MOVE_BACKWARD);
            playerPos -= movedir * timeDiff * speed;
            isOthogonal = false;
        }

        if (CursorKeys[EKA_STRAFE_LEFT])
        {
            float speed = playerNode->GetFloatData(EKA_STRAFE_LEFT) * 0.001f;
            if (speed == 0.f)
                speed = MoveSpeed;

            moveflags |= (1 << EKA_STRAFE_LEFT);
            playerPos += strafevect * timeDiff * speed;
            if (isOthogonal)
                orient += 90;
            else
                orient += 45;
        }

        if (CursorKeys[EKA_STRAFE_RIGHT])
        {
            float speed = playerNode->GetFloatData(EKA_STRAFE_RIGHT) * 0.001f;
            if (speed == 0.f)
                speed = MoveSpeed;

            moveflags |= (1 << EKA_STRAFE_RIGHT);
            playerPos -= strafevect * timeDiff * speed;
            if (isOthogonal)
                orient -= 90;
            else
                orient -= 45;
        }

        if (playerNode->getPosition() != playerPos)
        {
            if (!NoVerticalMovement)
                playerRot.X = -zdirection; // MaxVerticalAngle
            playerRot.Y = orient; //360.0f - direction;
            playerNode->OnMove(this, playerPos, playerRot, moveflags);

            // Get modified position and rotation
            playerPos = playerNode->getAbsolutePosition();
            playerRot = playerNode->getRotation();
        }
        else
            playerNode->OnIdle(this, playerPos, playerRot, moveflags);

        cameraPos += playerPos;

        if (Device)
        {
           scene::ISceneCollisionManager* collisionMgr = Device->getSceneManager()->getSceneCollisionManager();
           if (collisionMgr)
           {
               playerPos.Y += 2.f;
               float maxDist = (cameraPos - playerPos).getLength();
               // valid map coords should *never ever* produce float overflow, but this would produce NaNs too
               // prevent NaN values which can cause BIH intersection to enter infinite loop
               if (maxDist > 1e-10f)
               {
                   // direction with length of 1
                   G3D::Ray _ray = G3D::Ray::fromOriginAndDirection((G3D::Vector3&)playerPos, ((G3D::Vector3&)cameraPos - (G3D::Vector3&)playerPos) / maxDist);
                   playerPos.Y -= 2.f;

                   scene::StandardIntersectionCallback cb(maxDist);
                   cb.m_ray = _ray;
                   cb.m_enableFlags = ECEF_ENABLE_DEBUG_SCENE;
                   // TODO: implement node type and can skip active models
                   cb.m_usedNode.insert(playerNode); // skip self

                   if (collisionMgr->Intersect(_ray, &cb))
                   {
                       float nearDiff = (cb.GetHitPosition() - playerPos).getLength() - 1;
                       if (nearDiff > 2.f)
                       {
                           float diff = std::min(nearDiff, camera->getNearValue());
                           cameraPos = cb.GetHitPosition(-diff);
                           YTOffset = std::min(5.f, std::max(0.5f,cb.LastHitDistance() * 0.4f));
                       }
                       else
                       {
                           YTOffset = 0.5f;
                           G3D::Vector3 pos = (G3D::Vector3&)playerPos + (camera->getNearValue() + 0.5f) * _ray.direction();
                           cameraPos = irr::core::vector3df(pos.x, pos.y, pos.z);
                       }
                   }
               }
               else
                   playerPos.Y -= 2.f;
           }
        }

        camera->setPosition(cameraPos);
        camera->setTarget(playerPos + core::vector3df(0.f, YTOffset, 0.f));
    }
    else
    {
        // update position
        core::vector3df pos = camera->getPosition();

        core::vector3df target = (camera->getTarget() - camera->getAbsolutePosition());
        relativeRotation += target.getHorizontalAngle();

        // X < MaxVerticalAngle or X > 360-MaxVerticalAngle

        if (relativeRotation.X > MaxVerticalAngle * 2 &&
            relativeRotation.X < 360.0f - MaxVerticalAngle)
        {
            relativeRotation.X = 360.0f - MaxVerticalAngle;
        }
        else if (relativeRotation.X > MaxVerticalAngle &&
            relativeRotation.X < 360.0f - MaxVerticalAngle)
        {
            relativeRotation.X = MaxVerticalAngle;
        }

        // set target

        target.set(0, 0, core::max_(1.f, pos.getLength()));
        core::vector3df movedir = target;

        core::matrix4 mat;
        mat.setRotationDegrees(core::vector3df(relativeRotation.X, relativeRotation.Y, 0));
        mat.transformVect(target);

        if (NoVerticalMovement)
        {
            mat.setRotationDegrees(core::vector3df(0, relativeRotation.Y, 0));
            mat.transformVect(movedir);
        }
        else
        {
            movedir = target;
        }

        movedir.normalize();

        if (isLeftButtonPressed || CursorKeys[EKA_MOVE_FORWARD])
            pos += movedir * timeDiff * MoveSpeed;

        if (CursorKeys[EKA_MOVE_BACKWARD])
            pos -= movedir * timeDiff * MoveSpeed;

        // strafing

        core::vector3df strafevect = target;
        strafevect = strafevect.crossProduct(camera->getUpVector());

        if (NoVerticalMovement)
            strafevect.Y = 0.0f;

        strafevect.normalize();

        if (CursorKeys[EKA_STRAFE_LEFT])
            pos += strafevect * timeDiff * MoveSpeed;

        if (CursorKeys[EKA_STRAFE_RIGHT])
            pos -= strafevect * timeDiff * MoveSpeed;

        // For jumping, we find the collision response animator attached to our camera
        // and if it's not falling, we tell it to jump.
        if (CursorKeys[EKA_JUMP_UP])
        {
            const ISceneNodeAnimatorList& animators = camera->getAnimators();
            ISceneNodeAnimatorList::ConstIterator it = animators.begin();
            while (it != animators.end())
            {
                if (ESNAT_COLLISION_RESPONSE == (*it)->getType())
                {
                    ISceneNodeAnimatorCollisionResponse * collisionResponse =
                        static_cast<ISceneNodeAnimatorCollisionResponse *>(*it);

                    if (!collisionResponse->isFalling())
                        collisionResponse->jump(JumpSpeed);
                }

                it++;
            }
        }

        // write translation
        camera->setPosition(pos);

        // write right target
        target += pos;
        camera->setTarget(target);
    }
}


void CSceneNodeAnimatorCameraFPS::setMoveSceneNode(scene::ISceneNode* node)
{
    firstUpdate = true;

    if (MoveNode)
        MoveNode->drop();

    MoveNode = node;

    if (node)
    {
        direction = node->getRotation().Y;
        node->grab();
    }
}

void CSceneNodeAnimatorCameraFPS::setPosition(const core::vector3df& newpos)
{
    //if (getMoveSceneNode())
    //{
    //    getMoveSceneNode()->OnMove(newpos, getMoveSceneNode()->getRotation(), 0);
    //}
    //ISceneNode::setPosition(newpos);
}

void CSceneNodeAnimatorCameraFPS::allKeysUp()
{
    for (u32 i=0; i<EKA_COUNT; ++i)
        CursorKeys[i] = false;
}


//! Sets the rotation speed
void CSceneNodeAnimatorCameraFPS::setRotateSpeed(f32 speed)
{
    RotateSpeed = speed;
}


//! Sets the movement speed
void CSceneNodeAnimatorCameraFPS::setMoveSpeed(f32 speed)
{
    MoveSpeed = speed;
}


//! Gets the rotation speed
f32 CSceneNodeAnimatorCameraFPS::getRotateSpeed() const
{
    return RotateSpeed;
}


// Gets the movement speed
f32 CSceneNodeAnimatorCameraFPS::getMoveSpeed() const
{
    return MoveSpeed;
}


//! Sets the keyboard mapping for this animator
void CSceneNodeAnimatorCameraFPS::setKeyMap(SKeyMap *map, u32 count)
{
    // clear the keymap
    KeyMap.clear();

    // add actions
    for (u32 i=0; i<count; ++i)
    {
        KeyMap.push_back(map[i]);
    }
}

void CSceneNodeAnimatorCameraFPS::setKeyMap(const core::array<SKeyMap>& keymap)
{
    KeyMap=keymap;
}

const core::array<SKeyMap>& CSceneNodeAnimatorCameraFPS::getKeyMap() const
{
    return KeyMap;
}


//! Sets whether vertical movement should be allowed.
void CSceneNodeAnimatorCameraFPS::setVerticalMovement(bool allow)
{
    NoVerticalMovement = !allow;
}


//! Sets whether the Y axis of the mouse should be inverted.
void CSceneNodeAnimatorCameraFPS::setInvertMouse(bool invert)
{
    if (invert)
        MouseYDirection = -1.0f;
    else
        MouseYDirection = 1.0f;
}


ISceneNodeAnimator* CSceneNodeAnimatorCameraFPS::createClone(ISceneNode* node, ISceneManager* newManager)
{
    CSceneNodeAnimatorCameraFPS * newAnimator =
        new CSceneNodeAnimatorCameraFPS(CursorControl,	RotateSpeed, MoveSpeed, JumpSpeed,
                                            0, 0, NoVerticalMovement);
    newAnimator->setKeyMap(KeyMap);
    return newAnimator;
}


} // namespace scene
} // namespace irr

