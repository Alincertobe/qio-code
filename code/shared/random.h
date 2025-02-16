/*
============================================================================
Copyright (C) 2013 V.

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
// shared/random.h - simple random generator
// added for Doom3 particles support
#ifndef __SHARED_RANDOM_H__
#define __SHARED_RANDOM_H__

class rand_c {
	int seed;
public:
	void setSeed(int newSeed);

	int getRandomInt();
	// returns random float in range [0.f,1.f]
	float getRandomFloat();
	// returns random float in range [-1.f,1.f]
	float getCRandomFloat();
};

#endif // __SHARED_RANDOM_H__
