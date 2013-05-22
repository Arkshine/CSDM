#ifndef _INCLUDE_CSDM_UTIL_H
#define _INCLUDE_CSDM_UTIL_H

#include "chooker.h"

//returns true if plugins said ok to remove the weapons 
bool NotifyForRemove(unsigned int owner, edict_t *ent, edict_t *box);

void RespawnPlayer(edict_t *pEdict);
void FakespawnPlayer(edict_t *pEdict);
edict_t *GetEdict(int index);
void print_srvconsole(const char *fmt, ...);
void print_client(edict_t *pEdict, int type, const char *fmt, ...);
bool InitUtilCode();
void InternalSpawnPlayer(edict_t *pEdict);
void FFA_Enable();
void FFA_Disable();

#if defined( WIN32 )
	typedef void ( __fastcall *FuncDropPlayerItem )	( CBasePlayer*, DUMMY, const char* );
	typedef void ( __fastcall *FuncRestartRound )	( void* );
#else
	typedef void ( *FuncDropPlayerItem )	( void*, const char* );
	typedef void ( *FuncRestartRound )		( void* );
#endif

extern CFunc*				RestartRoundHook;
extern FuncRestartRound		RestartRoundOrig;
extern FuncDropPlayerItem	DropPlayerItem;

#endif //_INCLUDE_CSDM_UTIL_H
