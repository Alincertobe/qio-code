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
// BaseEntity.h - base class for all entities

#include "../g_local.h"
#include "BaseEntity.h"
#include <math/vec3.h>
#include <math/axis.h>
#include "Player.h"
#include <api/serverAPI.h>
#include <api/coreAPI.h>
#include <api/physAPI.h>
#include <shared/keyValuesListener.h>
#include <shared/trace.h>
// our internal event system
#include <shared/eventSystem.h>
#include <shared/eventBaseAPI.h>
#include <shared/autoCvar.h>
#include <shared/bspBoxDesc.h>
#include <api/entDefAPI.h>
#include <shared/entityType.h>
#include <shared/wolfScript.h>
#include <assert.h>

DEFINE_CLASS(BaseEntity, "None");
// Quake3 "misc_teleporter_dest" for q3dm0 teleporter 
DEFINE_CLASS_ALIAS(BaseEntity, misc_teleporter_dest);
// Q3 "target_position" for q3dm11 teleporter (yes, "misc_teleporter_dest" is not used on that map!)
DEFINE_CLASS_ALIAS(BaseEntity, target_position);
// for light targets
DEFINE_CLASS_ALIAS(BaseEntity, info_notnull);
// for ET
DEFINE_CLASS_ALIAS(BaseEntity, path_corner_2);
DEFINE_CLASS_ALIAS(BaseEntity, path_corner);
DEFINE_CLASS_ALIAS(BaseEntity, script_multiplayer);

// used to debug setting BaseEntity keyValues
static aCvar_c g_baseEntity_debugSetKeyValue("g_baseEntity_debugSetKeyValue","0");


class wsEntityScriptInstance_c {
friend class wsEntityInstance_c;
	const class wsScriptBlock_c *block;
	u32 statement;
	int wait;
public:
	wsEntityScriptInstance_c(const class wsScriptBlock_c *b) {
		this->block = b;
		this->statement = 0;
		this->wait = 0;
	}
};
enum wsWaitType_e {
	WSW_NONE,
	WSW_GOTOMARKER
};
class wsEntityInstance_c {
	class BaseEntity *ent;
	arraySTD_c<wsEntityScriptInstance_c *> instances;
	int wait_ms;
	wsWaitType_e waitingFor;
public:
	wsEntityInstance_c(BaseEntity *ne) {
		ent = ne;
		wait_ms = 0;
		waitingFor = WSW_NONE;
	}
	void startWolfScript(const class wsScriptBlock_c *b) {
		instances.push_back(new wsEntityScriptInstance_c(b));
	}
	bool runInstance(wsEntityScriptInstance_c *i) {
		if(wait_ms) {
			wait_ms -= g_frameTimeMs;
			if(wait_ms > 0)
				return false;
			if(wait_ms < 0)
				wait_ms = 0;
		}
		if(waitingFor != WSW_NONE) {
			if(waitingFor == WSW_GOTOMARKER) {
				if(ent->isMoverMoving())
					return false;
			}
		}
		const class wsScriptBlock_c *b = i->block;
		while(i->statement < b->getNumStatements()) {
			const str &txt = b->getStatement(i->statement)->getText();
			str key;
			const char *p = txt.getToken(key);
			if(!_stricmp(key,"wait")) {
				str waitVal;
				p = txt.getToken(waitVal,p);
				wait_ms += atoi(waitVal);
				i->statement++;
				return false; // stop and wait
			} else if(!_stricmp(key,"trigger")) {
				str scriptName, labelName;
				p = txt.getToken(scriptName,p);
				if(!stricmp(scriptName,"self")) {
					scriptName = ent->getScriptName();
				}
				p = txt.getToken(labelName,p);
				g_core->Print("Triggering %s - %s\n",scriptName.c_str(),labelName.c_str());
				G_WolfScript_StartScript(scriptName,labelName);
			} else if(!_stricmp(key,"gotomarker")) {
				const char *savedP = p;
				g_core->Print("Executing gotomarker %s\n",p);
				str targetName, speedStr, waitFlag;
				p = txt.getToken(targetName,p);
				p = txt.getToken(speedStr,p);
				//bool bWait = false;
				if(p) {
					p = txt.getToken(waitFlag,p);
					if(!_stricmp(waitFlag,"wait")) {
						//bWait = true;
						waitingFor = WSW_GOTOMARKER;
					}
				}
				ent->setKeyValue(key,savedP);
				if(waitingFor != WSW_NONE) {
					i->statement++;
					return false;
				}
			} else {
				str tmp = p;
				tmp.stripTrailing(" \n\r\t");
				ent->setKeyValue(key,tmp);
			}
			i->statement++;
		}
		return true;
	}
	void runWolfScript() {
		for(int i = 0; i < instances.size(); i++) {
			if(runInstance(instances[i])) {
				delete instances[i];
				instances.erase(i);
			}
		}
	}
};

// use this to force BaseEntity::ctor() to use a specific edict instead of allocating a new one
static edict_s *be_forcedEdict = 0;
void BE_SetForcedEdict(edict_s *nfe) {
	be_forcedEdict = nfe;
}

BaseEntity::BaseEntity() {
	if(be_forcedEdict) {
		myEdict = be_forcedEdict;
		be_forcedEdict = 0;
		// HACK: use playerState_s for players
		Player *pl = (Player*)this;
		// playerState_s is a superset of entityState, so we can point s to ps
		playerState_s *ps = pl->getPlayerState();
		myEdict->s = ps;
		_myEntityState = 0;
		// save clientNum
		ps->clientNum = myEdict - g_entities;
		// set entity type
		myEdict->s->eType = ET_PLAYER;
	} else {
		myEdict = G_AllocEdict();
		// if that's not a player, alloc an entityState_s
		_myEntityState = myEdict->s = new entityState_s;
		// set entity type
		myEdict->s->eType = ET_GENERAL;
	}
	// set entityState_s::number
	myEdict->s->number = myEdict - g_entities;
	myEdict->ent = this;
	parent = 0;
	eventList = 0;
	wsScript = 0;
	matrix.identity();
	bMarkedForDelete = false;
}
BaseEntity::~BaseEntity() {
	if(attachments.size()) {
		for(u32 i = 0; i < attachments.size(); i++) {
			BaseEntity *e = attachments[i];
			e->parent = 0;
			e->myEdict->s->parentNum = ENTITYNUM_NONE;
		}
		attachments.clear();
	}
	detachFromParent();
	unlink(); // fries edict_s::bspBoxDesc
	if(_myEntityState) {
		// free the entityState_s that we have alloced
		assert(_myEntityState == myEdict->s);
		delete _myEntityState;
		_myEntityState = 0;
	}
	if(eventList) {
		delete eventList;
	}
	if(wsScript) {
		delete wsScript;
	}
	memset (myEdict, 0, sizeof(*myEdict));
	myEdict->freetime = g_time;
	myEdict->s = 0;
}
void BaseEntity::printInfo() const {
	// TODO?
}
void BaseEntity::dropToFloor(float zOfs) {
	trace_c tr;
	tr.setupRay(getOrigin(),getOrigin()+vec3_c(0,0,-8192.f));
	g_physWorld->traceRay(tr,this);
	if(tr.hasHit()) {
		this->setOrigin(tr.getHitPos()+vec3_c(0,0,-zOfs));
	}
}
void BaseEntity::printTouchingAreas() const {
	g_core->Print("Bounds: %f %f %f, %f %f %f\n",
		this->myEdict->absBounds.mins.x,
		this->myEdict->absBounds.mins.y,
		this->myEdict->absBounds.mins.z,
		this->myEdict->absBounds.maxs.x,
		this->myEdict->absBounds.maxs.y,
		this->myEdict->absBounds.maxs.z
		);
	bspBoxDesc_s *bd = myEdict->bspBoxDesc;
	g_core->Print("Touching %i areas: ",bd->areas.size());
	for(u32 i = 0; i < bd->areas.size(); i++) {
		if(i)
			g_core->Print(",");
		g_core->Print("%i",bd->areas[i]);
	}
	g_core->Print("\n");
}
void BaseEntity::setKeyValue(const char *key, const char *value) {
	if(key[0] == '@')
		key++;

	if(g_baseEntity_debugSetKeyValue.getInt()) {
		g_core->Print("BaseEntity::setKeyValue: <%s> <%s>\n",key,value);
	}
	if(!_stricmp(key,"origin")) {
		this->setOrigin(value);
	} else if(!_stricmp(key,"angles")) {
		this->setAngles(value);
	} else if(!_stricmp(key,"rotation")) {
		axis_c ax;
		ax.fromString(value);
		this->setAngles(ax.toAngles());
	} else if(!_stricmp(key,"angle")) {
		// it's used in Q3 maps and somehow
		// in Prey's game/roadhouse as well
		float angle = atof(value);
		this->setAngles(vec3_c(0,angle,0));
	} else if(!_stricmp(key,"targetname")) {
		this->setTargetName(value);
	} else if(!_stricmp(key,"scriptName")) {
		this->setScriptName(value);
	} else if(!_stricmp(key,"target")) {
		this->setTarget(value);
	} else if(!_stricmp(key,"spawnFlags")) {
		this->spawnFlags = atoi(value);
	} else if(!_stricmp(key,"parent")) {
		this->postEvent(0,"setparent",value,"-1","1");
		//this->setParent(value,-1,true);
	} else if(!_stricmp(key,"addAttachment")) {
		// spawn new entity and attach it to this one
		char objName[256];
		char tagName[256];
		int bRemoveWithParent = 1;
		sscanf(value,"%s %s",objName,tagName,&bRemoveWithParent);
		g_core->Print("BaseEntity::addAttachment: %s %s (%i)\n",objName,tagName,bRemoveWithParent);
		BaseEntity *child = G_SpawnClass(objName);
		if(child) {
			child->setParent(this,0);
		} else {

		}
	} else if(!_stricmp(key,"setLastAttachmentKeyValue")) {
		// cast a key-value on the last spawned attachment
		if(attachments.size() == 0) {
			g_core->RedWarning("setLastAttachmentKeyValue: entity has no attachments\n");
		} else {
			// get last attachment
			BaseEntity *be = attachments[attachments.size()-1];
			str txt = value;
			str newKey;
			const char *newValue = txt.getToken(newKey);
			g_core->Print("Passing \"%s %s\" keyvalue to attachemnt\n",newKey.c_str(),newValue);
			be->setKeyValue(newKey.c_str(),newValue);
		}
#if 1
	} else if(!_stricmp(key,"wm_announce")) {
		g_core->Print("^2WM_ANNOUCE: %s\n",value);
	} else if(!_stricmp(key,"setPosition")) {
		const char *posStr = value;
		BaseEntity *e = G_FindFirstEntityWithTargetName(posStr);
		if(e) {
			this->setOrigin(e->getOrigin());
		} 
#endif
	} else if(!_stricmp(key,"printMsg")) {
		g_core->Print("%s\n",value);
	} else {

	}
}	
void BaseEntity::iterateKeyValues(class keyValuesListener_i *listener) const {
	vec3_c curPosition = this->getOrigin();
	if(curPosition.isNull() == false) {
		listener->addKeyValue("origin",curPosition);
	}
	vec3_c curAngles = this->getAngles();
	if(curAngles.isNull() == false) {
		listener->addKeyValue("angles",curAngles);
	}
	if(this->hasTargetName()) {
		listener->addKeyValue("targetname",getTargetName());
	}
	if(this->hasTarget()) {
		listener->addKeyValue("target",getTarget());
	}
}
void BaseEntity::applyKeyValues(const entDefAPI_i *list) {
	for(u32 i = 0; i < list->getNumKeyValues(); i++) {
		const char *k, *v;
		list->getKeyValue(i, &k, &v);
		setKeyValue(k,v);
	}
}
void BaseEntity::setEntityType(int newEType) {
	_myEntityState->eType = newEType;
}
void BaseEntity::processEvent(class eventBaseAPI_i *ev) {
	if(!_stricmp(ev->getEventName(),"setparent")) {
		const char *targetName = ev->getArg(0);
		const char *tag = ev->getArg(1);
		const char *enableLocalOffset = ev->getArg(2);
		this->setParent(targetName,atoi(tag),atoi(enableLocalOffset));
	} else if(!_stricmp(ev->getEventName(),"remove")) {
		bMarkedForDelete = true;
	}
}	
void BaseEntity::postEvent(int execTime, const char *eventName, const char *arg0, const char *arg1, const char *arg2, const char *arg3) {
	if(eventList == 0) {
		eventList = new eventList_c;
	}
	eventList->addEvent(execTime,eventName,arg0,arg1,arg2,arg3);
}	
void BaseEntity::removeAfterDelay(int delay) {
	postEvent(g_time+delay,"remove");
}
// maybe I should put those functions in ModelEntity...
void BaseEntity::link() {
	// let the server handle the linking
	g_server->linkEntity(this->myEdict);
}
void BaseEntity::unlink() {
	// unlink entity from server
	g_server->unlinkEntity(this->myEdict);
}
void BaseEntity::setOrigin(const vec3_c &newXYZ) {
	myEdict->s->origin = newXYZ;
	matrix.fromAnglesAndOrigin(this->myEdict->s->angles,this->myEdict->s->origin);
	recalcABSBounds();
	link();
}
void BaseEntity::setAngles(const class vec3_c &newAngles) {
	myEdict->s->angles = newAngles;
	matrix.fromAnglesAndOrigin(this->myEdict->s->angles,this->myEdict->s->origin);
	recalcABSBounds();
	link();
}
void BaseEntity::setMatrix(const class matrix_c &newMat) {
	myEdict->s->angles = newMat.getAngles();
	myEdict->s->origin = newMat.getOrigin();	
	this->matrix = newMat;
	recalcABSBounds();
	link();
}
class vec3_c BaseEntity::getCenter() const {
	return myEdict->absBounds.getCenter();
}
const vec3_c &BaseEntity::getOrigin() const {
	return myEdict->s->origin;
}
const class vec3_c &BaseEntity::getScale() const {
	return myEdict->s->scale;
}
const vec3_c &BaseEntity::getAngles() const {
	return myEdict->s->angles;
}
void BaseEntity::recalcABSBounds() {
	aabb tmpLocalBB;
	this->getLocalBounds(tmpLocalBB);
	matrix.transformAABB(tmpLocalBB,myEdict->absBounds);
}
void BaseEntity::getLocalBounds(aabb &out) const {
	out.fromHalfSize(16.f);
}
u32 BaseEntity::getEntNum() const {
	return myEdict->s->number;
}
bool BaseEntity::hasClassName(const char *className) const {
	const char *cn = getClassName();
	return !_stricmp(cn,className);
}
const char *BaseEntity::getScriptName() const {
	return scriptName;
}
void BaseEntity::setScriptName(const char *newScriptName) {
	scriptName = newScriptName;
}
bool BaseEntity::hasScriptName() const {
	if(scriptName.length())
		return true;
	return false;
}
const char *BaseEntity::getTargetName() const {
	return targetName;
}
void BaseEntity::setTargetName(const char *newTargetName) {
	targetName = newTargetName;
}
bool BaseEntity::hasTargetName() const {
	if(targetName.length())
		return true;
	return false;
}
const char *BaseEntity::getTarget() const {
	return target;
}
void BaseEntity::setTarget(const char *newTarget) {
	target = newTarget;
}
bool BaseEntity::hasTarget() const {
	if(target.length())
		return true;
	return false;
}
void BaseEntity::postSpawn() {
	if(scriptName.size()) {
		G_WolfScript_StartScript_Spawn(scriptName);
	}
}
u32 BaseEntity::processPendingEvents() {
	if(eventList == 0)
		return 0;
	u32 c_executed = eventList->executeEvents(g_time,this);
	if(bMarkedForDelete) {
		delete this;
	}
	return c_executed;
}
void BaseEntity::hideEntity() {
	myEdict->s->hideEntity();
}
void BaseEntity::showEntity() {
	myEdict->s->showEntity();
}
void BaseEntity::toggleEntityVisibility() {
	if(myEdict->s->isHidden()) {
		myEdict->s->showEntity();
	} else {
		myEdict->s->hideEntity();
	}
}
void BaseEntity::setParent(BaseEntity *newParent, const char *tagName, bool enableLocalOffset) {
	int tagNum = newParent->getBoneNumForName(tagName);
	setParent(newParent,tagNum,enableLocalOffset);
}
void BaseEntity::setParent(BaseEntity *newParent, int tagNum, bool enableLocalOffset) {
	if(parent) {
		detachFromParent();
	}
	if(newParent == 0)
		return;
	parent = newParent;
	myEdict->s->parentNum = newParent->getEntNum();
	myEdict->s->parentTagNum = tagNum;
	if(enableLocalOffset) {
		const matrix_c &thisMat = this->getMatrix();
		const matrix_c &parentMat = parent->getMatrix();
		matrix_c ofs = parentMat.getInversed() * thisMat;
		myEdict->s->parentOffset = ofs.getOrigin();
	} else {
		myEdict->s->parentOffset.zero();
	}
	parent->attachments.push_back(this);
}
void BaseEntity::setParent(const char *parentTargetName, int tagNum, bool enableLocalOffset) {
	setParent(G_FindFirstEntityWithTargetName(parentTargetName),tagNum,enableLocalOffset);
}
int BaseEntity::getParentTagNum() const {
	return myEdict->s->parentTagNum;
}
void BaseEntity::detachFromParent() {
	if(parent == 0) {
		assert(this->myEdict->s->parentNum == ENTITYNUM_NONE);
		return;
	}
	parent->attachments.remove(this);
	parent = 0;
	myEdict->s->parentNum = ENTITYNUM_NONE;
	myEdict->s->parentTagNum = -1;
}
void BaseEntity::setLocalAttachmentAngles(const vec3_c &newAngles) {
	myEdict->s->localAttachmentAngles = newAngles;
}
// BaseEntity has no model so we cant do much here.
// This function is virtual and overriden in ModelEntity
bool BaseEntity::getBoneWorldOrientation(u32 tagNum, class matrix_c &out) {
	out = this->matrix;
	return true; // error
}
// update origin/angles/matrix fields of entity attached to another
void BaseEntity::updateAttachmentOrigin() {
	if(parent == 0) {
		return;
	}
	// recursively update orientation
	parent->updateAttachmentOrigin();

	matrix_c mat;
	if(myEdict->s->parentTagNum == -1) { 
		// parent matrix
		mat = parent->getMatrix();
	} else {
		// get world space matrix of parent tag/bone
		parent->getBoneWorldOrientation(myEdict->s->parentTagNum,mat);
	}
	this->setOrigin(mat.getOrigin());
	this->setAngles(mat.getAngles());
	//if(cent->currentState.parentOffset.isAlmostZero() == false) {
	//	matrix_c matAngles = mat;
	//	matAngles.setOrigin(vec3_origin);
	//	vec3_c ofs;
	//	matAngles.transformPoint(cent->currentState.parentOffset,ofs);
	//	cent->lerpOrigin += ofs;
	//}
}
void BaseEntity::setEntityLightRadius(float newEntityLightRadius) {
	myEdict->s->lightRadius = newEntityLightRadius;
}
void BaseEntity::setEntityLightColor(const vec3_c &color) {
	myEdict->s->lightColor = color;
	myEdict->s->lightFlags |= LF_COLOURED;
}
void BaseEntity::setTrailEmitterMaterial(const char *matName) {
	myEdict->s->trailEmitterMaterial = G_RenderMaterialIndex(matName);
}
void BaseEntity::setTrailEmitterSpriteRadius(float newRadius) {
	myEdict->s->trailEmitterSpriteRadius = newRadius;
}
void BaseEntity::setTrailEmitterInterval(int newInterval) {
	myEdict->s->trailEmitterInterval = newInterval;
}
const aabb &BaseEntity::getAbsBounds() const {
	return this->myEdict->absBounds;
}
#include <shared/bspBoxDesc.h>
// returns the count of BSP areas touching this entity
u32 BaseEntity::getNumTouchingAreas() const {
	return this->myEdict->bspBoxDesc->getNumAreas();
}
u32 BaseEntity::getTouchingArea(u32 localIdx) const {
	return this->myEdict->bspBoxDesc->getArea(localIdx);
}
bool BaseEntity::openAreaPortalIfPossible() {
	u32 touchingAreas = getNumTouchingAreas();
	if(touchingAreas > 1) {
		if(touchingAreas != 2) {
			g_core->RedWarning("BaseEntity::openAreaPortalIfPossible: entity %i (%s) touches more than two areas - %i\n",
				getEntNum(),getRenderModelName(),touchingAreas);
		}
		int area0 = this->getTouchingArea(0);
		int area1 = this->getTouchingArea(1);
		// mark portal as open 
		g_server->adjustAreaPortalState(area0,area1,true);
		return true;
	}
	return false;
}
bool BaseEntity::closeAreaPortalIfPossible() {
	u32 touchingAreas = getNumTouchingAreas();
	if(touchingAreas > 1) {
		if(touchingAreas != 2) {
			g_core->RedWarning("BaseEntity::openAreaPortalIfPossible: entity %i (%s) touches more than two areas - %i\n",
				getEntNum(),getRenderModelName(),touchingAreas);
		}
		int area0 = this->getTouchingArea(0);
		int area1 = this->getTouchingArea(1);
		// mark portal as closed
		g_server->adjustAreaPortalState(area0,area1,false);
		return true;
	}
	return false;
}
void BaseEntity::startWolfScript(const class wsScriptBlock_c *block) {
	if(wsScript == 0)
		wsScript = new wsEntityInstance_c(this);;
	wsScript->startWolfScript(block);
}
void BaseEntity::runWolfScript() {
	if(wsScript == 0)
		return;
	wsScript->runWolfScript();
}
// for lua wrapper
bool BaseEntity::addLuaEventHandler(struct lua_State *L, const char *eventName, int func) {
#ifdef G_ENABLE_LUA_SCRIPTING
	if(!_stricmp(eventName,"runFrame")) {
		lua_runFrameHandlers.addEventHandler(L,func);
		return false;
	} 
	g_core->RedWarning("BaseEntity::addLuaEventHandler: unknown event name %s\n",eventName);
#else
	g_core->RedWarning("BaseEntity::addLuaEventHandler: LUA scripting disabled\n");
#endif
	return true;
}
#include <api/ddAPI.h>

void BaseEntity::debugDrawAbsBounds(class rDebugDrawer_i *dd) {
	dd->drawBBLines(this->myEdict->absBounds);
}