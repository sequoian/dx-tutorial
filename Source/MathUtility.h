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