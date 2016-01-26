#include "Aimbot.h"
#include "CBaseEntity.h"
#include "interfaces\InterfaceEngine.h"
#include "interfaces\InterfaceEntityList.h"
#include "Utils.h"

Aimbot::Aimbot() :
	m_recoil(0, 0, 0),
	m_aimPoint(0, 0, 0),
	m_aimAngles(0, 0, 0),
	m_velocity(0, 0, 0),
	m_previousVelocity(0, 0, 0),
	m_pLocalEnt(0),
	m_pTarget(0),
	m_State(NOTARGET),
	m_target(-1),
	m_hitbox(-1),
	m_bulletCounts(0),
	m_timerTick(0),
	m_waitUntilNewAction(false),
	m_smooth(1.f),
	m_fovlimit(4.f),
	m_bulletCountsAfterKill(2),
	m_aimKey(VK_LBUTTON),
	m_altAimKey(VK_XBUTTON1),
	m_aimtime(1.2f),
	m_curaimtime(0.f),
	m_newtarget(false)
{

}

bool Aimbot::HitboxVisible(int hitbox, CBaseEntity* ent)
{
	Vector  hitboxpos;
	hitboxpos = Utils::GetHitboxPosition(hitbox, ent);
	return (Utils::IsVisible(g_pEntityList->GetLocalPlayer()->GetEyePosition(), hitboxpos, g_pEntityList->GetLocalPlayer(), ent));
}

std::vector<int> Aimbot::VisibleBones(CBaseEntity* ent)
{
	std::vector<int> retn;
	for (int i = 0; i < 3; i++)
		if (HitboxVisible(i, ent))
			retn.push_back(i);
	return retn;
}

bool Aimbot::FindTarget()
{
	DropTarget();

	float bestfov = m_fovlimit;
	float bestdist = 8192.f;
	Vector viewangles;
	g_pEngine->GetViewAngles(viewangles);

	for (int i = 1; i < 64; i++) {
		CBaseEntity* ent = g_pEntityList->GetClientEntity(i);
		if (!ValidTarget(ent)) continue;
		auto boxes = VisibleBones(ent);
		if (!boxes.size()) continue;
		Vector angdelta = viewangles - (ent->GetAbsOrigin() - m_pLocalEnt->GetAbsOrigin()).Angle().ClampAngle();
		angdelta.y += 180;
		if (angdelta.y > 360) angdelta.y = fmodf(angdelta.y, 360);
		angdelta.y -= 180;
		float fov = angdelta.Length2D();
		if (fov > bestfov) continue;

		float dist = m_pLocalEnt->GetAbsOrigin().DistTo(ent->GetAbsOrigin());

		if (fov < bestfov) {
			m_target = i;
			bestfov = fov;
			bestdist = dist;
			m_pTarget = ent;
		}
	}

	if (m_target && m_pTarget) {
		auto bonelist = VisibleBones(m_pTarget);
		int randthing = rand() % (bonelist.size());
		if (bestdist > 1000.f)
			randthing = 0;
		CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)g_pEntityList->GetClientEntityFromHandle(m_pLocalEnt->GetActiveWeapon());
		if (weapon) {
			int id = *weapon->GetItemDefinitionIndex();
			//https://tf2b.com/itemlist.php?gid=730
			if (id == 7 || id == 16 || id == 60/*AK & M4*/)
			{
				if ((rand()) < 6) //60% chance
					randthing = 0;
			}
			else if (id == 9 /*AWP*/)
			{
				if (HitboxVisible(3, m_pTarget)) {
					bonelist.clear();
					bonelist.push_back(3);
					randthing = 0;
				}
			}
			else if (id == 1 || id == 2 || id == 3 || id == 4 || id == 36 || id == 30 || id == 32 || id == 61 || id == 63 || id == 64) {
				//pistols
				randthing = 0;
				if ((rand()) == 5) {
					if (HitboxVisible(3, m_pTarget)) {
						bonelist.clear();
						bonelist.push_back(3);
						randthing = 0;
					}
				}//1/10 chance to aim for body instead 
			}
		}

		m_hitbox = bonelist[randthing];
		m_aimPoint = Utils::GetHitboxPosition(m_hitbox, m_pTarget);

		m_aimPoint.x += RandomFloat(-13.f, 13.f);
		m_aimPoint.y += RandomFloat(-13.f, 13.f);
		m_aimPoint.z += RandomFloat(-13.f, 13.f);
		m_aimAngles = (m_aimPoint - m_pLocalEnt->GetEyePosition()).Angle().ClampAngle();
		return true;
	}

	return false;
}

Vector Aimbot::GetRecoil()
{
	//return g_Utils.handlercs();
	return m_pLocalEnt->shotsfired() > 1 ? m_pLocalEnt->GetAimPunch() * RandomFloat(1.8f, 1.99f) : Vector(0, 0, 0);
}

void Aimbot::Aim(Vector ang)
{
	Vector currentang;
	g_pEngine->GetViewAngles(currentang);
	//	Vector angdiff = ang.ClampAngle() - currentang;

	//	angdiff = angdiff.ClampAngle();
	SmoothAngle(currentang, ang, m_curaimtime / m_aimtime);

	g_pEngine->SetViewAngles(ang.ClampAngle());
}

bool Aimbot::ValidTarget(CBaseEntity* ent)
{
	player_info_t info;
	if (!ent || ent->GetLifestate() || ent->IsDormant() || ent->GetTeam() == m_pLocalEnt->GetTeam() || !ent->IsPlayer())
		return false;
	return true;
}

void Aimbot::DropTarget()
{
	m_State = NOTARGET;
	m_target = -1;
	m_pTarget = 0;
	m_curaimtime = 0.f;

}

void Aimbot::Think(float elapsedtime)
{

	if (!(GetAsyncKeyState(m_aimKey) & 0x8000) && !(GetAsyncKeyState(m_altAimKey) & 0x8000))
	{
		DropTarget();
		return;
	}

	m_pLocalEnt = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
	if (!m_pLocalEnt)
		return;

	CBaseCombatWeapon* weapon = (CBaseCombatWeapon*)g_pEntityList->GetClientEntityFromHandle(m_pLocalEnt->GetActiveWeapon());
	if (!weapon)
		return;

	if (weapon->GetClip() == 0)
		return;

	if (m_newtarget)
		m_curaimtime = 0;


	if (m_waitUntilNewAction) {
		if (GetTickCount() >= m_timerTick)
		{
			m_waitUntilNewAction = false;
		}
		return;
	}
	m_aimtime = 1.2f;
	m_aimtime += RandomFloat(-0.05, 0.1);
	if (m_State == NOTARGET) {
		if (FindTarget()) {
			m_State = REACHLOCATION;
			if ((rand()) == 8)
			{
				m_State = AIMTARGET; //1/10 chance to skip the random aim spot and go straight to player
			}
			if (m_pLocalEnt->GetAbsOrigin().DistTo(m_pTarget->GetAbsOrigin()) < 150)
			{
				m_State = AIMTARGET; //if theyre close just go to em
			}
			if (*weapon->GetItemDefinitionIndex() == 9 /*AWP*/) {
				if ((rand()) < 9) //2/10 chance to goto the random aimspot
					m_State = AIMTARGET;
			}
		}
	}

	Vector localang;
	g_pEngine->GetViewAngles(localang);

	switch (m_State) {
	case REACHLOCATION:
		m_newtarget = false;
		if (fabs((localang.x + GetRecoil().x) - m_aimAngles.x) <= 1.f || fabs((localang.y + GetRecoil().y) - m_aimAngles.y) <= 1.f) {
			m_waitUntilNewAction = true;
			m_timerTick = GetTickCount() + TIME_BEFORE_ADJUSTING;
			m_State = AIMTARGET;
		}
		break;
	case AIMTARGET:
		m_pTarget = g_pEntityList->GetClientEntity(m_target);
		if (!ValidTarget(m_pTarget) || !HitboxVisible(m_hitbox, m_pTarget)) {
			DropTarget();
			m_waitUntilNewAction = true;
			m_timerTick = GetTickCount() + TIME_BEFORE_NEW_TARGET;
			m_newtarget = true;
			return;
		}

		m_velocity = m_pTarget->GetVelocity();
		//if target changed direction
		if ((m_velocity.x < 0 && m_previousVelocity.x > 0) || (m_velocity.x > 0 && m_velocity.x < 0) ||
			(m_velocity.y < 0 && m_previousVelocity.y > 0) || (m_velocity.y > 0 && m_velocity.y < 0)) {
			m_waitUntilNewAction = true;
			m_timerTick = GetTickCount() + TIME_BEFORE_ADJUSTING;
		}
		m_previousVelocity = m_velocity;

		m_aimPoint = Utils::GetHitboxPosition(m_hitbox, m_pTarget);
		m_aimAngles = (m_aimPoint - m_pLocalEnt->GetEyePosition()).Angle().ClampAngle();
	default:
		break;
	}

	m_curaimtime += elapsedtime;
	if (m_curaimtime > m_aimtime)
		m_curaimtime = m_aimtime;
	if (m_State > NOTARGET && m_pTarget != m_pLocalEnt)
		Aim(m_aimAngles - GetRecoil());
}

void Aimbot::SmoothAngle(Vector& from, Vector& to, float percent)
{
	Vector delta = from - to;
	delta = delta.ClampAngle();
	delta.x *= percent;
	delta.y *= percent;
	to = from - delta;
}