#pragma once
#include "BaseInterface.h"


class CGlobals extends BaseInterface
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float unk;
	float curtime;
	float frametime;
	int maxClients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int simTicksThisFrame;
	int network_protocol;
};

extern CGlobals* g_pGlobals;