/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2012 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of squaring in a dodecic extension of a prime field.
 *
 * @version $Id$
 * @ingroup fpx
 */

#include <string.h>

#include "relic_core.h"
#include "relic_conf.h"
#include "relic_pp.h"
#include "relic_pp_low.h"
#include "relic_util.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

#if PP_EXT == LAZYR || !defined(STRIP)

static void fp4_mul_unr(dv2_t e, dv2_t f, fp2_t a, fp2_t b, fp2_t c, fp2_t d) {
	fp2_t t0, t1;
	dv2_t u0, u1;

	fp2_null(t0);
	fp2_null(t1);
	dv2_null(u0);
	dv2_null(u1);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		dv2_new(u0);
		dv2_new(u1);

#ifdef FP_SPACE
		fp2_mulc_low(u0, a, c);
		fp2_mulc_low(u1, b, d);
		fp2_addn_low(t0, c, d);
		fp2_addn_low(t1, a, b);
#else
		fp2_muln_low(u0, a, c);
		fp2_muln_low(u1, b, d);
		fp2_addm_low(t0, c, d);
		fp2_addm_low(t1, a, b);
#endif
		fp2_muln_low(f, t1, t0);
		fp2_subc_low(f, f, u0);
		fp2_subc_low(f, f, u1);
		fp2_nord_low(e, u1);
		fp2_addc_low(e, e, u0);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		dv2_free(t1);
		dv2_free(u0);
		dv2_free(u1);
	}
}

void fp4_sqr_unr(dv2_t c, dv2_t d, fp2_t a, fp2_t b) {
	dv2_t u0, u1;
	fp2_t t;

	TRY {
		/* t0 = a^2. */
		fp2_sqrn_low(u0, a);
		/* t1 = b^2. */
		fp2_sqrn_low(u1, b);

		fp2_addm_low(t, a, b);

		/* c = a^2  + b^2 * E. */
		fp2_nord_low(c, u1);
		fp2_addc_low(c, c, u0);

		/* d = (a + b)^2 - a^2 - b^2 = 2 * a * b. */
		fp2_addc_low(u1, u1, u0);
		fp2_sqrn_low(d, t);
		fp2_subc_low(d, d, u1);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
}

#endif

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void fp12_sqr(fp12_t c, fp12_t a) {
	fp6_t t0, t1;

	fp6_null(t0);
	fp6_null(t1);

	TRY {
		fp6_new(t0);
		fp6_new(t1);

		fp6_add(t0, a[0], a[1]);
		fp6_mul_art(t1, a[1]);
		fp6_add(t1, a[0], t1);
		fp6_mul(t0, t0, t1);
		fp6_mul(c[1], a[0], a[1]);
		fp6_sub(c[0], t0, c[1]);
		fp6_mul_art(t1, c[1]);
		fp6_sub(c[0], c[0], t1);
		fp6_dbl(c[1], c[1]);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp6_free(t0);
		fp6_free(t1);
	}
}

#if PP_EXT == LAZYR || !defined(STRIP)

void fp12_sqr2(fp12_t c, fp12_t a) {
	fp2_t t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
	dv2_t u0, u1, u2, u3, u4, u5, u6, u7, u8, u9;

	fp2_null(t0);
	fp2_null(t1);
	fp2_null(t3);
	fp2_null(t4);
	fp2_null(t5);
	fp2_null(t6);
	fp2_null(t7);
	fp2_null(t8);
	fp2_null(t9);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		fp2_new(t2);
		fp2_new(t3);
		fp2_new(t4);
		fp2_new(t5);
		fp2_new(t6);
		fp2_new(t7);
		fp2_new(t8);
		fp2_new(t9);

		/* a0 = (a00, a11). */
		/* a1 = (a10, a02). */
		/* a2 = (a01, a12). */

		/* (t0,t1) = a0^2 */
		fp4_sqr_unr(u0, u1, a[0][0], a[1][1]);
		fp2_rdcn_low(t0, u0);
		fp2_rdcn_low(t1, u1);

		/* (t2,t3) = 2 * a1 * a2 */
		fp4_mul_unr(u2, u3, a[1][0], a[0][2], a[0][1], a[1][2]);
		fp2_addc_low(u2, u2, u2);
		fp2_addc_low(u3, u3, u3);
		fp2_rdcn_low(t2, u2);
		fp2_rdcn_low(t3, u3);

		/* (t4,t5) = a2^2. */
		fp4_sqr_unr(u4, u5, a[0][1], a[1][2]);
		fp2_rdcn_low(t4, u4);
		fp2_rdcn_low(t5, u5);

		/* c2 = a0 + a2. */
		fp2_addn_low(t8, a[0][0], a[0][1]);
		fp2_addn_low(t9, a[1][1], a[1][2]);

		/* (t6, t7) = (a0 + a2 + a1)^2. */
		fp2_addn_low(t6, t8, a[1][0]);
		fp2_addn_low(t7, t9, a[0][2]);
		fp4_sqr_unr(u6, u7, t6, t7);
		fp2_rdcn_low(t6, u6);
		fp2_rdcn_low(t7, u7);

		/* c2 = (a0 + a2 - a1)^2. */
		fp2_subn_low(t8, t8, a[1][0]);
		fp2_subn_low(t9, t9, a[0][2]);
		fp4_sqr_unr(u8, u9, t8, t9);
		fp2_rdcn_low(t8, u8);
		fp2_rdcn_low(t9, u9);

		/* c2 = (c2 + (t6, t7))/2. */
		fp2_addn_low(t8, t8, t6);
		fp2_addn_low(t9, t9, t7);
		fp_hlv(t8[0], t8[0]);
		fp_hlv(t8[1], t8[1]);
		fp_hlv(t9[0], t9[0]);
		fp_hlv(t9[1], t9[1]);

		/* (t6, t7) = (t6, t7) - c2 - (t2, t3). */
		fp2_sub(t6, t6, t8);
		fp2_sub(t7, t7, t9);
		fp2_sub(t6, t6, t2);
		fp2_sub(t7, t7, t3);

		/* c2 = c2 - (t0,t1) - (t4,t5). */
		fp2_sub(t8, t8, t0);
		fp2_sub(t9, t9, t1);
		fp2_sub(c[0][1], t8, t4);
		fp2_sub(c[1][2], t9, t5);

		/* c1 = (t6, t7) + (t4, t5) * E. */
		fp2_mul_nor(t9, t5);
		fp2_add(c[1][0], t6, t9);
		fp2_add(c[0][2], t7, t4);

		/* c0 = (t0,t1) - (t2,t3) * E. */
		fp2_mul_nor(t9, t3);
		fp2_add(c[0][0], t0, t9);
		fp2_add(c[1][1], t1, t2);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		fp2_free(t2);
		fp2_free(t3);
		fp2_free(t4);
		fp2_free(t5);
	}
}

#endif

#if PP_EXT == BASIC || !defined(STRIP)

void fp12_sqr_cyc_basic(fp12_t c, fp12_t a) {
	fp2_t t0, t1, t2, t3, t4, t5, t6;

	fp2_null(t0);
	fp2_null(t1);
	fp2_null(t2);
	fp2_null(t3);
	fp2_null(t4);
	fp2_null(t5);
	fp2_null(t6);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		fp2_new(t2);
		fp2_new(t3);
		fp2_new(t4);
		fp2_new(t5);
		fp2_new(t6);

		/* Define z = sqrt(E) */

		/* Now a is seen as (t0,t1) + (t2,t3) * w + (t4,t5) * w^2 */

		/* (t0, t1) = (a00 + a11*z)^2. */
		fp2_sqr(t2, a[0][0]);
		fp2_sqr(t3, a[1][1]);
		fp2_add(t1, a[0][0], a[1][1]);

		fp2_mul_nor(t0, t3);
		fp2_add(t0, t0, t2);

		fp2_sqr(t1, t1);
		fp2_sub(t1, t1, t2);
		fp2_sub(t1, t1, t3);

		fp2_sub(c[0][0], t0, a[0][0]);
		fp2_add(c[0][0], c[0][0], c[0][0]);
		fp2_add(c[0][0], t0, c[0][0]);

		fp2_add(c[1][1], t1, a[1][1]);
		fp2_add(c[1][1], c[1][1], c[1][1]);
		fp2_add(c[1][1], t1, c[1][1]);

		fp2_sqr(t0, a[0][1]);
		fp2_sqr(t1, a[1][2]);
		fp2_add(t5, a[0][1], a[1][2]);
		fp2_sqr(t2, t5);

		fp2_add(t3, t0, t1);
		fp2_sub(t5, t2, t3);

		fp2_add(t6, a[1][0], a[0][2]);
		fp2_sqr(t3, t6);
		fp2_sqr(t2, a[1][0]);

		fp2_mul_nor(t6, t5);
		fp2_add(t5, t6, a[1][0]);
		fp2_dbl(t5, t5);
		fp2_add(c[1][0], t5, t6);

		fp2_mul_nor(t4, t1);
		fp2_add(t5, t0, t4);
		fp2_sub(t6, t5, a[0][2]);

		fp2_sqr(t1, a[0][2]);

		fp2_dbl(t6, t6);
		fp2_add(c[0][2], t6, t5);

		fp2_mul_nor(t4, t1);
		fp2_add(t5, t2, t4);
		fp2_sub(t6, t5, a[0][1]);
		fp2_dbl(t6, t6);
		fp2_add(c[0][1], t6, t5);

		fp2_add(t0, t2, t1);
		fp2_sub(t5, t3, t0);
		fp2_add(t6, t5, a[1][2]);
		fp2_dbl(t6, t6);
		fp2_add(c[1][2], t5, t6);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		fp2_free(t2);
		fp2_free(t3);
		fp2_free(t4);
		fp2_free(t5);
		fp2_free(t6);
	}
}

void fp12_sqr_pck_basic(fp12_t c, fp12_t a) {
	fp2_t t0, t1, t2, t3, t4, t5, t6;

	fp2_null(t0);
	fp2_null(t1);
	fp2_null(t2);
	fp2_null(t3);
	fp2_null(t4);
	fp2_null(t5);
	fp2_null(t6);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		fp2_new(t2);
		fp2_new(t3);
		fp2_new(t4);
		fp2_new(t5);
		fp2_new(t6);

		fp2_sqr(t0, a[0][1]);
		fp2_sqr(t1, a[1][2]);
		fp2_add(t5, a[0][1], a[1][2]);
		fp2_sqr(t2, t5);

		fp2_add(t3, t0, t1);
		fp2_sub(t5, t2, t3);

		fp2_add(t6, a[1][0], a[0][2]);
		fp2_sqr(t3, t6);
		fp2_sqr(t2, a[1][0]);

		fp2_mul_nor(t6, t5);
		fp2_add(t5, t6, a[1][0]);
		fp2_dbl(t5, t5);
		fp2_add(c[1][0], t5, t6);

		fp2_mul_nor(t4, t1);
		fp2_add(t5, t0, t4);
		fp2_sub(t6, t5, a[0][2]);

		fp2_sqr(t1, a[0][2]);

		fp2_dbl(t6, t6);
		fp2_add(c[0][2], t6, t5);

		fp2_mul_nor(t4, t1);
		fp2_add(t5, t2, t4);
		fp2_sub(t6, t5, a[0][1]);
		fp2_dbl(t6, t6);
		fp2_add(c[0][1], t6, t5);

		fp2_add(t0, t2, t1);
		fp2_sub(t5, t3, t0);
		fp2_add(t6, t5, a[1][2]);
		fp2_dbl(t6, t6);
		fp2_add(c[1][2], t5, t6);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		fp2_free(t2);
		fp2_free(t3);
		fp2_free(t4);
		fp2_free(t5);
		fp2_free(t6);
	}
}

#endif

#if PP_EXT == LAZYR || !defined(STRIP)

void fp12_sqr_cyc_lazyr(fp12_t c, fp12_t a) {
	fp2_t t0, t1;
	dv2_t u0, u1, u2, u3, u4;

	fp2_null(t0);
	fp2_null(t1);
	dv2_null(u0);
	dv2_null(u1);
	dv2_null(u2);
	dv2_null(u3);
	dv2_null(u4);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		dv2_new(u0);
		dv2_new(u1);
		dv2_new(u2);
		dv2_new(u3);
		dv2_new(u4);

		fp2_sqrn_low(u2, a[0][0]);
		fp2_sqrn_low(u3, a[1][1]);
		fp2_add(t1, a[0][0], a[1][1]);

		fp2_nord_low(u0, u3);
		fp2_addc_low(u0, u0, u2);
		fp2_rdcn_low(t0, u0);

		fp2_sqrn_low(u1, t1);
		fp2_addd_low(u2, u2, u3);
		fp2_subc_low(u1, u1, u2);
		fp2_rdcn_low(t1, u1);

		fp2_sub(c[0][0], t0, a[0][0]);
		fp2_dbl(c[0][0], c[0][0]);
		fp2_add(c[0][0], t0, c[0][0]);

		fp2_add(c[1][1], t1, a[1][1]);
		fp2_dbl(c[1][1], c[1][1]);
		fp2_add(c[1][1], t1, c[1][1]);

		fp2_sqrn_low(u0, a[0][1]);
		fp2_sqrn_low(u1, a[1][2]);
		fp2_add(t0, a[0][1], a[1][2]);
		fp2_sqrn_low(u2, t0);

		fp2_addd_low(u3, u0, u1);
		fp2_subc_low(u3, u2, u3);
		fp2_rdcn_low(t0, u3);

		fp2_add(t1, a[1][0], a[0][2]);
		fp2_sqrn_low(u3, t1);
		fp2_sqrn_low(u2, a[1][0]);

		fp2_mul_nor(t1, t0);
		fp2_add(t0, t1, a[1][0]);
		fp2_dbl(t0, t0);
		fp2_add(c[1][0], t0, t1);

		fp2_nord_low(u4, u1);
		fp2_addc_low(u4, u0, u4);
		fp2_rdcn_low(t0, u4);
		fp2_sub(t1, t0, a[0][2]);

		fp2_sqrn_low(u1, a[0][2]);

		fp2_dbl(t1, t1);
		fp2_add(c[0][2], t1, t0);

		fp2_nord_low(u4, u1);
		fp2_addc_low(u4, u2, u4);
		fp2_rdcn_low(t0, u4);
		fp2_sub(t1, t0, a[0][1]);
		fp2_dbl(t1, t1);
		fp2_add(c[0][1], t1, t0);

		fp2_addd_low(u0, u2, u1);
		fp2_subc_low(u3, u3, u0);
		fp2_rdcn_low(t0, u3);
		fp2_add(t1, t0, a[1][2]);
		fp2_dbl(t1, t1);
		fp2_add(c[1][2], t0, t1);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		dv2_free(u0);
		dv2_free(u1);
		dv2_free(u2);
		dv2_free(u3);
		dv2_free(u4);
	}
}

void fp12_sqr_pck_lazyr(fp12_t c, fp12_t a) {
	fp2_t t0, t1;
	dv2_t u0, u1, u2, u3, u4;

	fp2_null(t0);
	fp2_null(t1);
	dv2_null(u0);
	dv2_null(u1);
	dv2_null(u2);
	dv2_null(u3);
	dv2_null(u4);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		dv2_new(u0);
		dv2_new(u1);
		dv2_new(u2);
		dv2_new(u3);
		dv2_new(u4);

		fp2_sqrn_low(u0, a[0][1]);
		fp2_sqrn_low(u1, a[1][2]);
		fp2_add(t0, a[0][1], a[1][2]);
		fp2_sqrn_low(u2, t0);

		fp2_addc_low(u3, u0, u1);
		fp2_subc_low(u3, u2, u3);
		fp2_rdcn_low(t0, u3);

		fp2_add(t1, a[1][0], a[0][2]);
		fp2_sqrn_low(u3, t1);
		fp2_sqrn_low(u2, a[1][0]);

		fp2_mul_nor(t1, t0);
		fp2_add(t0, t1, a[1][0]);
		fp2_dbl(t0, t0);
		fp2_add(c[1][0], t0, t1);

		fp2_nord_low(u4, u1);
		fp2_addc_low(u4, u0, u4);
		fp2_rdcn_low(t0, u4);
		fp2_sub(t1, t0, a[0][2]);

		fp2_sqrn_low(u1, a[0][2]);

		fp2_dbl(t1, t1);
		fp2_add(c[0][2], t1, t0);

		fp2_nord_low(u4, u1);
		fp2_addc_low(u4, u2, u4);
		fp2_rdcn_low(t0, u4);
		fp2_sub(t1, t0, a[0][1]);
		fp2_dbl(t1, t1);
		fp2_add(c[0][1], t1, t0);

		fp2_addc_low(u0, u2, u1);
		fp2_subc_low(u3, u3, u0);
		fp2_rdcn_low(t0, u3);
		fp2_add(t1, t0, a[1][2]);
		fp2_dbl(t1, t1);
		fp2_add(c[1][2], t0, t1);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		dv2_free(u0);
		dv2_free(u1);
		dv2_free(u2);
		dv2_free(u3);
		dv2_free(u4);
	}
}

#endif