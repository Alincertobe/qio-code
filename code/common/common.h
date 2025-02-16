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
// common.h -- definitions common between client and server, but not game.or ref modules
#ifndef _QCOMMON_H_
#define _QCOMMON_H_

#include <api/vfsAPI.h>
#include <shared/cvarFlags.h>
#include <shared/cvar.h>
#include <shared/str.h>

#define C_ONLY

#define PRODUCT_NAME			"Qio"
#define BASEGAME			"baseqio"
#define CLIENT_WINDOW_TITLE     	"Qio"
#define CLIENT_WINDOW_MIN_TITLE 	"Qio"
#define GAMENAME_FOR_MASTER		"QioTestGame"	// must NOT contain whitespace

// Heartbeat for dpmaster protocol. You shouldn't change this unless you know what you're doing
#define HEARTBEAT_FOR_MASTER		"DarkPlaces"

#ifndef PRODUCT_VERSION
  #define PRODUCT_VERSION "0.1"
#endif

#define Q3_VERSION PRODUCT_NAME " " PRODUCT_VERSION

#define MAX_MASTER_SERVERS      5	// number of supported master servers

#define DEMOEXT	"dm_"			// standard demo extension



#ifdef _MSC_VER

#pragma warning(disable : 4018)     // signed/unsigned mismatch
#pragma warning(disable : 4032)
#pragma warning(disable : 4051)
#pragma warning(disable : 4057)		// slightly different base types
#pragma warning(disable : 4100)		// unreferenced formal parameter
#pragma warning(disable : 4115)
#pragma warning(disable : 4125)		// decimal digit terminates octal escape sequence
#pragma warning(disable : 4127)		// conditional expression is constant
#pragma warning(disable : 4136)
#pragma warning(disable : 4152)		// nonstandard extension, function/data pointer conversion in expression
//#pragma warning(disable : 4201)
//#pragma warning(disable : 4214)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)		// benign redefinition
//#pragma warning(disable : 4305)		// truncation from const double to float
//#pragma warning(disable : 4310)		// cast truncates constant value
//#pragma warning(disable:  4505) 	// unreferenced local function has been removed
#pragma warning(disable : 4514)
#pragma warning(disable : 4702)		// unreachable code
#pragma warning(disable : 4711)		// selected for automatic inline expansion
#pragma warning(disable : 4220)		// varargs matches remaining parameters
//#pragma intrinsic( memset, memcpy )
#endif

//Ignore __attribute__ on non-gcc platforms
#ifndef __GNUC__
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

#if (defined _MSC_VER)
#define Q_EXPORT __declspec(dllexport)
#elif (defined __SUNPRO_C)
#define Q_EXPORT __global
#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
#define Q_EXPORT __attribute__((visibility("default")))
#else
#define Q_EXPORT
#endif


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

#ifdef _MSC_VER
  #include <io.h>

  typedef __int64 int64_t;
  typedef __int32 int32_t;
  typedef __int16 int16_t;
//  typedef __int8 int8_t;
  typedef unsigned __int64 uint64_t;
  typedef unsigned __int32 uint32_t;
  typedef unsigned __int16 uint16_t;
  typedef unsigned __int8 uint8_t;
#else
  #include <stdint.h>

#endif



#include "q_platform.h"

//=============================================================

#include "../shared/typedefs.h"



//Ignore __attribute__ on non-gcc platforms
#ifndef __GNUC__
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

//#define	PRE_RELEASE_DEMO

//============================================================================


#ifndef NULL
#define NULL ((void *)0)
#endif

#define ARRAY_LEN(x)			(sizeof(x) / sizeof(*(x)))


#define STRING(s)			#s
// expand constants before stringifying them
#define XSTRING(s)			STRING(s)

#define	MAX_QPATH			256		// max length of a quake game pathname
#ifdef PATH_MAX
#define MAX_OSPATH			PATH_MAX
#else
#define	MAX_OSPATH			256		// max length of a filesystem pathname
#endif

#define	MAX_NAME_LENGTH		64		// max length of a client name


//=============================================

int Com_HexStrToInt( const char *str );

int QDECL Com_sprintf (char *dest, int size, const char *fmt, ...) __attribute__ ((format (printf, 3, 4)));

char *Com_SkipTokens( char *s, int numTokens, char *sep );


// buffer size safe library replacements
void	Q_strncpyz( char *dest, const char *src, int destsize );
void	Q_strcat( char *dest, int size, const char *src );


//
// msg.c
//
class msg_c {
public:
	bool	allowoverflow;	// if false, do a Com_Error
	bool	overflowed;		// set to true if the buffer size failed (with allowoverflow set)
	bool	oob;			// set to true if the buffer size failed (with allowoverflow set)
	byte	*data;
	int		maxsize;
	int		cursize;
	int		readcount;
	int		bit;				// for bitwise reads and writes

	void compress(int ofs);
	void decompress(int ofs);


	void init(byte *data, int length);
	void initOOB(byte *data, int length);
	void clear();
	void bitstream();
	void beginReading();
	void beginReadingOOB();

	void writeBits(int value, int bits);
	int readBits(int bits);


	void writeChar(int c);
	void writeByte(int c);
	void writeData(const void *data, int length);
	void writeShort(int c);
	void writeLong(int c);
	void writeFloat( float f);
	void writeString(const char *s);
	void writeBigString(const char *s);
	void writeAngle(float f);
	void writeAngle16(float f);



	// returns -1 if no more characters are available
	int readChar();
	int readByte();
	int readShort();
	int readUShort();
	int readLong();
	float readFloat();
	char *readString(bool bFixFormatChars = true);
	char *readBigString();
	char *readStringLine();
	float readAngle16();
	void readData(void *data, int len);




	void writeDeltaUsercmd(struct userCmd_s *from, struct userCmd_s *to);
	void readDeltaUsercmd(struct userCmd_s *from, struct userCmd_s *to);

	void writeDeltaUsercmdKey(int key, userCmd_s *from, userCmd_s *to);
	void readDeltaUsercmdKey(int key, userCmd_s *from, userCmd_s *to);

	void writeDeltaEntity(struct entityState_s *from, struct entityState_s *to, bool force);
	void readDeltaEntity(entityState_s *from, entityState_s *to, int number);

	void writeDeltaPlayerstate(struct playerState_s *from, struct playerState_s *to);
	void readDeltaPlayerstate(struct playerState_s *from, struct playerState_s *to);
};




// TTimo
// copy a msg_c in case we need to store it as is for a bit
// (as I needed this to keep an msg_c from a static var for later use)
// sets data buffer as MSG_Init does prior to do the copy
void MSG_Copy(msg_c *buf, byte *data, int length, msg_c *src);

struct userCmd_s;
struct entityState_s;
struct playerState_s;

int MSG_HashKey(const char *string, int maxlen);


void MSG_ReportChangeVectors_f();

//============================================================================

/*
==============================================================

NET

==============================================================
*/

#define NET_ENABLEV4            0x01
#define NET_ENABLEV6            0x02
// if this flag is set, always attempt ipv6 connections instead of ipv4 if a v6 address is found.
#define NET_PRIOV6              0x04
// disables ipv6 multicast support if set.
#define NET_DISABLEMCAST        0x08


#define	PACKET_BACKUP	32	// number of old messages that must be kept on client and
							// server for delta comrpession and ping estimation
#define	PACKET_MASK		(PACKET_BACKUP-1)

#define	MAX_PACKET_USERCMDS		32		// max number of userCmd_s in a packet

#define	PORT_ANY			-1

#define	MAX_RELIABLE_COMMANDS	64			// max string commands buffered for restransmit

enum netAdrType_e {
	NA_BAD = 0,					// an address lookup failed
	NA_BOT,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IP6,
	NA_MULTICAST6,
	NA_UNSPEC
};

enum netSrc_e {
	NS_CLIENT,
	NS_SERVER
};

#define NET_ADDRSTRMAXLEN 48	// maximum length of an IPv6 address string including trailing '\0'
struct netAdr_s {
	netAdrType_e	type;

	byte	ip[4];
	byte	ip6[16];

	unsigned short	port;
	unsigned long	scope_id;	// Needed for IPv6 link-local addresses
};

void		NET_Init();
void		NET_Shutdown();
void		NET_Restart_f();
void		NET_Config( bool enableNetworking );
void		NET_FlushPacketQueue(void);
void		NET_SendPacket (netSrc_e sock, int length, const void *data, netAdr_s to);
void		QDECL NET_OutOfBandPrint( netSrc_e net_socket, netAdr_s adr, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
void		QDECL NET_OutOfBandData( netSrc_e sock, netAdr_s adr, byte *format, int len );

bool	NET_CompareAdr (netAdr_s a, netAdr_s b);
bool	NET_CompareBaseAdrMask(netAdr_s a, netAdr_s b, int netmask);
bool	NET_CompareBaseAdr (netAdr_s a, netAdr_s b);
bool	NET_IsLocalAddress (netAdr_s adr);
const char	*NET_AdrToString (netAdr_s a);
const char	*NET_AdrToStringwPort (netAdr_s a);
int		NET_StringToAdr ( const char *s, netAdr_s *a, netAdrType_e family);
bool	NET_GetLoopPacket (netSrc_e sock, netAdr_s *net_from, msg_c *net_message);
void		NET_JoinMulticast6(void);
void		NET_LeaveMulticast6(void);
void		NET_Sleep(int msec);


#define	MAX_MSGLEN				131072 // 65536 //16384		// max length of a message, which may
											// be fragmented into multiple packets

#define MAX_DOWNLOAD_WINDOW		48	// ACK window of 48 download chunks. Cannot set this higher, or clients
						// will overflow the reliable commands buffer
#define MAX_DOWNLOAD_BLKSIZE		1024	// 896 byte block chunks

#define NETCHAN_GENCHECKSUM(challenge, sequence) ((challenge) ^ ((sequence) * (challenge)))

/*
Netchan handles packet fragmentation and out of order / duplicate suppression
*/

struct netchan_t  {
	netSrc_e	sock;

	int			dropped;			// between last packet and previous

	netAdr_s	remoteAddress;
	int			qport;				// qport value to write when transmitting

	// sequencing variables
	int			incomingSequence;
	int			outgoingSequence;

	// incoming fragment assembly buffer
	int			fragmentSequence;
	int			fragmentLength;	
	byte		fragmentBuffer[MAX_MSGLEN];

	// outgoing fragment buffer
	// we need to space out the sending of large fragmented messages
	bool	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];

	int			challenge;
	int		lastSentTime;
	int		lastSentSize;
};

void Netchan_Init( int qport );
void Netchan_Setup(netSrc_e sock, netchan_t *chan, netAdr_s adr, int qport, int challenge, bool compat);

void Netchan_Transmit( netchan_t *chan, int length, const byte *data );
void Netchan_TransmitNextFragment( netchan_t *chan );

bool Netchan_Process( netchan_t *chan, msg_c *msg );


/*
==============================================================

PROTOCOL

==============================================================
*/

#define	PROTOCOL_VERSION	1

// maintain a list of compatible protocols for demo playing
// NOTE: that stuff only works with two digits protocols
extern int demo_protocols[];

// we dont have anything like that now
//#if !defined UPDATE_SERVER_NAME && !defined STANDALONE
//#define	UPDATE_SERVER_NAME	"update.quake3arena.com"
//#endif
//// override on command line, config files etc.
//#ifndef MASTER_SERVER_NAME
//#define MASTER_SERVER_NAME	"master.quake3arena.com"
//#endif


#define	PORT_MASTER			27950
#define	PORT_UPDATE			27951
#define	PORT_SERVER			27960
#define	NUM_SERVER_PORTS	4		// broadcast scan this many ports after
									// PORT_SERVER so a single machine can
									// run multiple servers


// the svc_strings[] array in cl_parse.c should mirror this
//
// server to client
//
enum svc_ops_e {
	svc_bad,
	svc_nop,
	svc_gamestate,
	svc_configstring,			// [short] [string] only in gamestate messages
	svc_baseline,				// only in gamestate messages
	svc_serverCommand,			// [string] to be executed by client game module
	svc_download,				// [short] size [size bytes]
	svc_snapshot,
	svc_EOF,

// new commands, supported only by ioquake3 protocol but not legacy
	svc_voip,     // not wrapped in USE_VOIP, so this value is reserved.
};


//
// client to server
//
enum clc_ops_e {
	clc_bad,
	clc_nop, 		
	clc_move,				// [[userCmd_s]
	clc_moveNoDelta,		// [[userCmd_s]
	clc_clientCommand,		// [string] message
	clc_EOF,

// new commands, supported only by ioquake3 protocol but not legacy
	clc_voip,   // not wrapped in USE_VOIP, so this value is reserved.
};

/*
==============================================================

CMD

Command text buffering and command execution

==============================================================
*/

/*

Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but entire text
files can be execed.

*/

void Cbuf_Init (void);
// allocates an initial text buffer that will grow as needed

void Cbuf_AddText( const char *text );
// Adds command text at the end of the buffer, does NOT add a final \n

void Cbuf_ExecuteText( int exec_when, const char *text );
// this can be used in place of either Cbuf_AddText or Cbuf_InsertText

void Cbuf_Execute (void);
// Pulls off \n terminated lines of text from the command buffer and sends
// them through Cmd_ExecuteString.  Stops when the buffer is empty.
// Normally called once per frame, but may be explicitly invoked.
// Do not call inside a command function, or current args will be destroyed.

//===========================================================================

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

*/

typedef void (*xcommand_t) (void);

void	Cmd_Init (void);

void	Cmd_AddCommand( const char *cmd_name, xcommand_t function );
// called by the init functions of other parts of the program to
// register commands and functions to call for them.
// The cmd_name is referenced later, so it should not be in temp memory
// if function is NULL, the command will be forwarded to the server
// as a clc_clientCommand instead of executed locally

void	Cmd_RemoveCommand( const char *cmd_name );

typedef void (*completionFunc_t)( char *args, int argNum );

// don't allow VMs to remove system commands
void	Cmd_RemoveCommandSafe( const char *cmd_name );

void	Cmd_CommandCompletion( void(*callback)(const char *s) );
// callback with each valid string
void Cmd_SetCommandCompletionFunc( const char *command,
	completionFunc_t complete );
void Cmd_CompleteArgument( const char *command, char *args, int argNum );
void Cmd_CompleteCfgName( char *args, int argNum );
void Cmd_CompleteTGAName( char *args, int argNum );
void Cmd_CompleteJPGName( char *args, int argNum );
void Cmd_CompletePNGName( char *args, int argNum );

int		Cmd_Argc (void);
const char*Cmd_Argv (int arg);
void	Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength );
char	*Cmd_Args (void);
char	*Cmd_ArgsFrom( int arg );
void	Cmd_ArgsBuffer( char *buffer, int bufferLength );
char	*Cmd_Cmd (void);
void	Cmd_Args_Sanitize();
// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are allways safe.

void	Cmd_TokenizeString( const char *text );
void	Cmd_TokenizeStringIgnoreQuotes( const char *text_in );
// Takes a null terminated string.  Does not need to be /n terminated.
// breaks the string up into arg tokens.

void	Cmd_ExecuteString( const char *text );
// Parses a single line of text into arguments and tries to execute it
// as if it was typed at the console


/*
==============================================================

CVAR

==============================================================
*/

/*

cvar_s variables are used to hold scalar or string variables that can be changed
or displayed at the console or prog code as well as accessed directly
in C code.

The user can access cvars from the console in three ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
set r_draworder 0	as above, but creates the cvar if not present

Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.

The are also occasionally used to communicated information between different
modules of the program.

*/

cvar_s *Cvar_Get( const char *var_name, const char *value, int flags );
// creates the variable if it doesn't exist, or returns the existing one
// if it exists, the value will not be changed, but flags will be ORed in
// that allows variables to be unarchived without needing bitflags
// if value is "", the value will not override a previously set value.

void 	Cvar_Set( const char *var_name, const char *value );
// will create the variable with no flags if it doesn't exist

cvar_s	*Cvar_Set2(const char *var_name, const char *value, bool force);
// same as Cvar_Set, but allows more control over setting of cvar

void	Cvar_SetSafe( const char *var_name, const char *value );
// sometimes we set variables from an untrusted source: fail if flags & CVAR_PROTECTED

void Cvar_SetLatched( const char *var_name, const char *value);
// don't set the cvar immediately

void	Cvar_SetValue( const char *var_name, float value );
void	Cvar_SetValueSafe( const char *var_name, float value );
// expands value to a string and calls Cvar_Set/Cvar_SetSafe

float	Cvar_VariableValue( const char *var_name );
int		Cvar_VariableIntegerValue( const char *var_name );
// returns 0 if not defined or non numeric

char	*Cvar_VariableString( const char *var_name );
void	Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
// returns an empty string if not defined

int	Cvar_Flags(const char *var_name);
// returns CVAR_NONEXISTENT if cvar doesn't exist or the flags of that particular CVAR.

void	Cvar_CommandCompletion( void(*callback)(const char *s) );
// callback with each valid string

void 	Cvar_Reset( const char *var_name );
void 	Cvar_ForceReset(const char *var_name);

void	Cvar_SetCheatState();
// reset all testing vars to a safe value

bool Cvar_Command();
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void 	Cvar_WriteVariables( fileHandle_t f );
// writes lines containing "set variable value" for all variables
// with the archive flag set to true.

void	Cvar_Init();

char	*Cvar_InfoString( int bit );
// returns an info string containing all the cvars that have the given bit set
// in their flags ( CVAR_USERINFO, CVAR_SERVERINFO, CVAR_SYSTEMINFO, etc )
void	Cvar_InfoStringBuffer( int bit, char *buff, int buffsize );
void Cvar_CheckRange( cvar_s *cv, float minVal, float maxVal, bool shouldBeIntegral );

void	Cvar_Restart(bool unsetVM);
void	Cvar_Restart_f();

void Cvar_CompleteCvarName( char *args, int argNum );

extern	int			cvar_modifiedFlags;
// whenever a cvar is modifed, its flags will be OR'd into this, so
// a single check can determine if any CVAR_USERINFO, CVAR_SERVERINFO,
// etc, variables have been modified since the last check.  The bit
// can then be cleared to allow another change detection.

/*
==============================================================

FILESYSTEM

No stdio calls should be used by any part of the game, because
we need to deal with all sorts of directory and seperator char
issues.
==============================================================
*/

// referenced flags
// these are in loop specific order so don't change the order
#define FS_GENERAL_REF	0x01
#define FS_UI_REF		0x02
#define FS_CGAME_REF	0x04

#define	MAX_FILE_HANDLES	64

#ifdef DEDICATED
#	define QIOCONFIG_CFG "qioconfig_server.cfg"
#else
#	define QIOCONFIG_CFG "qioconfig.cfg"
#endif

bool FS_Initialized();

void	FS_InitFilesystem ();
void	FS_Shutdown( bool closemfp );

bool FS_ConditionalRestart(int checksumFeed, bool disconnect);
void	FS_Restart( int checksumFeed );
// shutdown and restart the filesystem so changes to fs_gamedir can take effect

void FS_AddGameDirectory( const char *path, const char *dir );

char	**FS_ListFiles( const char *directory, const char *extension, int *numfiles );
// directory should not have either a leading or trailing /
// if extension is "/", only subdirectories will be returned
// the returned files will not include any directories or /

void	FS_FreeFileList( char **list );

bool FS_FileExists( const char *file );

bool FS_CreatePath (char *OSPath);

char   *FS_BuildOSPath( const char *base, const char *game, const char *qpath );
bool FS_CompareZipChecksum(const char *zipfile);

int		FS_LoadStack();

int		FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize );
int		FS_GetModList(  char *listbuf, int bufsize );

fileHandle_t	FS_FOpenFileWrite( const char *qpath );
fileHandle_t	FS_FOpenFileAppend( const char *filename );
fileHandle_t	FS_FCreateOpenPipeFile( const char *filename );
// will properly create any needed paths and deal with seperater character issues

fileHandle_t FS_SV_FOpenFileWrite( const char *filename );
long		FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp );
void	FS_SV_Rename( const char *from, const char *to );
long		FS_FOpenFileRead( const char *qpath, fileHandle_t *file, bool uniqueFILE );
// if uniqueFILE is true, then a new FILE will be fopened even if the file
// is found in an already open pak file.  If uniqueFILE is false, you must call
// FS_FCloseFile instead of fclose, otherwise the pak FILE would be improperly closed
// It is generally safe to always set uniqueFILE to true, because the majority of
// file IO goes through FS_ReadFile, which Does The Right Thing already.

int		FS_FileIsInPAK(const char *filename, int *pChecksum );
// returns 1 if a file is in the PAK file, otherwise -1

int		FS_Write( const void *buffer, int len, fileHandle_t f );

int		FS_Read2( void *buffer, int len, fileHandle_t f );
int		FS_Read( void *buffer, int len, fileHandle_t f );
// properly handles partial reads and reads from other dlls

void	FS_FCloseFile( fileHandle_t f );
// note: you can't just fclose from another DLL, due to MS libc issues

long	FS_ReadFileDir(const char *qpath, void *searchPath, bool unpure, void **buffer);
long	FS_ReadFile(const char *qpath, void **buffer);
// returns the length of the file
// a null buffer will just return the file length without loading
// as a quick check for existance. -1 length == not present
// A 0 byte will always be appended at the end, so string ops are safe.
// the buffer should be considered read-only, because it may be cached
// for other uses.

void	FS_ForceFlush( fileHandle_t f );
// forces flush on files we're writing to.

void	FS_FreeFile( void *buffer );
// frees the memory returned by FS_ReadFile

void	FS_WriteFile( const char *qpath, const void *buffer, int size );
// writes a complete file, creating any subdirectories needed

long FS_filelength(fileHandle_t f);
// doesn't work for files that are opened from a pack file

int		FS_FTell( fileHandle_t f );
// where are we?

void	FS_Flush( fileHandle_t f );

void 	QDECL FS_Printf( fileHandle_t f, const char *fmt, ... ) __attribute__ ((format (printf, 2, 3)));
// like fprintf

int		FS_FOpenFileByMode( const char *qpath, fileHandle_t *f, fsMode_e mode );
// opens a file for reading, writing, or appending depending on the value of mode

int		FS_Seek( fileHandle_t f, long offset, int origin );
// seek on a file (doesn't work for zip files!!!!!!!!)

bool FS_FilenameCompare( const char *s1, const char *s2 );

const char *FS_LoadedPakNames();
const char *FS_LoadedPakChecksums();
const char *FS_LoadedPakPureChecksums();
// Returns a space separated string containing the checksums of all loaded pk3 files.
// Servers with sv_pure set will get this string and pass it to clients.

const char *FS_ReferencedPakNames();
const char *FS_ReferencedPakChecksums();
const char *FS_ReferencedPakPureChecksums();
// Returns a space separated string containing the checksums of all loaded 
// AND referenced pk3 files. Servers with sv_pure set will get this string 
// back from clients for pure validation 

void FS_ClearPakReferences( int flags );
// clears referenced booleans on loaded pk3s

void FS_PureServerSetReferencedPaks( const char *pakSums, const char *pakNames );
void FS_PureServerSetLoadedPaks( const char *pakSums, const char *pakNames );
// If the string is empty, all data sources will be allowed.
// If not empty, only pk3 files that match one of the space
// separated checksums will be checked for files, with the
// sole exception of .cfg files.

bool FS_CheckDirTraversal(const char *checkdir);
bool FS_ComparePaks( char *neededpaks, int len, bool dlstring );

void FS_Rename( const char *from, const char *to );

void FS_Remove( const char *osPath );
void FS_HomeRemove( const char *homePath );

void	FS_FilenameCompletion( const char *dir, const char *ext,
		bool stripExt, void(*callback)(const char *s), bool allowNonPureFilesOnDisk );

const char *FS_GetCurrentGameDir(void);
bool FS_Which(const char *filename, void *searchPath);

/*
==============================================================

MISC

==============================================================
*/

// returned by Sys_GetProcessorFeatures
typedef enum
{
  CF_RDTSC      = 1 << 0,
  CF_MMX        = 1 << 1,
  CF_MMX_EXT    = 1 << 2,
  CF_3DNOW      = 1 << 3,
  CF_3DNOW_EXT  = 1 << 4,
  CF_SSE        = 1 << 5,
  CF_SSE2       = 1 << 6,
  CF_ALTIVEC    = 1 << 7
} cpuFeatures_t;

// centralized and cleaned, that's the max string you can send to a Com_Printf / Com_DPrintf (above gets truncated)
#define	MAXPRINTMSG	4096

// real time
//=============================================

struct qtime_s {
	int tm_sec;     /* seconds after the minute - [0,59] */
	int tm_min;     /* minutes after the hour - [0,59] */
	int tm_hour;    /* hours since midnight - [0,23] */
	int tm_mday;    /* day of the month - [1,31] */
	int tm_mon;     /* months since January - [0,11] */
	int tm_year;    /* years since 1900 */
	int tm_wday;    /* days since Sunday - [0,6] */
	int tm_yday;    /* days since January 1 - [0,365] */
	int tm_isdst;   /* daylight savings time flag */
};

enum sysEventType_e {
	// SE_NONE must be zero
	SE_NONE = 0,		// evTime is still valid
	SE_KEY,			// evValue is a key code, evValue2 is the down flag
	SE_CHAR,		// evValue is an ascii char
	SE_MOUSE,		// evValue and evValue2 are relative signed x / y moves
	SE_JOYSTICK_AXIS,	// evValue is an axis number and evValue2 is the current state (-127 to 127)
	SE_CONSOLE		// evPtr is a char*
};

struct sysEvent_s {
	int				evTime;
	sysEventType_e	evType;
	int				evValue, evValue2;
	int				evPtrLength;	// bytes of data pointed to by evPtr, for journaling
	void			*evPtr;			// this must be manually freed if not NULL
};

void		Com_QueueEvent( int time, sysEventType_e type, int value, int value2, int ptrLength, void *ptr );
int			Com_EventLoop();
sysEvent_s	Com_GetSystemEvent();

void		Info_Print( const char *s );

void		Com_BeginRedirect (char *buffer, int buffersize, void (*flush)(char *));
void		Com_EndRedirect();
void 		QDECL Com_Printf( const char *fmt, ... ) __attribute__ ((format (printf, 1, 2)));
void 		QDECL Com_RedWarning( const char *fmt, ... ) __attribute__ ((format (printf, 1, 2)));
void 		QDECL Com_DPrintf( const char *fmt, ... ) __attribute__ ((format (printf, 1, 2)));
void 		QDECL Com_Error( int code, const char *fmt, ... ) __attribute__ ((noreturn, format(printf, 2, 3)));
void 		Com_Quit_f() __attribute__ ((noreturn));
void		Com_GameRestart(int checksumFeed, bool disconnect);

int			Com_Milliseconds();	// will be journaled properly
unsigned	Com_BlockChecksum( const void *buffer, int length );
char		*Com_MD5File(const char *filename, int length, const char *prefix, int prefix_len);
int			Com_Filter(const char *filter, char *name, int casesensitive);
int			Com_FilterPath(const char *filter, char *name, int casesensitive);
int			Com_RealTime(qtime_s *qtime);
bool	Com_SafeMode();
void		Com_RunAndTimeServerPacket(netAdr_s *evFrom, msg_c *buf);

bool	Com_IsVoipTarget(uint8_t *voipTargets, int voipTargetsSize, int clientNum);

void		Com_StartupVariable( const char *match );
// checks for and removes command line "+set var arg" constructs
// if match is NULL, all set commands will be executed, otherwise
// only a set with the exact name.  Only used during startup.


extern	cvar_s	*com_developer;
extern	cvar_s	*com_dedicated;
extern	cvar_s	*com_speeds;
extern	cvar_s	*com_timescale;
extern	cvar_s	*com_sv_running;
extern	cvar_s	*com_cl_running;
extern	cvar_s	*com_viewlog;
extern	cvar_s	*com_version;
extern	cvar_s	*com_blood;
extern	cvar_s	*com_journal;
extern	cvar_s	*com_cameraMode;
extern	cvar_s	*com_ansiColor;
extern	cvar_s	*com_unfocused;
extern	cvar_s	*com_maxfpsUnfocused;
extern	cvar_s	*com_minimized;
extern	cvar_s	*com_maxfpsMinimized;
extern	cvar_s	*com_altivec;
extern	cvar_s	*com_standalone;
extern	cvar_s	*com_basegame;
extern	cvar_s	*com_homepath;

// both client and server must agree to pause
extern	cvar_s	*cl_paused;
extern	cvar_s	*sv_paused;

extern	cvar_s	*cl_packetdelay;
extern	cvar_s	*sv_packetdelay;

extern	cvar_s	*com_gamename;
extern	cvar_s	*com_protocol;

// com_speeds times
extern	int		time_game;
extern	int		time_frontend;
extern	int		time_backend;		// renderer backend time

extern	int		com_frameTime;

extern	bool	com_errorEntered;
extern	bool	com_fullyInitialized;
// V: -editor starts engine in editor mode
extern	bool	com_bEditorMode;

extern	fileHandle_t	com_journalFile;
extern	fileHandle_t	com_journalDataFile;


// commandLine should not include the executable name (argv[0])
void Com_Init( char *commandLine );
void Com_Frame();
void Com_Shutdown();

bool Com_InitEditorDLL();
bool COM_RunEditorFrame();

void Hunk_Clear();

void Com_RandomBytes( byte *string, int len );

/*
==============================================================

CLIENT / SERVER SYSTEMS

==============================================================
*/

//
// client interface
//
void CL_InitKeyCommands();
// the keyboard binding interface must be setup before execing
// config files, but the rest of client startup will happen later

void CL_Init();
void CL_Disconnect( bool showMainMenu );
void CL_Shutdown(const char *finalmsg, bool disconnect, bool quit);
void CL_Frame( int msec );
bool CL_GameCommand();
void CL_KeyEvent (int key, bool down, unsigned time);

void CL_CharEvent( int key );
// char events are for field typing, not game control

void CL_MouseEvent( int dx, int dy, int time );

void CL_JoystickEvent( int axis, int value, int time );

void CL_PacketEvent( netAdr_s from, msg_c *msg );

void CL_ConsolePrint( char *text );

void CL_MapLoading();
// do a screen update before starting to load a map
// when the server is going to load a new map, the entire hunk
// will be cleared, so the client must shutdown cgame, ui, and
// the renderer

void	CL_ForwardCommandToServer( const char *string );
// adds the current command line as a clc_clientCommand to the client message.
// things like godmode, noclip, etc, are commands directed to the server,
// so when they are typed in at the console, they will need to be forwarded.

void CL_FlushMemory();
// dump all memory on an error

void CL_ShutdownAll(bool shutdownRef);
// shutdown client

void CL_InitRef(void);
// initialize renderer interface

void CL_StartHunkUsers( bool rendererOnly );
// start all the client stuff using the hunk

void CL_Snd_Shutdown(void);
// Restart sound subsystem

void Key_KeynameCompletion( void(*callback)(const char *s) );
// for keyname autocompletion

void Key_WriteBindings( fileHandle_t f );
// for writing the config files

// AVI files have the start of pixel lines 4 byte-aligned
#define AVI_LINE_PADDING 4

//
// server interface
//
void SV_Init();
void SV_Shutdown(const char *finalmsg );
void SV_Frame( int msec );
void SV_PacketEvent( netAdr_s from, msg_c *msg );
int SV_FrameMsec(void);
bool SV_GameCommand();
int SV_SendQueuedPackets(void);

//
// UI interface
//
bool GUI_GameCommand();

/*
==============================================================

NON-PORTABLE SYSTEM SERVICES

==============================================================
*/

#define MAX_JOYSTICK_AXIS 16

void	Sys_Init (void);

void	Sys_UnloadDll( void *dllHandle );

char	*Sys_GetCurrentUser();

void	QDECL Sys_Error( const char *error, ...) __attribute__ ((noreturn, format (printf, 1, 2)));
void	Sys_Quit (void) __attribute__ ((noreturn));
char	*Sys_GetClipboardData();	// note that this isn't journaled...

void	Sys_Print( const char *msg );

// Sys_Milliseconds should only be used for profiling purposes,
// any game related timing information should come from event timestamps
int		Sys_Milliseconds (void);

bool Sys_RandomBytes( byte *string, int len );

cpuFeatures_t Sys_GetProcessorFeatures();

void	Sys_SetErrorText( const char *text );

void	Sys_SendPacket( int length, const void *data, netAdr_s to );

bool	Sys_StringToAdr( const char *s, netAdr_s *a, netAdrType_e family );
//Does NOT parse port numbers, only base addresses.

bool	Sys_IsLANAddress (netAdr_s adr);
void		Sys_ShowIP(void);

bool Sys_Mkdir( const char *path );
FILE	*Sys_Mkfifo( const char *ospath );
char	*Sys_Cwd();
void	Sys_SetDefaultInstallPath(const char *path);
char	*Sys_DefaultInstallPath(void);

#ifdef MACOS_X
char    *Sys_DefaultAppPath(void);
#endif

void  Sys_SetDefaultHomePath(const char *path);
char	*Sys_DefaultHomePath(void);
const char *Sys_Dirname( char *path );
const char *Sys_Basename( char *path );
char *Sys_ConsoleInput(void);

char **Sys_ListFiles( const char *directory, const char *extension, const char *filter, int *numfiles, bool wantsubs );
void	Sys_FreeFileList( char **list );
void	Sys_Sleep(int msec);

bool Sys_LowPhysicalMemory();

void Sys_SetEnv(const char *name, const char *value);

enum dialogResult_e
{
	DR_YES = 0,
	DR_NO = 1,
	DR_OK = 0,
	DR_CANCEL = 1
};

enum dialogType_e
{
	DT_INFO,
	DT_WARNING,
	DT_ERROR,
	DT_YES_NO,
	DT_OK_CANCEL
};

dialogResult_e Sys_Dialog( dialogType_e type, const char *message, const char *title );

bool Sys_WritePIDFile();

// flags for sv_allowDownload and cl_allowDownload
#define DLF_ENABLE 1
#define DLF_NO_REDIRECT 2
#define DLF_NO_UDP 4
#define DLF_NO_DISCONNECT 8

#endif // _QCOMMON_H_
