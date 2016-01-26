#pragma once
#include "Vector.h"
#include <vector>
#include <Windows.h>
#include <random>

#define TIME_BEFORE_ADJUSTING 50
#define TIME_BEFORE_NEW_TARGET 250

class CBaseEntity;

class Aimbot
{
	enum State
	{
		NOTARGET,
		REACHLOCATION,
		AIMTARGET,
	};
public:
	void Think(float elapsedtime);
	bool FindTarget();

	Vector GetRecoil();
	void Aim(Vector targetangles);
	void DropTarget();
	bool ValidTarget(CBaseEntity* ent);
	bool HitboxVisible(int hitboxid, CBaseEntity* ent);
	std::vector<int> VisibleBones(CBaseEntity* ent);

	void SmoothAngle(Vector& from, Vector& to, float percent);

	static Aimbot& GetAimbot()
	{
		static Aimbot bot;
		return bot;
		
	}

private:
	Aimbot();
	Aimbot(const Aimbot&); //do not define
	void operator=(const Aimbot&); //do not define
	Vector m_recoil;
	Vector m_aimPoint;
	Vector m_aimAngles;
	Vector m_velocity;
	Vector m_previousVelocity;

	CBaseEntity* m_pLocalEnt;
	CBaseEntity* m_pTarget;

	State m_State;

	int m_target;
	int m_hitbox;
	int m_bulletCounts;
	DWORD m_timerTick;
	bool m_waitUntilNewAction;

	float m_curaimtime;
	float m_aimtime;
	bool m_newtarget;

	float m_smooth;
	float m_fovlimit;
	int m_bulletCountsAfterKill;
	char m_aimKey, m_altAimKey;

	float RandomFloat(float a, float b)
	{
		std::uniform_real_distribution<float> u(a, b);
		return u(rgen);
	}
	std::random_device rseed;
	std::mt19937 rgen = std::mt19937(rseed());
	std::uniform_int_distribution<int> dist = std::uniform_int_distribution<int>(0,10);

};


#define rand() dist(rgen)