#pragma once

#include <DirectXMath.h>
using namespace std;

const long double PI = 3.14159265358979323846;

inline float RadiansToDegrees(float radians)
{
	return (radians * 180) / PI;
}

inline float operator "" _deg(long double radians)
{
	return RadiansToDegrees(radians);
}

inline float DegreesToRadians(float degrees)
{
	return (degrees * PI) / 180;
}

inline float operator "" _rad(long double degrees)
{
	return DegreesToRadians(degrees);
}

inline float Lerp(float start, float end, float t)
{
	return start + t * (end - start);
}

// =======================================================================
// DirectX Math functions
// =======================================================================

inline XMVECTOR Vector3(float x, float y, float z, float w = 1)
{
	return XMVectorSet(x, y, z, w);
}

inline XMVECTOR Vector3(float xyz, float w = 1)
{
	return XMVectorSet(xyz, xyz, xyz, w);
}

// return identity quaternion
inline XMVECTOR Quaternion()
{
	return XMQuaternionIdentity();
}

inline XMVECTOR Quaternion(float pitch, float yaw, float roll)
{
	return XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
}

inline XMVECTOR ToEulerAngles(XMVECTOR quat)
{
	float pitch, yaw, roll;

	// pitch
	float sinr_cosp = +2.0 * (quat.m128_f32[3] * quat.m128_f32[0] + quat.m128_f32[1] * quat.m128_f32[2]);
	float cosr_cosp = +1.0 - 2.0 * (quat.m128_f32[0] * quat.m128_f32[0] + quat.m128_f32[1] * quat.m128_f32[1]);
	pitch = atan2f(sinr_cosp, cosr_cosp);

	// yaw
	float sinp = +2.0 * (quat.m128_f32[3] * quat.m128_f32[1] - quat.m128_f32[2] * quat.m128_f32[0]);
	if (fabs(sinp) >= 1)
		yaw = copysign(PI / 2, sinp); // use 90 degrees if out of range
	else
		yaw = asin(sinp);

	// roll
	float siny_cosp = +2.0 * (quat.m128_f32[3] * quat.m128_f32[2] + quat.m128_f32[0] * quat.m128_f32[1]);
	float cosy_cosp = +1.0 - 2.0 * (quat.m128_f32[1] * quat.m128_f32[1] + quat.m128_f32[2] * quat.m128_f32[2]);
	roll = atan2(siny_cosp, cosy_cosp);

	return Vector3(pitch, yaw, roll);
}

inline void PrintVector(XMVECTOR v)
{
	DEBUG_PRINT("[%f, %f, %f]", v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]);
}

inline void PrintEulerAngles(XMVECTOR e)
{
	DEBUG_PRINT("pitch: %f, yaw: %f, roll: %f", RadiansToDegrees(e.m128_f32[0]), RadiansToDegrees(e.m128_f32[1]), RadiansToDegrees(e.m128_f32[2]));
}