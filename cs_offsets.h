/**
 * Today, and forever more, I am ending this mess.
 */

#ifndef _INCLUDE_OFFSETS_H
#define _INCLUDE_OFFSETS_H

/**
 * NOTE: On X86, our patches look like:
 *  0xFF 0x25 * * * * (6 bytes)
 * On AMD64, we need 12 bytes:
 *  0x48 0xB8 * * * * * * * * (10 bytes)
 *  0xFF 0xE0 (2 bytes)
 */

//"six byte nop"
#define SBN		"\x90\x90\x90\x90\x90\x90"

#if defined WIN32

//find mp_tkpunish + kill
#define CSPLAYER_ROUNDRESPAWN			"\x83\xEC\x0C\x53\x56\x8B\xF1\x33\xDB\x57\x8A\x86\x60\x03\x00\x00\xC6\x86\x7D\x09\x00\x00\x01\x3C\x01\x75\x40\x68\x2A\x2A\x2A\x2A"
#define CSPLAYER_ROUNDRESPAWN_BYTES		32

//find mp_autoteambalance
//subtract the return from the start
#define CSPLAYER_RESTARTROUND			"\x51\x53\x56\x8B\xF1\x8B\x0D\x2A\x2A\x2A\x2A\x8B\x01\xFF\x50\x1C\x8B\x0D\x2A\x2A\x2A\x2A\x33\xDB\x3B\xCB\x74\x05\xE8\x2A\x2A\x2A"
#define CSPLAYER_RESTARTROUND_BYTES		32
//there are 0x9DE bytes in this function.
//the last ones are a mov, 3 pops, and a retn
//that gives us, from the end...
#define CSPLAYER_RESTARTROUND_END		0x9DE
#define CSPLAYER_RESTARTROUND_PATCH_BYTES	10

#define CSPLAYER_TAKEDAMAGE				"\x83\xEC\x40\x53\x8B\x5C\x24\x54\x55\x33\xED\x56\x57\xF7\xC3\x60\x00\x00\x01\x8B\xF1\xC7\x44\x24\x28\x00\x00\x00\x00\x89\x6C\x24"
#define CSPLAYER_TAKEDAMAGE_BYTES	16
/**
 * 08FFED (0x4BD) - first check, jz->2byte nop, purpose unknown
 * 090328 (0x7F8) - second check, jnz->2byte jmp, displays "hint"
 * 09046A (0x93A) - third check, jnz->1byte jmp, displays attack
 * 0904C9 (0x999) - fourth check, jnz->1byte jmp, does dmg deamp
 * 0909CD (0xE9D) - fifth check, jz->2byte nop, purpose unknown*/
#define CSP_TD_PATCHES					{{"\x90\x90",0x4F3},{"\x90\xE9",0x87B},{"\xEB",0x9BB},{"\xEB",0xA19},{"\x90\x90",0xF43}}
#define CSP_TD_PATCH_COUNT	5

#define CSGAME_PLAYERKILLED			"\x51\x53\x8B\x5C\x24\x10\x55\x8B\xE9\x8B\x4C\x24\x18\x56\x8B\x45\x00\x57\x8B\x7C\x24\x18\x51\x53\x57\x8B\xCD\xFF\x50\x74\x8B\x97"
#define CSGAME_PLAYERKILLED_BYTES	32
/**
 * 085B22 - (0x162) - first check, jnz->2byte jump, does kill stuff
 */
#define CSG_PK_PATCHES		{{"\x90\xE9",0x167}}
#define CSG_PK_PATCH_COUNT	1

#else

/**
 * LINUX OFFSETS 
 */

#if defined AMD64

#error No longer supported!

///////
// AMD64
///////

#define CSPLAYER_ROUNDRESPAWN			"\x48\x83\xEC\x18\x48\x89\x5C\x24\x08\x48\x89\x6C\x24\x10\x48\x89\xFB\x80\xBF\xF8\x03\x00\x00\x01\xC6\x87\x59\x0A\x00\x00\x01\x74"
#define CSPLAYER_ROUNDRESPAWN_BYTES		32

#define CSPLAYER_RESTARTROUND			"\x41\x56\x41\x55\x41\x54\x49\x89\xFC\x55\x53\x48\x83\xEC\x10\x48\x8B\x0D\x2A\x2A\x2A\x2A\x48\x8B\x39\x48\x8B\x17\xFF\x52\x38\x48"
#define CSPLAYER_RESTARTROUND_BYTES		32
//there are 0x847 bytes in this function.
//note that this val does not disasm to the VERY end,
//the return is in the middle
#define CSPLAYER_RESTARTROUND_END		0x847
#define CSPLAYER_RESTARTROUND_PATCH_BYTES	13

#define CSPLAYER_TAKEDAMAGE				"\x41\x57\x41\x56\x41\x89\xCE\x41\x55\x41\x54\x49\x89\xFC\x55\x53\x48\x81\xEC\x98\x00\x00\x00\xF7\xC1\x60\x00\x00\x01\x48\x89\x54"
#define CSPLAYER_TAKEDAMAGE_BYTES		32
/**
 * 1248D4 (0x4A4) - first check, jz->6byte nop, purpose unknown
 * 124A79 (0x649) - second check, jz->6byte nop, displays "Hint" about not shooting mates
 * 124F3F (0xB0F) - third check, jnz->2byte JMP, does dmg deamplification
 * 124FEF (0xBBF) - fourth check, jz->2byte nop, displays team attack msg
 * 1255C2 (0x1192) - fifth check, jz->6byte nop, purpose unknown
 */
#define CSP_TD_PATCHES		{{SBN,0x4A4},{SBN,0x649},{"\x90\xE9",0xB0F},{"\x90\x90",0xBBF},{SBN,0x1192}}
#define CSP_TD_PATCH_COUNT	5

#define CSGAME_PLAYERKILLED				"\x41\x57\x41\x56\x49\x89\xD6\x41\x55\x41\x54\x49\x89\xFC\x55\x48\x89\xF5\x53\x48\x83\xEC\x18\x48\x89\x4C\x24\x10\x48\x8B\x1F\xFF"
#define CSGAME_PLAYERKILLED_BYTES		32
/**
 * 117179 (0x289) - first check, jz->6byte nop, does kill stuff
 */
#define CSG_PK_PATCHES			{{SBN,0x289}}
#define CSG_PK_PATCH_COUNT		1

#else

///////
// X86
///////

#define CSPLAYER_ROUNDRESPAWN			"RoundRespawn__11CBasePlayer"

#define CSPLAYER_RESTARTROUND			"RestartRound__18CHalfLifeMultiplay"
//there are 0xB8A bytes in this function.
//the last ones are all two/one byte instrs
//that gives us, from the end...
#define CSPLAYER_RESTARTROUND_END			0xB8E
#define CSPLAYER_RESTARTROUND_PATCH_BYTES	6

#define CSPLAYER_TAKEDAMAGE				"TakeDamage__11CBasePlayerP9entvars_sT1fi"
/**
 * 0CD576 (0x70A) - first check, jz->2byte nop, purpose unknown
 * 0CD93D (0xAD1) - second check, jnz->2byte jmp, displays hint"
 * 0CDA10 (0xBA4) - third check, jnz->1byte jmp, displays team attack msg
 * 0CDA75 (0xC09) - fourth check, jnz->1byte jmp, does dmg deamp
 * 0CE1D8 (0x136c) - fifth check, jz->2byte nop, purpose unknown
 */
#define CSP_TD_PATCHES		{{"\x90\x90",0x709},{"\x90\xE9",0xAD6},{"\xEB",0xBAB},{"\xEB",0xC0A},{"\x90\x90",0x1369}}
#define CSP_TD_PATCH_COUNT	5

#define CSGAME_PLAYERKILLED				"PlayerKilled__18CHalfLifeMultiplayP11CBasePlayerP9entvars_sT2"
/**
 * 0BF32A (0x182) - first check, jnz->2byte jmp, does kill stuff
 */
#define CSG_PK_PATCHES		{{"\x90\xE9",0x182}}
#define CSG_PK_PATCH_COUNT	1

#endif //AMD64

#endif //WIN32

#endif //_INCLUDE_OFFSETS_H
