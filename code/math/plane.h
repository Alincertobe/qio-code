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
// plane.h - 3d plane class
#ifndef __MATH_PLANE_H__
#define __MATH_PLANE_H__

#include "vec3.h"

enum planeSide_e {
	SIDE_FRONT		= 0,
	SIDE_BACK		= 1,
	SIDE_ON			= 2,
	SIDE_CROSS		= 3,
	SIDE_UNKNOWN	= 4,
	SIDE_ERROR		= 5,	
};

#ifndef assert
#define assert(condition)
#endif // assert

#define PL_DIST_EPS 0.01f
#define PL_NORM_EPS 0.0001f

//
// this plane_c class uses standard,
// A * x + B * y + C * z + D = 0
// plane equation, while qmath.c
// code from Quake3 SDK used similiar 
// equation, but with 'D' inverted ! :
// (A * x + B * y + C * z - D = 0)\
// So calculating plane's distance to point
// looks like this (Doom3 way):
//	float d = normal.dotProduct(point) + dist;
// Instead of this (Quake3 way)
// float d = normal.dotProduct(point) - dist;
//
class plane_c {
public:
	vec3_c norm;
	float dist;

	plane_c() {

	}
	// plane equation format is such: normX, normY, normZ, dist
	plane_c(const float *planeEQ) {
		norm = planeEQ;
		dist = planeEQ[3];
	}
	void clear() {
		norm.clear();
		dist = 0.f;
	}
	void setNormal(const float *p) {
		norm.set(p);
	}
	// NOTE: Doom3 setDist function negates the dist value here!
	void setDist(float newDist) {
		dist = newDist;
	}
	void addDist(float delta) {
		dist += delta;
	}

	float distance(const vec3_c &point) const {
		float d = norm.dotProduct(point) + dist;
		return d;
	}
	planeSide_e onSide(const vec3_c &p) const {
		float d = distance(p);
		if(d == 0) {
			return SIDE_ON;
		}
		if(d < 0) {
			return SIDE_BACK;
		}
		return SIDE_FRONT;
	}
	enum planeSide_e onSide(const class aabb &bb) const;
	enum planeSide_e onSide(const class vec3_c &center, float radius) const;
	// returns true if given triangle points are degenerate
	bool fromThreePoints(const vec3_c &p0, const vec3_c &p1, const vec3_c &p2) {
		// build directional vectors
		vec3_c p01 = p1 - p0;
		vec3_c p21 = p2 - p0;

		// create normal
		this->norm.crossProduct(p01, p21);
		float len = norm.normalize2();
		// check if degenerated triangle
		if(len == 0.f)
			return true; // degenerate

		// create distance from origin
		this->dist = -p0.dotProduct(norm);
	
		assert(this->distance(p0)<0.1);
		assert(this->distance(p1)<0.1);
		assert(this->distance(p2)<0.1);

		return false;
	}
	bool fromThreePointsINV(const vec3_c &p0, const vec3_c &p1, const vec3_c &p2) {
		return fromThreePoints(p2,p1,p0);
	}
	bool fromThreePoints(const vec3_c *tri) {
		return fromThreePoints(tri[0],tri[1],tri[2]);
	}
	void fromPointAndNormal(const vec3_c &p, const vec3_c &_normal) {
		this->norm = _normal;
		this->norm.normalizeFast();
		this->dist = -this->norm.dotProduct(p);
#if 0
		float check = this->distance(p);
		assert(abs(check) < 0.01f);
#endif
	}
	void translate(const vec3_c &delta) {
		// simply update plane's distance
		this->dist -= this->norm.dotProduct(delta);
	}
	void set(const vec3_c &newNormal, float newDist) {
		this->norm = newNormal;
		this->dist = newDist;
	}
	plane_c getOpposite() const {
		plane_c ret;
		ret.norm = -norm;
		ret.dist = -dist;
		return ret;
	}
	plane_c getScaled(float f) const {
		plane_c ret;
		ret.norm = norm;
		ret.dist = f*dist;
		return ret;
	}
	void makeOpposite() {
		norm = -norm;
		dist = -dist;
	}
	bool compare2(const vec3_c &oNorm, const float oDist, const float normalEps = PL_NORM_EPS, const float distEps = PL_DIST_EPS) const {
		if(abs(this->dist - oDist) > distEps)
			return false;
		if(oNorm.compare(this->norm,normalEps))
			return true;
		return false;
	}
	bool compare(const plane_c &pl, const float normalEps = PL_NORM_EPS, const float distEps = PL_DIST_EPS) const {
		return compare2(pl.norm,pl.dist,normalEps,distEps);
	}

#define	NORMAL_EPSILON	0.0001
#define	DIST_EPSILON	0.02
#define	ON_EPSILON	0.01


	// TODO: integrate
	int isPlaneEqual(const plane_c &b, int flip) const {
		vec3_c tempNormal;
		double tempDist;

		if (flip) {
			tempNormal = -b.norm;
			tempDist = -b.dist;
		} else {
			tempNormal = b.norm;
			tempDist = b.dist;
		}
		if (
		   fabs(this->norm[0] - tempNormal[0]) < NORMAL_EPSILON
		&& fabs(this->norm[1] - tempNormal[1]) < NORMAL_EPSILON
		&& fabs(this->norm[2] - tempNormal[2]) < NORMAL_EPSILON
		&& fabs(this->dist - tempDist) < DIST_EPSILON )
			return true;
		return false;
	}
	int fromPoints(const vec3_c &p1, const vec3_c &p2, const vec3_c &p3)
	{
		vec3_c v1 = p2 - p1;
		vec3_c v2 = p3 - p1;
		this->norm.crossProduct(v1,v2);
		if (this->norm.normalize2() < 0.1) 
			return false;
		this->dist = this->norm.dotProduct(p1);
		return true;
	}
	// project on normal plane
	// along ez 
	// assumes plane normal is normalized
	void projectOnPlane(const vec3_c &ez, vec3_c &p) {
		if (fabs(ez[0]) == 1)
			p[0] = (dist - norm[1] * p[1] - norm[2] * p[2]) / norm[0];
		else if (fabs(ez[1]) == 1)
			p[1] = (dist - norm[0] * p[0] - norm[2] * p[2]) / norm[1];
		else
			p[2] = (dist - norm[0] * p[0] - norm[1] * p[1]) / norm[2];
	}
	const vec3_c &getNormal() const {
		return norm;
	}
	float getDist() const {
		return dist;
	}
};

// extended plane class with same extra information (signbits and planeType) 
// used to speed up box/plane tests.
// Needed to optimize proc tree boxAreas call (both serverside and clientside)
// clientside .proc code is in rf_proc.cpp
// serverside .proc code is in portalizedBSPTree.cpp
class cachedPlane_c {
public:
	vec3_c norm;
	float dist;
	u16 signBits;
	u16 type;

	void setSignBitsAndType() {
		// for fast box on planeside test
		signBits = 0;
		for (u32 i = 0; i < 3; i++) {
			if(norm[i] < 0) {
				signBits |= 1 << i;
			}
		}
		// set plane type
		for(u32 i = 0; i < 3; i++) {
			if(abs(norm[i]) == 1.f) {
				type = i;
				return;
			}
		}
		type = 3; // non axial
	}
	float distance(const vec3_c &point) const {
		float d = norm.dotProduct(point) + dist;
		return d;
	}
	planeSide_e onSide(const aabb &bb) const;
};

#endif // __MATH_PLANE_H__
