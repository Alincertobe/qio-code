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
// colMeshBuilderAPI.h
#ifndef __COLMESHBUILDERAPI_H__
#define __COLMESHBUILDERAPI_H__

class colMeshBuilderAPI_i {
public:
	virtual void addVert(const class vec3_c &nv) = 0;
	virtual void addIndex(const u32 idx) = 0;
	virtual u32 getNumVerts() const = 0;
	virtual u32 getNumIndices() const = 0;
	virtual void addXYZTri(const vec3_c &p0, const vec3_c &p1, const vec3_c &p2) = 0;
	virtual void addMesh(const float *verts, u32 vertStride, u32 numVerts, const void *indices, bool indices32Bits, u32 numIndices) = 0;
};

#endif // __COLMESHBUILDERAPI_H__

