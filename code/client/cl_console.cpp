/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2012-2014 V.

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
// console.c

#include "client.h"
#include <api/rAPI.h>
#include <api/declManagerAPI.h>
#include <api/coreAPI.h>
#include <shared/colorTable.h>
#include <shared/keyCatchers.h>
#include <common/autocompletion.h>

int g_console_field_width = 78;


#define	NUM_CON_TIMES 8

//#define		CON_TEXTSIZE	32768
#define		CON_TEXTSIZE	524288
struct console_t {
	bool	initialized;

	short	text[CON_TEXTSIZE];
	int		current;		// line where next message will be printed
	int		x;				// offset in current line for next print
	int		display;		// bottom of console displays this line

	int 	linewidth;		// characters across screen
	int		totallines;		// total lines in console scrollback

	float	xadjust;		// for wide aspect screens

	float	displayFrac;	// aproaches finalFrac at scr_conspeed
	float	finalFrac;		// 0.0 to 1.0 lines of console to display

	int		vislines;		// in scanlines

	int		times[NUM_CON_TIMES];	// cls.realtime time the line was generated
								// for transparent notify lines
	vec4_t	color;
};

extern	console_t	con;

console_t	con;

cvar_s		*con_conspeed;
cvar_s		*con_notifytime;

#define	DEFAULT_CONSOLE_WIDTH	78


/*
================
Con_ToggleConsole_f
================
*/
void Con_ToggleConsole_f (void) {
	// Can't toggle the console when it's the only thing available
	if ( clc.state == CA_DISCONNECTED && Key_GetCatcher( ) == KEYCATCH_CONSOLE ) {
		return;
	}

	g_consoleField.clear();
	g_consoleField.widthInChars = g_console_field_width;

	Con_ClearNotify ();
	Key_SetCatcher( Key_GetCatcher( ) ^ KEYCATCH_CONSOLE );
}

/*
================
Con_MessageMode_f
================
*/
void Con_MessageMode_f (void) {
	chat_playerNum = -1;
	chat_team = false;
	chatField.clear();
	chatField.widthInChars = 30;

	Key_SetCatcher( Key_GetCatcher( ) ^ KEYCATCH_MESSAGE );
}

/*
================
Con_MessageMode2_f
================
*/
void Con_MessageMode2_f (void) {
	chat_playerNum = -1;
	chat_team = true;
	chatField.clear();
	chatField.widthInChars = 25;
	Key_SetCatcher( Key_GetCatcher( ) ^ KEYCATCH_MESSAGE );
}



/*
================
Con_Clear_f
================
*/
void Con_Clear_f (void) {
	int		i;

	for ( i = 0 ; i < CON_TEXTSIZE ; i++ ) {
		con.text[i] = (ColorIndex(COLOR_WHITE)<<8) | ' ';
	}

	Con_Bottom();		// go to end
}

						
/*
================
Con_Dump_f

Save the console contents out to a file
================
*/
void Con_Dump_f (void)
{
	int		l, x, i;
	short	*line;
	fileHandle_t	f;
	char	buffer[1024];

	if (Cmd_Argc() != 2)
	{
		Com_Printf ("usage: condump <filename>\n");
		return;
	}

	Com_Printf ("Dumped console text to %s.\n", Cmd_Argv(1) );

	f = FS_FOpenFileWrite( Cmd_Argv( 1 ) );
	if (!f)
	{
		Com_Printf ("ERROR: couldn't open.\n");
		return;
	}

	// skip empty lines
	for (l = con.current - con.totallines + 1 ; l <= con.current ; l++)
	{
		line = con.text + (l%con.totallines)*con.linewidth;
		for (x=0 ; x<con.linewidth ; x++)
			if ((line[x] & 0xff) != ' ')
				break;
		if (x != con.linewidth)
			break;
	}

	// write the remaining lines
	buffer[con.linewidth] = 0;
	for ( ; l <= con.current ; l++)
	{
		line = con.text + (l%con.totallines)*con.linewidth;
		for(i=0; i<con.linewidth; i++)
			buffer[i] = line[i] & 0xff;
		for (x=con.linewidth-1 ; x>=0 ; x--)
		{
			if (buffer[x] == ' ')
				buffer[x] = 0;
			else
				break;
		}
		strcat( buffer, "\n" );
		FS_Write(buffer, strlen(buffer), f);
	}

	FS_FCloseFile( f );
}

						
/*
================
Con_ClearNotify
================
*/
void Con_ClearNotify() {
	int		i;
	
	for ( i = 0 ; i < NUM_CON_TIMES ; i++ ) {
		con.times[i] = 0;
	}
}

						

/*
================
Con_CheckResize

If the line width has changed, reformat the buffer.
================
*/
void Con_CheckResize (void)
{
	int		i, j, width, oldwidth, oldtotallines, numlines, numchars;
	short	tbuf[CON_TEXTSIZE];

	width = (SCREEN_WIDTH / SMALLCHAR_WIDTH) - 2;

	if (width == con.linewidth)
		return;

	if (width < 1)			// video hasn't been initialized yet
	{
		width = DEFAULT_CONSOLE_WIDTH;
		con.linewidth = width;
		con.totallines = CON_TEXTSIZE / con.linewidth;
		for(i=0; i<CON_TEXTSIZE; i++)

			con.text[i] = (ColorIndex(COLOR_WHITE)<<8) | ' ';
	}
	else
	{
		oldwidth = con.linewidth;
		con.linewidth = width;
		oldtotallines = con.totallines;
		con.totallines = CON_TEXTSIZE / con.linewidth;
		numlines = oldtotallines;

		if (con.totallines < numlines)
			numlines = con.totallines;

		numchars = oldwidth;
	
		if (con.linewidth < numchars)
			numchars = con.linewidth;

		memcpy (tbuf, con.text, CON_TEXTSIZE * sizeof(short));
		for(i=0; i<CON_TEXTSIZE; i++)

			con.text[i] = (ColorIndex(COLOR_WHITE)<<8) | ' ';


		for (i=0 ; i<numlines ; i++)
		{
			for (j=0 ; j<numchars ; j++)
			{
				con.text[(con.totallines - 1 - i) * con.linewidth + j] =
						tbuf[((con.current - i + oldtotallines) %
							  oldtotallines) * oldwidth + j];
			}
		}

		Con_ClearNotify ();
	}

	con.current = con.totallines - 1;
	con.display = con.current;
}

/*
==================
Cmd_CompleteTxtName
==================
*/
void Cmd_CompleteTxtName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "", "txt", 0, 0, false, true );
	}
}

/*
==================
CL_AutocompleteSpawnCommand
==================
*/
void CL_AutocompleteSpawnCommand( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteEntityDefName();
	}
}

void Cmd_CompleteModelName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "md3", "md5mesh", "obj", false, false );
	}
}
void Cmd_CompleteMDLPPName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "mdlpp", 0, 0, false, false );
	}
}
void Cmd_CompleteMDLName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "mdl", 0, 0, false, false );
	}
}
void Cmd_CompleteEntDefName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "entDef", 0, 0, false, false );
	}
}
void Cmd_CompleteMD5RName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "md5r", 0, 0, false, false );
	}
}
void Cmd_CompleteMD5MeshName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "md5mesh", 0, 0, false, false );
	}
}
void Cmd_CompleteMD5AnimName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "md5anim", 0, 0, false, false );
	}
}
void Cmd_CompleteOBJName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "obj", 0, 0, false, false );
	}
}
void Cmd_CompletePSKName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "psk", "pskx", 0, false, false );
	}
}
void Cmd_CompleteTANName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "tan", 0, 0, false, false );
	}
}
void Cmd_CompleteMDMName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "mdm", 0, 0, false, false );
	}
}
void Cmd_CompleteAnimationName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "mds", "psa", "smd", false, false );
	}
}
void Cmd_CompleteSkelModelName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "mds", "psk", "smd", false, false );
	}
}
void Cmd_CompletePSAName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "psa", 0, 0, false, false );
	}
}
void Cmd_CompleteMDSName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "mds", 0, 0, false, false );
	}
}
void Cmd_CompleteMDCName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "mdc", 0, 0, false, false );
	}
}
void Cmd_CompleteMD3Name( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "md3", 0, 0, false, false );
	}
}
void Cmd_CompleteTIKName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "tik", 0, 0, false, false );
	}
}
void Cmd_CompleteURCName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "ui", "urc", 0, 0, false, false );
	}
}

void Cmd_CompleteSMDName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "smd", 0, 0, false, false );
	}
}

void Cmd_Complete3DSName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteFilename( "models", "3ds", 0, 0, false, false );
	}
}
void Cmd_CompleteEmitterName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteEmitterName();
	}
}
void Cmd_CompleteMaterialName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteMaterialName();
	}
}
void Cmd_CompleteMaterialFileName( char *args, int argNum ) {
	if( argNum == 2 ) {
		AC_CompleteMaterialFileName();
	}
}


/*
==================
Cmd_CacheDeclModel_f
==================
*/
void Cmd_CacheDeclModel_f() {
	if(g_declMgr == 0) {
		g_core->RedWarning("Cmd_CacheDeclModel_f: decl manager not ready\n");
		return;
	}

}

void Cmd_TestModel_f() {
	if (Cmd_Argc() != 2){
		Com_Printf ("Not enough arguments\n");
		return;
	}
	str modelName = Cmd_Argv(1);
	if(g_vfs && g_vfs->FS_FileExists(str("models/")+modelName)) {
		modelName = str("models/")+modelName;
		g_core->Print("Fixed path to %s\n",modelName.c_str());
	}
	Cvar_Set("cg_testModel",modelName);
}


/*
================
Con_Init
================
*/
void Con_Init (void) {
	int		i;

	Com_Printf("\n----- Console Initialization -------\n");

	con_notifytime = Cvar_Get ("con_notifytime", "3", 0);
	con_conspeed = Cvar_Get ("scr_conspeed", "3", 0);

	g_consoleField.clear();
	g_consoleField.widthInChars = g_console_field_width;
	for ( i = 0 ; i < COMMAND_HISTORY ; i++ ) {
		historyEditLines[i].clear();
		historyEditLines[i].widthInChars = g_console_field_width;
	}
	CL_LoadConsoleHistory( );

	Cmd_AddCommand ("toggleconsole", Con_ToggleConsole_f);
	Cmd_AddCommand ("messagemode", Con_MessageMode_f);
	Cmd_AddCommand ("messagemode2", Con_MessageMode2_f);
	Cmd_AddCommand ("clear", Con_Clear_f);
	Cmd_AddCommand ("condump", Con_Dump_f);
	Cmd_SetCommandCompletionFunc( "condump", Cmd_CompleteTxtName );
	// preadd spawn command for autocompletion
	Cmd_AddCommand ("spawn", 0);
	Cmd_SetCommandCompletionFunc( "spawn", CL_AutocompleteSpawnCommand );
	Cmd_AddCommand ("model_spawn", 0);
	Cmd_SetCommandCompletionFunc( "model_spawn", Cmd_CompleteModelName );
	Cmd_AddCommand ("physics_spawn", 0);
	Cmd_SetCommandCompletionFunc( "physics_spawn", Cmd_CompleteModelName );
	Cmd_AddCommand ("mdlpp_spawn", 0);
	Cmd_SetCommandCompletionFunc( "mdlpp_spawn", Cmd_CompleteMDLPPName );
	Cmd_AddCommand ("mdl_spawn", 0);
	Cmd_SetCommandCompletionFunc( "mdl_spawn", Cmd_CompleteMDLName );
	Cmd_AddCommand ("entDef_spawn", 0);
	Cmd_SetCommandCompletionFunc( "entDef_spawn", Cmd_CompleteEntDefName );
	Cmd_AddCommand ("phys_spawn_mdl", 0);
	Cmd_SetCommandCompletionFunc( "phys_spawn_mdl", Cmd_CompleteMDLName );
	Cmd_AddCommand ("obj_spawn", 0);
	Cmd_SetCommandCompletionFunc( "obj_spawn", Cmd_CompleteOBJName );
	Cmd_AddCommand ("phys_spawn_obj", 0);
	Cmd_SetCommandCompletionFunc( "phys_spawn_obj", Cmd_CompleteOBJName );
	Cmd_AddCommand ("md5r_spawn", 0);
	Cmd_SetCommandCompletionFunc( "md5r_spawn", Cmd_CompleteMD5RName );
	Cmd_AddCommand ("md5mesh_spawn", 0);
	Cmd_SetCommandCompletionFunc( "md5mesh_spawn", Cmd_CompleteMD5MeshName );
	Cmd_AddCommand ("convertToMD5Anim", 0);
	Cmd_SetCommandCompletionFunc( "convertToMD5Anim", Cmd_CompleteAnimationName );
	Cmd_AddCommand ("convertMDSToMD5Anim", 0);
	Cmd_SetCommandCompletionFunc( "convertMDSToMD5Anim", Cmd_CompleteMDSName );
	Cmd_AddCommand ("convertPSAToMD5Anim", 0);
	Cmd_SetCommandCompletionFunc( "convertPSAToMD5Anim", Cmd_CompletePSAName );
	Cmd_AddCommand ("convertToMD5Mesh", 0);
	Cmd_SetCommandCompletionFunc( "convertToMD5Mesh", Cmd_CompleteSkelModelName );
	Cmd_AddCommand ("convertMDSToMD5Mesh", 0);
	Cmd_SetCommandCompletionFunc( "convertMDSToMD5Mesh", Cmd_CompleteMDSName );
	Cmd_AddCommand ("convertPSKToMD5Mesh", 0);
	Cmd_SetCommandCompletionFunc( "convertPSKToMD5Mesh", Cmd_CompletePSKName );
	Cmd_AddCommand ("psk_spawn", 0);
	Cmd_SetCommandCompletionFunc( "psk_spawn", Cmd_CompletePSKName );
	Cmd_AddCommand ("tan_spawn", 0);
	Cmd_SetCommandCompletionFunc( "tan_spawn", Cmd_CompleteTANName );
	Cmd_AddCommand ("phys_spawn_psk", 0);
	Cmd_SetCommandCompletionFunc( "phys_spawn_psk", Cmd_CompletePSKName );
	Cmd_AddCommand ("mdm_spawn", 0);
	Cmd_SetCommandCompletionFunc( "mdm_spawn", Cmd_CompleteMDMName );
	Cmd_AddCommand ("mdc_spawn", 0);
	Cmd_SetCommandCompletionFunc( "mdc_spawn", Cmd_CompleteMDCName );
	Cmd_AddCommand ("tik_spawn", 0);
	Cmd_SetCommandCompletionFunc( "tik_spawn", Cmd_CompleteTIKName );
	Cmd_AddCommand ("phys_spawn_mdc", 0);
	Cmd_SetCommandCompletionFunc( "phys_spawn_mdc", Cmd_CompleteMDCName );
	Cmd_AddCommand ("smd_spawn", 0);
	Cmd_SetCommandCompletionFunc( "smd_spawn", Cmd_CompleteSMDName );
	Cmd_AddCommand ("phys_spawn_smd", 0);
	Cmd_SetCommandCompletionFunc( "phys_spawn_smd", Cmd_CompleteSMDName );
	Cmd_AddCommand ("3ds_spawn", 0);
	Cmd_SetCommandCompletionFunc( "3ds_spawn", Cmd_Complete3DSName );
	Cmd_AddCommand ("phys_spawn_3ds", 0);
	Cmd_SetCommandCompletionFunc( "phys_spawn_3ds", Cmd_Complete3DSName );
	Cmd_AddCommand ("cg_testEmitter", 0);
	Cmd_SetCommandCompletionFunc( "cg_testEmitter", Cmd_CompleteEmitterName );
	Cmd_AddCommand ("decl_cacheModel", Cmd_CacheDeclModel_f);
	Cmd_AddCommand ("cg_testMaterial", 0);
	Cmd_SetCommandCompletionFunc( "cg_testMaterial", Cmd_CompleteMaterialName );
	Cmd_AddCommand ("rf_setCrosshairSurfaceMaterial", 0);
	Cmd_SetCommandCompletionFunc( "rf_setCrosshairSurfaceMaterial", Cmd_CompleteMaterialName );
	Cmd_AddCommand ("ter_all_setMaterial", 0);
	Cmd_SetCommandCompletionFunc( "ter_all_setMaterial", Cmd_CompleteMaterialName );
	Cmd_AddCommand ("rf_setSunMaterial", 0);
	Cmd_SetCommandCompletionFunc( "rf_setSunMaterial", Cmd_CompleteMaterialName );
	Cmd_AddCommand ("rf_setSkyMaterial", 0);
	Cmd_SetCommandCompletionFunc( "rf_setSkyMaterial", Cmd_CompleteMaterialName );
	Cmd_SetCommandCompletionFunc( "mat_refreshSingleMaterial", Cmd_CompleteMaterialName );
	Cmd_SetCommandCompletionFunc( "mat_refreshMaterialSourceFile", Cmd_CompleteMaterialFileName );

	Cmd_AddCommand ("pushmenu", 0);
	Cmd_SetCommandCompletionFunc( "pushmenu", Cmd_CompleteURCName );

	// test model
	Cmd_AddCommand ("testModelTIKI", Cmd_TestModel_f);
	Cmd_SetCommandCompletionFunc( "testModelTIKI", Cmd_CompleteTIKName );

	Cmd_AddCommand ("testModelMD5Mesh", Cmd_TestModel_f);
	Cmd_SetCommandCompletionFunc( "testModelMD5Mesh", Cmd_CompleteMD5MeshName );

	Cmd_AddCommand ("testModelMDS", Cmd_TestModel_f);
	Cmd_SetCommandCompletionFunc( "testModelMDS", Cmd_CompleteMDSName );

	Cmd_AddCommand ("testModelMD3", Cmd_TestModel_f);
	Cmd_SetCommandCompletionFunc( "testModelMD3", Cmd_CompleteMD3Name );

	Cmd_AddCommand ("testModelMDC", Cmd_TestModel_f);
	Cmd_SetCommandCompletionFunc( "testModelMDC", Cmd_CompleteMDCName );

	Com_Printf("Console initialized.\n");
}

/*
================
Con_Shutdown
================
*/
void Con_Shutdown(void)
{
	Cmd_RemoveCommand("toggleconsole");
	Cmd_RemoveCommand("messagemode");
	Cmd_RemoveCommand("messagemode2");
	Cmd_RemoveCommand("messagemode3");
	Cmd_RemoveCommand("messagemode4");
	Cmd_RemoveCommand("clear");
	Cmd_RemoveCommand("condump");
	Cmd_RemoveCommand("spawn");
	Cmd_RemoveCommand("model_spawn");
	Cmd_RemoveCommand("cg_testEmitter");
	Cmd_RemoveCommand("cg_testEmitter");
	Cmd_RemoveCommand("decl_cacheModel");
}

/*
===============
Con_Linefeed
===============
*/
void Con_Linefeed (bool skipnotify)
{
	int		i;

	// mark time for transparent overlay
	if (con.current >= 0)
	{
    if (skipnotify)
		  con.times[con.current % NUM_CON_TIMES] = 0;
    else
		  con.times[con.current % NUM_CON_TIMES] = cls.realtime;
	}

	con.x = 0;
	if (con.display == con.current)
		con.display++;
	con.current++;
	for(i=0; i<con.linewidth; i++)
		con.text[(con.current%con.totallines)*con.linewidth+i] = (ColorIndex(COLOR_WHITE)<<8) | ' ';
}

/*
================
CL_ConsolePrint

Handles cursor positioning, line wrapping, etc
All console printing must go through this in order to be logged to disk
If no console is visible, the text will appear at the top of the game window
================
*/
void CL_ConsolePrint( char *txt ) {
	int		y, l;
	unsigned char	c;
	unsigned short	color;
	bool skipnotify = false;		// NERVE - SMF
	int prev;							// NERVE - SMF

	// TTimo - prefix for text that shows up in console but not in notify
	// backported from RTCW
	if ( !strncmp( txt, "[skipnotify]", 12 ) ) {
		skipnotify = true;
		txt += 12;
	}
	
	// for some demos we don't want to ever show anything on the console
	if ( cl_noprint && cl_noprint->integer ) {
		return;
	}
	
	if (!con.initialized) {
		con.color[0] = 
		con.color[1] = 
		con.color[2] =
		con.color[3] = 1.0f;
		con.linewidth = -1;
		Con_CheckResize ();
		con.initialized = true;
	}

	color = ColorIndex(COLOR_WHITE);

	while ( (c = *((unsigned char *) txt)) != 0 ) {
		if ( Q_IsColorString( txt ) ) {
			color = ColorIndex( *(txt+1) );
			txt += 2;
			continue;
		}

		// count word length
		for (l=0 ; l< con.linewidth ; l++) {
			if ( txt[l] <= ' ') {
				break;
			}

		}

		// word wrap
		if (l != con.linewidth && (con.x + l >= con.linewidth) ) {
			Con_Linefeed(skipnotify);

		}

		txt++;

		switch (c)
		{
		case '\n':
			Con_Linefeed (skipnotify);
			break;
		case '\r':
			con.x = 0;
			break;
		default:	// display character and advance
			y = con.current % con.totallines;
			con.text[y*con.linewidth+con.x] = (color << 8) | c;
			con.x++;
			if(con.x >= con.linewidth)
				Con_Linefeed(skipnotify);
			break;
		}
	}


	// mark time for transparent overlay
	if (con.current >= 0) {
		// NERVE - SMF
		if ( skipnotify ) {
			prev = con.current % NUM_CON_TIMES - 1;
			if ( prev < 0 )
				prev = NUM_CON_TIMES - 1;
			con.times[prev] = 0;
		}
		else
		// -NERVE - SMF
			con.times[con.current % NUM_CON_TIMES] = cls.realtime;
	}
}


/*
==============================================================================

DRAWING

==============================================================================
*/


/*
================
Con_DrawInput

Draw the editline after a ] prompt
================
*/
void Con_DrawInput (void) {
	int		y;

	if ( clc.state != CA_DISCONNECTED && !(Key_GetCatcher( ) & KEYCATCH_CONSOLE ) ) {
		return;
	}

	y = con.vislines - ( SMALLCHAR_HEIGHT * 2 );

	rf->set2DColor( con.color );

	rf->drawChar( con.xadjust + 1 * SMALLCHAR_WIDTH, y, ']' );

	g_consoleField.draw(con.xadjust + 2 * SMALLCHAR_WIDTH, y,
		SCREEN_WIDTH - 3 * SMALLCHAR_WIDTH, true, true);
}


/*
================
Con_DrawNotify

Draws the last few lines of output transparently over the game top
================
*/
void Con_DrawNotify (void)
{
	int		x, v;
	short	*text;
	int		i;
	int		time;
	int		skip;
	int		currentColor;

	currentColor = 7;
	rf->set2DColor( g_color_table[currentColor] );

	v = 0;
	for (i= con.current-NUM_CON_TIMES+1 ; i<=con.current ; i++)
	{
		if (i < 0)
			continue;
		time = con.times[i % NUM_CON_TIMES];
		if (time == 0)
			continue;
		time = cls.realtime - time;
		if (time > con_notifytime->value*1000)
			continue;
		text = con.text + (i % con.totallines)*con.linewidth;

		if (Key_GetCatcher( ) & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
			continue;
		}

		float curX = cl_conXOffset->integer + con.xadjust;
		for (x = 0 ; x < con.linewidth ; x++) {
			//if ( ( text[x] & 0xff ) == ' ' ) {
			//	continue;
			//}
			if ( ( (text[x]>>8)&7 ) != currentColor ) {
				currentColor = (text[x]>>8)&7;
				rf->set2DColor( g_color_table[currentColor] );
			}
			curX += rf->drawChar(curX, v, text[x] & 0xff );
		}

		v += SMALLCHAR_HEIGHT;
	}

	rf->set2DColor( NULL );

	if (Key_GetCatcher( ) & (KEYCATCH_UI | KEYCATCH_CGAME) ) {
		return;
	}

	// draw the chat line
	if ( Key_GetCatcher( ) & KEYCATCH_MESSAGE )
	{
		if (chat_team)
		{
			rf->drawString (8, v, "say_team:" );
			skip = 10;
		}
		else
		{
			rf->drawString (8, v, "say:" );
			skip = 5;
		}

		chatField.draw(skip * 16, v,
			SCREEN_WIDTH - ( skip + 1 ) * 16, true, true);

		v += 16;
	}

}

/*
================
Con_DrawSolidConsole

Draws the console with the solid background
================
*/
void Con_DrawSolidConsole( float frac ) {
	int				i, x, y;
	int				rows;
	short			*text;
	int				row;
	int				lines;
//	qhandle_t		conShader;
	int				currentColor;
	vec4_t			color;

	lines = rf->getWinHeight() * frac;
	if (lines <= 0)
		return;

	if (lines > rf->getWinHeight() )
		lines = rf->getWinHeight();

	// on wide screens, we will center the text
	con.xadjust = 0;

	// draw the background
	y = frac * rf->getWinHeight();
	if ( y < 1 ) {
		y = 0;
	}
	else {
		rf->drawStretchPic( 0, 0, rf->getWinWidth(), y, 0, 0, 1,1, cls.consoleShader );
	}

	color[0] = 1;
	color[1] = 0;
	color[2] = 0;
	color[3] = 1;
	rf->drawStretchPic( 0, y, SCREEN_WIDTH, 2, 0, 0,1,1,rf->registerMaterial("(0 0 0)"));


	// draw the version number

	rf->set2DColor( g_color_table[ColorIndex(COLOR_RED)] );
	x = rf->getWinWidth() - rf->getStringWidth(Q3_VERSION);
	rf->drawString(x,lines - SMALLCHAR_HEIGHT,Q3_VERSION);


	// draw the text
	con.vislines = lines;
	rows = (lines-SMALLCHAR_WIDTH)/SMALLCHAR_WIDTH;		// rows of text to draw

	y = lines - (SMALLCHAR_HEIGHT*3);

	// draw from the bottom up
	if (con.display != con.current)
	{
	// draw arrows to show the buffer is backscrolled
		rf->set2DColor( g_color_table[ColorIndex(COLOR_RED)] );
		for (x=0 ; x<con.linewidth ; x+=4)
			rf->drawChar( con.xadjust + (x+1)*SMALLCHAR_WIDTH, y, '^' );
		y -= SMALLCHAR_HEIGHT;
		rows--;
	}
	
	row = con.display;

	if ( con.x == 0 ) {
		row--;
	}

	currentColor = 7;
	rf->set2DColor( g_color_table[currentColor] );

	for (i=0 ; i<rows ; i++, y -= SMALLCHAR_HEIGHT, row--)
	{
		if (row < 0)
			break;
		if (con.current - row >= con.totallines) {
			// past scrollback wrap point
			continue;	
		}

		text = con.text + (row % con.totallines)*con.linewidth;

		float curX = con.xadjust;
		for (x=0 ; x<con.linewidth ; x++) {
	/*		if ( ( text[x] & 0xff ) == ' ' ) {
				continue;
			}*/

			if ( ( (text[x]>>8)&7 ) != currentColor ) {
				currentColor = (text[x]>>8)&7;
				rf->set2DColor( g_color_table[currentColor] );
			}
			curX += rf->drawChar(curX, y, text[x] & 0xff );
		}
	}

	// draw the input prompt, user text, and cursor if desired
	Con_DrawInput ();

	rf->set2DColor( NULL );
}



/*
==================
Con_DrawConsole
==================
*/
void Con_DrawConsole() {
	// check for console width changes from a vid mode change
	Con_CheckResize ();

	// if disconnected, render console full screen
	if ( clc.state == CA_DISCONNECTED ) {
		if ( !( Key_GetCatcher( ) & (KEYCATCH_UI | KEYCATCH_CGAME)) ) {
			Con_DrawSolidConsole( 1.0 );
			return;
		}
	}

	if ( con.displayFrac ) {
		Con_DrawSolidConsole( con.displayFrac );
	} else {
		// draw notify lines
		if ( clc.state == CA_ACTIVE ) {
			Con_DrawNotify ();
		}
	}
}

//================================================================

/*
==================
Con_RunConsole

Scroll it up or down
==================
*/
void Con_RunConsole (void) {
	// decide on the destination height of the console
	if ( Key_GetCatcher( ) & KEYCATCH_CONSOLE )
		con.finalFrac = 0.5;		// half screen
	else
		con.finalFrac = 0;				// none visible
	
	// scroll towards the destination height
	if (con.finalFrac < con.displayFrac)
	{
		con.displayFrac -= con_conspeed->value*cls.realFrametime*0.001;
		if (con.finalFrac > con.displayFrac)
			con.displayFrac = con.finalFrac;

	}
	else if (con.finalFrac > con.displayFrac)
	{
		con.displayFrac += con_conspeed->value*cls.realFrametime*0.001;
		if (con.finalFrac < con.displayFrac)
			con.displayFrac = con.finalFrac;
	}

}


void Con_PageUp() {
	con.display -= 2;
	if ( con.current - con.display >= con.totallines ) {
		con.display = con.current - con.totallines + 1;
	}
}

void Con_PageDown() {
	con.display += 2;
	if (con.display > con.current) {
		con.display = con.current;
	}
}

void Con_Top() {
	con.display = con.totallines;
	if ( con.current - con.display >= con.totallines ) {
		con.display = con.current - con.totallines + 1;
	}
}

void Con_Bottom() {
	con.display = con.current;
}


void Con_Close() {
	if ( !com_cl_running->integer ) {
		return;
	}
	g_consoleField.clear();
	Con_ClearNotify ();
	Key_SetCatcher( Key_GetCatcher( ) & ~KEYCATCH_CONSOLE );
	con.finalFrac = 0;				// none visible
	con.displayFrac = 0;
}
