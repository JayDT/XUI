#include "CIrrVertexDeclaration.h"

using namespace irr;
using namespace irr::video;

const SVertexElement & irr::video::VertexDeclaration::addElement(E_VERTEX_ELEMENT_SEMANTIC semanticType, E_VERTEX_ELEMENT_TYPE type, s32 offset,
    u32 semanticIndex /*= 0*/, u32 slotIndex /*= 0*/,
    u32 instanceStep /*= 0*/, bool perInstance /*= false*/)
{
    mVertexElements.push_back(SVertexElement(semanticType, type, offset, semanticIndex, slotIndex, instanceStep, perInstance));
    return mVertexElements.getLast();
}
