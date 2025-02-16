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
// staticModelCreatorAPI.h
#ifndef __STATICMODELCREATORAPI_H__
#define __STATICMODELCREATORAPI_H__

#include "modelPostProcessFuncs.h"

#include <math/vec3.h>
#include <math/vec2.h>

#include <shared/simpleVert.h>

// this class is used by modelLoader.dll
// to pass model file data to cm/renderer modules
class staticModelCreatorAPI_i : public modelPostProcessFuncs_i {
public:
	virtual void addTriangle(const char *matName, const struct simpleVert_s &v0,
		const struct simpleVert_s &v1, const struct simpleVert_s &v2) = 0;
	virtual void addTriangleOnlyXYZ(const char *matName, const vec3_c &p0,
		const vec3_c &p1, const vec3_c &p2) {
		simpleVert_s v0;
		v0.xyz = p0;
		simpleVert_s v1;
		v1.xyz = p1;
		simpleVert_s v2;
		v2.xyz = p2;
		addTriangle(matName,v0,v1,v2);
	}
	virtual void addTriangleToSF(u32 surfNum, const struct simpleVert_s &v0,
		const struct simpleVert_s &v1, const struct simpleVert_s &v2) = 0;

	virtual void addTriangle(class mtrAPI_i *mat, const vec3_c &v0, const vec2_c &t0, const vec3_c &v1, const vec2_c &t1, 
		const vec3_c &v2, const vec2_c &t2) { }
	void addQuad(class mtrAPI_i *mat, const vec3_c &v0, const vec2_c &t0, const vec3_c &v1, const vec2_c &t1, 
		const vec3_c &v2, const vec2_c &t2, const vec3_c &v3, const vec2_c &t3) {
		addTriangle(mat,v0,t0,v1,t1,v2,t2);
		addTriangle(mat,v2,t2,v3,t3,v0,t0);
	}


	virtual void resizeVerts(u32 newNumVerts) = 0;
	virtual void setVert(u32 vertexIndex, const struct simpleVert_s &v) = 0;
	virtual void setVertexPos(u32 vertexIndex, const vec3_c &newPos) = 0;
	virtual void resizeIndices(u32 newNumIndices) = 0;
	virtual void setIndex(u32 indexNum, u32 value) = 0;

	virtual void addWinding(class mtrAPI_i *mat, const class texturedVertex_c *verts, u32 numVerts) { }

	virtual void clear() = 0;

	virtual void setAllVertexColors(byte r, byte g, byte b, byte a) {

	}

	virtual void addSprite(const class vec3_c &origin, float radius, class mtrAPI_i *mat, const class axis_c &viewerAxis, byte alpha = 255) {

	}
	virtual void addSurface(const char *matName, const simpleVert_s *verts, u32 numVerts, const u16 *indices, u32 numIndices) {

	}
	virtual void addSurface(const char *matName, const class rVert_c *verts, u32 numVerts, const u16 *indices, u32 numIndices) {

	}
	virtual void recalcTBNs() { 

	}
	virtual void scaleTexCoords(float f) {

	}

	// only for .map -> trimesh converter
	virtual void onNewMapEntity(u32 entityNum) {

	}
	// only for .map -> trimesh converter, called by parser code
	virtual bool onBezierPatch(const char *patchDefStart, const char *patchDefEnd) {
		return false;
	}

	// for debugging
	virtual u32 countDuplicatedTriangles() const { 
		return 0;
	}
	virtual bool hasTriangle(u32 i0, u32 i1, u32 i2) const {
		return false;
	}
};

#endif // __STATICMODELCREATORAPI_H__
