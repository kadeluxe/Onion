#pragma once
#include "NetVar.h"
#include "VMT.h"
#include "Vector.h"
#include "Defines.h"
#include "interfaces\InterfaceClient.h"

class CBaseEntity
{
public:
	int GetTeam()
	{
		return *(int*)(this + g_NetVars.GetNetVar("DT_BaseEntity", "m_iTeamNum"));
	}

	Vector& GetAbsOrigin()
	{
		typedef Vector&(__thiscall* fn)(void*);
		return VMT::getvfunc<fn>(this, 10)(this);
	}

	int GetHealth()
	{
		return *(int*)(this + g_NetVars.GetNetVar("DT_BasePlayer", "m_iHealth"));
	}

	unsigned int GetFlags()
	{
		return *(unsigned int*)(this + g_NetVars.GetNetVar("DT_BasePlayer", "m_fFlags"));
	}

	unsigned char GetLifestate()
	{
		return *(unsigned char*)(this + g_NetVars.GetNetVar("DT_BasePlayer", "m_lifeState"));
	}

	bool* GetSpotted()
	{
		return (bool*)(this + g_NetVars.GetNetVar("DT_BaseCombatCharacter", "m_bSpotted"));
	}

	HANDLE GetActiveWeapon()
	{
		return *(HANDLE*)(this + g_NetVars.GetNetVar("DT_BaseCombatCharacter", "m_hActiveWeapon"));
	}

	Vector GetEyePosition()
	{
		return GetAbsOrigin() + *(Vector*)(this + g_NetVars.GetNetVar("DT_LocalPlayerExclusive", "m_vecViewOffset[0]"));
	}

	bool SetupBones(matrix3x4_t* bone, int maxbones, int bonemask, float currenttime)
	{
		void* renderable = this + 0x4;
		typedef bool(__thiscall* fn)(void*, matrix3x4_t*, int, int, float);
		return VMT::getvfunc<fn>(renderable, 13)(renderable, bone, maxbones, bonemask, currenttime);
	}

	void* GetModel()
	{
		void* renderable = (void*)(this + 0x4);
		typedef void*(__thiscall* fn)(void*);
		return VMT::getvfunc<fn>(renderable, 8)(renderable);
	}

	int GetIndex()
	{
		void* networkable = this + 0x8;
		typedef int(__thiscall* fn)(void*);
		return VMT::getvfunc<fn>(networkable, 10)(networkable);
	}

	ClientClass* GetClientClass()
	{
		void* networkable = this + 0x8;
		typedef ClientClass*(__thiscall* fn)(void*);
		return VMT::getvfunc<fn>(networkable, 2)(networkable);
	}

	bool IsPlayer()
	{
		std::string name(GetClientClass()->m_pNetworkName);
		return (name.find("CCSPlayer") != std::string::npos);
	}

	int GetHitboxSet()
	{
		return *(int*)(this + g_NetVars.GetNetVar("DT_BaseAnimating", "m_nHitboxSet"));
	}

	int shotsfired()
	{
		return *(int*)(this + g_NetVars.GetNetVar("DT_CSLocalPlayerExclusive", "m_iShotsFired"));
	}

	Vector GetAimPunch()
	{
		return *(Vector*)(this + g_NetVars.GetNetVar("DT_LocalPlayerExclusive", "m_Local") + g_NetVars.GetNetVar("DT_Local", "m_aimPunchAngle"));
	}

	bool IsDormant()
	{
		void* networkable = (void*)(this + 0x8);
		typedef bool(__thiscall* fn)(void*);
		return VMT::getvfunc<fn>(networkable, 9)(networkable);
	}

	Vector GetVelocity()
	{
		return *(Vector*)(this + g_NetVars.GetNetVar("DT_BasePlayer", "m_vecVelocity[0]"));
	}
};

class CBaseCombatWeapon extends CBaseEntity
{
public:
	int* GetItemDefinitionIndex()
	{
		return (int*)(this + g_NetVars.GetNetVar("DT_BaseAttributableItem", "m_AttributeManager") + g_NetVars.GetNetVar("DT_AttributeContainer", "m_Item") + g_NetVars.GetNetVar("DT_ScriptCreatedItem", "m_iItemDefinitionIndex"));
	}

	int GetClip()
	{
		return *(int*)(this + g_NetVars.GetNetVar("DT_BaseCombatWeapon", "m_iClip1"));
	}


};