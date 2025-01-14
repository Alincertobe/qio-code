/*
============================================================================
Copyright (C) 2016 V.

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
// guiAPI.h
#ifndef __GUI_API_H__
#define __GUI_API_H__

#include "iFaceBase.h"
#include <shared/typedefs.h>

#define GUI_API_IDENTSTR "GUI0001"

class guiAPI_i : public iFaceBase_i {
public:
	virtual void shutdownGUI() = 0;

	virtual void drawGUI() = 0;
	virtual void onMouseMove(int dX, int dY) = 0;
	virtual void onKeyDown(int keyCode) = 0;
	virtual void onKeyUp(int keyCode) = 0;
	// this is not the same coordinate as windows mouse
	virtual int getMouseX() const = 0;
	virtual int getMouseY() const = 0;
};

extern guiAPI_i *gui;


#endif // __GUI_API_H__

