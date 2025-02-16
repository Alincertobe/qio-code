/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// ed_map.h -- the state of the current world that all views are displaying
#ifndef __EDITOR_MAP_H__
#define __EDITOR_MAP_H__

#include "ed_brush.h"

extern	char		currentmap[1024];

// head/tail of doubly linked lists
extern	edBrush_c	active_brushes;	// brushes currently being displayed
extern	edBrush_c	selected_brushes;	// highlighted


extern CPtrArray g_SelectedFaces;
extern CPtrArray g_SelectedFaceBrushes;
extern	edBrush_c	filtered_brushes;	// brushes that have been filtered or regioned

extern	entity_s	entities;
extern	entity_s	*world_entity;	// the world entity is NOT included in
									// the entities chain

extern	int	modified;		// for quit confirmations

extern	class vec3_c region_mins, region_maxs;
extern	bool	region_active;

void 	Map_LoadFile (const char *filename);
void 	Map_SaveFile (const char *filename, bool use_region);
void	Map_New ();
void	Map_BuildBrushData();

void	Map_RegionOff ();
void	Map_RegionXY ();
void	Map_RegionTallBrush ();
void	Map_RegionBrush ();
void	Map_RegionSelectedBrushes ();
bool Map_IsBrushFiltered (edBrush_c *b);

void Map_SaveSelected(CMemFile* pMemFile, CMemFile* pPatchFile = NULL);
void Map_ImportBuffer (char* buf);

#endif // __EDITOR_MAP_H__
