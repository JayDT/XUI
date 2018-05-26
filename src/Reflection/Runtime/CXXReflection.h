/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// WorkAround for vs 15.5
#if defined(__REFLECTION_PARSER__)
# define __is_trivially_destructible(...) false
#endif

#include "Macros.h"
#include "MetaProperty.h"

#include "Object.h"
#include "Variant.h"
#include "Type.h"

#ifndef __REFLECTION_PARSER__

#include "MetaManager.h"

#include "Enum.h"
#include "Constructor.h"
#include "Destructor.h"
#include "Field.h"
#include "Method.h"
#include "Function.h"
#include "Global.h"

#include "VariantContainer.h"
#include "ArrayWrapper.h"
#endif