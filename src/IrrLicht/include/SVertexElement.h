// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_VERTEX_ELEMENT_H_INCLUDED__
#define __S_VERTEX_ELEMENT_H_INCLUDED__

#include "irrTypes.h"
#include "irrString.h"

namespace irr
{
namespace video
{

//! Enumeration for all vertex types there are.
enum E_VERTEX_ELEMENT_SEMANTIC
{
	EVES_POSITION = 0,
	
	EVES_NORMAL,
	
	EVES_TEXTURE_COORD,
	
	EVES_COLOR,
	
	EVES_TANGENT,
	
	EVES_BINORMAL,
	
	EVES_BLEND_WEIGHTS,
	
	EVES_BLEND_INDICES,
	
	EVES_COUNT = 0x7fffffff
};

enum E_VERTEX_ELEMENT_TYPE
{
	EVET_FLOAT1 = 0,
	EVET_FLOAT2,
	EVET_FLOAT3,
	EVET_FLOAT4,

	EVET_UINT1,
	EVET_UINT2,
	EVET_UINT3,
	EVET_UINT4,
	
	EVET_SHORT2,
	EVET_SHORT4,
	
	EVET_UBYTE4,
	
	EVET_COLOUR,
	
	EVET_COUNT = 0x7fffffff
};

struct SVertexElement
{
	// Constructor
	SVertexElement(E_VERTEX_ELEMENT_SEMANTIC semanticType, E_VERTEX_ELEMENT_TYPE type,
				   u32 semanticIndex = 0, u32 slotIndex = 0, 
				   u32 instanceStep = 0, bool perInstance = false)
	: SemanticType(semanticType)
	, Type(type)
	, SemanticIndex(semanticIndex)
	, SlotIndex(slotIndex)
	, InstanceStep(instanceStep)
	, PerInstance(perInstance)
	{

	}

	// Choose one of these to name the semantic
	E_VERTEX_ELEMENT_SEMANTIC SemanticType;
	
	// Type of data
	E_VERTEX_ELEMENT_TYPE Type;
	
	// How much elements with this semantic
	u32 SemanticIndex;
	
	// Element provided by which input buffer?
	u32 SlotIndex;

	// Index of instance
	u32 InstanceStep;

	// Is this element per instance?
	bool PerInstance;
};



}
}

#endif