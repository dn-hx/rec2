#include "36-trig.h"

#include "c2_math.h"
#include "rec2_macros.h"

// The trig routines below address gFloat_sine_table through folded bases:
// table + 90/180/270/360 floats for positive angles and table - 90/180/270 for
// negative ones. reccmp resolves such a folded address to whatever symbol
// covers it, so the layout here keeps them resolving as in the original image:
//  - gFloat_sine_table_270/360 below sit exactly at table + 270*4 / + 360*4
//    and are annotated with the original folded addresses;
//  - gFloat_sine_table starts directly at this file's data so that the three
//    negative folded bases fall inside the neighbouring string literals, which
//    reccmp does not name (as in the original image). gTrig_layout_pad below
//    keeps the following translation units' data where it was.
// GLOBAL: CARMA2_HW 0x00661398
// GLOBAL: CARMA2_SW 0x0065bcf0
float gFloat_sine_table[91] = {
    0.0f,
    0.017452f,
    0.034899f,
    0.052336f,
    0.069756f,
    0.087156f,
    0.104528f,
    0.121869f,
    0.139173f,
    0.156434f,
    0.173648f,
    0.190809f,
    0.207912f,
    0.224951f,
    0.241922f,
    0.258819f,
    0.275637f,
    0.292372f,
    0.309017f,
    0.325568f,
    0.342020f,
    0.358368f,
    0.374607f,
    0.390731f,
    0.406737f,
    0.422618f,
    0.438371f,
    0.453990f,
    0.469472f,
    0.484810f,
    0.5f,
    0.51503801f,
    0.52991903f,
    0.54463899f,
    0.55919302f,
    0.57357597f,
    0.58778501f,
    0.60181499f,
    0.61566103f,
    0.62932003f,
    0.64278799f,
    0.65605903f,
    0.66913098f,
    0.68199801f,
    0.69465798f,
    0.70710701f,
    0.71934003f,
    0.731354f,
    0.74314499f,
    0.75471002f,
    0.76604402f,
    0.77714598f,
    0.78801101f,
    0.79863602f,
    0.809017f,
    0.819152f,
    0.82903802f,
    0.83867103f,
    0.84804797f,
    0.85716701f,
    0.86602497f,
    0.87462002f,
    0.88294798f,
    0.89100701f,
    0.898794f,
    0.906308f,
    0.91354501f,
    0.92050499f,
    0.92718399f,
    0.93357998f,
    0.93969297f,
    0.94551897f,
    0.95105702f,
    0.95630503f,
    0.96126199f,
    0.96592599f,
    0.97029603f,
    0.97437f,
    0.97814798f,
    0.98162699f,
    0.98480803f,
    0.987688f,
    0.99026799f,
    0.99254602f,
    0.99452198f,
    0.99619502f,
    0.99756402f,
    0.99862999f,
    0.99939102f,
    0.99984801f,
    1.0f,
};

// GLOBAL: CARMA2_HW 0x00661508
br_fixed_ls gFixed_sine_table[91] = {
    0x0, 0x477, 0x8ef, 0xd65, 0x11db, 0x164f, 0x1ac2, 0x1f32,
    0x23a0, 0x280c, 0x2c74, 0x30d8, 0x3539, 0x3996, 0x3dee, 0x4241,
    0x4690, 0x4ad8, 0x4f1b, 0x5358, 0x578e, 0x5bbe, 0x5fe6, 0x6406,
    0x681f, 0x6c30, 0x7039, 0x7438, 0x782f, 0x7c1c, 0x7fff, 0x83d9,
    0x87a8, 0x8b6d, 0x8f27, 0x92d5, 0x9679, 0x9a10, 0x9d9b, 0xa11b,
    0xa48d, 0xa7f3, 0xab4c, 0xae97, 0xb1d5, 0xb504, 0xb826, 0xbb39,
    0xbe3e, 0xc134, 0xc41b, 0xc6f3, 0xc9bb, 0xcc73, 0xcf1b, 0xd1b3,
    0xd43b, 0xd6b3, 0xd919, 0xdb6f, 0xddb3, 0xdfe7, 0xe208, 0xe419,
    0xe617, 0xe803, 0xe9de, 0xeba6, 0xed5b, 0xeeff, 0xf08f, 0xf20d,
    0xf378, 0xf4d0, 0xf615, 0xf746, 0xf865, 0xf970, 0xfa67, 0xfb4b,
    0xfc1c, 0xfcd9, 0xfd82, 0xfe17, 0xfe98, 0xff06, 0xff60, 0xffa6,
    0xffd8, 0xfff6, 0x10000,
};

// Address anchors for the positive folded bases (see the comment above).
// They are only used as the base of `table - angle` style expressions, so the
// values they read live in gFloat_sine_table itself; the padding keeps them at
// table + 270*4 and table + 360*4 respectively.
char gFloat_sine_table_pad270[344] = { 1 };

// GLOBAL: CARMA2_HW 0x006617d0
float gFloat_sine_table_270[1] = { 1.0f };

char gFloat_sine_table_pad360[352] = { 1 };

// GLOBAL: CARMA2_HW 0x00661938
float gFloat_sine_table_360[1] = { 1.0f };

// Layout filler: compensates for the removed sine-table anchor so that the
// data of the translation units following this one keeps its addresses.
char gTrig_layout_pad[8] = { 1 };


// GLOBAL: CARMA2_HW 0x006ab950
br_matrix23 mat23tmp1;

// GLOBAL: CARMA2_HW 0x006ab968
br_matrix23 mat23tmp2;

// GLOBAL: CARMA2_HW 0x006ab980
br_matrix34 mattmp1__trig;

// GLOBAL: CARMA2_HW 0x006ab9b0
br_matrix34 mattmp2__trig;

// FastFloatSin

// FastFloatCos

// FastScalarSin

// FastScalarCos

// FUNCTION: CARMA2_HW 0x00511780
br_scalar C2_HOOK_FASTCALL FastScalarSinAngle(br_angle pBR_angle) {
    int angle_in_degrees;

    angle_in_degrees = (int)((double)pBR_angle * (360.0 / 65536.0));
    if (angle_in_degrees < 0) {
        angle_in_degrees = 360 - -angle_in_degrees % 360;
    } else {
        angle_in_degrees = angle_in_degrees % 360;
    }
    if (angle_in_degrees > 270) {
        return -*(gFloat_sine_table_360 - angle_in_degrees);
    }
    if (angle_in_degrees > 180) {
        return -gFloat_sine_table[angle_in_degrees - 180];
    }
    if (angle_in_degrees > 90) {
        return *(gFloat_sine_table + 180 - angle_in_degrees);
    }
    return gFloat_sine_table[angle_in_degrees];
}

// FastScalarCosAngle

// FUNCTION: CARMA2_HW 0x005119c0
// FUNCTION: CARMA2_SW 0x0050ce10
float C2_HOOK_STDCALL FastFloatArcSin(float pValue) {
    float low_limit;
    float high_limit;
    float mid_point;

    C2_HOOK_BUG_ON(REC2_ASIZE(gFloat_sine_table) != 91);

    if (pValue < 0.f) {
        return -FastFloatArcSin(-pValue);
    }
    high_limit = 90.f * pValue;
    low_limit = high_limit - 19.f;
    if (low_limit < 0.f) {
        low_limit = 0.f;
    }
    while (high_limit - low_limit >= 1.f) {
        mid_point = (low_limit + high_limit) / 2.f;

        if (gFloat_sine_table[(int)mid_point] > pValue) {
            high_limit = mid_point;
        } else {
            low_limit = mid_point;
        }
    }
    return low_limit;
}

// FUNCTION: CARMA2_HW 0x00511a80
float C2_HOOK_STDCALL FastFloatArcCos(float pValue) {

    return 90.f - FastFloatArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511b50
br_scalar C2_HOOK_STDCALL FastScalarArcSin(br_scalar pValue) {

    return FastFloatArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511c10
br_scalar C2_HOOK_STDCALL FastScalarArcCos(br_scalar pValue) {

    return 90.f - FastScalarArcSin(pValue);
}

// FUNCTION: CARMA2_HW 0x00511ce0
float C2_HOOK_STDCALL FastFloatArcTan2(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = (float)fabs(pX);
    abs_y = (float)fabs(pY);
    if (pX == 0.f) {
        if (pY < 0.f) {
            return 270.f;
        }
        if (pY > 0.f) {
            return 90.f;
        }
        return 0.f;
    }
    if (pX < 0.f) {
        if (pY < 0.f) {
            if (abs_x < abs_y) {
                return (float)((6.f - abs_x / abs_y) * 45.0);
            }
            return (float)((abs_y / abs_x + 4.f) * 45.0);
        }
        if (abs_x < abs_y) {
            return (float)((abs_x / abs_y + 2.f) * 45.0);
        }
        return (float)((4.f - abs_y / abs_x) * 45.0);
    }
    if (pY < 0.f) {
        if (abs_x < abs_y) {
            return (float)((abs_x / abs_y + 6.f) * 45.0);
        }
        return (float)((8.f - abs_y / abs_x) * 45.0);
    }
    if (abs_x < abs_y) {
        return (float)((2.f - abs_x / abs_y) * 45.0);
    }
    return (float)(abs_y / abs_x * 45.0);
}

// FUNCTION: CARMA2_HW 0x00511e50
br_scalar C2_HOOK_STDCALL FastScalarArcTan2(br_scalar pY, br_scalar pX) {
    return FastFloatArcTan2(pY, pX);
}

// FUNCTION: CARMA2_HW 0x00511e70
br_angle C2_HOOK_STDCALL FastFloatArcTan2Angle(float pY, float pX) {
    float abs_x;
    float abs_y;

    abs_x = (float)fabs(pX);
    abs_y = (float)fabs(pY);
    if (pX == 0.f) {
        if (pY < 0.f) {
            return BR_ANGLE_DEG(270);
        }
        if (pY > 0.f) {
            return BR_ANGLE_DEG(90);
        }
        return 0;
    }
    if (pX < 0.f) {
        if (pY < 0.f) {
            if (abs_x < abs_y) {
                return (br_angle)((6.f - abs_x / abs_y) * 8192.f);
            }
            return (br_angle)((abs_y / abs_x + 4.f) * 8192.f);
        }
        if (abs_x < abs_y) {
            return (br_angle)((abs_x / abs_y + 2.f) * 8192.f);
        }
        return (br_angle)((4.f - abs_y / abs_x) * 8192.f);
    }
    if (pY < 0.f) {
        if (abs_x < abs_y) {
            return (br_angle)((abs_x / abs_y + 6.f) * 8192.f);
        }
        return (br_angle)((8.f - abs_y / abs_x) * 8192.f);
    }
    if (abs_x < abs_y) {
        return (br_angle)((2.f - abs_x / abs_y) * 8192.f);
    }
    return (br_angle)(abs_y / abs_x * 8192.f);
}

// FUNCTION: CARMA2_HW 0x00512010
br_angle C2_HOOK_STDCALL FastScalarArcTan2Angle(br_scalar pY, br_scalar pX) {
    return FastFloatArcTan2Angle(pY, pX);
}

// FUNCTION: CARMA2_HW 0x00512030
void C2_HOOK_FASTCALL DRMatrix34RotateX(br_matrix34* mat, br_angle rx) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rx * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat->m[0][0] = 1.0f;
    mat->m[0][1] = 0.0f;
    mat->m[0][2] = 0.0f;
    mat->m[1][0] = 0.0f;
    mat->m[1][1] = c;
    mat->m[1][2] = s;
    mat->m[2][0] = 0.0f;
    mat->m[2][1] = -s;
    mat->m[2][2] = c;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x00512170
void C2_HOOK_FASTCALL DRMatrix34RotateY(br_matrix34* mat, br_angle ry) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)ry * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat->m[0][0] = c;
    mat->m[0][1] = 0.0f;
    mat->m[0][2] = -s;
    mat->m[1][0] = 0.0f;
    mat->m[1][1] = 1.0f;
    mat->m[1][2] = 0.0f;
    mat->m[2][0] = s;
    mat->m[2][1] = 0.0f;
    mat->m[2][2] = c;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x005122b0
void C2_HOOK_FASTCALL DRMatrix34RotateZ(br_matrix34* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat->m[0][0] = c;
    mat->m[0][1] = s;
    mat->m[0][2] = 0.0f;
    mat->m[1][0] = -s;
    mat->m[1][1] = c;
    mat->m[1][2] = 0.0f;
    mat->m[2][0] = 0.0f;
    mat->m[2][1] = 0.0f;
    mat->m[2][2] = 1.0f;
    mat->m[3][0] = 0.0f;
    mat->m[3][1] = 0.0f;
    mat->m[3][2] = 0.0f;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x005123f0
void C2_HOOK_FASTCALL DRMatrix34Rotate(br_matrix34* mat, br_angle r, br_vector3* a) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x005127b0
void C2_HOOK_FASTCALL DRMatrix34PostRotateX(br_matrix34* mat, br_angle rx) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rx * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mattmp2__trig.m[0][0] = 1.0f;
    mattmp2__trig.m[0][1] = 0.0f;
    mattmp2__trig.m[0][2] = 0.0f;
    mattmp2__trig.m[1][0] = 0.0f;
    mattmp2__trig.m[1][1] = c;
    mattmp2__trig.m[1][2] = s;
    mattmp2__trig.m[2][0] = 0.0f;
    mattmp2__trig.m[2][1] = -s;
    mattmp2__trig.m[2][2] = c;
    mattmp2__trig.m[3][0] = 0.0f;
    mattmp2__trig.m[3][1] = 0.0f;
    mattmp2__trig.m[3][2] = 0.0f;
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512ad0
// FUNCTION: CARMA2_SW 0x0050df10
void C2_HOOK_FASTCALL DRMatrix34PostRotateY(br_matrix34* mat, br_angle ry) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)ry * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mattmp2__trig.m[0][0] = c;
    mattmp2__trig.m[0][1] = 0.0f;
    mattmp2__trig.m[0][2] = -s;
    mattmp2__trig.m[1][0] = 0.0f;
    mattmp2__trig.m[1][1] = 1.0f;
    mattmp2__trig.m[1][2] = 0.0f;
    mattmp2__trig.m[2][0] = s;
    mattmp2__trig.m[2][1] = 0.0f;
    mattmp2__trig.m[2][2] = c;
    mattmp2__trig.m[3][0] = 0.0f;
    mattmp2__trig.m[3][1] = 0.0f;
    mattmp2__trig.m[3][2] = 0.0f;
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512c60
void C2_HOOK_FASTCALL DRMatrix34PreRotateZ(br_matrix34* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mattmp2__trig.m[0][0] = c;
    mattmp2__trig.m[0][1] = s;
    mattmp2__trig.m[0][2] = 0.0f;
    mattmp2__trig.m[1][0] = -s;
    mattmp2__trig.m[1][1] = c;
    mattmp2__trig.m[1][2] = 0.0f;
    mattmp2__trig.m[2][0] = 0.0f;
    mattmp2__trig.m[2][1] = 0.0f;
    mattmp2__trig.m[2][2] = 1.0f;
    mattmp2__trig.m[3][0] = 0.0f;
    mattmp2__trig.m[3][1] = 0.0f;
    mattmp2__trig.m[3][2] = 0.0f;
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512df0
void C2_HOOK_FASTCALL DRMatrix34PostRotateZ(br_matrix34* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mattmp2__trig.m[0][0] = c;
    mattmp2__trig.m[0][1] = s;
    mattmp2__trig.m[0][2] = 0.0f;
    mattmp2__trig.m[1][0] = -s;
    mattmp2__trig.m[1][1] = c;
    mattmp2__trig.m[1][2] = 0.0f;
    mattmp2__trig.m[2][0] = 0.0f;
    mattmp2__trig.m[2][1] = 0.0f;
    mattmp2__trig.m[2][2] = 1.0f;
    mattmp2__trig.m[3][0] = 0.0f;
    mattmp2__trig.m[3][1] = 0.0f;
    mattmp2__trig.m[3][2] = 0.0f;
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512f80
void C2_HOOK_FASTCALL DRMatrix34PreRotate(br_matrix34* mat, br_angle r, br_vector3* axis) {

    DRMatrix34Rotate(&mattmp2__trig, r, axis);
    BrMatrix34Mul(&mattmp1__trig, &mattmp2__trig, mat);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00512fd0
void C2_HOOK_FASTCALL DRMatrix34PostRotate(br_matrix34* mat, br_angle r, br_vector3* axis) {

    DRMatrix34Rotate(&mattmp2__trig, r, axis);
    BrMatrix34Mul(&mattmp1__trig, mat, &mattmp2__trig);
    BrMatrix34Copy(mat, &mattmp1__trig);
}

// FUNCTION: CARMA2_HW 0x00513020
void C2_HOOK_FASTCALL DRMatrix23Rotate(br_matrix23* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat->m[0][0] = c;
    mat->m[0][1] = s;
    mat->m[1][0] = -s;
    mat->m[1][1] = c;
    mat->m[2][0] = 0.f;
    mat->m[2][1] = 0.f;
}

// FUNCTION: CARMA2_HW 0x005132a0
void C2_HOOK_FASTCALL DRMatrix23PostRotate(br_matrix23* mat, br_angle rz) {
    int angle_in_degrees;
    int angle;
    br_scalar s;
    br_scalar c;

    angle_in_degrees = (int)((double)rz * (360.0 / 65536.0));
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        s = -*(gFloat_sine_table_360 - angle);
    } else if (angle > 180) {
        s = -gFloat_sine_table[angle - 180];
    } else if (angle > 90) {
        s = *(gFloat_sine_table + 180 - angle);
    } else {
        s = gFloat_sine_table[angle];
    }
    angle = angle_in_degrees;
    if (angle < 0) {
        angle = 360 - -angle % 360;
    } else {
        angle = angle % 360;
    }
    if (angle > 270) {
        c = gFloat_sine_table[angle - 270];
    } else if (angle > 180) {
        c = -*(gFloat_sine_table_270 - angle);
    } else if (angle > 90) {
        c = -gFloat_sine_table[angle - 90];
    } else {
        c = *(gFloat_sine_table + 90 - angle);
    }
    mat23tmp2.m[0][0] = c;
    mat23tmp2.m[0][1] = s;
    mat23tmp2.m[1][0] = -s;
    mat23tmp2.m[1][1] = c;
    mat23tmp2.m[2][0] = 0.0f;
    mat23tmp2.m[2][1] = 0.0f;
    BrMatrix23Mul(&mat23tmp1, mat, &mat23tmp2);
    BrMatrix23Copy(mat, &mat23tmp1);
}

