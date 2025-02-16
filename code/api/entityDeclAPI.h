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
// entityDeclAPI.h
#ifndef __ENTITYDECLAPI_H__
#define __ENTITYDECLAPI_H__

#include <shared/typedefs.h>

// NOTE: those are loaded from .def files in Doom3 and are also used by Doom3 editor
// Editor keys used in entity defs:
// editor_color, editor_mins, editor_maxs, editor_var <name>

class entityDeclAPI_i {
public:
	virtual const char *getDeclName() const = 0;
	virtual const class entDefAPI_i *getEntDefAPI() const = 0;
	// for build-in editor
	virtual bool isFixedSize() const = 0;
	virtual const char *getEditorComments() const = 0;
	virtual const class vec3_c &getEditorColor() const = 0;
	virtual const class vec3_c &getEditorMins() const = 0;
	virtual const class vec3_c &getEditorMaxs() const = 0;
	virtual bool hasEditorFlagLight() const = 0;
	// used for prop_physics and misc_model which can have any model
	virtual bool hasEditorFlagMiscModel() const = 0;
	virtual bool hasEditorFlagAngle() const = 0;
	virtual const char *getEditorFlagName(u32 i) const = 0;
	virtual const char *getEditorMaterialName() const = 0;
	// used eg. for monster_qshambler entity which has a certain model specified
	virtual bool hasDefinedModel() const = 0;
	virtual const char *getModelName() const = 0;
};

#endif // __ENTITYDECLAPI_H__

