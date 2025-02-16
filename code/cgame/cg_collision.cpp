/*
============================================================================
Copyright (C) 2012 V.

This file is part of Qio source code.

Qio source code is free software; you can redistribute it 
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

Qio source code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA,
or simply visit <http://www.gnu.org/licenses/>.
============================================================================
*/
// cg_collision.cpp - clientside-only collision detection system.
// Usefull for 3rd person camera clipping, local entities and particles.
#include "cg_local.h"
#include "cg_entities.h"
#include <api/rAPI.h>
#include <api/rEntityAPI.h>
#include <shared/trace.h>

bool CG_RayTrace(class trace_c &tr, u32 skipEntNum) {
	if(rf->rayTraceWorldMap(tr)) {
		tr.setHitCGEntity(&cg_entities[ENTITYNUM_WORLD]);	
	}
	for(u32 i = 0; i < MAX_GENTITIES; i++) {
		cgEntity_c *cent = &cg_entities[i];
		// skipEntityNum is also compared against the parent number of the entity
		if(cent->rayTrace(tr,skipEntNum)) {
			tr.setHitCGEntity(cent);
		}
	}
	return tr.hasHit();
}





