/*
============================================================================
Copyright (C) 2015 V.

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
// brush.cpp
#include "brush.h"

brush_c::brush_c() {	
	brush_faces = 0;
}

brush_c::~brush_c() {	
	// free faces
	face_s *next;
	for (face_s *f = brush_faces; f; f=next)
	{
		next = f->next;
		delete f;
	}
	brush_faces = 0;
}
/*
returns the visible polygon on a face
*/
texturedWinding_c *brush_c::makeFaceWinding (face_s *face)
{
#if 0
	texturedWinding_c	*w;
	face_s		*clip;
	plane_c			plane;
	bool		past;

	// V: let's try several times.
	float ranges[] = { 2048.f, 8192.f, 32768.f };
	bool bThisFaceWindingGenerationFailed = false;
	u32 numRanges = sizeof(ranges)/sizeof(ranges[0]);
	for(u32 ri = 0; ri < numRanges; ri++) {
		bThisFaceWindingGenerationFailed = false;
		float range = ranges[ri];
		// get a poly that covers an effectively infinite area
		w = new texturedWinding_c (face->plane,range);

		// chop the poly by all of the other faces
		past = false;
		u32 clipCount = 0;
		for (clip = this->getFirstFace() ; clip && w ; clip=clip->next)
		{
			clipCount++;
			if (clip == face)
			{
				past = true;
				continue;
			}
			if (face->plane.getNormal().dotProduct(clip->plane.getNormal()) > 0.999
				&& fabs(face->plane.getDist() - clip->plane.getDist()) < 0.01 )
			{	// identical plane, use the later one
				if (past)
				{
					delete (w);
					return NULL;
				}
				continue;
			}

			// flip the plane, because we want to keep the back side
			plane.norm = -clip->plane.norm;
			plane.dist = -clip->plane.dist;
			
			w = w->clip(plane, false);
			if (!w)
				return w;
		}
		
		if (w->size() < 3)
		{
			delete(w);
			w = NULL;
		}
		if(w->hasAnyCoordLargerThan(range)) {
			delete(w);
			w = NULL;
			bThisFaceWindingGenerationFailed = true;
			continue;
		}

		if (!w)
			printf ("unused plane\n");
		return w;
	}
	if(bThisFaceWindingGenerationFailed) {
		this->bWindingsGenerationFailed = true;
	}
	return 0;
#else

	texturedWinding_c	*w;
	face_s		*clip;
	plane_c			plane;
	bool		past;

	float range = 32768.f;
	// get a poly that covers an effectively infinite area
	w = new texturedWinding_c (face->plane,range);

	// chop the poly by all of the other faces
	past = false;
	u32 clipCount = 0;
	for (clip = this->getFirstFace() ; clip && w ; clip=clip->next)
	{
		clipCount++;
		if (clip == face)
		{
			past = true;
			continue;
		}
		if (face->plane.getNormal().dotProduct(clip->plane.getNormal()) > 0.999
			&& fabs(face->plane.getDist() - clip->plane.getDist()) < 0.01 )
		{	// identical plane, use the later one
			if (past)
			{
				delete (w);
				return NULL;
			}
			continue;
		}

		// flip the plane, because we want to keep the back side
		plane.norm = -clip->plane.norm;
		plane.dist = -clip->plane.dist;
		
		w = w->clip(plane, false);
		if (!w)
			return w;
	}
	
	if (w->size() < 3)
	{
		delete(w);
		w = NULL;
	}
	if(w->hasAnyCoordLargerThan(range)) {
		delete(w);
		w = NULL;
		bWindingsGenerationFailed = true;
		return 0;
	}

	if (!w)
		printf ("unused plane\n");
	return w;
#endif
}
void brush_c::setupSphere(const vec3_c &mid, u32 sides, float radius, const texdef_t *texdef) {
	float dt = float(2 * M_PI / sides);
	float dp = float(M_PI / sides);
	float t,p;
	int i,j;
	face_s *f;
	for(i=0; i <= sides-1; i++)
	{
		for(j=0;j <= sides-2; j++)
		{
			t = i * dt;
			p = float(j * dp - M_PI / 2);

			f = new face_s();
			f->texdef = *texdef;
			f->next = this->getFirstFace();
			this->brush_faces = f;

			f->planepts[0].setupPolar(radius, t, p);
			f->planepts[1].setupPolar(radius, t, p + dp);
			f->planepts[2].setupPolar(radius, t + dt, p + dp);

			for (int k = 0; k < 3; k++)
				f->planepts[k] += mid;
		}
	}

	p = float((sides - 1) * dp - M_PI / 2);
	for(i = 0; i <= sides-1; i++)
	{
		t = i * dt;

		f = new face_s();
		f->texdef = *texdef;
		f->next = this->getFirstFace();
		this->brush_faces = f;

		f->planepts[0].setupPolar(radius, t, p);
		f->planepts[1].setupPolar(radius, t + dt, p + dp);
		f->planepts[2].setupPolar(radius, t + dt, p);

		for (int k = 0; k < 3; k++)
			f->planepts[k] += mid;
	}
}

void brush_c::setupBox(const vec3_c &mins, const vec3_c &maxs, const texdef_t *texdef) {
	int		i, j;
	vec3_t	pts[4][2];
	face_s	*f;

	// free old faces
	face_s *next;
	for (face_s *f = brush_faces; f; f=next)
	{
		next = f->next;
		delete f;
	}
	brush_faces = 0;

	pts[0][0][0] = mins[0];
	pts[0][0][1] = mins[1];
	
	pts[1][0][0] = mins[0];
	pts[1][0][1] = maxs[1];
	
	pts[2][0][0] = maxs[0];
	pts[2][0][1] = maxs[1];
	
	pts[3][0][0] = maxs[0];
	pts[3][0][1] = mins[1];
	
	for (i=0 ; i<4 ; i++)
	{
		pts[i][0][2] = mins[2];
		pts[i][1][0] = pts[i][0][0];
		pts[i][1][1] = pts[i][0][1];
		pts[i][1][2] = maxs[2];
	}

	for (i=0 ; i<4 ; i++)
	{
		f = new face_s();
		f->texdef = *texdef;
		f->texdef.flags &= ~SURF_KEEP;
		f->texdef.contents &= ~CONTENTS_KEEP;
		f->next = this->getFirstFace();
		this->brush_faces = f;
		j = (i+1)%4;

		f->planepts[0] = pts[j][1];
		f->planepts[1] = pts[i][1];
		f->planepts[2] = pts[i][0];
	}
	
	f = new face_s();
	f->texdef = *texdef;
	f->texdef.flags &= ~SURF_KEEP;
	f->texdef.contents &= ~CONTENTS_KEEP;
	f->next = this->getFirstFace();
	this->brush_faces = f;

	f->planepts[0] = pts[0][1];
	f->planepts[1] = pts[1][1];
	f->planepts[2] = pts[2][1];

	f = new face_s();
	f->texdef = *texdef;
	f->texdef.flags &= ~SURF_KEEP;
	f->texdef.contents &= ~CONTENTS_KEEP;
	f->next = this->getFirstFace();
	this->brush_faces = f;

	f->planepts[0] = pts[2][0];
	f->planepts[1] = pts[1][0];
	f->planepts[2] = pts[0][0];
}
void brush_c::makeFacePlanes() {
	for (face_s	*f=this->getFirstFace(); f ; f=f->next) {
		f->calculatePlaneFromPoints();
	}
}

void brush_c::fitTexture(int nHeight, int nWidth) {
	for (face_s *face = this->getFirstFace() ; face ; face=face->next) {
		face->fitTexture(nHeight, nWidth );
	}
}

// frees any overconstraining faces
void brush_c::removeEmptyFaces () {
	face_s *f = this->getFirstFace();
	this->brush_faces = NULL;
	face_s *next;
	for ( ; f ; f=next) {
		next = f->next;
		if (!f->face_winding)
			delete f;
		else {
			f->next = this->getFirstFace();
			this->brush_faces = f;
		}
	}
}

void brush_c::snapPlanePoints() {
	for (face_s	*f = this->getFirstFace(); f; f=f->next) {
		for (u32 i=0; i < 3 ;i++) {
			for (u32 j=0; j < 3; j++) {
				f->planepts[i][j] = floor (f->planepts[i][j] + 0.5);
			}
		}
	}
}