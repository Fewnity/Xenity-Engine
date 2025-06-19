# ParticleSystem

Add this in your code:
```cpp
#include <engine/particle_system/particle_system.h>
```

## Description

Component to spawn particles

## Public methods

---
### Play
Emitte all particles when not in loop mode.
```cpp
void Play()
```

---
### ResetParticles
Remove all particles and restart the emission.
```cpp
void ResetParticles()
```

---
### IsEmitting
Get if the particle system is emitting particles.
```cpp
bool IsEmitting() const
```

---
### SetIsEmitting
Set if the particle system is emitting particles.

Parameters:
- `isEmitting`: Light color
```cpp
void SetIsEmitting(bool isEmitting)
```

---
### GetSpawnRate
Get the particle spawn rate.
```cpp
float GetSpawnRate() const
```

---
### SetIsEmitting
Set the particle spawn rate.

Parameters:
- `spawnRate`: Number of particules per seconds
```cpp
void SetSpawnRate(float spawnRate)
```

---
### SetScaleOverLifeTimeFunction
Set you own function to set the global scale of the particle over its life time.<br>
The function takes the life time ratio [0;1] and return the scale of the particle.

Parameters:
- `function`: function The function to set, should be static (nullptr to use the default function)
```cpp
void SetScaleOverLifeTimeFunction(float (*function)(float))
```
Code sample:
```cpp
float GetScaleOverLifeTime(float lifeTime)
{
	return std::sin(lifeTime * Math::PI); // Default function in every particle systems
}

std::shared_ptr<ParticleSystem> particleSystem;
particleSystem->SetScaleOverLifeTimeFunction(&GetScaleOverLifeTime);
```

---
### SetSpeedMultiplierOverLifeTimeFunction
Set you own function to set the speed multiplier of the particle over its life time.<br>
The function takes the life time ratio [0;1] and return the speed multiplier of the particle.

Parameters:
- `function`: function The function to set, should be static (nullptr to use the default function)
```cpp
void SetSpeedMultiplierOverLifeTimeFunction(float (*function)(float))
```
Code sample:
```cpp
float GetSpeedOverLifeTime(float lifeTime)
{
	return 1; // Default function in every particle systems
}

std::shared_ptr<ParticleSystem> particleSystem;
particleSystem->SetScaleOverLifeTimeFunction(&GetSpeedOverLifeTime);
```

---
### SetColorOverLifeTimeFunction
Set you own function to set the color of the particle over its life time.<br>
The function takes the life time ratio [0;1] and return the color of the particle.<br>
x = red, y = green, z = blue, w = alpha (0 to 1).

Parameters:
- `function`: function The function to set, should be static (nullptr to use the default function)
```cpp
void SetColorOverLifeTimeFunction(Vector4 (*function)(float))
```
Code sample:
```cpp
Vector4 GetColorOverLifeTime(float lifeTime)
{
	return Vector4(1, 1, 1, std::sin(lifeTime * Math::PI)); // Default function in every particle systems
}

std::shared_ptr<ParticleSystem> particleSystem;
particleSystem->SetScaleOverLifeTimeFunction(&GetColorOverLifeTime);
```