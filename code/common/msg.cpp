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
#include "common.h"
#include <api/coreAPI.h>
#include <protocol/userCmd.h>
#include <protocol/playerState.h>
#include <shared/animationFlags.h>
#include <shared/infoString.h>
#include <shared/huffman.h>


int msg_hData[256] = {
	250315,			// 0
	41193,			// 1
	6292,			// 2
	7106,			// 3
	3730,			// 4
	3750,			// 5
	6110,			// 6
	23283,			// 7
	33317,			// 8
	6950,			// 9
	7838,			// 10
	9714,			// 11
	9257,			// 12
	17259,			// 13
	3949,			// 14
	1778,			// 15
	8288,			// 16
	1604,			// 17
	1590,			// 18
	1663,			// 19
	1100,			// 20
	1213,			// 21
	1238,			// 22
	1134,			// 23
	1749,			// 24
	1059,			// 25
	1246,			// 26
	1149,			// 27
	1273,			// 28
	4486,			// 29
	2805,			// 30
	3472,			// 31
	21819,			// 32
	1159,			// 33
	1670,			// 34
	1066,			// 35
	1043,			// 36
	1012,			// 37
	1053,			// 38
	1070,			// 39
	1726,			// 40
	888,			// 41
	1180,			// 42
	850,			// 43
	960,			// 44
	780,			// 45
	1752,			// 46
	3296,			// 47
	10630,			// 48
	4514,			// 49
	5881,			// 50
	2685,			// 51
	4650,			// 52
	3837,			// 53
	2093,			// 54
	1867,			// 55
	2584,			// 56
	1949,			// 57
	1972,			// 58
	940,			// 59
	1134,			// 60
	1788,			// 61
	1670,			// 62
	1206,			// 63
	5719,			// 64
	6128,			// 65
	7222,			// 66
	6654,			// 67
	3710,			// 68
	3795,			// 69
	1492,			// 70
	1524,			// 71
	2215,			// 72
	1140,			// 73
	1355,			// 74
	971,			// 75
	2180,			// 76
	1248,			// 77
	1328,			// 78
	1195,			// 79
	1770,			// 80
	1078,			// 81
	1264,			// 82
	1266,			// 83
	1168,			// 84
	965,			// 85
	1155,			// 86
	1186,			// 87
	1347,			// 88
	1228,			// 89
	1529,			// 90
	1600,			// 91
	2617,			// 92
	2048,			// 93
	2546,			// 94
	3275,			// 95
	2410,			// 96
	3585,			// 97
	2504,			// 98
	2800,			// 99
	2675,			// 100
	6146,			// 101
	3663,			// 102
	2840,			// 103
	14253,			// 104
	3164,			// 105
	2221,			// 106
	1687,			// 107
	3208,			// 108
	2739,			// 109
	3512,			// 110
	4796,			// 111
	4091,			// 112
	3515,			// 113
	5288,			// 114
	4016,			// 115
	7937,			// 116
	6031,			// 117
	5360,			// 118
	3924,			// 119
	4892,			// 120
	3743,			// 121
	4566,			// 122
	4807,			// 123
	5852,			// 124
	6400,			// 125
	6225,			// 126
	8291,			// 127
	23243,			// 128
	7838,			// 129
	7073,			// 130
	8935,			// 131
	5437,			// 132
	4483,			// 133
	3641,			// 134
	5256,			// 135
	5312,			// 136
	5328,			// 137
	5370,			// 138
	3492,			// 139
	2458,			// 140
	1694,			// 141
	1821,			// 142
	2121,			// 143
	1916,			// 144
	1149,			// 145
	1516,			// 146
	1367,			// 147
	1236,			// 148
	1029,			// 149
	1258,			// 150
	1104,			// 151
	1245,			// 152
	1006,			// 153
	1149,			// 154
	1025,			// 155
	1241,			// 156
	952,			// 157
	1287,			// 158
	997,			// 159
	1713,			// 160
	1009,			// 161
	1187,			// 162
	879,			// 163
	1099,			// 164
	929,			// 165
	1078,			// 166
	951,			// 167
	1656,			// 168
	930,			// 169
	1153,			// 170
	1030,			// 171
	1262,			// 172
	1062,			// 173
	1214,			// 174
	1060,			// 175
	1621,			// 176
	930,			// 177
	1106,			// 178
	912,			// 179
	1034,			// 180
	892,			// 181
	1158,			// 182
	990,			// 183
	1175,			// 184
	850,			// 185
	1121,			// 186
	903,			// 187
	1087,			// 188
	920,			// 189
	1144,			// 190
	1056,			// 191
	3462,			// 192
	2240,			// 193
	4397,			// 194
	12136,			// 195
	7758,			// 196
	1345,			// 197
	1307,			// 198
	3278,			// 199
	1950,			// 200
	886,			// 201
	1023,			// 202
	1112,			// 203
	1077,			// 204
	1042,			// 205
	1061,			// 206
	1071,			// 207
	1484,			// 208
	1001,			// 209
	1096,			// 210
	915,			// 211
	1052,			// 212
	995,			// 213
	1070,			// 214
	876,			// 215
	1111,			// 216
	851,			// 217
	1059,			// 218
	805,			// 219
	1112,			// 220
	923,			// 221
	1103,			// 222
	817,			// 223
	1899,			// 224
	1872,			// 225
	976,			// 226
	841,			// 227
	1127,			// 228
	956,			// 229
	1159,			// 230
	950,			// 231
	7791,			// 232
	954,			// 233
	1289,			// 234
	933,			// 235
	1127,			// 236
	3207,			// 237
	1020,			// 238
	927,			// 239
	1355,			// 240
	768,			// 241
	1040,			// 242
	745,			// 243
	952,			// 244
	805,			// 245
	1073,			// 246
	740,			// 247
	1013,			// 248
	805,			// 249
	1008,			// 250
	796,			// 251
	996,			// 252
	1057,			// 253
	11457,			// 254
	13504,			// 255
};
static huffman_c msgHuff(msg_hData);

int pcount[256];

union floatInt_u {
	float f;
	int i;
	unsigned int ui;
};

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

int oldsize = 0;

void msg_c::init(byte *data, int length ) {
	memset (this, 0, sizeof(*this));
	this->data = data;
	this->maxsize = length;
}

void msg_c::initOOB(byte *data, int length ) {
	memset (this, 0, sizeof(*this));
	this->data = data;
	this->maxsize = length;
	this->oob = true;
}

void msg_c::clear() {
	this->cursize = 0;
	this->overflowed = false;
	this->bit = 0;					//<- in bits
}


void msg_c::bitstream() {
	this->oob = false;
}

void msg_c::beginReading() {
	this->readcount = 0;
	this->bit = 0;
	this->oob = false;
}

void msg_c::beginReadingOOB() {
	this->readcount = 0;
	this->bit = 0;
	this->oob = true;
}

void MSG_Copy(msg_c *buf, byte *data, int length, msg_c *src)
{
	if (length<src->cursize) {
		Com_Error( ERR_DROP, "MSG_Copy: can't copy into a smaller msg_c buffer (%i < %i)",length,src->cursize);
	}
	memcpy(buf, src, sizeof(msg_c));
	buf->data = data;
	memcpy(buf->data, src->data, src->cursize);
}

/*
=============================================================================

bit functions
  
=============================================================================
*/

int	overflows;

// negative bit values include signs
void msg_c::writeBits(int value, int bits ) {
	int	i;
//	FILE*	fp;

	oldsize += bits;

	// this isn't an exact overflow check, but close enough
	if ( this->maxsize - this->cursize < 4 ) {
		this->overflowed = true;
		return;
	}

	if ( bits == 0 || bits < -31 || bits > 32 ) {
		Com_Error( ERR_DROP, "MSG_WriteBits: bad bits %i", bits );
	}

	// check for overflows
	if ( bits != 32 ) {
		if ( bits > 0 ) {
			if ( value > ( ( 1 << bits ) - 1 ) || value < 0 ) {
				overflows++;
			}
		} else {
			int	r;

			r = 1 << (bits-1);

			if ( value >  r - 1 || value < -r ) {
				overflows++;
			}
		}
	}
	if ( bits < 0 ) {
		bits = -bits;
	}
	if (this->oob) {
		if(bits==8)
		{
			this->data[this->cursize] = value;
			this->cursize += 1;
			this->bit += 8;
		}
		else if(bits==16)
		{
			short temp = value;
			
			CopyLittleShort(&this->data[this->cursize], &temp);
			this->cursize += 2;
			this->bit += 16;
		}
		else if(bits==32)
		{
			CopyLittleLong(&this->data[this->cursize], &value);
			this->cursize += 4;
			this->bit += 32;
		}
		else 
			Com_Error(ERR_DROP, "can't write %d bits", bits);
	} else {
//		fp = fopen("c:\\netchan.bin", "a");
		value &= (0xffffffff>>(32-bits));
		if (bits&7) {
			int nbits;
			nbits = bits&7;
			for(i=0;i<nbits;i++) {
				Huff_putBit((value&1), this->data, &this->bit);
				value = (value>>1);
			}
			bits = bits - nbits;
		}
		if (bits) {
			for(i=0;i<bits;i+=8) {
//				fwrite(bp, 1, 1, fp);
				msgHuff.offsetTransmit((value&0xff), this->data, &this->bit);
				value = (value>>8);
			}
		}
		this->cursize = (this->bit>>3)+1;
//		fclose(fp);
	}
}

int msg_c::readBits( int bits ) {
	int			value;
	int			get;
	bool	sgn;
	int			i, nbits;
//	FILE*	fp;

	value = 0;

	if ( bits < 0 ) {
		bits = -bits;
		sgn = true;
	} else {
		sgn = false;
	}

	if (this->oob) {
		if(bits==8)
		{
			value = this->data[this->readcount];
			this->readcount += 1;
			this->bit += 8;
		}
		else if(bits==16)
		{
			short temp;
			
			CopyLittleShort(&temp, &this->data[this->readcount]);
			value = temp;
			this->readcount += 2;
			this->bit += 16;
		}
		else if(bits==32)
		{
			CopyLittleLong(&value, &this->data[this->readcount]);
			this->readcount += 4;
			this->bit += 32;
		}
		else
			Com_Error(ERR_DROP, "can't read %d bits", bits);
	} else {
		nbits = 0;
		if (bits&7) {
			nbits = bits&7;
			for(i=0;i<nbits;i++) {
				value |= (Huff_getBit(this->data, &this->bit)<<i);
			}
			bits = bits - nbits;
		}
		if (bits) {
//			fp = fopen("c:\\netchan.bin", "a");
			for(i=0;i<bits;i+=8) {
				msgHuff.offsetReceive(&get, this->data, &this->bit);
//				fwrite(&get, 1, 1, fp);
				value |= (get<<(i+nbits));
			}
//			fclose(fp);
		}
		this->readcount = (this->bit>>3)+1;
	}
	if ( sgn ) {
		if ( value & ( 1 << ( bits - 1 ) ) ) {
			value |= -1 ^ ( ( 1 << bits ) - 1 );
		}
	}

	return value;
}



//================================================================================

//
// writing functions
//

void msg_c::writeChar(int c ) {
#ifdef PARANOID
	if (c < -128 || c > 127)
		Com_Error (ERR_FATAL, "MSG_WriteChar: range error");
#endif

	this->writeBits(c, 8);
}

void msg_c::writeByte(int c ) {
#ifdef PARANOID
	if (c < 0 || c > 255)
		Com_Error (ERR_FATAL, "MSG_WriteByte: range error");
#endif

	this->writeBits(c, 8 );
}

void msg_c::writeData(  const void *data, int length ) {
	int i;
	for(i=0;i<length;i++) {
		this->writeByte(((byte *)data)[i]);
	}
}

void msg_c::writeShort(int c ) {
#ifdef PARANOID
	if (c < ((short)0x8000) || c > (short)0x7fff)
		Com_Error (ERR_FATAL, "MSG_WriteShort: range error");
#endif

	this->writeBits(c, 16 );
}

void msg_c::writeLong(int c ) {
	this->writeBits(c, 32 );
}

void msg_c::writeFloat(float f ) {
	floatInt_u dat;
	dat.f = f;
	this->writeBits(dat.i, 32 );
}

void msg_c::writeString(const char *s ) {
	if ( !s ) {
		writeData ( "", 1);
	} else {
		int		l,i;
		char	string[MAX_STRING_CHARS];

		l = strlen( s );
		if ( l >= MAX_STRING_CHARS ) {
			Com_Printf( "MSG_WriteString: MAX_STRING_CHARS" );
			writeData ( "", 1);
			return;
		}
		Q_strncpyz( string, s, sizeof( string ) );

		// get rid of 0x80+ and '%' chars, because old clients don't like them
		for ( i = 0 ; i < l ; i++ ) {
			if ( ((byte *)string)[i] > 127 || string[i] == '%' ) {
				string[i] = '.';
			}
		}

		writeData (string, l+1);
	}
}

void msg_c::writeBigString(  const char *s ) {
	if ( !s ) {
		writeData ("", 1);
	} else {
		int		l,i;
		char	string[MAX_INFO_STRING];

		l = strlen( s );
		if ( l >= MAX_INFO_STRING ) {
			Com_Printf( "MSG_WriteString: MAX_INFO_STRING" );
			writeData ("", 1);
			return;
		}
		Q_strncpyz( string, s, sizeof( string ) );

		// get rid of 0x80+ and '%' chars, because old clients don't like them
		for ( i = 0 ; i < l ; i++ ) {
			if ( ((byte *)string)[i] > 127 || string[i] == '%' ) {
				string[i] = '.';
			}
		}

		writeData (string, l+1);
	}
}

void msg_c::writeAngle(float f ) {
	writeByte ((int)(f*256/360) & 255);
}

void msg_c::writeAngle16(float f ) {
	writeShort (ANGLE2SHORT(f));
}


//============================================================

//
// reading functions
//

// returns -1 if no more characters are available
int msg_c::readChar () {
	int	c;
	
	c = (signed char)readBits( 8 );
	if ( this->readcount > this->cursize ) {
		c = -1;
	}	
	
	return c;
}

int msg_c::readByte() {
	int	c;
	
	c = (unsigned char)readBits( 8 );
	if (this->readcount > this->cursize ) {
		c = -1;
	}	
	return c;
}


int msg_c::readShort() {
	int	c;
	
	c = (short)readBits( 16 );
	if (this->readcount > this->cursize ) {
		c = -1;
	}	

	return c;
}

int msg_c::readUShort() {
	int	c;
	
	c = (unsigned short)readBits( 16 );
	if (this->readcount > this->cursize ) {
		c = -1;
	}	

	return c;
}

int msg_c::readLong() {
	int	c;
	
	c = this->readBits(32);
	if (this->readcount > this->cursize ) {
		c = -1;
	}	
	
	return c;
}

float msg_c::readFloat( ) {
	floatInt_u dat;
	
	dat.i = readBits(32 );
	if ( this->readcount > this->cursize ) {
		dat.f = -1;
	}	
	
	return dat.f;	
}

char *msg_c::readString(  bool bFixFormatChars ) {
	static char	string[MAX_STRING_CHARS];
	int		l,c;
	
	l = 0;
	do {
		c = this->readByte();		// use ReadByte so -1 is out of bounds
		if ( c == -1 || c == 0 ) {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( bFixFormatChars && c == '%' ) {
			c = '.';
		}
		// don't allow higher ascii values
		if ( c > 127 ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

char *msg_c::readBigString() {
	static char	string[MAX_INFO_STRING];
	int		l,c;
	
	l = 0;
	do {
		c = this->readByte();		// use ReadByte so -1 is out of bounds
		if ( c == -1 || c == 0 ) {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' ) {
			c = '.';
		}
		// don't allow higher ascii values
		if ( c > 127 ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

char *msg_c::readStringLine() {
	static char	string[MAX_STRING_CHARS];
	int		l,c;

	l = 0;
	do {
		c = this->readByte();		// use ReadByte so -1 is out of bounds
		if (c == -1 || c == 0 || c == '\n') {
			break;
		}
		// translate all fmt spec to avoid crash bugs
		if ( c == '%' ) {
			c = '.';
		}
		// don't allow higher ascii values
		if ( c > 127 ) {
			c = '.';
		}

		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);
	
	string[l] = 0;
	
	return string;
}

float msg_c::readAngle16() {
	return SHORT2ANGLE(readShort());
}

void msg_c::readData(void *data, int len ) {
	int		i;

	for (i=0 ; i<len ; i++) {
		((byte *)data)[i] = readByte ();
	}
}

// a string hasher which gives the same hash value even if the
// string is later modified via the legacy MSG read/write code
int MSG_HashKey(const char *string, int maxlen) {
	int hash, i;

	hash = 0;
	for (i = 0; i < maxlen && string[i] != '\0'; i++) {
		if (string[i] & 0x80 || string[i] == '%')
			hash += '.' * (119 + i);
		else
			hash += string[i] * (119 + i);
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20));
	return hash;
}

/*
=============================================================================

delta functions
  
=============================================================================
*/

extern cvar_s *cl_shownet;

#define	LOG(x) if( cl_shownet->integer == 4 ) { Com_Printf("%s ", x ); };

void MSG_WriteDelta( msg_c *msg, int oldV, int newV, int bits ) {
	if ( oldV == newV ) {
		msg->writeBits(0, 1 );
		return;
	}
	msg->writeBits(1, 1 );
	msg->writeBits(newV, bits );
}

int	MSG_ReadDelta( msg_c *msg, int oldV, int bits ) {
	if ( msg->readBits(1 ) ) {
		return msg->readBits(bits );
	}
	return oldV;
}

void MSG_WriteDeltaFloat( msg_c *msg, float oldV, float newV ) {
	floatInt_u fi;
	if ( oldV == newV ) {
		msg->writeBits(0, 1 );
		return;
	}
	fi.f = newV;
	msg->writeBits(1, 1 );
	msg->writeBits(fi.i, 32 );
}

float MSG_ReadDeltaFloat( msg_c *msg, float oldV ) {
	if ( msg->readBits(1 ) ) {
		floatInt_u fi;

		fi.i = msg->readBits(32 );
		return fi.f;
	}
	return oldV;
}

/*
=============================================================================

delta functions with keys
  
=============================================================================
*/

int kbitmask[32] = {
	0x00000001, 0x00000003, 0x00000007, 0x0000000F,
	0x0000001F,	0x0000003F,	0x0000007F,	0x000000FF,
	0x000001FF,	0x000003FF,	0x000007FF,	0x00000FFF,
	0x00001FFF,	0x00003FFF,	0x00007FFF,	0x0000FFFF,
	0x0001FFFF,	0x0003FFFF,	0x0007FFFF,	0x000FFFFF,
	0x001FFFFf,	0x003FFFFF,	0x007FFFFF,	0x00FFFFFF,
	0x01FFFFFF,	0x03FFFFFF,	0x07FFFFFF,	0x0FFFFFFF,
	0x1FFFFFFF,	0x3FFFFFFF,	0x7FFFFFFF,	0xFFFFFFFF,
};

void MSG_WriteDeltaKey( msg_c *msg, int key, int oldV, int newV, int bits ) {
	if ( oldV == newV ) {
		msg->writeBits(0, 1 );
		return;
	}
	msg->writeBits(1, 1 );
	msg->writeBits(newV ^ key, bits );
}

int	MSG_ReadDeltaKey( msg_c *msg, int key, int oldV, int bits ) {
	if ( msg->readBits(1 ) ) {
		return msg->readBits(bits ) ^ (key & kbitmask[bits]);
	}
	return oldV;
}

void MSG_WriteDeltaKeyFloat( msg_c *msg, int key, float oldV, float newV ) {
	floatInt_u fi;
	if ( oldV == newV ) {
		msg->writeBits(0, 1 );
		return;
	}
	fi.f = newV;
	msg->writeBits(1, 1 );
	msg->writeBits(fi.i ^ key, 32 );
}

float MSG_ReadDeltaKeyFloat( msg_c *msg, int key, float oldV ) {
	if ( msg->readBits(1 ) ) {
		floatInt_u fi;

		fi.i = msg->readBits(32 ) ^ key;
		return fi.f;
	}
	return oldV;
}


/*
============================================================================

userCmd_s communication

============================================================================
*/

// ms is allways sent, the others are optional
#define	CM_ANGLE1 	(1<<0)
#define	CM_ANGLE2 	(1<<1)
#define	CM_ANGLE3 	(1<<2)
#define	CM_FORWARD	(1<<3)
#define	CM_SIDE		(1<<4)
#define	CM_UP		(1<<5)
#define	CM_BUTTONS	(1<<6)
#define CM_WEAPON	(1<<7)

void msg_c::writeDeltaUsercmd(userCmd_s *from, userCmd_s *to ) {
	if ( to->serverTime - from->serverTime < 256 ) {
		this->writeBits(1, 1 );
		this->writeBits(to->serverTime - from->serverTime, 8 );
	} else {
		this->writeBits(0, 1 );
		this->writeBits(to->serverTime, 32 );
	}
	MSG_WriteDelta( this, from->angles[0], to->angles[0], 16 );
	MSG_WriteDelta( this, from->angles[1], to->angles[1], 16 );
	MSG_WriteDelta( this, from->angles[2], to->angles[2], 16 );
	MSG_WriteDelta( this, from->forwardmove, to->forwardmove, 8 );
	MSG_WriteDelta( this, from->rightmove, to->rightmove, 8 );
	MSG_WriteDelta( this, from->upmove, to->upmove, 8 );
	MSG_WriteDelta( this, from->buttons, to->buttons, 16 );
	MSG_WriteDelta( this, from->weapon, to->weapon, 8 );
}

void msg_c::readDeltaUsercmd(userCmd_s *from, userCmd_s *to ) {
	if ( this->readBits(1 ) ) {
		to->serverTime = from->serverTime + this->readBits(8 );
	} else {
		to->serverTime = this->readBits(32 );
	}
	to->angles[0] = MSG_ReadDelta( this, from->angles[0], 16);
	to->angles[1] = MSG_ReadDelta( this, from->angles[1], 16);
	to->angles[2] = MSG_ReadDelta( this, from->angles[2], 16);
	to->forwardmove = MSG_ReadDelta( this, from->forwardmove, 8);
	if( to->forwardmove == -128 )
		to->forwardmove = -127;
	to->rightmove = MSG_ReadDelta( this, from->rightmove, 8);
	if( to->rightmove == -128 )
		to->rightmove = -127;
	to->upmove = MSG_ReadDelta( this, from->upmove, 8);
	if( to->upmove == -128 )
		to->upmove = -127;
	to->buttons = MSG_ReadDelta( this, from->buttons, 16);
	to->weapon = MSG_ReadDelta( this, from->weapon, 8);
}

void msg_c::writeDeltaUsercmdKey(int key, userCmd_s *from, userCmd_s *to ) {
	if ( to->serverTime - from->serverTime < 256 ) {
		this->writeBits(1, 1 );
		this->writeBits(to->serverTime - from->serverTime, 8 );
	} else {
		this->writeBits(0, 1 );
		this->writeBits(to->serverTime, 32 );
	}
	if (from->angles[0] == to->angles[0] &&
		from->angles[1] == to->angles[1] &&
		from->angles[2] == to->angles[2] &&
		from->forwardmove == to->forwardmove &&
		from->rightmove == to->rightmove &&
		from->upmove == to->upmove &&
		from->buttons == to->buttons &&
		from->weapon == to->weapon) {
			this->writeBits(0, 1 );				// no change
			oldsize += 7;
			return;
	}
	key ^= to->serverTime;
	this->writeBits(1, 1 );
	MSG_WriteDeltaKey( this, key, from->angles[0], to->angles[0], 16 );
	MSG_WriteDeltaKey( this, key, from->angles[1], to->angles[1], 16 );
	MSG_WriteDeltaKey( this, key, from->angles[2], to->angles[2], 16 );
	MSG_WriteDeltaKey( this, key, from->forwardmove, to->forwardmove, 8 );
	MSG_WriteDeltaKey( this, key, from->rightmove, to->rightmove, 8 );
	MSG_WriteDeltaKey( this, key, from->upmove, to->upmove, 8 );
	MSG_WriteDeltaKey( this, key, from->buttons, to->buttons, 16 );
	MSG_WriteDeltaKey( this, key, from->weapon, to->weapon, 8 );
}

void msg_c::readDeltaUsercmdKey(int key, userCmd_s *from, userCmd_s *to ) {
	if ( this->readBits(1 ) ) {
		to->serverTime = from->serverTime + this->readBits(8 );
	} else {
		to->serverTime = this->readBits(32 );
	}
	if ( this->readBits(1 ) ) {
		key ^= to->serverTime;
		to->angles[0] = MSG_ReadDeltaKey( this, key, from->angles[0], 16);
		to->angles[1] = MSG_ReadDeltaKey( this, key, from->angles[1], 16);
		to->angles[2] = MSG_ReadDeltaKey( this, key, from->angles[2], 16);
		to->forwardmove = MSG_ReadDeltaKey( this, key, from->forwardmove, 8);
		if( to->forwardmove == -128 )
			to->forwardmove = -127;
		to->rightmove = MSG_ReadDeltaKey( this, key, from->rightmove, 8);
		if( to->rightmove == -128 )
			to->rightmove = -127;
		to->upmove = MSG_ReadDeltaKey( this, key, from->upmove, 8);
		if( to->upmove == -128 )
			to->upmove = -127;
		to->buttons = MSG_ReadDeltaKey( this, key, from->buttons, 16);
		to->weapon = MSG_ReadDeltaKey( this, key, from->weapon, 8);
	} else {
		to->angles[0] = from->angles[0];
		to->angles[1] = from->angles[1];
		to->angles[2] = from->angles[2];
		to->forwardmove = from->forwardmove;
		to->rightmove = from->rightmove;
		to->upmove = from->upmove;
		to->buttons = from->buttons;
		to->weapon = from->weapon;
	}
}

/*
=============================================================================

entityState_s communication
  
=============================================================================
*/

/*
=================
MSG_ReportChangeVectors_f

Prints out a table from the current statistics for copying to code
=================
*/
void MSG_ReportChangeVectors_f() {
	int i;
	for(i=0;i<256;i++) {
		if (pcount[i]) {
			Com_Printf("%d used %d\n", i, pcount[i]);
		}
	}
}

typedef struct {
	char	*name;
	int		offset;
	int		bits;		// 0 = float
} netField_t;

// using the stringizing operator to save typing...
#define	NETF(x) #x,(size_t)&((entityState_s*)0)->x

netField_t	entityStateFields[] = 
{
{ NETF(eType), 8 },
{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },
{ NETF(angles[0]), 0 },
{ NETF(angles[1]), 0 },
{ NETF(angles[2]), 0 },
{ NETF(groundEntityNum), GENTITYNUM_BITS },
//{ NETF(clientNum), 8 },
//{ NETF(solid), 24 },
{ NETF(colModelIndex), MODELNUM_BITS },
{ NETF(rModelIndex), MODELNUM_BITS },
{ NETF(rSkinIndex), SKINNUM_BITS },
{ NETF(parentNum), GENTITYNUM_BITS },
{ NETF(parentTagNum), TAGNUM_BITS },
{ NETF(animIndex), ANIMNUM_BITS },
{ NETF(torsoAnim), ANIMNUM_BITS },
{ NETF(lightRadius), 0 },
{ NETF(lightFlags), LIGHTFLAGS_BITS },
{ NETF(lightTarget), GENTITYNUM_BITS },
{ NETF(spotLightRadius), 0 },
{ NETF(activeRagdollDefNameIndex), RAGDOLLDEFNUM_BITS },
{ NETF(parentOffset[0]), 0 },
{ NETF(parentOffset[1]), 0 },
{ NETF(parentOffset[2]), 0 },
{ NETF(lightColor[0]), 0 },
{ NETF(lightColor[1]), 0 },
{ NETF(lightColor[2]), 0 },
{ NETF(scale[0]), 0 },
{ NETF(scale[1]), 0 },
{ NETF(scale[2]), 0 },
{ NETF(localAttachmentAngles[0]), 0 },
{ NETF(localAttachmentAngles[1]), 0 },
{ NETF(localAttachmentAngles[2]), 0 },
{ NETF(trailEmitterMaterial), MATERIALNUM_BITS, },
{ NETF(trailEmitterSpriteRadius), 0 },
{ NETF(trailEmitterInterval), 0 },
{ NETF(eFlags), EF_NUM_FLAG_BITS },
{ NETF(boneOrs[0].xyz.x), 0 },
{ NETF(boneOrs[0].xyz.y), 0 },
{ NETF(boneOrs[0].xyz.z), 0 },
{ NETF(boneOrs[0].quatXYZ.x), 0 },
{ NETF(boneOrs[0].quatXYZ.y), 0 },
{ NETF(boneOrs[0].quatXYZ.z), 0 },
{ NETF(boneOrs[1].xyz.x), 0 },
{ NETF(boneOrs[1].xyz.y), 0 },
{ NETF(boneOrs[1].xyz.z), 0 },
{ NETF(boneOrs[1].quatXYZ.x), 0 },
{ NETF(boneOrs[1].quatXYZ.y), 0 },
{ NETF(boneOrs[1].quatXYZ.z), 0 },
{ NETF(boneOrs[2].xyz.x), 0 },
{ NETF(boneOrs[2].xyz.y), 0 },
{ NETF(boneOrs[2].xyz.z), 0 },
{ NETF(boneOrs[2].quatXYZ.x), 0 },
{ NETF(boneOrs[2].quatXYZ.y), 0 },
{ NETF(boneOrs[2].quatXYZ.z), 0 },
{ NETF(boneOrs[3].xyz.x), 0 },
{ NETF(boneOrs[3].xyz.y), 0 },
{ NETF(boneOrs[3].xyz.z), 0 },
{ NETF(boneOrs[3].quatXYZ.x), 0 },
{ NETF(boneOrs[3].quatXYZ.y), 0 },
{ NETF(boneOrs[3].quatXYZ.z), 0 },
{ NETF(boneOrs[4].xyz.x), 0 },
{ NETF(boneOrs[4].xyz.y), 0 },
{ NETF(boneOrs[4].xyz.z), 0 },
{ NETF(boneOrs[4].quatXYZ.x), 0 },
{ NETF(boneOrs[4].quatXYZ.y), 0 },
{ NETF(boneOrs[4].quatXYZ.z), 0 },
{ NETF(boneOrs[5].xyz.x), 0 },
{ NETF(boneOrs[5].xyz.y), 0 },
{ NETF(boneOrs[5].xyz.z), 0 },
{ NETF(boneOrs[5].quatXYZ.x), 0 },
{ NETF(boneOrs[5].quatXYZ.y), 0 },
{ NETF(boneOrs[5].quatXYZ.z), 0 },
{ NETF(boneOrs[6].xyz.x), 0 },
{ NETF(boneOrs[6].xyz.y), 0 },
{ NETF(boneOrs[6].xyz.z), 0 },
{ NETF(boneOrs[6].quatXYZ.x), 0 },
{ NETF(boneOrs[6].quatXYZ.y), 0 },
{ NETF(boneOrs[6].quatXYZ.z), 0 },
{ NETF(boneOrs[7].xyz.x), 0 },
{ NETF(boneOrs[7].xyz.y), 0 },
{ NETF(boneOrs[7].xyz.z), 0 },
{ NETF(boneOrs[7].quatXYZ.x), 0 },
{ NETF(boneOrs[7].quatXYZ.y), 0 },
{ NETF(boneOrs[7].quatXYZ.z), 0 },
{ NETF(boneOrs[8].xyz.x), 0 },
{ NETF(boneOrs[8].xyz.y), 0 },
{ NETF(boneOrs[8].xyz.z), 0 },
{ NETF(boneOrs[8].quatXYZ.x), 0 },
{ NETF(boneOrs[8].quatXYZ.y), 0 },
{ NETF(boneOrs[8].quatXYZ.z), 0 },
{ NETF(boneOrs[9].xyz.x), 0 },
{ NETF(boneOrs[9].xyz.y), 0 },
{ NETF(boneOrs[9].xyz.z), 0 },
{ NETF(boneOrs[9].quatXYZ.x), 0 },
{ NETF(boneOrs[9].quatXYZ.y), 0 },
{ NETF(boneOrs[9].quatXYZ.z), 0 },
{ NETF(boneOrs[10].xyz.x), 0 },
{ NETF(boneOrs[10].xyz.y), 0 },
{ NETF(boneOrs[10].xyz.z), 0 },
{ NETF(boneOrs[10].quatXYZ.x), 0 },
{ NETF(boneOrs[10].quatXYZ.y), 0 },
{ NETF(boneOrs[10].quatXYZ.z), 0 },
{ NETF(boneOrs[11].xyz.x), 0 },
{ NETF(boneOrs[11].xyz.y), 0 },
{ NETF(boneOrs[11].xyz.z), 0 },
{ NETF(boneOrs[11].quatXYZ.x), 0 },
{ NETF(boneOrs[11].quatXYZ.y), 0 },
{ NETF(boneOrs[11].quatXYZ.z), 0 },
{ NETF(boneOrs[12].xyz.x), 0 },
{ NETF(boneOrs[12].xyz.y), 0 },
{ NETF(boneOrs[12].xyz.z), 0 },
{ NETF(boneOrs[12].quatXYZ.x), 0 },
{ NETF(boneOrs[12].quatXYZ.y), 0 },
{ NETF(boneOrs[12].quatXYZ.z), 0 },
{ NETF(boneOrs[13].xyz.x), 0 },
{ NETF(boneOrs[13].xyz.y), 0 },
{ NETF(boneOrs[13].xyz.z), 0 },
{ NETF(boneOrs[13].quatXYZ.x), 0 },
{ NETF(boneOrs[13].quatXYZ.y), 0 },
{ NETF(boneOrs[13].quatXYZ.z), 0 },
{ NETF(boneOrs[14].xyz.x), 0 },
{ NETF(boneOrs[14].xyz.y), 0 },
{ NETF(boneOrs[14].xyz.z), 0 },
{ NETF(boneOrs[14].quatXYZ.x), 0 },
{ NETF(boneOrs[14].quatXYZ.y), 0 },
{ NETF(boneOrs[14].quatXYZ.z), 0 },
{ NETF(boneOrs[15].xyz.x), 0 },
{ NETF(boneOrs[15].xyz.y), 0 },
{ NETF(boneOrs[15].xyz.z), 0 },
{ NETF(boneOrs[15].quatXYZ.x), 0 },
{ NETF(boneOrs[15].quatXYZ.y), 0 },
{ NETF(boneOrs[15].quatXYZ.z), 0 },
{ NETF(boneOrs[16].xyz.x), 0 },
{ NETF(boneOrs[16].xyz.y), 0 },
{ NETF(boneOrs[16].xyz.z), 0 },
{ NETF(boneOrs[16].quatXYZ.x), 0 },
{ NETF(boneOrs[16].quatXYZ.y), 0 },
{ NETF(boneOrs[16].quatXYZ.z), 0 },
{ NETF(boneOrs[17].xyz.x), 0 },
{ NETF(boneOrs[17].xyz.y), 0 },
{ NETF(boneOrs[17].xyz.z), 0 },
{ NETF(boneOrs[17].quatXYZ.x), 0 },
{ NETF(boneOrs[17].quatXYZ.y), 0 },
{ NETF(boneOrs[17].quatXYZ.z), 0 },
{ NETF(boneOrs[18].xyz.x), 0 },
{ NETF(boneOrs[18].xyz.y), 0 },
{ NETF(boneOrs[18].xyz.z), 0 },
{ NETF(boneOrs[18].quatXYZ.x), 0 },
{ NETF(boneOrs[18].quatXYZ.y), 0 },
{ NETF(boneOrs[18].quatXYZ.z), 0 },
{ NETF(boneOrs[19].xyz.x), 0 },
{ NETF(boneOrs[19].xyz.y), 0 },
{ NETF(boneOrs[19].xyz.z), 0 },
{ NETF(boneOrs[19].quatXYZ.x), 0 },
{ NETF(boneOrs[19].quatXYZ.y), 0 },
{ NETF(boneOrs[19].quatXYZ.z), 0 },
{ NETF(boneOrs[20].xyz.x), 0 },
{ NETF(boneOrs[20].xyz.y), 0 },
{ NETF(boneOrs[20].xyz.z), 0 },
{ NETF(boneOrs[20].quatXYZ.x), 0 },
{ NETF(boneOrs[20].quatXYZ.y), 0 },
{ NETF(boneOrs[20].quatXYZ.z), 0 },
{ NETF(boneOrs[21].xyz.x), 0 },
{ NETF(boneOrs[21].xyz.y), 0 },
{ NETF(boneOrs[21].xyz.z), 0 },
{ NETF(boneOrs[21].quatXYZ.x), 0 },
{ NETF(boneOrs[21].quatXYZ.y), 0 },
{ NETF(boneOrs[21].quatXYZ.z), 0 },
{ NETF(boneOrs[22].xyz.x), 0 },
{ NETF(boneOrs[22].xyz.y), 0 },
{ NETF(boneOrs[22].xyz.z), 0 },
{ NETF(boneOrs[22].quatXYZ.x), 0 },
{ NETF(boneOrs[22].quatXYZ.y), 0 },
{ NETF(boneOrs[22].quatXYZ.z), 0 },
{ NETF(boneOrs[23].xyz.x), 0 },
{ NETF(boneOrs[23].xyz.y), 0 },
{ NETF(boneOrs[23].xyz.z), 0 },
{ NETF(boneOrs[23].quatXYZ.x), 0 },
{ NETF(boneOrs[23].quatXYZ.y), 0 },
{ NETF(boneOrs[23].quatXYZ.z), 0 },
{ NETF(boneOrs[24].xyz.x), 0 },
{ NETF(boneOrs[24].xyz.y), 0 },
{ NETF(boneOrs[24].xyz.z), 0 },
{ NETF(boneOrs[24].quatXYZ.x), 0 },
{ NETF(boneOrs[24].quatXYZ.y), 0 },
{ NETF(boneOrs[24].quatXYZ.z), 0 },
{ NETF(boneOrs[25].xyz.x), 0 },
{ NETF(boneOrs[25].xyz.y), 0 },
{ NETF(boneOrs[25].xyz.z), 0 },
{ NETF(boneOrs[25].quatXYZ.x), 0 },
{ NETF(boneOrs[25].quatXYZ.y), 0 },
{ NETF(boneOrs[25].quatXYZ.z), 0 },
{ NETF(boneOrs[26].xyz.x), 0 },
{ NETF(boneOrs[26].xyz.y), 0 },
{ NETF(boneOrs[26].xyz.z), 0 },
{ NETF(boneOrs[26].quatXYZ.x), 0 },
{ NETF(boneOrs[26].quatXYZ.y), 0 },
{ NETF(boneOrs[26].quatXYZ.z), 0 },
{ NETF(boneOrs[27].xyz.x), 0 },
{ NETF(boneOrs[27].xyz.y), 0 },
{ NETF(boneOrs[27].xyz.z), 0 },
{ NETF(boneOrs[27].quatXYZ.x), 0 },
{ NETF(boneOrs[27].quatXYZ.y), 0 },
{ NETF(boneOrs[27].quatXYZ.z), 0 },
{ NETF(boneOrs[28].xyz.x), 0 },
{ NETF(boneOrs[28].xyz.y), 0 },
{ NETF(boneOrs[28].xyz.z), 0 },
{ NETF(boneOrs[28].quatXYZ.x), 0 },
{ NETF(boneOrs[28].quatXYZ.y), 0 },
{ NETF(boneOrs[28].quatXYZ.z), 0 },
{ NETF(boneOrs[29].xyz.x), 0 },
{ NETF(boneOrs[29].xyz.y), 0 },
{ NETF(boneOrs[29].xyz.z), 0 },
{ NETF(boneOrs[29].quatXYZ.x), 0 },
{ NETF(boneOrs[29].quatXYZ.y), 0 },
{ NETF(boneOrs[29].quatXYZ.z), 0 },
{ NETF(boneOrs[30].xyz.x), 0 },
{ NETF(boneOrs[30].xyz.y), 0 },
{ NETF(boneOrs[30].xyz.z), 0 },
{ NETF(boneOrs[30].quatXYZ.x), 0 },
{ NETF(boneOrs[30].quatXYZ.y), 0 },
{ NETF(boneOrs[30].quatXYZ.z), 0 },
{ NETF(boneOrs[31].xyz.x), 0 },
{ NETF(boneOrs[31].xyz.y), 0 },
{ NETF(boneOrs[31].xyz.z), 0 },
{ NETF(boneOrs[31].quatXYZ.x), 0 },
{ NETF(boneOrs[31].quatXYZ.y), 0 },
{ NETF(boneOrs[31].quatXYZ.z), 0 },
{ NETF(boneOrs[32].xyz.x), 0 },
{ NETF(boneOrs[32].xyz.y), 0 },
{ NETF(boneOrs[32].xyz.z), 0 },
{ NETF(boneOrs[32].quatXYZ.x), 0 },
{ NETF(boneOrs[32].quatXYZ.y), 0 },
{ NETF(boneOrs[32].quatXYZ.z), 0 },
{ NETF(boneOrs[33].xyz.x), 0 },
{ NETF(boneOrs[33].xyz.y), 0 },
{ NETF(boneOrs[33].xyz.z), 0 },
{ NETF(boneOrs[33].quatXYZ.x), 0 },
{ NETF(boneOrs[33].quatXYZ.y), 0 },
{ NETF(boneOrs[33].quatXYZ.z), 0 },
{ NETF(boneOrs[34].xyz.x), 0 },
{ NETF(boneOrs[34].xyz.y), 0 },
{ NETF(boneOrs[34].xyz.z), 0 },
{ NETF(boneOrs[34].quatXYZ.x), 0 },
{ NETF(boneOrs[34].quatXYZ.y), 0 },
{ NETF(boneOrs[34].quatXYZ.z), 0 },
{ NETF(boneOrs[35].xyz.x), 0 },
{ NETF(boneOrs[35].xyz.y), 0 },
{ NETF(boneOrs[35].xyz.z), 0 },
{ NETF(boneOrs[35].quatXYZ.x), 0 },
{ NETF(boneOrs[35].quatXYZ.y), 0 },
{ NETF(boneOrs[35].quatXYZ.z), 0 },
{ NETF(boneOrs[36].xyz.x), 0 },
{ NETF(boneOrs[36].xyz.y), 0 },
{ NETF(boneOrs[36].xyz.z), 0 },
{ NETF(boneOrs[36].quatXYZ.x), 0 },
{ NETF(boneOrs[36].quatXYZ.y), 0 },
{ NETF(boneOrs[36].quatXYZ.z), 0 },
{ NETF(boneOrs[37].xyz.x), 0 },
{ NETF(boneOrs[37].xyz.y), 0 },
{ NETF(boneOrs[37].xyz.z), 0 },
{ NETF(boneOrs[37].quatXYZ.x), 0 },
{ NETF(boneOrs[37].quatXYZ.y), 0 },
{ NETF(boneOrs[37].quatXYZ.z), 0 },
{ NETF(boneOrs[38].xyz.x), 0 },
{ NETF(boneOrs[38].xyz.y), 0 },
{ NETF(boneOrs[38].xyz.z), 0 },
{ NETF(boneOrs[38].quatXYZ.x), 0 },
{ NETF(boneOrs[38].quatXYZ.y), 0 },
{ NETF(boneOrs[38].quatXYZ.z), 0 },
{ NETF(boneOrs[39].xyz.x), 0 },
{ NETF(boneOrs[39].xyz.y), 0 },
{ NETF(boneOrs[39].xyz.z), 0 },
{ NETF(boneOrs[39].quatXYZ.x), 0 },
{ NETF(boneOrs[39].quatXYZ.y), 0 },
{ NETF(boneOrs[39].quatXYZ.z), 0 },
{ NETF(boneOrs[40].xyz.x), 0 },
{ NETF(boneOrs[40].xyz.y), 0 },
{ NETF(boneOrs[40].xyz.z), 0 },
{ NETF(boneOrs[40].quatXYZ.x), 0 },
{ NETF(boneOrs[40].quatXYZ.y), 0 },
{ NETF(boneOrs[40].quatXYZ.z), 0 },
{ NETF(boneOrs[41].xyz.x), 0 },
{ NETF(boneOrs[41].xyz.y), 0 },
{ NETF(boneOrs[41].xyz.z), 0 },
{ NETF(boneOrs[41].quatXYZ.x), 0 },
{ NETF(boneOrs[41].quatXYZ.y), 0 },
{ NETF(boneOrs[41].quatXYZ.z), 0 },
{ NETF(boneOrs[42].xyz.x), 0 },
{ NETF(boneOrs[42].xyz.y), 0 },
{ NETF(boneOrs[42].xyz.z), 0 },
{ NETF(boneOrs[42].quatXYZ.x), 0 },
{ NETF(boneOrs[42].quatXYZ.y), 0 },
{ NETF(boneOrs[42].quatXYZ.z), 0 },
{ NETF(boneOrs[43].xyz.x), 0 },
{ NETF(boneOrs[43].xyz.y), 0 },
{ NETF(boneOrs[43].xyz.z), 0 },
{ NETF(boneOrs[43].quatXYZ.x), 0 },
{ NETF(boneOrs[43].quatXYZ.y), 0 },
{ NETF(boneOrs[43].quatXYZ.z), 0 },
{ NETF(boneOrs[44].xyz.x), 0 },
{ NETF(boneOrs[44].xyz.y), 0 },
{ NETF(boneOrs[44].xyz.z), 0 },
{ NETF(boneOrs[44].quatXYZ.x), 0 },
{ NETF(boneOrs[44].quatXYZ.y), 0 },
{ NETF(boneOrs[44].quatXYZ.z), 0 },
{ NETF(boneOrs[45].xyz.x), 0 },
{ NETF(boneOrs[45].xyz.y), 0 },
{ NETF(boneOrs[45].xyz.z), 0 },
{ NETF(boneOrs[45].quatXYZ.x), 0 },
{ NETF(boneOrs[45].quatXYZ.y), 0 },
{ NETF(boneOrs[45].quatXYZ.z), 0 },
{ NETF(boneOrs[46].xyz.x), 0 },
{ NETF(boneOrs[46].xyz.y), 0 },
{ NETF(boneOrs[46].xyz.z), 0 },
{ NETF(boneOrs[46].quatXYZ.x), 0 },
{ NETF(boneOrs[46].quatXYZ.y), 0 },
{ NETF(boneOrs[46].quatXYZ.z), 0 },
{ NETF(boneOrs[47].xyz.x), 0 },
{ NETF(boneOrs[47].xyz.y), 0 },
{ NETF(boneOrs[47].xyz.z), 0 },
{ NETF(boneOrs[47].quatXYZ.x), 0 },
{ NETF(boneOrs[47].quatXYZ.y), 0 },
{ NETF(boneOrs[47].quatXYZ.z), 0 },
{ NETF(boneOrs[48].xyz.x), 0 },
{ NETF(boneOrs[48].xyz.y), 0 },
{ NETF(boneOrs[48].xyz.z), 0 },
{ NETF(boneOrs[48].quatXYZ.x), 0 },
{ NETF(boneOrs[48].quatXYZ.y), 0 },
{ NETF(boneOrs[48].quatXYZ.z), 0 },
{ NETF(boneOrs[49].xyz.x), 0 },
{ NETF(boneOrs[49].xyz.y), 0 },
{ NETF(boneOrs[49].xyz.z), 0 },
{ NETF(boneOrs[49].quatXYZ.x), 0 },
{ NETF(boneOrs[49].quatXYZ.y), 0 },
{ NETF(boneOrs[49].quatXYZ.z), 0 },
{ NETF(boneOrs[50].xyz.x), 0 },
{ NETF(boneOrs[50].xyz.y), 0 },
{ NETF(boneOrs[50].xyz.z), 0 },
{ NETF(boneOrs[50].quatXYZ.x), 0 },
{ NETF(boneOrs[50].quatXYZ.y), 0 },
{ NETF(boneOrs[50].quatXYZ.z), 0 },
{ NETF(boneOrs[51].xyz.x), 0 },
{ NETF(boneOrs[51].xyz.y), 0 },
{ NETF(boneOrs[51].xyz.z), 0 },
{ NETF(boneOrs[51].quatXYZ.x), 0 },
{ NETF(boneOrs[51].quatXYZ.y), 0 },
{ NETF(boneOrs[51].quatXYZ.z), 0 },
{ NETF(boneOrs[52].xyz.x), 0 },
{ NETF(boneOrs[52].xyz.y), 0 },
{ NETF(boneOrs[52].xyz.z), 0 },
{ NETF(boneOrs[52].quatXYZ.x), 0 },
{ NETF(boneOrs[52].quatXYZ.y), 0 },
{ NETF(boneOrs[52].quatXYZ.z), 0 },
{ NETF(boneOrs[53].xyz.x), 0 },
{ NETF(boneOrs[53].xyz.y), 0 },
{ NETF(boneOrs[53].xyz.z), 0 },
{ NETF(boneOrs[53].quatXYZ.x), 0 },
{ NETF(boneOrs[53].quatXYZ.y), 0 },
{ NETF(boneOrs[53].quatXYZ.z), 0 },
{ NETF(boneOrs[54].xyz.x), 0 },
{ NETF(boneOrs[54].xyz.y), 0 },
{ NETF(boneOrs[54].xyz.z), 0 },
{ NETF(boneOrs[54].quatXYZ.x), 0 },
{ NETF(boneOrs[54].quatXYZ.y), 0 },
{ NETF(boneOrs[54].quatXYZ.z), 0 },
{ NETF(boneOrs[55].xyz.x), 0 },
{ NETF(boneOrs[55].xyz.y), 0 },
{ NETF(boneOrs[55].xyz.z), 0 },
{ NETF(boneOrs[55].quatXYZ.x), 0 },
{ NETF(boneOrs[55].quatXYZ.y), 0 },
{ NETF(boneOrs[55].quatXYZ.z), 0 },
{ NETF(boneOrs[56].xyz.x), 0 },
{ NETF(boneOrs[56].xyz.y), 0 },
{ NETF(boneOrs[56].xyz.z), 0 },
{ NETF(boneOrs[56].quatXYZ.x), 0 },
{ NETF(boneOrs[56].quatXYZ.y), 0 },
{ NETF(boneOrs[56].quatXYZ.z), 0 },
{ NETF(boneOrs[57].xyz.x), 0 },
{ NETF(boneOrs[57].xyz.y), 0 },
{ NETF(boneOrs[57].xyz.z), 0 },
{ NETF(boneOrs[57].quatXYZ.x), 0 },
{ NETF(boneOrs[57].quatXYZ.y), 0 },
{ NETF(boneOrs[57].quatXYZ.z), 0 },
{ NETF(boneOrs[58].xyz.x), 0 },
{ NETF(boneOrs[58].xyz.y), 0 },
{ NETF(boneOrs[58].xyz.z), 0 },
{ NETF(boneOrs[58].quatXYZ.x), 0 },
{ NETF(boneOrs[58].quatXYZ.y), 0 },
{ NETF(boneOrs[58].quatXYZ.z), 0 },
{ NETF(boneOrs[59].xyz.x), 0 },
{ NETF(boneOrs[59].xyz.y), 0 },
{ NETF(boneOrs[59].xyz.z), 0 },
{ NETF(boneOrs[59].quatXYZ.x), 0 },
{ NETF(boneOrs[59].quatXYZ.y), 0 },
{ NETF(boneOrs[59].quatXYZ.z), 0 },
{ NETF(boneOrs[60].xyz.x), 0 },
{ NETF(boneOrs[60].xyz.y), 0 },
{ NETF(boneOrs[60].xyz.z), 0 },
{ NETF(boneOrs[60].quatXYZ.x), 0 },
{ NETF(boneOrs[60].quatXYZ.y), 0 },
{ NETF(boneOrs[60].quatXYZ.z), 0 },
{ NETF(boneOrs[61].xyz.x), 0 },
{ NETF(boneOrs[61].xyz.y), 0 },
{ NETF(boneOrs[61].xyz.z), 0 },
{ NETF(boneOrs[61].quatXYZ.x), 0 },
{ NETF(boneOrs[61].quatXYZ.y), 0 },
{ NETF(boneOrs[61].quatXYZ.z), 0 },
{ NETF(boneOrs[62].xyz.x), 0 },
{ NETF(boneOrs[62].xyz.y), 0 },
{ NETF(boneOrs[62].xyz.z), 0 },
{ NETF(boneOrs[62].quatXYZ.x), 0 },
{ NETF(boneOrs[62].quatXYZ.y), 0 },
{ NETF(boneOrs[62].quatXYZ.z), 0 },
{ NETF(boneOrs[63].xyz.x), 0 },
{ NETF(boneOrs[63].xyz.y), 0 },
{ NETF(boneOrs[63].xyz.z), 0 },
{ NETF(boneOrs[63].quatXYZ.x), 0 },
{ NETF(boneOrs[63].quatXYZ.y), 0 },
{ NETF(boneOrs[63].quatXYZ.z), 0 },
{ NETF(attachments[0].modelIndex), MODELNUM_BITS },
{ NETF(attachments[0].boneIndex), MODELNUM_BITS },
{ NETF(attachments[1].modelIndex), MODELNUM_BITS },
{ NETF(attachments[1].boneIndex), MODELNUM_BITS },
{ NETF(attachments[2].modelIndex), MODELNUM_BITS },
{ NETF(attachments[2].boneIndex), MODELNUM_BITS },
{ NETF(attachments[3].modelIndex), MODELNUM_BITS },
{ NETF(attachments[3].boneIndex), MODELNUM_BITS },
};


// if (int)f == f and (int)f + ( 1<<(FLOAT_INT_BITS-1) ) < ( 1 << FLOAT_INT_BITS )
// the float will be sent with FLOAT_INT_BITS, otherwise all 32 bits will be sent
#define	FLOAT_INT_BITS	13
#define	FLOAT_INT_BIAS	(1<<(FLOAT_INT_BITS-1))

/*
==================
MSG_WriteDeltaEntity

Writes part of a packetentities message, including the entity number.
Can delta from either a baseline or a previous packet_entity
If to is NULL, a remove entity update will be sent
If force is not set, then nothing at all will be generated if the entity is
identical, under the assumption that the in-order delta code will catch it.
==================
*/
void msg_c::writeDeltaEntity(struct entityState_s *from, struct entityState_s *to, 
						   bool force ) {
	int			i, lc;
	int			numFields;
	netField_t	*field;
	int			trunc;
	float		fullFloat;
	int			*fromF, *toF;

	numFields = ARRAY_LEN( entityStateFields );

	// all fields should be 32 bits to avoid any compiler packing issues
	// the "number" field is not part of the field list
	// if this assert fails, someone added a field to the entityState_s
	// struct without updating the message fields
	assert( numFields + 1 == sizeof( *from )/4 );

	// a NULL to is a delta remove message
	if ( to == NULL ) {
		if ( from == NULL ) {
			return;
		}
		this->writeBits(from->number, GENTITYNUM_BITS );
		this->writeBits(1, 1 );
		return;
	}

	if ( to->number < 0 || to->number >= MAX_GENTITIES ) {
		Com_Error (ERR_FATAL, "MSG_WriteDeltaEntity: Bad entity number: %i", to->number );
	}

	lc = 0;
	// build the change vector as bytes so it is endien independent
	for ( i = 0, field = entityStateFields ; i < numFields ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		if ( *fromF != *toF ) {
			lc = i+1;
		}
	}

	if ( lc == 0 ) {
		// nothing at all changed
		if ( !force ) {
			return;		// nothing at all
		}
		// write two bits for no change
		this->writeBits(to->number, GENTITYNUM_BITS );
		this->writeBits(0, 1 );		// not removed
		this->writeBits(0, 1 );		// no delta
		return;
	}

	this->writeBits(to->number, GENTITYNUM_BITS );
	this->writeBits(0, 1 );			// not removed
	this->writeBits(1, 1 );			// we have a delta

	this->writeBits(lc, 9 );	// # of changes

	oldsize += numFields;

	for ( i = 0, field = entityStateFields ; i < lc ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( *fromF == *toF ) {
			this->writeBits(0, 1 );	// no change
			continue;
		}

		this->writeBits(1, 1 );	// changed

		if ( field->bits == 0 ) {
			// float
			fullFloat = *(float *)toF;
			trunc = (int)fullFloat;

			if (fullFloat == 0.0f) {
					this->writeBits(0, 1 );
					oldsize += FLOAT_INT_BITS;
			} else {
				this->writeBits(1, 1 );
				if ( trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && 
					trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) ) {
					// send as small integer
					this->writeBits(0, 1 );
					this->writeBits(trunc + FLOAT_INT_BIAS, FLOAT_INT_BITS );
				} else {
					// send as full floating point value
					this->writeBits(1, 1 );
					this->writeBits(*toF, 32 );
				}
			}
		} else {
			if (*toF == 0) {
				this->writeBits(0, 1 );
			} else {
				this->writeBits(1, 1 );
				// integer
				this->writeBits(*toF, field->bits );
			}
		}
	}
}

/*
==================
MSG_ReadDeltaEntity

The entity number has already been read from the message, which
is how the from state is identified.

If the delta removes the entity, entityState_s->number will be set to MAX_GENTITIES-1

Can go from either a baseline or a previous packet_entity
==================
*/
void msg_c::readDeltaEntity(entityState_s *from, entityState_s *to, 
						 int number) {
	int			i, lc;
	int			numFields;
	netField_t	*field;
	int			*fromF, *toF;
	int			print;
	int			trunc;
	int			startBit, endBit;

	if ( number < 0 || number >= MAX_GENTITIES) {
		Com_Error( ERR_DROP, "Bad delta entity number: %i", number );
	}

	if ( this->bit == 0 ) {
		startBit = this->readcount * 8 - GENTITYNUM_BITS;
	} else {
		startBit = ( this->readcount - 1 ) * 8 + this->bit - GENTITYNUM_BITS;
	}

	// check for a remove
	if ( this->readBits(1 ) == 1 ) {
		memset( to, 0, sizeof( *to ) );	
		to->number = MAX_GENTITIES - 1;
		if ( cl_shownet->integer >= 2 || cl_shownet->integer == -1 ) {
			Com_Printf( "%3i: #%-3i remove\n", this->readcount, number );
		}
		return;
	}

	// check for no delta
	if ( this->readBits(1 ) == 0 ) {
		*to = *from;
		to->number = number;
		return;
	}

	numFields = ARRAY_LEN( entityStateFields );
	lc = this->readBits(9);

	if ( lc > numFields || lc < 0 ) {
		Com_Error( ERR_DROP, "invalid entityState field count" );
	}

	// shownet 2/3 will interleave with other printed info, -1 will
	// just print the delta records`
	if ( cl_shownet->integer >= 2 || cl_shownet->integer == -1 ) {
		print = 1;
		Com_Printf( "%3i: #%-3i ", this->readcount, to->number );
	} else {
		print = 0;
	}

	to->number = number;

	for ( i = 0, field = entityStateFields ; i < lc ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( ! this->readBits(1 ) ) {
			// no change
			*toF = *fromF;
		} else {
			if ( field->bits == 0 ) {
				// float
				if ( this->readBits(1 ) == 0 ) {
						*(float *)toF = 0.0f; 
				} else {
					if ( this->readBits(1 ) == 0 ) {
						// integral float
						trunc = this->readBits(FLOAT_INT_BITS );
						// bias to allow equal parts positive and negative
						trunc -= FLOAT_INT_BIAS;
						*(float *)toF = trunc; 
						if ( print ) {
							Com_Printf( "%s:%i ", field->name, trunc );
						}
					} else {
						// full floating point value
						*toF = this->readBits(32 );
						if ( print ) {
							Com_Printf( "%s:%f ", field->name, *(float *)toF );
						}
					}
				}
			} else {
				if ( this->readBits(1 ) == 0 ) {
					*toF = 0;
				} else {
					// integer
					*toF = this->readBits(field->bits );
					if ( print ) {
						Com_Printf( "%s:%i ", field->name, *toF );
					}
				}
			}
//			pcount[i]++;
		}
	}
	for ( i = lc, field = &entityStateFields[lc] ; i < numFields ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		// no change
		*toF = *fromF;
	}

	if ( print ) {
		if ( this->bit == 0 ) {
			endBit = this->readcount * 8 - GENTITYNUM_BITS;
		} else {
			endBit = ( this->readcount - 1 ) * 8 + this->bit - GENTITYNUM_BITS;
		}
		Com_Printf( " (%i bits)\n", endBit - startBit  );
	}
}


/*
============================================================================

plyer_state_t communication

============================================================================
*/

// using the stringizing operator to save typing...
#define	PSF(x) #x,(size_t)&((playerState_s*)0)->x

netField_t	playerStateFields[] = 
{
{ PSF(commandTime), 32 },			
{ PSF(origin[0]), 0 },
{ PSF(origin[1]), 0 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(viewangles[2]), 0 },		
{ PSF(angles[0]), 0 },
{ PSF(angles[1]), 0 },
{ PSF(angles[2]), 0 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(viewheight), -8 },				
{ PSF(delta_angles[0]), 16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(delta_angles[2]), 16 },
{ PSF(clientNum), 8 },
{ PSF(colModelIndex), MODELNUM_BITS },
{ PSF(rModelIndex), MODELNUM_BITS },
{ PSF(rSkinIndex), SKINNUM_BITS },
{ PSF(parentNum), GENTITYNUM_BITS },
{ PSF(parentTagNum), TAGNUM_BITS },
{ PSF(animIndex), ANIMNUM_BITS },
{ PSF(torsoAnim), ANIMNUM_BITS },
{ PSF(curWeaponEntNum), GENTITYNUM_BITS },
{ PSF(customViewRModelIndex), MODELNUM_BITS },
{ PSF(customViewRModelIndex2), MODELNUM_BITS },
{ PSF(activeRagdollDefNameIndex), RAGDOLLDEFNUM_BITS },
{ PSF(viewModelAnim), ANIMNUM_BITS },
{ PSF(viewModelAnimFlags), ANIMFLAG_BITS },		
{ PSF(viewModel2Anim), ANIMNUM_BITS },
{ PSF(viewModel2AnimFlags), ANIMFLAG_BITS },	
{ PSF(viewModelAngles[0]), 0 },
{ PSF(viewModelAngles[1]), 0 },
{ PSF(viewModelAngles[2]), 0 },
{ PSF(viewModelOffset[0]), 0 },
{ PSF(viewModelOffset[1]), 0 },
{ PSF(viewModelOffset[2]), 0 },
{ PSF(viewWeaponMaxClipSize), 0 },
{ PSF(viewWeaponCurClipSize), 0 },
{ PSF(eFlags), EF_NUM_FLAG_BITS },
{ PSF(scale[0]), 0 },
{ PSF(scale[1]), 0 },
{ PSF(scale[2]), 0 },	
};

void msg_c::writeDeltaPlayerstate(struct playerState_s *from, struct playerState_s *to ) {
	int				i;
	playerState_s	dummy;
	int				numFields;
	netField_t		*field;
	int				*fromF, *toF;
	float			fullFloat;
	int				trunc, lc;

	if (!from) {
		from = &dummy;
		memset (&dummy, 0, sizeof(dummy));
	}

	numFields = ARRAY_LEN( playerStateFields );

	lc = 0;
	for ( i = 0, field = playerStateFields ; i < numFields ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		if ( *fromF != *toF ) {
			lc = i+1;
		}
	}

	this->writeByte(lc );	// # of changes

	oldsize += numFields - lc;

	for ( i = 0, field = playerStateFields ; i < lc ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( *fromF == *toF ) {
			this->writeBits(0, 1 );	// no change
			continue;
		}

		this->writeBits(1, 1 );	// changed
//		pcount[i]++;

		if ( field->bits == 0 ) {
			// float
			fullFloat = *(float *)toF;
			trunc = (int)fullFloat;

			if ( trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && 
				trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) ) {
				// send as small integer
				this->writeBits(0, 1 );
				this->writeBits(trunc + FLOAT_INT_BIAS, FLOAT_INT_BITS );
			} else {
				// send as full floating point value
				this->writeBits(1, 1 );
				this->writeBits(*toF, 32 );
			}
		} else {
			// integer
			this->writeBits(*toF, field->bits );
		}
	}

}


void msg_c::readDeltaPlayerstate (playerState_s *from, playerState_s *to ) {
	int			i, lc;
	netField_t	*field;
	int			numFields;
	int			startBit, endBit;
	int			print;
	int			*fromF, *toF;
	int			trunc;
	playerState_s	dummy;

	if ( !from ) {
		from = &dummy;
		memset( &dummy, 0, sizeof( dummy ) );
	}
	*to = *from;

	if ( this->bit == 0 ) {
		startBit = this->readcount * 8 - GENTITYNUM_BITS;
	} else {
		startBit = ( this->readcount - 1 ) * 8 + this->bit - GENTITYNUM_BITS;
	}

	// shownet 2/3 will interleave with other printed info, -2 will
	// just print the delta records
	if ( cl_shownet->integer >= 2 || cl_shownet->integer == -2 ) {
		print = 1;
		Com_Printf( "%3i: playerstate ", this->readcount );
	} else {
		print = 0;
	}

	numFields = ARRAY_LEN( playerStateFields );
	lc = this->readByte();

	if ( lc > numFields || lc < 0 ) {
		Com_Error( ERR_DROP, "invalid playerState field count" );
	}

	for ( i = 0, field = playerStateFields ; i < lc ; i++, field++ ) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( ! this->readBits(1 ) ) {
			// no change
			*toF = *fromF;
		} else {
			if ( field->bits == 0 ) {
				// float
				if ( this->readBits(1 ) == 0 ) {
					// integral float
					trunc = this->readBits(FLOAT_INT_BITS );
					// bias to allow equal parts positive and negative
					trunc -= FLOAT_INT_BIAS;
					*(float *)toF = trunc; 
					if ( print ) {
						Com_Printf( "%s:%i ", field->name, trunc );
					}
				} else {
					// full floating point value
					*toF = this->readBits(32 );
					if ( print ) {
						Com_Printf( "%s:%f ", field->name, *(float *)toF );
					}
				}
			} else {
				// integer
				*toF = this->readBits(field->bits );
				if ( print ) {
					Com_Printf( "%s:%i ", field->name, *toF );
				}
			}
		}
	}
	for ( i=lc,field = &playerStateFields[lc];i<numFields; i++, field++) {
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		// no change
		*toF = *fromF;
	}


	if ( print ) {
		if ( this->bit == 0 ) {
			endBit = this->readcount * 8 - GENTITYNUM_BITS;
		} else {
			endBit = ( this->readcount - 1 ) * 8 + this->bit - GENTITYNUM_BITS;
		}
		Com_Printf( " (%i bits)\n", endBit - startBit  );
	}
}


void msg_c::compress(int ofs) {


}
void msg_c::decompress(int ofs) {


}
//===========================================================================
