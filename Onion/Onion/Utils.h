#pragma once
#include "Vector.h"
#include "interfaces\InterfaceEngine.h"
#include "interfaces\InterfaceTrace.h"

struct Color {
	int r, g, b, a;
	Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a)
	{

	}
};
class CBaseEntity;
class Utils {
public:
	static void InitConsole(char* name);
	static void Msg(char*, ...);
	static void Warning(char*, ...);
	static void Error(char*, ...);
	static Vector GetHitboxPosition(int Hitbox, CBaseEntity* ent);
	static trace_t TraceRay(Vector start, Vector end, CBaseEntity* skipent);
	static bool IsVisible(Vector start, Vector end, CBaseEntity* skipent, CBaseEntity* target);
private:
	__forceinline static float DotProduct(const Vector v1, const float* v2)
	{
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	}

	static void VectorTransform(const Vector in1, const VMatrix& in2, Vector& out)
	{
		out[0] = DotProduct(in1, in2[0]) + in2[0][3];
		out[1] = DotProduct(in1, in2[1]) + in2[1][3];
		out[2] = DotProduct(in1, in2[2]) + in2[2][3];
	}
};
