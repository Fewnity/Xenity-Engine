# RigidBody

Add this in your code:
```cpp
#include <engine/physics/rigidbody.h>
```

## Description

Component to add physics to the GameObject.

## Public Methods

---
### GetVelocity
Get the velocity.
```cpp
const Vector3& GetVelocity() const
```

---
### SetVelocity
Set the velocity.

Parameters:
- `velocity`: New velocity
```cpp
void SetVelocity(const Vector3& velocity)
```

---
### GetTorque
Get the torque applied to the rigidbody.
```cpp
Vector3 GetTorque() const
```

---
### ApplyTorque
Apply torque.

Parameters:
- `torque`: Torque to apply
```cpp
void ApplyTorque(const Vector3& torque)
```

---
### GetAngularVelocity
Get the angular velocity.
```cpp
Vector3 GetAngularVelocity() const
```

---
### SetAngularVelocity
Set the angular velocity.

Parameters:
- `angularVelocity`: Angular velocity to set
```cpp
void SetAngularVelocity(const Vector3& angularVelocity)
```

---
### AddAngularVelocity
Add angular velocity.

Parameters:
- `angularVelocity`: Angular velocity to add
```cpp
void AddAngularVelocity(const Vector3& angularVelocity)
```

---
### GetDrag
Get the drag value.
```cpp
float GetDrag() const
```

---
### SetDrag
Set the drag value.

Parameters:
- `drag`: Drag value to set
```cpp
void SetDrag(float drag)
```

---
### GetAngularDrag
Get the angular drag value.
```cpp
float GetAngularDrag() const
```

---
### SetAngularDrag
Set the angular drag value.

Parameters:
- `angularDrag`: Angular drag value to set
```cpp
void SetAngularDrag(float angularDrag)
```

---
### GetBounce
Get the bounce value.
```cpp
float GetBounce() const
```

---
### SetBounce
Set the bounce value.

Parameters:
- `bounce`: Bounce value to set
```cpp
void SetBounce(float bounce)
```

---
### GetGravityMultiplier
Get the gravity multiplier.
```cpp
float GetGravityMultiplier() const
```

---
### SetGravityMultiplier
Set the gravity multiplier.

Parameters:
- `gravityMultiplier`: Gravity multiplier value to set
```cpp
void SetGravityMultiplier(float gravityMultiplier)
```

---
### IsStatic
Get if the rigidbody is static.
```cpp
float IsStatic() const
```

---
### SetIsStatic
Set if the rigidbody is static.

Parameters:
- `isStatic`: Is static value to set
```cpp
void SetIsStatic(float isStatic)
```

---
### GetMass
Get the mass of the rigidbody.
```cpp
float GetMass() const
```

---
### SetMass
Set the mass of the rigidbody.

Parameters:
- `mass`: Mass value to set
```cpp
void SetMass(float mass)
```

---
### GetFriction
Get the friction value.
```cpp
float GetFriction() const
```

---
### SetFriction
Set the friction value.

Parameters:
- `friction`: Friction value to set
```cpp
void SetFriction(float friction)
```

---
### Activate
Activate the rigidbody (used to wake up the rigidbody if it was sleeping).
```cpp
void Activate()
```

---
### GetLockedMovementAxis
Get locked movement axis.
```cpp
const LockedAxis& GetLockedMovementAxis() const
```

---
### SetLockedMovementAxis
Set locked movement axis.

Parameters:
- `axis`: Axis to lock
```cpp
void SetLockedMovementAxis(LockedAxis axis)
```

---
### GetLockedMovementAxis
Get locked rotation axis.
```cpp
const LockedAxis& GetLockedRotationAxis() const
```

---
### SetLockedMovementAxis
Set locked rotation axis.

Parameters:
- `axis`: Axis to lock
```cpp
void SetLockedMovementAxis(LockedAxis axis)
```