#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

class API Noise
{
public:

	/**
	* Get 1D noise
	*/
	static float noise1(float x);

	/**
	* Get 2D noise
	*/
	static float noise2(float x, float y);

	/**
	* Get 3D noise
	*/
	static float noise3(float x, float y, float z);

	/**
	* Get 4D noise
	*/
	static float noise4(float x, float y, float z, float w);
private:
	static float grad1(int hash, float x);
	static float grad2(int hash, float x, float y);
	static float grad3(int hash, float x, float y, float z);
	static float grad4(int hash, float x, float y, float z, float t);
	static float pnoise1(float x, int px);
	static float pnoise2(float x, float y, int px, int py);
	static float pnoise3(float x, float y, float z, int px, int py, int pz);
	static float pnoise4(float x, float y, float z, float w, int px, int py, int pz, int pw);
};