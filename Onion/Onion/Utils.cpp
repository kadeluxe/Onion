#include "Utils.h"
#include <Windows.h>
#include <iostream>
#include "Log.h"
#include "CBaseEntity.h"
#include "interfaces\InterfaceModelInfo.h"

void Utils::InitConsole(char* name)
{
	AllocConsole();

	HWND hwnd = GetConsoleWindow();

	HMENU hMenu = GetSystemMenu(hwnd, FALSE);
	if (hMenu) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);

	SetConsoleTitle(name);
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}

typedef void(__cdecl *MsgFn)(char const* pMsg, va_list);
void Utils::Msg(char * msg, ...)
{
	if (!msg)
		return;
	static MsgFn oMsg = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg");
	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	oMsg(buffer, list);
}

void Utils::Warning(char* msg, ...)
{
	if (!msg)
		return;
	static MsgFn oMsg = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Warning");
	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	oMsg(buffer, list);
}

void Utils::Error(char* msg, ...)
{
	if (!msg)
		return;
	static MsgFn oMsg = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Error");
	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	oMsg(buffer, list);
}


Vector Utils::GetHitboxPosition(int Hitbox, CBaseEntity* ent)
{
	CBaseEntity* target = (CBaseEntity*)ent;
	void* model = target->GetModel();
	if (!model)
		return Vector(0, 0, 0);

	studiohdr_t* hdr = g_pModelInfo->GetStudioModel(model);
	if (!hdr)
		return Vector(0, 0, 0);

	VMatrix matrix[128];
	if (!target->SetupBones((matrix3x4_t*)matrix, 128, 0x100, 0))
		return Vector(0, 0, 0);

	mstudiohitboxset_t *set = hdr->pHitboxSet(target->GetHitboxSet());
	if (!set)
		return Vector(0, 0, 0);

	mstudiobbox_t* box = set->pHitbox(Hitbox);

	if (!box)
		return Vector(0, 0, 0);

	Vector center = ((box->bbmin + box->bbmax) * .5f);
	Vector hitboxpos;
	VectorTransform(center, matrix[box->bone], hitboxpos);

	return hitboxpos;
}

trace_t Utils::TraceRay(Vector start, Vector end, CBaseEntity* skipent)
{
	if (!start.IsValid() || !end.IsValid() || !(CBaseEntity*)skipent)
		return trace_t();
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = (CBaseEntity*)skipent;

	ray.Init(start, end);
	g_pTrace->TraceRay(ray, 0x4600400B, &filter, &tr);
	return tr;
}

bool Utils::IsVisible(Vector start, Vector end, CBaseEntity* skipent, CBaseEntity* target)
{
	trace_t tr = TraceRay(start, end, skipent);
	return tr.m_pEnt == target;
}