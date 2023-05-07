#pragma once

#include <time.h>
class Noise 
{
public:
static float grad1(int hash, float x);
static float grad2(int hash, float x, float y);
static float grad3(int hash, float x, float y, float z);
static float grad4(int hash, float x, float y, float z, float t);
static float noise1(float x);
static float pnoise1(float x, int px);
static float noise2(float x, float y);
static float pnoise2(float x, float y, int px, int py);
static float noise3(float x, float y, float z);
static float pnoise3(float x, float y, float z, int px, int py, int pz);
static float noise4(float x, float y, float z, float w);
static float pnoise4(float x, float y, float z, float w, int px, int py, int pz, int pw);
};