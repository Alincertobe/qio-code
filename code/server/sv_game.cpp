/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Qio source code.

Qio source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Qio source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Qio source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// sv_game.c -- interface to the game dll

#include "server.h"
#include <api/coreAPI.h>
#include <api/iFaceMgrAPI.h>
#include <api/moduleManagerAPI.h>
#include <api/gameAPI.h>
#include <shared/portalizedBSPTree.h>

static moduleAPI_i *sv_gameDLL = 0;
gameAPI_s *g_game = 0;
gameClientAPI_s *g_gameClients = 0;



edict_s *SV_GentityNum( int num ) {
	return g_game->GetEdict(num);
}

playerState_s *SV_GameClientNum( int num ) {
	playerState_s	*ps;

	ps = g_gameClients->ClientGetPlayerState(num);

	return ps;
}

svEntity_s	*SV_SvEntityForGentity( edict_s *gEnt ) {
	if ( !gEnt || gEnt->s->number < 0 || gEnt->s->number >= MAX_GENTITIES ) {
		Com_Error( ERR_DROP, "SV_SvEntityForGentity: bad gEnt" );
	}
	return &sv.svEntities[ gEnt->s->number ];
}

edict_s *SV_GEntityForSvEntity( svEntity_s *svEnt ) {
	int		num;

	num = svEnt - sv.svEntities;
	return SV_GentityNum( num );
}

/*
===============
SV_GameSendServerCommand

Sends a command string to a client
===============
*/
void SV_GameSendServerCommand( int clientNum, const char *text ) {
	if ( clientNum == -1 ) {
		SV_SendServerCommand( NULL, "%s", text );
	} else {
		if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
			return;
		}
		SV_SendServerCommand( svs.clients + clientNum, "%s", text );	
	}
}


/*
===============
SV_GameDropClient

Disconnects the client with a message
===============
*/
void SV_GameDropClient( int clientNum, const char *reason ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		return;
	}
	SV_DropClient( svs.clients + clientNum, reason );	
}

#include "sv_vis.h"
void SV_LinkEntity(edict_s *ed) {
	// ensure that we have bspBoxDesc allocated
	if(ed->bspBoxDesc == 0) {
		ed->bspBoxDesc = new bspBoxDesc_s;
	}
	if(sv_bsp) {
		sv_bsp->filterBB(ed->absBounds,*ed->bspBoxDesc);
	} else if(sv_procTree) {
		sv_procTree->boxAreaNums(ed->absBounds,ed->bspBoxDesc->areas);
	}
}
void SV_UnlinkEntity(edict_s *ed) {
	// free bspBoxDesc
	if(ed->bspBoxDesc) {
		delete ed->bspBoxDesc;
		ed->bspBoxDesc = 0;
	}	
}
void SV_AdjustAreaPortalState(int area0, int area1, bool open) {
	if(sv_bsp) {
		sv_bsp->adjustAreaPortalState(area0,area1,open);
	}
}
bool SV_IsWorldTypeBSP() {
	if(sv_bsp)
		return true;
	return false;
}
/*
===============
SV_GetServerinfo

===============
*/
void SV_GetServerinfo( char *buffer, int bufferSize ) {
	if ( bufferSize < 1 ) {
		Com_Error( ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize );
	}
	Q_strncpyz( buffer, Cvar_InfoString( CVAR_SERVERINFO ), bufferSize );
}

/*
===============
SV_GetUsercmd

===============
*/
void SV_GetUsercmd( int clientNum, userCmd_s *cmd ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		Com_Error( ERR_DROP, "SV_GetUsercmd: bad clientNum:%i", clientNum );
	}
	*cmd = svs.clients[clientNum].lastUsercmd;
}

//==============================================

/*
===============
SV_ShutdownGameProgs

Called every time a map changes
===============
*/
void SV_ShutdownGameProgs() {
	if ( !sv_gameDLL ) {
		return;
	}
	g_game->ShutdownGame(false);
	g_moduleMgr->unload(&sv_gameDLL);
}

/*
==================
SV_InitGameVM

Called for both a full init and a restart
==================
*/
static void SV_InitGameVM( bool restart ) {
	int		i;

	// clear all gentity pointers that might still be set from
	// a previous level
	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=522
	//   now done before GAME_INIT call
	for ( i = 0 ; i < sv_maxclients->integer ; i++ ) {
		svs.clients[i].gentity = NULL;
	}
	
	// use the current msec count for a random seed
	// init for this gamestate
	g_game->InitGame(sv.time, Com_Milliseconds(), restart);
}



/*
===================
SV_RestartGameProgs

Called on a map_restart, but not on a normal map change
===================
*/
void SV_RestartGameProgs() {
	if ( !sv_gameDLL ) {
		return;
	}
	g_game->ShutdownGame(true);

	// do a restart instead of a free
	sv_gameDLL = g_moduleMgr->restart(sv_gameDLL, true);
	if ( !sv_gameDLL ) {
		Com_Error( ERR_FATAL, "VM_Restart on game failed" );
	}

	SV_InitGameVM( true );
}

/*
===============
SV_InitGameProgs

Called on a normal map change, not on a map_restart
===============
*/
void SV_InitGameProgs() {
	// load the dll or bytecode
	sv_gameDLL = g_moduleMgr->load("game");
	if ( !sv_gameDLL ) {
		Com_Error( ERR_FATAL, "Failed to load game module" );
	}
	g_iFaceMan->registerIFaceUser(&g_game,GAME_API_IDENTSTR);
	if(!g_game) {
		Com_Error( ERR_DROP, "Game module has wrong interface version (%s required)", GAME_API_IDENTSTR );
	}
	g_iFaceMan->registerIFaceUser(&g_gameClients,GAMECLIENTS_API_IDENTSTR);
	if(!g_gameClients) {
		Com_Error( ERR_DROP, "GameClients module has wrong interface version (%s required)", GAMECLIENTS_API_IDENTSTR );
	}
	SV_InitGameVM( false );
}


/*
====================
SV_GameCommand

See if the current console command is claimed by the game
====================
*/
bool SV_GameCommand() {
	//if ( sv.state != SS_GAME ) {
		return false;
	//}
	// TODO
	//return VM_Call( gvm, GAME_CONSOLE_COMMAND );
}

