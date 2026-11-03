#include "light24.h"

#include "lightmac.h"
#include "setup.h"

// FUNCTION: CARMA2_HW 0x00548c10
void C2_HOOK_CDECL SurfaceColourZero(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_R] = self->state.cache.comp_offsets[C_R];
    comp[C_G] = self->state.cache.comp_offsets[C_G];
    comp[C_B] = self->state.cache.comp_offsets[C_B];
}

// FUNCTION: CARMA2_HW 0x00548c40
void C2_HOOK_CDECL SurfaceColourUnlit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_R] = self->state.cache.comp_scales[C_R] * (float)BR_RED(colour) / 256.f + self->state.cache.comp_offsets[C_R];
    comp[C_G] = self->state.cache.comp_scales[C_G] * (float)BR_GRN(colour) / 256.f + self->state.cache.comp_offsets[C_G];
    comp[C_B] = self->state.cache.comp_scales[C_B] * (float)BR_BLU(colour) / 256.f + self->state.cache.comp_offsets[C_B];
}

// FUNCTION: CARMA2_HW 0x00548cf0
void C2_HOOK_CDECL SurfaceColourLit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
    int i;
    active_light* alp;
    br_vector3 vp;
    br_vector3 vn;
    br_vector3 fvn;
    br_scalar red, green, blue;

    red = BrFixedToScalar(BR_RED(colour) << 8);
    green = BrFixedToScalar(BR_GRN(colour) << 8);
    blue = BrFixedToScalar(BR_BLU(colour) << 8);

    comp[C_R] = self->state.surface.ka * red;
    comp[C_G] = self->state.surface.ka * green;
    comp[C_B] = self->state.surface.ka * blue;

    BrVector3Copy(&rend.eye_l, &scache.eye_m_normalised);

    alp = scache.lights;

    for (i = 0; i < scache.nlights_model; i++, alp++) {
        alp->accumulate_colour(self, p, n, colour, alp, comp);
    }

    if (scache.nlights_view != 0) {
        BrMatrix34ApplyP(&vp, p, &self->state.matrix.model_to_view);
        BrMatrix34TApplyV(&vn, n, &scache.view_to_model);
        BrVector3Normalise(&fvn, &vn);

        BrVector3Set(&rend.eye_l, 0.f, 0.f, 1.f);

        for (i = 0; i < scache.nlights_view; i++, alp++) {
            alp->accumulate_colour(self, &vp, &fvn, colour, alp, comp);
        }
    }

    CLAMP_SCALE(C_R);
    CLAMP_SCALE(C_G);
    CLAMP_SCALE(C_B);
}

// FUNCTION: CARMA2_HW 0x005491a0
void C2_HOOK_STDCALL lightingColourNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

}

// PARTIAL 76.41%: lightingColourDirectSpecular 0x5491b0 - frame is 8 bytes vs 0x18 (MSVC gives the three unsigned->float temps one slot instead of three) plus x87 scheduling tie-breaks (kd product vs channel conversion order, power load vs dot duplicate)
// FUNCTION: CARMA2_HW 0x005491b0
void C2_HOOK_STDCALL lightingColourDirectSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
    br_scalar dot;
    br_scalar red, grn, blu;

    dot = BrVector3Dot(&alp->direction, n);
    if (dot <= 0.f) {
        return;
    }

    red = BrFixedToScalar(BR_RED(colour) << 8) * (dot * self->state.surface.kd);
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * (dot * self->state.surface.kd);
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * (dot * self->state.surface.kd);

    if (self->state.surface.ks != 0.f) {
        dot = BrVector3Dot(&alp->half, n);
        if (dot > SPECULARPOW_CUTOFF) {
            red += SPECULAR_POWER(self->state.surface.ks * alp->intensity);
            grn += SPECULAR_POWER(self->state.surface.ks * alp->intensity);
            blu += SPECULAR_POWER(self->state.surface.ks * alp->intensity);
        }
    }

    comp[C_R] += red * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
}

// FUNCTION: CARMA2_HW 0x00549380
void C2_HOOK_STDCALL lightingColourLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
    br_scalar dot;
    br_scalar dist2;
	br_vector3 dirn;

    BrVector3Sub(&dirn, &alp->position, p);
    dist2 = BrVector3LengthSquared(&dirn);
    if (dist2 <= alp->s->attenuation_q && dist2 >= BR_SCALAR_EPSILON) {
        dot = BrVector3Dot(&dirn, n);
        if (dot >= 0.f) {
            br_scalar attn;

            attn = self->state.surface.kd * dot * (alp->s->attenuation_q - dist2) / (alp->s->attenuation_q * (br_scalar)sqrt(dist2));
            comp[C_R] += attn * BrFixedToFloat(BR_RED(colour)) * BR_RED(alp->s->colour);
            comp[C_G] += attn * BrFixedToFloat(BR_GRN(colour)) * BR_GRN(alp->s->colour);
            comp[C_B] += attn * BrFixedToFloat(BR_BLU(colour)) * BR_BLU(alp->s->colour);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00549510
void C2_HOOK_STDCALL lightingColourLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
    br_scalar dist2;
	br_vector3 dirn;

    BrVector3Sub(&dirn, &alp->position, p);
    dist2 = BrVector3LengthSquared(&dirn);
    if (dist2 <= alp->s->attenuation_q && dist2 >= BR_SCALAR_EPSILON) {
        br_scalar attn;

        attn = self->state.surface.kd * (alp->s->attenuation_q - dist2) / alp->s->attenuation_q ;
        comp[C_R] += attn * BrFixedToFloat(BR_RED(colour)) * BR_RED(alp->s->colour);
        comp[C_G] += attn * BrFixedToFloat(BR_GRN(colour)) * BR_GRN(alp->s->colour);
        comp[C_B] += attn * BrFixedToFloat(BR_BLU(colour)) * BR_BLU(alp->s->colour);
    }
}

// FUNCTION: CARMA2_HW 0x00549630
void C2_HOOK_STDCALL lightingColourPointSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
    br_scalar dot;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    BrVector3Normalise(&dirn, &dirn);

    dot = BrVector3Dot(&dirn, n);
    if (dot <= 0.f) {
        return;
    }
    red = BrFixedToScalar(BR_RED(colour) << 8) * self->state.surface.kd * dot;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * self->state.surface.kd * dot;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * self->state.surface.kd * dot;

    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp , &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += red * alp->intensity * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * alp->intensity * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * alp->intensity * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
}

// PARTIAL 67.63%: lightingColourPointAttnSpecular 0x5498f0 - frame and instruction count match (0x14/246 vs 241) but x87 scheduling differs: ours leaves attn in a register and reloads dot from its home slot, the original spills attn to [esp+0x2c] before the dot test and keeps dot live in st(0) (fcom+fst), which also gives it the shared dot*kd product (fmulp st(4),st)
// FUNCTION: CARMA2_HW 0x005498f0
void C2_HOOK_STDCALL lightingColourPointAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
    br_scalar dot;
    br_scalar dist, dist2;
    br_scalar attn;
    br_vector3 dirn;
    br_scalar inv;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    dist = BrVector3Length(&dirn);
    if (dist <= BR_SCALAR_EPSILON * 2) {
        return;
    }
    inv = (br_scalar)(1.0 / dist);
    dirn.v[0] = dirn.v[0] * inv;
    dirn.v[1] = dirn.v[1] * inv;
    dirn.v[2] = dirn.v[2] * inv;
    dist2 = (dist >= 180.f) ? 32767.f : dist * dist;
    dot = BrVector3Dot(&dirn, n);
    attn = 1.0 / (alp->s->attenuation_q * dist2 + alp->s->attenuation_l * dist + alp->s->attenuation_c);
    if (dot <= 0.f) {
        return;
    }
    red = BrFixedToScalar(BR_RED(colour) << 8) * (dot * self->state.surface.kd);
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * (dot * self->state.surface.kd);
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * (dot * self->state.surface.kd);
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > 0.f) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += (red * attn) * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += (grn * attn) * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += (blu * attn) * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
}

// FUNCTION: CARMA2_HW 0x00549be0
void C2_HOOK_STDCALL lightingColourSpotSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
    br_scalar dot;
    br_scalar dot_spot;
    br_scalar spot;
    br_scalar l;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    BrVector3Normalise(&dirn, &dirn);
    dot_spot = BrVector3Dot(&dirn, &alp->direction);
    if (dot_spot < alp->s->spot_outer) {
        return;
    }
    dot = BrVector3Dot(&dirn, n);
    if (dot <= 0.f) {
        return;
    }
    spot = (dot_spot >= alp->s->spot_inner) ? alp->intensity : alp->intensity * (alp->s->spot_outer - dot_spot) / (alp->s->spot_outer - alp->s->spot_inner);

    l = dot * self->state.surface.kd;
    red = BrFixedToScalar(BR_RED(colour) << 8) * l;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * l;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * l;

    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += red * spot * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * spot * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * spot * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
}

// FUNCTION: CARMA2_HW 0x00549ef0
void C2_HOOK_STDCALL lightingColourSpotAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {
    br_scalar dot;
    br_scalar dot_spot;
    br_scalar dist, dist2;
    br_scalar l;
    br_scalar attn;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    dist = BrVector3Length(&dirn);
    if (dist <= BR_SCALAR_EPSILON) {
        return;
    }
    BrVector3InvScale(&dirn, &dirn, dist);
    dot_spot = BrVector3Dot(&dirn, &alp->position);
    if (dot_spot < alp->s->spot_outer) {
        return;
    }
    dot = BrVector3Dot(&dirn, n);
    dist2 = (dist < 180.f) ? dist * dist : 32767.f;

    l = self->state.surface.kd * dot;
    red = BrFixedToScalar(BR_RED(colour) << 8) * l;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * l;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * l;
    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp, &rend.eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }
    attn = 1.f / (alp->s->attenuation_q * dist2 + alp->s->attenuation_l * dist + alp->s->attenuation_c);
    if (dot_spot < alp->s->spot_inner) {
        attn = (alp->s->spot_outer - dot_spot) * attn / (alp->s->spot_outer - alp->s->spot_inner);
    }

    comp[C_R] += red * attn * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * attn * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * attn * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
}

// FUNCTION: CARMA2_HW 0x00548fe0
void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp) {

    switch (alp->type) {
    case BRT_DIRECT:
        alp->accumulate_colour = lightingColourDirectSpecular;
        break;
    case BRT_POINT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_colour = lightingColourPointSpecular;
        } else {
            alp->accumulate_colour = lightingColourPointAttnSpecular;
        }
        break;
    case BRT_SPOT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_colour = lightingColourSpotSpecular;
        } else {
            alp->accumulate_colour = lightingColourSpotAttnSpecular;
        }
        break;
    case BRT_POINT_LOCAL_1:
        alp->accumulate_colour = lightingColourLocal1;
        break;
    case BRT_POINT_LOCAL_2:
        alp->accumulate_colour = lightingColourLocal2;
        break;
    default:
        alp->accumulate_colour = lightingColourNull;
        break;
    }
}
