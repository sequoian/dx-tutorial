#pragma once

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