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
#include "client.h"
#include <shared/colorTable.h>
#include <shared/keyCatchers.h>
#include <shared/parser.h>
#include <api/guiAPI.h>
#include <api/rAPI.h>
#include <common/autocompletion.h>

/*

key up events are sent even if in console mode

*/

field_s	historyEditLines[COMMAND_HISTORY];

int			nextHistoryLine;		// the last line in the history buffer, not masked
int			historyLine;	// the line being displayed from history buffer
							// will be <= nextHistoryLine

field_s		g_consoleField;
field_s		chatField;
bool	chat_team;

int			chat_playerNum;

int				anykeydown;
keyBind_s		keys[MAX_KEYS];


typedef struct {
	char	*name;
	int		keynum;
} keyname_t;


// names not in this list can either be lowercase ascii, or '0xnn' hex sequences
keyname_t keynames[] =
{
	{"TAB", K_TAB},
	{"ENTER", K_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},
	{"BACKSPACE", K_BACKSPACE},
	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},

	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},

	{"COMMAND", K_COMMAND},

	{"CAPSLOCK", K_CAPSLOCK},

	
	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},
	{"F13", K_F13},
	{"F14", K_F14},
	{"F15", K_F15},

	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGDN", K_PGDN},
	{"PGUP", K_PGUP},
	{"HOME", K_HOME},
	{"END", K_END},

	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},
	{"MOUSE4", K_MOUSE4},
	{"MOUSE5", K_MOUSE5},

	{"MWHEELUP",	K_MWHEELUP },
	{"MWHEELDOWN",	K_MWHEELDOWN },

	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},
	{"JOY5", K_JOY5},
	{"JOY6", K_JOY6},
	{"JOY7", K_JOY7},
	{"JOY8", K_JOY8},
	{"JOY9", K_JOY9},
	{"JOY10", K_JOY10},
	{"JOY11", K_JOY11},
	{"JOY12", K_JOY12},
	{"JOY13", K_JOY13},
	{"JOY14", K_JOY14},
	{"JOY15", K_JOY15},
	{"JOY16", K_JOY16},
	{"JOY17", K_JOY17},
	{"JOY18", K_JOY18},
	{"JOY19", K_JOY19},
	{"JOY20", K_JOY20},
	{"JOY21", K_JOY21},
	{"JOY22", K_JOY22},
	{"JOY23", K_JOY23},
	{"JOY24", K_JOY24},
	{"JOY25", K_JOY25},
	{"JOY26", K_JOY26},
	{"JOY27", K_JOY27},
	{"JOY28", K_JOY28},
	{"JOY29", K_JOY29},
	{"JOY30", K_JOY30},
	{"JOY31", K_JOY31},
	{"JOY32", K_JOY32},

	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"AUX16", K_AUX16},

	{"KP_HOME",			K_KP_HOME },
	{"KP_UPARROW",		K_KP_UPARROW },
	{"KP_PGUP",			K_KP_PGUP },
	{"KP_LEFTARROW",	K_KP_LEFTARROW },
	{"KP_5",			K_KP_5 },
	{"KP_RIGHTARROW",	K_KP_RIGHTARROW },
	{"KP_END",			K_KP_END },
	{"KP_DOWNARROW",	K_KP_DOWNARROW },
	{"KP_PGDN",			K_KP_PGDN },
	{"KP_ENTER",		K_KP_ENTER },
	{"KP_INS",			K_KP_INS },
	{"KP_DEL",			K_KP_DEL },
	{"KP_SLASH",		K_KP_SLASH },
	{"KP_MINUS",		K_KP_MINUS },
	{"KP_PLUS",			K_KP_PLUS },
	{"KP_NUMLOCK",		K_KP_NUMLOCK },
	{"KP_STAR",			K_KP_STAR },
	{"KP_EQUALS",		K_KP_EQUALS },

	{"PAUSE", K_PAUSE},
	
	{"SEMICOLON", ';'},	// because a raw semicolon seperates commands

	{"WORLD_0", K_WORLD_0},
	{"WORLD_1", K_WORLD_1},
	{"WORLD_2", K_WORLD_2},
	{"WORLD_3", K_WORLD_3},
	{"WORLD_4", K_WORLD_4},
	{"WORLD_5", K_WORLD_5},
	{"WORLD_6", K_WORLD_6},
	{"WORLD_7", K_WORLD_7},
	{"WORLD_8", K_WORLD_8},
	{"WORLD_9", K_WORLD_9},
	{"WORLD_10", K_WORLD_10},
	{"WORLD_11", K_WORLD_11},
	{"WORLD_12", K_WORLD_12},
	{"WORLD_13", K_WORLD_13},
	{"WORLD_14", K_WORLD_14},
	{"WORLD_15", K_WORLD_15},
	{"WORLD_16", K_WORLD_16},
	{"WORLD_17", K_WORLD_17},
	{"WORLD_18", K_WORLD_18},
	{"WORLD_19", K_WORLD_19},
	{"WORLD_20", K_WORLD_20},
	{"WORLD_21", K_WORLD_21},
	{"WORLD_22", K_WORLD_22},
	{"WORLD_23", K_WORLD_23},
	{"WORLD_24", K_WORLD_24},
	{"WORLD_25", K_WORLD_25},
	{"WORLD_26", K_WORLD_26},
	{"WORLD_27", K_WORLD_27},
	{"WORLD_28", K_WORLD_28},
	{"WORLD_29", K_WORLD_29},
	{"WORLD_30", K_WORLD_30},
	{"WORLD_31", K_WORLD_31},
	{"WORLD_32", K_WORLD_32},
	{"WORLD_33", K_WORLD_33},
	{"WORLD_34", K_WORLD_34},
	{"WORLD_35", K_WORLD_35},
	{"WORLD_36", K_WORLD_36},
	{"WORLD_37", K_WORLD_37},
	{"WORLD_38", K_WORLD_38},
	{"WORLD_39", K_WORLD_39},
	{"WORLD_40", K_WORLD_40},
	{"WORLD_41", K_WORLD_41},
	{"WORLD_42", K_WORLD_42},
	{"WORLD_43", K_WORLD_43},
	{"WORLD_44", K_WORLD_44},
	{"WORLD_45", K_WORLD_45},
	{"WORLD_46", K_WORLD_46},
	{"WORLD_47", K_WORLD_47},
	{"WORLD_48", K_WORLD_48},
	{"WORLD_49", K_WORLD_49},
	{"WORLD_50", K_WORLD_50},
	{"WORLD_51", K_WORLD_51},
	{"WORLD_52", K_WORLD_52},
	{"WORLD_53", K_WORLD_53},
	{"WORLD_54", K_WORLD_54},
	{"WORLD_55", K_WORLD_55},
	{"WORLD_56", K_WORLD_56},
	{"WORLD_57", K_WORLD_57},
	{"WORLD_58", K_WORLD_58},
	{"WORLD_59", K_WORLD_59},
	{"WORLD_60", K_WORLD_60},
	{"WORLD_61", K_WORLD_61},
	{"WORLD_62", K_WORLD_62},
	{"WORLD_63", K_WORLD_63},
	{"WORLD_64", K_WORLD_64},
	{"WORLD_65", K_WORLD_65},
	{"WORLD_66", K_WORLD_66},
	{"WORLD_67", K_WORLD_67},
	{"WORLD_68", K_WORLD_68},
	{"WORLD_69", K_WORLD_69},
	{"WORLD_70", K_WORLD_70},
	{"WORLD_71", K_WORLD_71},
	{"WORLD_72", K_WORLD_72},
	{"WORLD_73", K_WORLD_73},
	{"WORLD_74", K_WORLD_74},
	{"WORLD_75", K_WORLD_75},
	{"WORLD_76", K_WORLD_76},
	{"WORLD_77", K_WORLD_77},
	{"WORLD_78", K_WORLD_78},
	{"WORLD_79", K_WORLD_79},
	{"WORLD_80", K_WORLD_80},
	{"WORLD_81", K_WORLD_81},
	{"WORLD_82", K_WORLD_82},
	{"WORLD_83", K_WORLD_83},
	{"WORLD_84", K_WORLD_84},
	{"WORLD_85", K_WORLD_85},
	{"WORLD_86", K_WORLD_86},
	{"WORLD_87", K_WORLD_87},
	{"WORLD_88", K_WORLD_88},
	{"WORLD_89", K_WORLD_89},
	{"WORLD_90", K_WORLD_90},
	{"WORLD_91", K_WORLD_91},
	{"WORLD_92", K_WORLD_92},
	{"WORLD_93", K_WORLD_93},
	{"WORLD_94", K_WORLD_94},
	{"WORLD_95", K_WORLD_95},

	{"WINDOWS", K_SUPER},
	{"COMPOSE", K_COMPOSE},
	{"MODE", K_MODE},
	{"HELP", K_HELP},
	{"PRINT", K_PRINT},
	{"SYSREQ", K_SYSREQ},
	{"SCROLLOCK", K_SCROLLOCK },
	{"BREAK", K_BREAK},
	{"MENU", K_MENU},
	{"POWER", K_POWER},
	{"EURO", K_EURO},
	{"UNDO", K_UNDO},

	{NULL,0}
};


/*
=============================================================================

CONSOLE LINE EDITING

==============================================================================
*/

/*
====================
Console_Key

Handles history and console scrollback
====================
*/
void Console_Key (int key) {
	// ctrl-L clears screen
	if ( key == 'l' && keys[K_CTRL].down ) {
		Cbuf_AddText ("clear\n");
		return;
	}

	// enter finishes the line
	if ( key == K_ENTER || key == K_KP_ENTER ) {
		// if not in the game explicitly prepend a slash if needed
		if ( clc.state != CA_ACTIVE &&
				g_consoleField.buffer[0] &&
				g_consoleField.buffer[0] != '\\' &&
				g_consoleField.buffer[0] != '/' ) {
			char	temp[MAX_EDIT_LINE-1];

			Q_strncpyz( temp, g_consoleField.buffer, sizeof( temp ) );
			Com_sprintf( g_consoleField.buffer, sizeof( g_consoleField.buffer ), "\\%s", temp );
			g_consoleField.cursor++;
		}

		Com_Printf ( "]%s\n", g_consoleField.buffer );

		// leading slash is an explicit command
		if ( g_consoleField.buffer[0] == '\\' || g_consoleField.buffer[0] == '/' ) {
			Cbuf_AddText( g_consoleField.buffer+1 );	// valid command
			Cbuf_AddText ("\n");
		} else {
			// other text will be chat messages
			if ( !g_consoleField.buffer[0] ) {
				return;	// empty lines just scroll the console without adding to history
			} else {
				Cbuf_AddText ("cmd say ");
				Cbuf_AddText( g_consoleField.buffer );
				Cbuf_AddText ("\n");
			}
		}

		// copy line to history buffer
		historyEditLines[nextHistoryLine % COMMAND_HISTORY] = g_consoleField;
		nextHistoryLine++;
		historyLine = nextHistoryLine;

		g_consoleField.clear();

		g_consoleField.widthInChars = g_console_field_width;

		CL_SaveConsoleHistory( );

		if ( clc.state == CA_DISCONNECTED ) {
			SCR_UpdateScreen ();	// force an update, because the command
		}							// may take some time
		return;
	}

	// command completion

	if (key == K_TAB) {
		AC_AutoComplete(g_consoleField.buffer,g_consoleField.getMaxSize(),g_consoleField.getCursorPtr());
		return;
	}

	// command history (ctrl-p ctrl-n for unix style)

	if ( (key == K_MWHEELUP && keys[K_SHIFT].down) || ( key == K_UPARROW ) || ( key == K_KP_UPARROW ) ||
		 ( ( tolower(key) == 'p' ) && keys[K_CTRL].down ) ) {
		if ( nextHistoryLine - historyLine < COMMAND_HISTORY 
			&& historyLine > 0 ) {
			historyLine--;
		}
		g_consoleField = historyEditLines[ historyLine % COMMAND_HISTORY ];
		return;
	}

	if ( (key == K_MWHEELDOWN && keys[K_SHIFT].down) || ( key == K_DOWNARROW ) || ( key == K_KP_DOWNARROW ) ||
		 ( ( tolower(key) == 'n' ) && keys[K_CTRL].down ) ) {
		historyLine++;
		if (historyLine >= nextHistoryLine) {
			historyLine = nextHistoryLine;
			g_consoleField.clear();
			g_consoleField.widthInChars = g_console_field_width;
			return;
		}
		g_consoleField = historyEditLines[ historyLine % COMMAND_HISTORY ];
		return;
	}

	// console scrolling
	if ( key == K_PGUP ) {
		Con_PageUp();
		return;
	}

	if ( key == K_PGDN) {
		Con_PageDown();
		return;
	}

	if ( key == K_MWHEELUP) {	//----(SA)	added some mousewheel functionality to the console
		Con_PageUp();
		if(keys[K_CTRL].down) {	// hold <ctrl> to accelerate scrolling
			Con_PageUp();
			Con_PageUp();
		}
		return;
	}

	if ( key == K_MWHEELDOWN) {	//----(SA)	added some mousewheel functionality to the console
		Con_PageDown();
		if(keys[K_CTRL].down) {	// hold <ctrl> to accelerate scrolling
			Con_PageDown();
			Con_PageDown();
		}
		return;
	}

	// ctrl-home = top of console
	if ( key == K_HOME && keys[K_CTRL].down ) {
		Con_Top();
		return;
	}

	// ctrl-end = bottom of console
	if ( key == K_END && keys[K_CTRL].down ) {
		Con_Bottom();
		return;
	}

	// pass to the normal editline routine
	g_consoleField.keyDownEvent(key);
}

//============================================================================


/*
================
Message_Key

In game talk message
================
*/
void Message_Key( int key ) {

	char	buffer[MAX_STRING_CHARS];


	if (key == K_ESCAPE) {
		Key_SetCatcher( Key_GetCatcher( ) & ~KEYCATCH_MESSAGE );
		chatField.clear();
		return;
	}

	if ( key == K_ENTER || key == K_KP_ENTER )
	{
		if ( chatField.buffer[0] && clc.state == CA_ACTIVE ) {
			if (chat_playerNum != -1 )

				Com_sprintf( buffer, sizeof( buffer ), "tell %i \"%s\"\n", chat_playerNum, chatField.buffer );

			else if (chat_team)

				Com_sprintf( buffer, sizeof( buffer ), "say_team \"%s\"\n", chatField.buffer );
			else
				Com_sprintf( buffer, sizeof( buffer ), "say \"%s\"\n", chatField.buffer );



			CL_AddReliableCommand(buffer, false);
		}
		Key_SetCatcher( Key_GetCatcher( ) & ~KEYCATCH_MESSAGE );
		chatField.clear();
		return;
	}

	chatField.keyDownEvent(key);
}

//============================================================================

/*
===================
Key_IsDown
===================
*/
bool Key_IsDown( int keynum ) {
	if ( keynum < 0 || keynum >= MAX_KEYS ) {
		return false;
	}

	return keys[keynum].down;
}

/*
===================
Key_StringToKeynum

Returns a key number to be used to index keys[] by looking at
the given string.  Single ascii characters return themselves, while
the K_* names are matched up.

0x11 will be interpreted as raw hex, which will allow new controlers

to be configured even if they don't have defined names.
===================
*/
int Key_StringToKeynum( const char *str ) {
	keyname_t	*kn;
	
	if ( !str || !str[0] ) {
		return -1;
	}
	if ( !str[1] ) {
		return str[0];
	}

	// check for hex code
	if ( strlen( str ) == 4 ) {
		int n = Com_HexStrToInt( str );

		if ( n >= 0 ) {
			return n;
		}
	}

	// scan for a text match
	for ( kn=keynames ; kn->name ; kn++ ) {
		if ( !stricmp( str,kn->name ) )
			return kn->keynum;
	}

	return -1;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, a K_* name, or a 0x11 hex string) for the
given keynum.
===================
*/
char *Key_KeynumToString( int keynum ) {
	keyname_t	*kn;	
	static	char	tinystr[5];
	int			i, j;

	if ( keynum == -1 ) {
		return "<KEY NOT FOUND>";
	}

	if ( keynum < 0 || keynum >= MAX_KEYS ) {
		return "<OUT OF RANGE>";
	}

	// check for printable ascii (don't use quote)
	if ( keynum > 32 && keynum < 127 && keynum != '"' && keynum != ';' ) {
		tinystr[0] = keynum;
		tinystr[1] = 0;
		return tinystr;
	}

	// check for a key string
	for ( kn=keynames ; kn->name ; kn++ ) {
		if (keynum == kn->keynum) {
			return kn->name;
		}
	}

	// make a hex string
	i = keynum >> 4;
	j = keynum & 15;

	tinystr[0] = '0';
	tinystr[1] = 'x';
	tinystr[2] = i > 9 ? i - 10 + 'a' : i + '0';
	tinystr[3] = j > 9 ? j - 10 + 'a' : j + '0';
	tinystr[4] = 0;

	return tinystr;
}


/*
===================
Key_SetBinding
===================
*/
void Key_SetBinding( int keynum, const char *binding ) {
	if ( keynum < 0 || keynum >= MAX_KEYS ) {
		return;
	}

	// allocate memory for new binding
	keys[keynum].binding = binding;

	// consider this like modifying an archived cvar, so the
	// file write will be triggered at the next oportunity
	cvar_modifiedFlags |= CVAR_ARCHIVE;
}


/*
===================
Key_GetBinding
===================
*/
const char *Key_GetBinding( int keynum ) {
	if ( keynum < 0 || keynum >= MAX_KEYS ) {
		return "";
	}

	return keys[ keynum ].binding;
}

/* 
===================
Key_GetKey
===================
*/

int Key_GetKey(const char *binding) {
  int i;

  if (binding) {
  	for (i=0 ; i < MAX_KEYS ; i++) {
      if (keys[i].binding.length() && stricmp(binding, keys[i].binding) == 0) {
        return i;
      }
    }
  }
  return -1;
}

/*
===================
Key_Unbind_f
===================
*/
void Key_Unbind_f (void)
{
	int		b;

	if (Cmd_Argc() != 2)
	{
		Com_Printf ("unbind <key> : remove commands from a key\n");
		return;
	}
	
	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Com_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding (b, "");
}

/*
===================
Key_Unbindall_f
===================
*/
void Key_Unbindall_f (void)
{
	int		i;
	
	for (i=0 ; i < MAX_KEYS; i++)
		if (keys[i].binding.length())
			Key_SetBinding (i, "");
}


/*
===================
Key_Bind_f
===================
*/
void Key_Bind_f (void)
{
	int			i, c, b;
	char		cmd[1024];
	
	c = Cmd_Argc();

	if (c < 2)
	{
		Com_Printf ("bind <key> [command] : attach a command to a key\n");
		return;
	}
	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Com_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if (c == 2)
	{
		if (keys[b].binding.length())
			Com_Printf ("\"%s\" = \"%s\"\n", Cmd_Argv(1), keys[b].binding );
		else
			Com_Printf ("\"%s\" is not bound\n", Cmd_Argv(1) );
		return;
	}
	
// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for (i=2 ; i< c ; i++)
	{
		strcat (cmd, Cmd_Argv(i));
		if (i != (c-1))
			strcat (cmd, " ");
	}

	Key_SetBinding (b, cmd);
}

/*
============
Key_WriteBindings

Writes lines containing "bind key value"
============
*/
void Key_WriteBindings( fileHandle_t f ) {
	int		i;

	FS_Printf (f, "unbindall\n" );

	for (i=0 ; i<MAX_KEYS ; i++) {
		if (keys[i].binding.length()) {
			FS_Printf (f, "bind %s \"%s\"\n", Key_KeynumToString(i), keys[i].binding);

		}

	}
}


/*
============
Key_Bindlist_f

============
*/
void Key_Bindlist_f() {
	int		i;

	for ( i = 0 ; i < MAX_KEYS ; i++ ) {
		if ( keys[i].binding .length()) {
			Com_Printf( "%s \"%s\"\n", Key_KeynumToString(i), keys[i].binding );
		}
	}
}

/*
============
Key_KeynameCompletion
============
*/
void Key_KeynameCompletion( void(*callback)(const char *s) ) {
	int		i;

	for( i = 0; keynames[ i ].name != NULL; i++ )
		callback( keynames[ i ].name );
}

/*
====================
Key_CompleteUnbind
====================
*/
static void Key_CompleteUnbind( char *args, int argNum )
{
	if( argNum == 2 )
	{
		// Skip "unbind "
		char *p = Com_SkipTokens( args, 1, " " );

		if( p > args )
			AC_CompleteKeyname( );
	}
}

/*
====================
Key_CompleteBind
====================
*/
static void Key_CompleteBind( char *args, int argNum )
{
	char *p;

	if( argNum == 2 )
	{
		// Skip "bind "
		p = Com_SkipTokens( args, 1, " " );

		if( p > args )
			AC_CompleteKeyname( );
	}
	else if( argNum >= 3 )
	{
		// Skip "bind <key> "
		p = Com_SkipTokens( args, 2, " " );

		if( p > args )
			AC_CompleteCommand( p, true, true );
	}
}

/*
===================
CL_InitKeyCommands
===================
*/
void CL_InitKeyCommands() {
	// register our functions
	Cmd_AddCommand ("bind",Key_Bind_f);
	Cmd_SetCommandCompletionFunc( "bind", Key_CompleteBind );
	Cmd_AddCommand ("unbind",Key_Unbind_f);
	Cmd_SetCommandCompletionFunc( "unbind", Key_CompleteUnbind );
	Cmd_AddCommand ("unbindall",Key_Unbindall_f);
	Cmd_AddCommand ("bindlist",Key_Bindlist_f);
}

/*
===================
CL_ParseBinding

Execute the commands in the bind string
===================
*/
void CL_ParseBinding( int key, bool down, unsigned time )
{
	char buf[ MAX_STRING_CHARS ], *p = buf, *end;

	if( !keys[key].binding.length() )
		return;
	Q_strncpyz( buf, keys[key].binding, sizeof( buf ) );

	while( 1 )
	{
		while( isspace( *p ) )
			p++;
		end = strchr( p, ';' );
		if( end )
			*end = '\0';
		if( *p == '+' )
		{
			// button commands add keynum and time as parameters
			// so that multiple sources can be discriminated and
			// subframe corrected
			char cmd[1024];
			Com_sprintf( cmd, sizeof( cmd ), "%c%s %d %d\n",
				( down ) ? '+' : '-', p + 1, key, time );
			Cbuf_AddText( cmd );
		}
		else if( down )
		{
			// normal commands only execute on key press
			Cbuf_AddText( p );
			Cbuf_AddText( "\n" );
		}
		if( !end )
			break;
		p = end + 1;
	}
}

/*
===================
CL_KeyDownEvent

Called by CL_KeyEvent to handle a keypress
===================
*/
void CL_KeyDownEvent( int key, unsigned time )
{
	keys[key].down = true;
	keys[key].repeats++;
	if( keys[key].repeats == 1 && key != K_SCROLLOCK && key != K_KP_NUMLOCK && key != K_CAPSLOCK )
		anykeydown++;

	if( keys[K_ALT].down && key == K_ENTER )
	{
		Cvar_SetValue( "r_fullscreen",
			!Cvar_VariableIntegerValue( "r_fullscreen" ) );
		return;
	}

	// console key is hardcoded, so the user can never unbind it
	if( key == K_CONSOLE || ( keys[K_SHIFT].down && key == K_ESCAPE ) )
	{
		Con_ToggleConsole_f ();
		Key_ClearStates ();
		return;
	}


	// keys can still be used for bound actions
	if ( ( key < 128 || key == K_MOUSE1 ) &&
		( clc.demoplaying || clc.state == CA_CINEMATIC ) && Key_GetCatcher( ) == 0 ) {

		if (Cvar_VariableValue ("com_cameraMode") == 0) {
			Cvar_Set ("nextdemo","");
			key = K_ESCAPE;
		}
	}

	// escape is always handled special
	if ( key == K_ESCAPE ) {
		if ( Key_GetCatcher( ) & KEYCATCH_MESSAGE ) {
			// clear message mode
			Message_Key( key );
			return;
		}

		// escape always gets out of CGAME stuff
		if (Key_GetCatcher( ) & KEYCATCH_CGAME) {
			Key_SetCatcher( Key_GetCatcher( ) & ~KEYCATCH_CGAME );

			return;
		}

		if ( !( Key_GetCatcher( ) & KEYCATCH_UI ) ) {
			if ( clc.state == CA_ACTIVE && !clc.demoplaying ) {
//				VM_Call( uivm, UI_SET_ACTIVE_MENU, UIMENU_INGAME );
			}
			else if ( clc.state != CA_DISCONNECTED ) {
				CL_Disconnect_f();
		//		S_StopAllSounds();
//				VM_Call( uivm, UI_SET_ACTIVE_MENU, UIMENU_MAIN );
			}
			return;
		}

//		VM_Call( uivm, UI_KEY_EVENT, key, true );
		if(gui) {
		}
		return;
	}

	// distribute the key down event to the apropriate handler
	if ( Key_GetCatcher( ) & KEYCATCH_CONSOLE ) {
		Console_Key( key );
	} else if ( Key_GetCatcher( ) & KEYCATCH_UI ) {
//		if ( uivm ) {
//			VM_Call( uivm, UI_KEY_EVENT, key, true );
//		} 
		if(gui) {
			gui->onKeyDown(key);
		}
	} else if ( Key_GetCatcher( ) & KEYCATCH_CGAME ) {

	} else if ( Key_GetCatcher( ) & KEYCATCH_MESSAGE ) {
		Message_Key( key );
	} else if ( clc.state == CA_DISCONNECTED ) {
		Console_Key( key );
	} else {
		// send the bound action
		CL_ParseBinding( key, true, time );
	}
	return;
}

/*
===================
CL_KeyUpEvent

Called by CL_KeyEvent to handle a keyrelease
===================
*/
void CL_KeyUpEvent( int key, unsigned time )
{
	keys[key].repeats = 0;
	keys[key].down = false;
	if (key != K_SCROLLOCK && key != K_KP_NUMLOCK && key != K_CAPSLOCK)
		anykeydown--;

	if (anykeydown < 0) {
		anykeydown = 0;
	}

	// don't process key-up events for the console key
	if ( key == K_CONSOLE || ( key == K_ESCAPE && keys[K_SHIFT].down ) )
		return;

	//
	// key up events only perform actions if the game key binding is
	// a button command (leading + sign).  These will be processed even in
	// console mode and menu mode, to keep the character from continuing
	// an action started before a mode switch.
	//
	if( clc.state != CA_DISCONNECTED )
		CL_ParseBinding( key, false, time );

	if ( Key_GetCatcher( ) & KEYCATCH_UI && gui ) {
		gui->onKeyUp(key);
	//} else if ( Key_GetCatcher( ) & KEYCATCH_CGAME && g ) {

	}
}

/*
===================
CL_KeyEvent

Called by the system for both key up and key down events
===================
*/
void CL_KeyEvent (int key, bool down, unsigned time) {
	if( down )
		CL_KeyDownEvent( key, time );
	else
		CL_KeyUpEvent( key, time );
}

/*
===================
CL_CharEvent

Normal keyboard characters, already shifted / capslocked / etc
===================
*/
void CL_CharEvent( int key ) {
	// delete is not a printable character and is
	// otherwise handled by Field_KeyDownEvent
	if ( key == 127 ) {
		return;
	}

	// distribute the key down event to the apropriate handler
	if ( Key_GetCatcher( ) & KEYCATCH_CONSOLE )
	{
		g_consoleField.charEvent(key);
	}
	else if ( Key_GetCatcher( ) & KEYCATCH_UI )
	{
//		VM_Call( uivm, UI_KEY_EVENT, key | K_CHAR_FLAG, true );
	}
	else if ( Key_GetCatcher( ) & KEYCATCH_MESSAGE ) 
	{
		chatField.charEvent(key);
	}
	else if ( clc.state == CA_DISCONNECTED )
	{
		g_consoleField.charEvent(key);
	}
}


/*
===================
Key_ClearStates
===================
*/
void Key_ClearStates (void)
{
	int		i;

	anykeydown = 0;

	for ( i=0 ; i < MAX_KEYS ; i++ ) {
		if (i == K_SCROLLOCK || i == K_KP_NUMLOCK || i == K_CAPSLOCK)
			continue;

		if ( keys[i].down ) {
			CL_KeyEvent( i, false, 0 );

		}
		keys[i].down = 0;
		keys[i].repeats = 0;
	}
}

static int keyCatchers = 0;

/*
====================
Key_GetCatcher
====================
*/
int Key_GetCatcher() {
	return keyCatchers;
}

/*
====================
Key_SetCatcher
====================
*/
void Key_SetCatcher( int catcher ) {
	// If the catcher state is changing, clear all key states
	if( catcher != keyCatchers )
		Key_ClearStates( );

	keyCatchers = catcher;
}

// This must not exceed MAX_CMD_LINE
#define			MAX_CONSOLE_SAVE_BUFFER	1024
#define			CONSOLE_HISTORY_FILE    "qiohistory.console"
static char	consoleSaveBuffer[ MAX_CONSOLE_SAVE_BUFFER ];
static int	consoleSaveBufferSize = 0;

/*
================
CL_LoadConsoleHistory

Load the console history from cl_consoleHistory
================
*/
void CL_LoadConsoleHistory()
{
	const char					*token;
	int						i, numChars, numLines = 0;
	fileHandle_t	f;

	consoleSaveBufferSize = FS_FOpenFileRead( CONSOLE_HISTORY_FILE, &f, false );
	if( !f )
	{
		Com_Printf( "Couldn't read %s.\n", CONSOLE_HISTORY_FILE );
		return;
	}

	if( consoleSaveBufferSize <= MAX_CONSOLE_SAVE_BUFFER &&
			FS_Read( consoleSaveBuffer, consoleSaveBufferSize, f ) == consoleSaveBufferSize )
	{
		parser_c p;
		p.setup(consoleSaveBuffer);
	
		for( i = COMMAND_HISTORY - 1; i >= 0; i-- )
		{
			if( p.atEOF() )
				break;

			token = p.getToken();

			historyEditLines[ i ].cursor = atoi( token );

			if( p.atEOF() )
				break;

			token = p.getToken();

			historyEditLines[ i ].scroll = atoi( token );

			if( p.atEOF() )
				break;

			token = p.getToken();

			numChars = atoi( token );
			p.skipToNextToken();

			if( numChars > ( strlen( consoleSaveBuffer ) -	( p.getCurDataPtr() - consoleSaveBuffer ) ) )
			{
				Com_DPrintf( S_COLOR_YELLOW "WARNING: probable corrupt history\n" );
				break;
			}
			const char *nowAt = p.getCurDataPtr();
			memcpy( historyEditLines[ i ].buffer,
				nowAt, numChars );
			historyEditLines[ i ].buffer[ numChars ] = '\0';
			p.skipBytes(numChars);

			numLines++;
		}

		memmove( &historyEditLines[ 0 ], &historyEditLines[ i + 1 ],
				numLines * sizeof( field_s ) );
		for( i = numLines; i < COMMAND_HISTORY; i++ )
			historyEditLines[ i ].clear();

		historyLine = nextHistoryLine = numLines;
	}
	else
		Com_Printf( "Couldn't read %s.\n", CONSOLE_HISTORY_FILE );

	FS_FCloseFile( f );
}

/*
================
CL_SaveConsoleHistory

Save the console history into the cvar cl_consoleHistory
so that it persists across invocations of q3
================
*/
void CL_SaveConsoleHistory()
{
	int						i;
	int						lineLength, saveBufferLength, additionalLength;
	fileHandle_t	f;

	consoleSaveBuffer[ 0 ] = '\0';

	i = ( nextHistoryLine - 1 ) % COMMAND_HISTORY;
	do
	{
		if( historyEditLines[ i ].buffer[ 0 ] )
		{
			lineLength = strlen( historyEditLines[ i ].buffer );
			saveBufferLength = strlen( consoleSaveBuffer );

			//ICK
			additionalLength = lineLength + strlen( "999 999 999  " );

			if( saveBufferLength + additionalLength < MAX_CONSOLE_SAVE_BUFFER )
			{
				Q_strcat( consoleSaveBuffer, MAX_CONSOLE_SAVE_BUFFER,
						va( "%d %d %d %s ",
						historyEditLines[ i ].cursor,
						historyEditLines[ i ].scroll,
						lineLength,
						historyEditLines[ i ].buffer ) );
			}
			else
				break;
		}
		i = ( i - 1 + COMMAND_HISTORY ) % COMMAND_HISTORY;
	}
	while( i != ( nextHistoryLine - 1 ) % COMMAND_HISTORY );

	consoleSaveBufferSize = strlen( consoleSaveBuffer );

	f = FS_FOpenFileWrite( CONSOLE_HISTORY_FILE );
	if( !f )
	{
		Com_Printf( "Couldn't write %s.\n", CONSOLE_HISTORY_FILE );
		return;
	}

	if( FS_Write( consoleSaveBuffer, consoleSaveBufferSize, f ) < consoleSaveBufferSize )
		Com_Printf( "Couldn't write %s.\n", CONSOLE_HISTORY_FILE );

	FS_FCloseFile( f );
}
