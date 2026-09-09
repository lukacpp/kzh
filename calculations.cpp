#include "calculations.h"

float GroundDistance()
{
	vec3_t vOrigin = g_Local.vOrigin;
	vec3_t vPoint;

	vPoint[0] = vOrigin[0];
	vPoint[1] = vOrigin[1];
	vPoint[2] = vOrigin[2] - 8192;

	pmtrace_t *trace = g_Engine.PM_TraceLine(vOrigin, vPoint, PM_NORMAL, g_Local.iUseHull, -1);

	return vOrigin[2] - trace->endpos[2];
}

#define VectorAdd(a, b, c) ((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define	VectorScale(v, s, o) ((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))

int AirFrames()
{
	if (g_Local.iFlags & FL_ONGROUND)
		return 0;

	int id = g_Engine.GetLocalPlayer()->index;

	int iFrames = 0;

	vec3_t vTemp, vEnd;

	vec3_t vStart = g_Local.vOrigin;
	vStart[2] -= 2.0;
	vec3_t vVelocity = g_Local.vVelocity;
	int iUseHull = g_Local.iFlags & FL_DUCKING ? 1 : 0;
	VectorScale(vVelocity, g_Local.flFrameTime, vTemp);
	VectorAdd(vStart, vTemp, vEnd);

	float flFraction = 1.0f;
	while (flFraction == 1.0f)
	{
		if (++iFrames > 72)
			break;

		pmtrace_t *trace = g_Engine.PM_TraceLine(vStart, vEnd, 0, iUseHull, id);

		flFraction = trace->fraction;

		vStart = vEnd;

		VectorScale(vVelocity, g_Local.flFrameTime, vTemp);
		VectorAdd(vStart, vTemp, vEnd);

		vVelocity[2] -= 800.f * flFraction * g_Local.flFrameTime;
	}

	return iFrames;
}