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
// V: Doom3 moveable.
// Example from .map file: 
/*
"classname" "moveable_base_brick"
"name" "moveable_base_brick_1"
"model" "moveable_base_brick_1"
"origin" "-768 -384 0"
*/
// NOTE: Doom3 is loading entity defs from .map files
// NOTE: "model" is the name of both render model and collision model
#include "Moveable.h"

DEFINE_CLASS(Moveable, "ModelEntity");
DEFINE_CLASS_ALIAS(Moveable, idMoveable);

void Moveable::setKeyValue(const char *key, const char *value) {
	if(!_stricmp(key,"model")) {
		this->setRenderModel(value);
		this->setColModel(value);
	} else {
		ModelEntity::setKeyValue(key,value);
	}
}

