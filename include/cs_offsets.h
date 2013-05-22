/**
 * Today, and forever more, I am ending this mess.
 */

#ifndef _INCLUDE_OFFSETS_H
#define _INCLUDE_OFFSETS_H

/**
 * NOTE: On X86, our patches look like:
 *  0xFF 0x25 * * * * (6 bytes)
 */

//"six byte nop"
#define SBN		"0x90,0x90,0x90,0x90,0x90,0x90"

#if defined WIN32

	#define CSPLAYER_DROPPLAYERITEM		"0x83,*,*,0x33,*,0x53,0x55,0x56,0x8B,*,*,*,0x57" 
	#define CSPLAYER_ROUNDRESPAWN		"0x83,*,*,0x53,0x56,0x8B,*,0x33,*,0x57,0x8A"
	#define CSPLAYER_RESTARTROUND		"0x51,0x53,0x56,0x8B,*,0x8B,*,*,*,*,*,0x8B"
	#define CSPLAYER_TAKEDAMAGE			"0x83,*,*,0x53,0x8B,*,*,*,0x55,0x33,*,0x56,0x57"
	/**
	 * 09C840 (base)
	 * 09CA04 (0x1C4) - first  check, jnz->1byte jmp, purpose unknown
	 * 09CD14 (0x4D4) - second check, jz->2byte nop, purpose unknown
	 * 09D04F (0x80F) - third  check, jnz->2byte jmp, displays "hint"
	 * 09D191 (0x951) - fourth check, jnz->1byte jmp, displays attack
	 * 09D1F0 (0x9B0) - fifth  check, jnz->1byte jmp, does dmg deamp
	 * 09D6F4 (0xEB4) - sixth  check, jz->2byte nop, purpose unknown*/
	#define CSP_TD_PATCHES				{{"0xEB",0x1C4},{"0x90,0x90",0x4D4},{"0x90,0xE9",0x80F},{"0xEB",0x951},{"0xEB",0x9B0},{"0x90,0x90",0xEB4}}
	#define CSP_TD_PATCH_COUNT			6

	#define CSGAME_PLAYERKILLED			"0x51,0x53,0x8B,*,*,*,0x55,0x8B,*,0x8B"
	#define CSGAME_PLAYERKILLED_BYTES	10
	/**
	 * 091A70 (base)	
	 * 091BD2 - (0x162) - first check, jnz->2byte jump, does kill stuff
	 */
	#define CSG_PK_PATCHES		{{"0x90,0xE9",0x162}}
	#define CSG_PK_PATCH_COUNT	1

#else

	#define CSPLAYER_DROPPLAYERITEM	"_ZN11CBasePlayer14DropPlayerItemEPKc" 
	#define CSPLAYER_ROUNDRESPAWN	"_ZN11CBasePlayer12RoundRespawnEv"
	#define CSPLAYER_RESTARTROUND	"_ZN18CHalfLifeMultiplay12RestartRoundEv"
	#define CSPLAYER_TAKEDAMAGE		"_ZN11CBasePlayer10TakeDamageEP9entvars_sS1_fi"

	/**
	 * 111720 (base)
	 * 111DF6 (0x6D6)  - first check	, jz->5byte nop	, displays hint
	 * 112414 (0xCF4)  - second check	, jnz->2byte jmp, related to career task
	 * 11256E (0xE4E)  - third check	, jnz->2byte jmp, does dmg deamp
	 * 1127B3 (0x1093) - fourth check	, jnz->2byte jmp, related to grenade and mp_friendlyfire
	 * 11284C (0x112C) - fifth check	, jnz->2byte jmp, related to career task
	 * 1124BB (0xD9B)  - sixth check	, jnz->2byte jmp, related to grenade and mp_friendlyfire
	 * 112B94 (0x1474) - seventh check	, jnz->1byte jmp, displays team attack msg
	 */
	#define CSP_TD_PATCHES		{{SBN,0x6D6},{"0x90,0xE9",0xCF4},{"0x90,0xE9",0xE4E},{"0x90,0xE9",0x1093},{"0x90,0xE9",0x112C},{"0x90,0xE9",0xD9B},{"xEB",0x1474}}
	#define CSP_TD_PATCH_COUNT	7

	#define CSGAME_PLAYERKILLED	"_ZN18CHalfLifeMultiplay12PlayerKilledEP11CBasePlayerP9entvars_sS3_"
	/**
	 * F2880 (base)
	 * F2A4C (0x1CC) - first check, jnz->5byte nop, does kill stuff
	 */		
	#define CSG_PK_PATCHES		{{"0x90,0x90,0x90,0x90,0x90,0x90",0x1CC}}
	#define CSG_PK_PATCH_COUNT	1

#endif //WIN32

#endif //_INCLUDE_OFFSETS_H
