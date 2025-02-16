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
// declRefState.h - used to track which module are using decls
#ifndef __DECLREFSTATE_H__
#define __DECLREFSTATE_H__

#include <api/iFaceMgrAPI.h> // only for QM_IsServerSide

class declRefState_c {
	bool referencedByClient;
	bool referencedByServer;
public:
	declRefState_c() {
		referencedByClient = false;
		referencedByServer = false;
	}
	void setReferencedByClient() {
		referencedByClient = true;
	}
	void setReferencedByServer() {
		referencedByServer = true;
	}
	void setReferencedByModule(enum qioModule_e userModule) {
		if(QM_IsServerSide(userModule) == false) {
			this->setReferencedByClient();
		} else {
			this->setReferencedByServer();
		}
	}
	void clearServerRef() {
		referencedByServer = false;
	}
	void clearClientRef() {
		referencedByClient = false;
	}
	bool isReferenced() const {
		if(referencedByClient)
			return true;
		if(referencedByServer)
			return true;
		return false;
	}
};

#endif // __DECLREFSTATE_H__
