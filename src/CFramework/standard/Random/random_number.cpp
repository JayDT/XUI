/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "standard/Random/random_number.h"
#include "standard/Random/SFMT.h"
#include "Threading/Threading.h"

typedef System::Threading::TSSObject<SFMTRand> SFMTRandTSS;
static SFMTRandTSS sfmtRand;

int32 irand(int32 min, int32 max)
{
    return int32(sfmtRand->IRandom(min, max));
}

uint32 urand(uint32 min, uint32 max)
{
    return sfmtRand->URandom(min, max);
}

float frand(float min, float max)
{
    return float(sfmtRand->Random() * (max - min) + min);
}

int32 rand32()
{
    return int32(sfmtRand->BRandom());
}

double rand_norm(void)
{
    return sfmtRand->Random();
}

float rand_norm_f(void)
{
    return (float)sfmtRand->Random();
}

double rand_chance(void)
{
    return sfmtRand->Random() * 100.0;
}

float rand_chance_f(void)
{
    return (float)(sfmtRand->Random() * 100.0);
}
