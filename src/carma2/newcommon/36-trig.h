#ifndef GUARD_36_TRIG_H
#define GUARD_36_TRIG_H

#include "brender/brender.h"
#include "c2_hooks.h"

extern float gFloat_sine_table[91];

// FastFloatSin

// FastFloatCos

// FastScalarSin

// FastScalarCos

extern br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle);

// FastScalarCosAngle

extern float C2_HOOK_STDCALL FastFloatArcSin(float pValue);

extern float C2_HOOK_STDCALL FastFloatArcCos(float pValue);

extern br_scalar C2_HOOK_STDCALL FastScalarArcSin(br_scalar pValue);

extern br_scalar C2_HOOK_STDCALL FastScalarArcCos(br_scalar pValue);

extern float C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX);

extern br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX);

extern br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX);

extern br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX);

extern void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx);

extern void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry);

extern void C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a);

extern void C2_HOOK_FASTCALL DRMatrix34PostRotateX(br_matrix34* mat, br_angle rx);

extern void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry);

extern void C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix34PostRotateZ(br_matrix34* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix34PreRotate(br_matrix34* mat, br_angle r, br_vector3* axis);

extern void C2_HOOK_FASTCALL DRMatrix34PostRotate(br_matrix34* mat, br_angle r, br_vector3* axis);

extern void C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz);

extern void C2_HOOK_FASTCALL DRMatrix23PostRotate(br_matrix23* mat, br_angle rz);

#endif // GUARD_36_TRIG_H
