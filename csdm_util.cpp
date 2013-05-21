#include <malloc.h>
#include <stdlib.h>
#include "amxxmodule.h"
#include "csdm_amxx.h"
#include "csdm_util.h"
#include "csdm_tasks.h"
#include "csdm_player.h"
#include "cs_offsets.h"
#include "chooker.h"

CHooker		HookerClass;
CHooker*	Hooker = &HookerClass;

CFunc*				RestartRoundHook = NULL;
FuncRestartRound	RestartRoundOrig = NULL;

void *g_restartround_func = NULL;
void *g_respawn_func = NULL;
float g_last_ff_set = 0.0f;

struct patch_set
{
	unsigned char *base;
	int *offsets;
	unsigned char **patches;
	unsigned char **unpatches;
	unsigned int num;
};

patch_set g_takedmg_patches;
patch_set g_pkilled_patches;

struct _p_ldr
{
	const char *cod;
	int offs;
};

void InitPatchControl(_p_ldr src[], patch_set *dest, unsigned int num)
{
	dest->offsets = new int[num];
	dest->patches = new unsigned char *[num];
	dest->unpatches = new unsigned char *[num];
	for (unsigned int i=0; i<num; i++)
	{
		dest->offsets[i] = src[i].offs;
		dest->patches[i] = (unsigned char *)strdup(src[i].cod);
		dest->unpatches[i] = (unsigned char *)strdup(src[i].cod);
	}
	dest->num = num;
}

bool InitUtilCode()
{
	void* gameAddress = Hooker->memFunc->GetLibraryFromAddress( ( void* )MDLL_Spawn );

	#if defined __linux__
		BOOL useSymbol = TRUE;
	#else
		BOOL useSymbol = FALSE;
	#endif

	g_respawn_func			= Hooker->MemorySearch< void* >			( CSPLAYER_ROUNDRESPAWN	, gameAddress, useSymbol );
	g_restartround_func		= Hooker->MemorySearch< void* >			( CSPLAYER_RESTARTROUND	, gameAddress, useSymbol );
	g_takedmg_patches.base	= Hooker->MemorySearch< unsigned char* >( CSPLAYER_TAKEDAMAGE	, gameAddress, useSymbol );
	g_pkilled_patches.base	= Hooker->MemorySearch< unsigned char* >( CSGAME_PLAYERKILLED	, gameAddress, useSymbol );

	RestartRoundHook = Hooker->CreateHook( g_restartround_func, ( void* )RestartRound, TRUE );
	RestartRoundOrig = reinterpret_cast< FuncRestartRound >( RestartRoundHook->GetOriginal() );

	_p_ldr ffa[] = CSP_TD_PATCHES;
	InitPatchControl(ffa, &g_takedmg_patches, CSP_TD_PATCH_COUNT);

	_p_ldr pk[] = CSG_PK_PATCHES;
	InitPatchControl(pk, &g_pkilled_patches, CSG_PK_PATCH_COUNT);

	return true;
}

void DoPatch(patch_set *pt)
{
	size_t pt_len;
	unsigned char *offs;
	for (unsigned int i=0; i<pt->num; i++)
	{
		offs = pt->base + pt->offsets[i];
		pt_len = strlen((char *)pt->patches[i]);
		Hooker->memFunc->ChangeMemoryProtection( offs, pt_len, PAGE_EXECUTE_READWRITE );
		memcpy(pt->unpatches[i], offs, pt_len);
		memcpy(offs, pt->patches[i], pt_len);
	}
}

void DoUnPatch(patch_set *pt)
{
	size_t pt_len;
	unsigned char *offs;
	for (unsigned int i=0; i<pt->num; i++)
	{
		offs = pt->base + pt->offsets[i];
		pt_len = strlen((char *)pt->patches[i]);
		Hooker->memFunc->ChangeMemoryProtection(offs, pt_len, PAGE_EXECUTE_READWRITE);
		memcpy(offs, pt->unpatches[i], pt_len);
	}
}

void FFA_Disable()
{
	// unpatch!
	DoUnPatch(&g_takedmg_patches);
	DoUnPatch(&g_pkilled_patches);
	g_ffa_state = false;
	//we don't care about efficiency here
	CVAR_SET_FLOAT("mp_friendlyfire", g_last_ff_set);
	CVAR_SET_STRING("mp_freeforall", "0");
	MF_ExecuteForward(g_StateChange, CSDM_FFA_DISABLE);
}

void FFA_Enable()
{
	// patch!
	DoPatch(&g_takedmg_patches);
	DoPatch(&g_pkilled_patches);
	g_ffa_state = true;
	//we don't care about efficiency here
	g_last_ff_set = CVAR_GET_FLOAT("mp_friendlyfire");
	CVAR_SET_FLOAT("mp_friendlyfire", 1.0f);
	CVAR_SET_STRING("mp_freeforall", "1");
	MF_ExecuteForward(g_StateChange, CSDM_FFA_ENABLE);
}

void InternalSpawnPlayer(edict_t *pEdict)
{
	pEdict->v.deadflag = DEAD_RESPAWNABLE;
	pEdict->v.flags |= FL_FROZEN;

	CBasePlayer *pPlayer = (CBasePlayer *)pEdict->pvPrivateData;

#if defined( WIN32 )
	typedef void ( __fastcall *RESPAWNFUNC)( CBasePlayer* );
#else
	typedef void ( *RESPAWNFUNC )( CBasePlayer* );
#endif

	RESPAWNFUNC rfunc = ( RESPAWNFUNC )g_respawn_func;
	rfunc( pPlayer );
}

FakeCommand::~FakeCommand()
{
	Clear();
}

void FakeCommand::Reset()
{
	num_args = 0;
}

int FakeCommand::GetArgc() const
{
	return num_args;
}

void FakeCommand::SetFullString(const char *fmt, ...)
{
	char buffer[1024];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
	va_end(ap);
	
	full.assign(buffer);
}

const char *FakeCommand::GetFullString() const
{
	return full.c_str();
}

const char *FakeCommand::GetArg(int i) const
{
	if (i < 0 || i > GetArgc())
		return "";

	return args[i]->c_str();
}

void FakeCommand::Clear()
{
	Reset();
	for (unsigned int i=0; i<args.size(); i++)
	{
		delete args[i];
		args[i] = NULL;
	}

	args.clear();
}

void FakeCommand::AddArg(const char *str)
{
	num_args++;
	if (num_args > args.size())
	{
		String *pString = new String(str);
		args.push_back(pString);
	} else {
		args[num_args-1]->assign(str);
	}
}

void RespawnPlayer(edict_t *pEdict)
{
	Respawn *pRespawn;

	int send;
	if (g_PreSpawn == -1)
		send = -2;
	else
		send = g_PreSpawn;

	player_states *pPlayer = GET_PLAYER(ENTINDEX(pEdict));

	//player is already respawning!
	if (pPlayer->spawning)
	{
		return;
	}

	pPlayer->spawning = true;

	pRespawn = Respawn::NewRespawn(pEdict, send);
	g_Timer.AddTask(pRespawn, g_SpawnWaitTime);
}

void FakespawnPlayer(edict_t *pEdict)
{
	int team = GetPlayerTeam(pEdict);

	if (team != TEAM_T && team != TEAM_CT)
	{
		return;
	}

	int index = ENTINDEX(pEdict);

	player_states *pPlayer = GET_PLAYER(index);

	if (pPlayer->spawning)
		return;

	pPlayer->spawning = true;

	if (g_PreSpawn > 0  && MF_ExecuteForward(g_PreSpawn, (cell)index, (cell)1) > 0)
	{
		pPlayer->spawning = false;
		return;
	}

	SpawnHandler(index, true);
	pPlayer->spawning = false;
}

edict_t *GetEdict(int index)
{
	if (index < 1 || index > gpGlobals->maxClients)
		return NULL;

	edict_t *pEdict = MF_GetPlayerEdict(index);
	if (!pEdict)
		return NULL;

	if (pEdict->v.flags & FL_FAKECLIENT)
	{
		player_states *pPlayer = GET_PLAYER(index);
		pPlayer->ingame = true;

		return pEdict;
	}

	if (pEdict->v.flags & FL_CLIENT)
		return pEdict;

	return NULL;
}

void print_srvconsole( const char *fmt, ...)
{
	va_list argptr;
	static char string[384];
	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string) - 1, fmt, argptr);
	string[sizeof(string) - 1] = '\0';
	va_end(argptr);
	
	SERVER_PRINT(string);
}

static int g_TextMsg = 0;

void print_client(edict_t *pEdict, int type, const char *fmt, ...)
{
	//don't do this to bots
	if (pEdict->v.flags & FL_FAKECLIENT)
		return;

	char buffer[255];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
	va_end(ap);

	if (!g_TextMsg)
	{
		g_TextMsg = GET_USER_MSG_ID(PLID, "TextMsg", NULL);
		if (!g_TextMsg)
			return;
	}

	MESSAGE_BEGIN(pEdict ? MSG_ONE : MSG_BROADCAST, g_TextMsg, NULL, pEdict);
		WRITE_BYTE(type);
		WRITE_STRING(buffer);
	MESSAGE_END();
}

bool NotifyForRemove(unsigned int owner, edict_t *ent, edict_t *box)
{
	cell idx1 = (cell)ENTINDEX(ent);
	cell idx2 = box ? (cell)ENTINDEX(box) : 0;

	return (MF_ExecuteForward(g_RemoveWeapon, (cell)owner, idx1, idx2) == 0);
}
