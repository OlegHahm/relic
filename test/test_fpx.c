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
 * Tests for extensions defined over prime fields.
 *
 * @version $Id$
 * @ingroup test
 */

#include <stdio.h>

#include "relic.h"
#include "relic_test.h"
#include "relic_bench.h"

static int memory2(void) {
	err_t e;
	int code = STS_ERR;
	fp2_t a;

	TRY {
		TEST_BEGIN("memory can be allocated") {
			fp2_new(a);
			fp2_free(a);
		} TEST_END;
	} CATCH(e) {
		switch (e) {
			case ERR_NO_MEMORY:
				util_print("FATAL ERROR!\n");
				ERROR(end);
				break;
		}
	}
	(void)a;
	code = STS_OK;
  end:
	return code;
}

static int util2(void) {
	int code = STS_ERR;
	fp2_t a, b, c;

	fp2_null(a);
	fp2_null(b);
	fp2_null(c);

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);

		TEST_BEGIN("comparison is consistent") {
			fp2_rand(a);
			fp2_rand(b);
			if (fp2_cmp(a, b) != CMP_EQ) {
				TEST_ASSERT(fp2_cmp(b, a) == CMP_NE, end);
			}
		}
		TEST_END;

		TEST_BEGIN("copy and comparison are consistent") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_rand(c);
			if (fp2_cmp(a, c) != CMP_EQ) {
				fp2_copy(c, a);
				TEST_ASSERT(fp2_cmp(c, a) == CMP_EQ, end);
			}
			if (fp2_cmp(b, c) != CMP_EQ) {
				fp2_copy(c, b);
				TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
			}
		}
		TEST_END;

		TEST_BEGIN("negation is consistent") {
			fp2_rand(a);
			fp2_neg(b, a);
			if (fp2_cmp(a, b) != CMP_EQ) {
				TEST_ASSERT(fp2_cmp(b, a) == CMP_NE, end);
			}
			fp2_neg(b, b);
			TEST_ASSERT(fp2_cmp(a, b) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to zero and comparison are consistent") {
			fp2_rand(a);
			fp2_zero(c);
			TEST_ASSERT(fp2_cmp(a, c) == CMP_NE, end);
			TEST_ASSERT(fp2_cmp(c, a) == CMP_NE, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to random and comparison are consistent") {
			fp2_rand(a);
			fp2_zero(c);
			TEST_ASSERT(fp2_cmp(a, c) == CMP_NE, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to zero and zero test are consistent") {
			fp2_zero(a);
			TEST_ASSERT(fp2_is_zero(a), end);
		}
		TEST_END;

	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	return code;
}

static int addition2(void) {
	int code = STS_ERR;
	fp2_t a, b, c, d, e;

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);
		fp2_new(d);
		fp2_new(e);

		TEST_BEGIN("addition is commutative") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_add(d, a, b);
			fp2_add(e, b, a);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition is associative") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_rand(c);
			fp2_add(d, a, b);
			fp2_add(d, d, c);
			fp2_add(e, b, c);
			fp2_add(e, a, e);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition has identity") {
			fp2_rand(a);
			fp2_zero(d);
			fp2_add(e, a, d);
			TEST_ASSERT(fp2_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition has inverse") {
			fp2_rand(a);
			fp2_neg(d, a);
			fp2_add(e, a, d);
			TEST_ASSERT(fp2_is_zero(e), end);
		} TEST_END;

#if PP_QDR == BASIC || !defined(STRIP)
		TEST_BEGIN("basic addition is correct") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_add(d, a, b);
			fp2_add_basic(e, a, b);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_QDR == INTEG || !defined(STRIP)
		TEST_BEGIN("integrated addition is correct") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_add(d, a, b);
			fp2_add_integ(e, a, b);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	fp2_free(d);
	fp2_free(e);
	return code;
}

static int subtraction2(void) {
	int code = STS_ERR;
	fp2_t a, b, c, d;

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);
		fp2_new(d);

		TEST_BEGIN("subtraction is anti-commutative") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_sub(c, a, b);
			fp2_sub(d, b, a);
			fp2_neg(d, d);
			TEST_ASSERT(fp2_cmp(c, d) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("subtraction has identity") {
			fp2_rand(a);
			fp2_zero(c);
			fp2_sub(d, a, c);
			TEST_ASSERT(fp2_cmp(d, a) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("subtraction has inverse") {
			fp2_rand(a);
			fp2_sub(c, a, a);
			TEST_ASSERT(fp2_is_zero(c), end);
		}
		TEST_END;

#if PP_QDR == BASIC || !defined(STRIP)
		TEST_BEGIN("basic subtraction is correct") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_sub(c, a, b);
			fp2_sub_basic(d, a, b);
			TEST_ASSERT(fp2_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_QDR == INTEG || !defined(STRIP)
		TEST_BEGIN("integrated subtraction is correct") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_sub(c, a, b);
			fp2_sub_integ(d, a, b);
			TEST_ASSERT(fp2_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	fp2_free(d);
	return code;
}

static int doubling2(void) {
	int code = STS_ERR;
	fp2_t a, b, c;

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);

		TEST_BEGIN("doubling is correct") {
			fp2_rand(a);
			fp2_dbl(b, a);
			fp2_add(c, a, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		} TEST_END;

#if PP_QDR == BASIC || !defined(STRIP)
		TEST_BEGIN("basic doubling is correct") {
			fp2_rand(a);
			fp2_dbl(b, a);
			fp2_dbl_basic(c, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_QDR == INTEG || !defined(STRIP)
		TEST_BEGIN("integrated doubling is correct") {
			fp2_rand(a);
			fp2_dbl(b, a);
			fp2_dbl_integ(c, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	return code;
}

static int multiplication2(void) {
	int code = STS_ERR;
	fp2_t a, b, c, d, e, f;
	bn_t g;

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);
		fp2_new(d);
		fp2_new(e);
		fp2_new(f);
		bn_new(g);

		TEST_BEGIN("multiplication is commutative") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_mul(d, a, b);
			fp2_mul(e, b, a);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication is associative") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_rand(c);
			fp2_mul(d, a, b);
			fp2_mul(d, d, c);
			fp2_mul(e, b, c);
			fp2_mul(e, a, e);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication is distributive") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_rand(c);
			fp2_add(d, a, b);
			fp2_mul(d, c, d);
			fp2_mul(e, c, a);
			fp2_mul(f, c, b);
			fp2_add(e, e, f);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication has identity") {
			fp2_rand(a);
			fp2_zero(d);
			fp_set_dig(d[0], 1);
			fp2_mul(e, a, d);
			TEST_ASSERT(fp2_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication has zero property") {
			fp2_rand(a);
			fp2_zero(d);
			fp2_mul(e, a, d);
			TEST_ASSERT(fp2_is_zero(e), end);
		} TEST_END;

#if PP_QDR == BASIC || !defined(STRIP)
		TEST_BEGIN("basic multiplication is correct") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_mul(d, a, b);
			fp2_mul_basic(e, b, a);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_QDR == INTEG || !defined(STRIP)
		TEST_BEGIN("integrated multiplication is correct") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_mul(d, a, b);
			fp2_mul_integ(e, b, a);
			TEST_ASSERT(fp2_cmp(d, e) == CMP_EQ, end);
		} TEST_END;
#endif

		TEST_BEGIN("multiplication by adjoined root is correct") {
			fp2_rand(a);
			fp2_zero(b);
			fp_set_dig(b[1], 1);
			fp2_mul(c, a, b);
			fp2_mul_art(d, a);
			TEST_ASSERT(fp2_cmp(c, d) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication by quadratic/cubic non-residue is correct") {
			fp2_rand(a);
			fp2_mul_nor(b, a);
			switch (fp_prime_get_mod8()) {
				case 5:
					fp2_mul_art(c, a);
					break;
				case 3:
					fp2_mul_art(c, a);
					fp2_add(c, c, a);
					break;
				case 7:
					fp2_mul_art(d, a);
					fp2_dbl(c, a);
					fp_prime_back(g, ep_curve_get_b());
					for (int i = 1; i < bn_bits(g) / 2; i++) {
						fp2_dbl(c, c);
					}
					fp2_add(c, c, d);
					break;
			}
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		}
		TEST_END;

#if PP_QDR == BASIC || !defined(STRIP)
		TEST_BEGIN("basic multiplication by qnr/cnr is correct") {
			fp2_rand(a);
			fp2_mul_nor(b, a);
			fp2_mul_nor_basic(c, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		}
		TEST_END;
#endif

#if PP_QDR == INTEG || !defined(STRIP)
		TEST_BEGIN("integrated multiplication by qnr/cnr is correct") {
			fp2_rand(a);
			fp2_mul_nor(b, a);
			fp2_mul_nor_integ(c, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		}
		TEST_END;
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	fp2_free(d);
	fp2_free(e);
	fp2_free(f);
	bn_free(g);
	return code;
}

static int squaring2(void) {
	int code = STS_ERR;
	fp2_t a, b, c;

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);

		TEST_BEGIN("squaring is correct") {
			fp2_rand(a);
			fp2_mul(b, a, a);
			fp2_sqr(c, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		} TEST_END;

#if PP_QDR == BASIC || !defined(STRIP)
		TEST_BEGIN("basic squaring is correct") {
			fp2_rand(a);
			fp2_sqr(b, a);
			fp2_sqr_basic(c, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_QDR == INTEG || !defined(STRIP)
		TEST_BEGIN("integrated squaring is correct") {
			fp2_rand(a);
			fp2_sqr(b, a);
			fp2_sqr_integ(c, a);
			TEST_ASSERT(fp2_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	return code;
}

static int inversion2(void) {
	int code = STS_ERR;
	fp2_t a, b, c, d[2];

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);
		fp2_new(d[0]);
		fp2_new(d[1]);

		TEST_BEGIN("inversion is correct") {
			fp2_rand(a);
			fp2_inv(b, a);
			fp2_mul(c, a, b);
			fp2_zero(b);
			fp_set_dig(b[0], 1);
			TEST_ASSERT(fp2_cmp(c, b) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("simultaneous inversion is correct") {
			fp2_rand(a);
			fp2_rand(b);
			fp2_copy(d[0], a);
			fp2_copy(d[1], b);
			fp2_inv(a, a);
			fp2_inv(b, b);
			fp2_inv_sim(d, d, 2);
			TEST_ASSERT(fp2_cmp(d[0], a) == CMP_EQ &&
					fp2_cmp(d[1], b) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	fp2_free(d[0]);
	fp2_free(d[1]);
	return code;
}

static int exponentiation2(void) {
	int code = STS_ERR;
	fp2_t a, b, c;
	bn_t d;

	fp2_null(a);
	fp2_null(b);
	fp2_null(c);
	bn_null(d);

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);
		bn_new(d);

		TEST_BEGIN("frobenius and exponentiation are consistent") {
			fp2_rand(a);
			fp2_frb(b, a, 1);
			d->used = FP_DIGS;
			dv_copy(d->dp, fp_prime_get(), FP_DIGS);
			fp2_exp(c, a, d);
			TEST_ASSERT(fp2_cmp(c, b) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("frobenius and squared frobenius are consistent") {
			fp2_rand(a);
			fp2_frb(b, a, 1);
			fp2_frb(b, b, 1);
			fp2_frb(c, a, 2);
			TEST_ASSERT(fp2_cmp(c, b) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	bn_free(d);
	return code;
}

static int square_root2(void) {
	int code = STS_ERR;
	fp2_t a, b, c;
	int r;

	TRY {
		fp2_new(a);
		fp2_new(b);
		fp2_new(c);

		TEST_BEGIN("square root extraction is correct") {
			fp2_rand(a);
			fp2_sqr(c, a);
			r = fp2_srt(b, c);
			fp2_neg(c, b);
			TEST_ASSERT(r, end);
			TEST_ASSERT(fp2_cmp(b, a) == CMP_EQ ||
					fp2_cmp(c, a) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp2_free(a);
	fp2_free(b);
	fp2_free(c);
	return code;
}

static int memory6(void) {
	err_t e;
	int code = STS_ERR;
	fp6_t a;

	TRY {
		TEST_BEGIN("memory can be allocated") {
			fp6_new(a);
			fp6_free(a);
		} TEST_END;
	} CATCH(e) {
		switch (e) {
			case ERR_NO_MEMORY:
				util_print("FATAL ERROR!\n");
				ERROR(end);
				break;
		}
	}
	(void)a;
	code = STS_OK;
  end:
	return code;
}

static int util6(void) {
	int code = STS_ERR;
	fp6_t a, b, c;

	fp6_null(a);
	fp6_null(b);
	fp6_null(c);

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);

		TEST_BEGIN("comparison is consistent") {
			fp6_rand(a);
			fp6_rand(b);
			if (fp6_cmp(a, b) != CMP_EQ) {
				TEST_ASSERT(fp6_cmp(b, a) == CMP_NE, end);
			}
		}
		TEST_END;

		TEST_BEGIN("copy and comparison are consistent") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_rand(c);
			if (fp6_cmp(a, c) != CMP_EQ) {
				fp6_copy(c, a);
				TEST_ASSERT(fp6_cmp(c, a) == CMP_EQ, end);
			}
			if (fp6_cmp(b, c) != CMP_EQ) {
				fp6_copy(c, b);
				TEST_ASSERT(fp6_cmp(b, c) == CMP_EQ, end);
			}
		}
		TEST_END;

		TEST_BEGIN("negation is consistent") {
			fp6_rand(a);
			fp6_neg(b, a);
			if (fp6_cmp(a, b) != CMP_EQ) {
				TEST_ASSERT(fp6_cmp(b, a) == CMP_NE, end);
			}
			fp6_neg(b, b);
			TEST_ASSERT(fp6_cmp(a, b) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to zero and comparison are consistent") {
			fp6_rand(a);
			fp6_zero(c);
			TEST_ASSERT(fp6_cmp(a, c) == CMP_NE, end);
			TEST_ASSERT(fp6_cmp(c, a) == CMP_NE, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to random and comparison are consistent") {
			fp6_rand(a);
			fp6_zero(c);
			TEST_ASSERT(fp6_cmp(a, c) == CMP_NE, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to zero and zero test are consistent") {
			fp6_zero(a);
			TEST_ASSERT(fp6_is_zero(a), end);
		}
		TEST_END;

	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	return code;
}

static int addition6(void) {
	int code = STS_ERR;
	fp6_t a, b, c, d, e;

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);
		fp6_new(d);
		fp6_new(e);

		TEST_BEGIN("addition is commutative") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_add(d, a, b);
			fp6_add(e, b, a);
			TEST_ASSERT(fp6_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition is associative") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_rand(c);
			fp6_add(d, a, b);
			fp6_add(d, d, c);
			fp6_add(e, b, c);
			fp6_add(e, a, e);
			TEST_ASSERT(fp6_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition has identity") {
			fp6_rand(a);
			fp6_zero(d);
			fp6_add(e, a, d);
			TEST_ASSERT(fp6_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition has inverse") {
			fp6_rand(a);
			fp6_neg(d, a);
			fp6_add(e, a, d);
			TEST_ASSERT(fp6_is_zero(e), end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	fp6_free(d);
	fp6_free(e);
	return code;
}

static int subtraction6(void) {
	int code = STS_ERR;
	fp6_t a, b, c, d;

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);
		fp6_new(d);

		TEST_BEGIN("subtraction is anti-commutative") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_sub(c, a, b);
			fp6_sub(d, b, a);
			fp6_neg(d, d);
			TEST_ASSERT(fp6_cmp(c, d) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("subtraction has identity") {
			fp6_rand(a);
			fp6_zero(c);
			fp6_sub(d, a, c);
			TEST_ASSERT(fp6_cmp(d, a) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("subtraction has inverse") {
			fp6_rand(a);
			fp6_sub(c, a, a);
			TEST_ASSERT(fp6_is_zero(c), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	fp6_free(d);
	return code;
}

static int doubling6(void) {
	int code = STS_ERR;
	fp6_t a, b, c;

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);

		TEST_BEGIN("doubling is correct") {
			fp6_rand(a);
			fp6_dbl(b, a);
			fp6_add(c, a, a);
			TEST_ASSERT(fp6_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	return code;
}

static int multiplication6(void) {
	int code = STS_ERR;
	fp6_t a, b, c, d, e, f;

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);
		fp6_new(d);
		fp6_new(e);
		fp6_new(f);

		TEST_BEGIN("multiplication is commutative") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_mul(d, a, b);
			fp6_mul(e, b, a);
			TEST_ASSERT(fp6_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication is associative") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_rand(c);
			fp6_mul(d, a, b);
			fp6_mul(d, d, c);
			fp6_mul(e, b, c);
			fp6_mul(e, a, e);
			TEST_ASSERT(fp6_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication is distributive") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_rand(c);
			fp6_add(d, a, b);
			fp6_mul(d, c, d);
			fp6_mul(e, c, a);
			fp6_mul(f, c, b);
			fp6_add(e, e, f);
			TEST_ASSERT(fp6_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication has identity") {
			fp6_zero(d);
			fp_set_dig(d[0][0], 1);
			fp6_mul(e, a, d);
			TEST_ASSERT(fp6_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication has zero property") {
			fp6_zero(d);
			fp6_mul(e, a, d);
			TEST_ASSERT(fp6_is_zero(e), end);
		} TEST_END;

		TEST_BEGIN("multiplication by adjoined root is correct") {
			fp6_rand(a);
			fp6_zero(b);
			fp_set_dig(b[1][0], 1);
			fp6_mul(c, a, b);
			fp6_mul_art(d, a);
			TEST_ASSERT(fp6_cmp(c, d) == CMP_EQ, end);
		} TEST_END;

#if PP_EXT == BASIC | !defined(STRIP)
		TEST_BEGIN("basic multiplication is correct") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_mul(c, a, b);
			fp6_mul_basic(d, a, b);
			TEST_ASSERT(fp6_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_EXT == LAZYR || !defined(STRIP)
		TEST_BEGIN("lazy reduced multiplication is correct") {
			fp6_rand(a);
			fp6_rand(b);
			fp6_mul(c, a, b);
			fp6_mul_lazyr(d, a, b);
			TEST_ASSERT(fp6_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	fp6_free(d);
	fp6_free(e);
	fp6_free(f);
	return code;
}

static int squaring6(void) {
	int code = STS_ERR;
	fp6_t a, b, c;

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);

		TEST_BEGIN("squaring is correct") {
			fp6_rand(a);
			fp6_mul(b, a, a);
			fp6_sqr(c, a);
			TEST_ASSERT(fp6_cmp(b, c) == CMP_EQ, end);
		} TEST_END;

#if PP_EXT == BASIC | !defined(STRIP)
		TEST_BEGIN("basic squaring is correct") {
			fp6_rand(a);
			fp6_sqr(b, a);
			fp6_sqr_basic(c, a);
			TEST_ASSERT(fp6_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_EXT == LAZYR || !defined(STRIP)
		TEST_BEGIN("lazy reduced squaring is correct") {
			fp6_rand(a);
			fp6_sqr(b, a);
			fp6_sqr_lazyr(c, a);
			TEST_ASSERT(fp6_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	return code;
}

static int inversion6(void) {
	int code = STS_ERR;
	fp6_t a, b, c;

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);

		TEST_BEGIN("inversion is correct") {
			fp6_rand(a);
			fp6_inv(b, a);
			fp6_mul(c, a, b);
			fp6_zero(b);
			fp_set_dig(b[0][0], 1);
			TEST_ASSERT(fp6_cmp(c, b) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	return code;
}

static int exponentiation6(void) {
	int code = STS_ERR;
	fp6_t a, b, c;
	bn_t d;

	fp6_null(a);
	fp6_null(b);
	fp6_null(c);
	bn_null(d);

	TRY {
		fp6_new(a);
		fp6_new(b);
		fp6_new(c);
		bn_new(d);

		TEST_BEGIN("frobenius and exponentiation are consistent") {
			fp6_rand(a);
			fp6_frb(b, a, 1);
			d->used = FP_DIGS;
			dv_copy(d->dp, fp_prime_get(), FP_DIGS);
			fp6_exp(c, a, d);
			TEST_ASSERT(fp6_cmp(c, b) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("frobenius and squared frobenius are consistent") {
			fp6_rand(a);
			fp6_frb(b, a, 1);
			fp6_frb(b, b, 1);
			fp6_frb(c, a, 2);
			TEST_ASSERT(fp6_cmp(c, b) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp6_free(a);
	fp6_free(b);
	fp6_free(c);
	bn_free(d);
	return code;
}

static int memory12(void) {
	err_t e;
	int code = STS_ERR;
	fp12_t a;

	TRY {
		TEST_BEGIN("memory can be allocated") {
			fp12_new(a);
			fp12_free(a);
		} TEST_END;
	} CATCH(e) {
		switch (e) {
			case ERR_NO_MEMORY:
				util_print("FATAL ERROR!\n");
				ERROR(end);
				break;
		}
	}
	(void)a;
	code = STS_OK;
  end:
	return code;
}

static int util12(void) {
	int code = STS_ERR;
	fp12_t a, b, c;

	fp12_null(a);
	fp12_null(b);
	fp12_null(c);

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);

		TEST_BEGIN("comparison is consistent") {
			fp12_rand(a);
			fp12_rand(b);
			if (fp12_cmp(a, b) != CMP_EQ) {
				TEST_ASSERT(fp12_cmp(b, a) == CMP_NE, end);
			}
		}
		TEST_END;

		TEST_BEGIN("copy and comparison are consistent") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_rand(c);
			if (fp12_cmp(a, c) != CMP_EQ) {
				fp12_copy(c, a);
				TEST_ASSERT(fp12_cmp(c, a) == CMP_EQ, end);
			}
			if (fp12_cmp(b, c) != CMP_EQ) {
				fp12_copy(c, b);
				TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
			}
		}
		TEST_END;

		TEST_BEGIN("negation is consistent") {
			fp12_rand(a);
			fp12_neg(b, a);
			if (fp12_cmp(a, b) != CMP_EQ) {
				TEST_ASSERT(fp12_cmp(b, a) == CMP_NE, end);
			}
			fp12_neg(b, b);
			TEST_ASSERT(fp12_cmp(a, b) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to zero and comparison are consistent") {
			fp12_rand(a);
			fp12_zero(c);
			TEST_ASSERT(fp12_cmp(a, c) == CMP_NE, end);
			TEST_ASSERT(fp12_cmp(c, a) == CMP_NE, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to random and comparison are consistent") {
			fp12_rand(a);
			fp12_zero(c);
			TEST_ASSERT(fp12_cmp(a, c) == CMP_NE, end);
		}
		TEST_END;

		TEST_BEGIN("assignment to zero and zero test are consistent") {
			fp12_zero(a);
			TEST_ASSERT(fp12_is_zero(a), end);
		}
		TEST_END;

	}
	CATCH_ANY {
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	return code;
}

static int addition12(void) {
	int code = STS_ERR;
	fp12_t a, b, c, d, e;

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);
		fp12_new(d);
		fp12_new(e);

		TEST_BEGIN("addition is commutative") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_add(d, a, b);
			fp12_add(e, b, a);
			TEST_ASSERT(fp12_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition is associative") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_rand(c);
			fp12_add(d, a, b);
			fp12_add(d, d, c);
			fp12_add(e, b, c);
			fp12_add(e, a, e);
			TEST_ASSERT(fp12_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition has identity") {
			fp12_rand(a);
			fp12_zero(d);
			fp12_add(e, a, d);
			TEST_ASSERT(fp12_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("addition has inverse") {
			fp12_rand(a);
			fp12_neg(d, a);
			fp12_add(e, a, d);
			TEST_ASSERT(fp12_is_zero(e), end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	fp12_free(d);
	fp12_free(e);
	return code;
}

static int subtraction12(void) {
	int code = STS_ERR;
	fp12_t a, b, c, d;

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);
		fp12_new(d);

		TEST_BEGIN("subtraction is anti-commutative") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_sub(c, a, b);
			fp12_sub(d, b, a);
			fp12_neg(d, d);
			TEST_ASSERT(fp12_cmp(c, d) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("subtraction has identity") {
			fp12_rand(a);
			fp12_zero(c);
			fp12_sub(d, a, c);
			TEST_ASSERT(fp12_cmp(d, a) == CMP_EQ, end);
		}
		TEST_END;

		TEST_BEGIN("subtraction has inverse") {
			fp12_rand(a);
			fp12_sub(c, a, a);
			TEST_ASSERT(fp12_is_zero(c), end);
		}
		TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	fp12_free(d);
	return code;
}

static int multiplication12(void) {
	int code = STS_ERR;
	fp12_t a, b, c, d, e, f;

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);
		fp12_new(d);
		fp12_new(e);
		fp12_new(f);

		TEST_BEGIN("multiplication is commutative") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_mul(d, a, b);
			fp12_mul(e, b, a);
			TEST_ASSERT(fp12_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication is associative") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_rand(c);
			fp12_mul(d, a, b);
			fp12_mul(d, d, c);
			fp12_mul(e, b, c);
			fp12_mul(e, a, e);
			TEST_ASSERT(fp12_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication is distributive") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_rand(c);
			fp12_add(d, a, b);
			fp12_mul(d, c, d);
			fp12_mul(e, c, a);
			fp12_mul(f, c, b);
			fp12_add(e, e, f);
			TEST_ASSERT(fp12_cmp(d, e) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication has identity") {
			fp12_zero(d);
			fp_set_dig(d[0][0][0], 1);
			fp12_mul(e, a, d);
			TEST_ASSERT(fp12_cmp(e, a) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("multiplication has zero property") {
			fp12_zero(d);
			fp12_mul(e, a, d);
			TEST_ASSERT(fp12_is_zero(e), end);
		} TEST_END;

#if PP_EXT == BASIC | !defined(STRIP)
		TEST_BEGIN("basic multiplication is correct") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_mul(c, a, b);
			fp12_mul_basic(d, a, b);
			TEST_ASSERT(fp12_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_EXT == LAZYR || !defined(STRIP)
		TEST_BEGIN("lazy reduced multiplication is correct") {
			fp12_rand(a);
			fp12_rand(b);
			fp12_mul(c, a, b);
			fp12_mul_lazyr(d, a, b);
			TEST_ASSERT(fp12_cmp(c, d) == CMP_EQ, end);
		} TEST_END;
#endif
	} CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	fp12_free(d);
	fp12_free(e);
	fp12_free(f);
	return code;
}

static int squaring12(void) {
	int code = STS_ERR;
	fp12_t a, b, c;

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);

		TEST_BEGIN("squaring is correct") {
			fp12_rand(a);
			fp12_mul(b, a, a);
			fp12_sqr(c, a);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	return code;
}

static int cyclotomic12(void) {
	int code = STS_ERR;
	fp12_t a, b, c, d[2], e[2];
	bn_t f;

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);
		fp12_new(d[0]);
		fp12_new(d[1]);
		fp12_new(e[0]);
		fp12_new(e[1]);
		bn_new(f);

		TEST_BEGIN("cyclotomic test is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			TEST_ASSERT(fp12_test_cyc(a) == 1, end);
		} TEST_END;

		TEST_BEGIN("compression in cyclotomic subgroup is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp12_back_cyc(c, a);
			TEST_ASSERT(fp12_cmp(a, c) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("simultaneous compression in cyclotomic subgroup is correct") {
			fp12_rand(d[0]);
			fp12_rand(d[1]);
			fp12_conv_cyc(d[0], d[0]);
			fp12_conv_cyc(d[1], d[1]);
			fp12_back_cyc_sim(e, d, 2);
			TEST_ASSERT(fp12_cmp(d[0], e[0]) == CMP_EQ &&
					fp12_cmp(d[1], e[1]) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("cyclotomic squaring is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp12_sqr(b, a);
			fp12_sqr_cyc(c, a);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;

#if PP_EXT == BASIC || !defined(STRIP)
		TEST_BEGIN("basic cyclotomic squaring is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp12_sqr_cyc(b, a);
			fp12_sqr_cyc_basic(c, a);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_EXT == LAZYR || !defined(STRIP)
		TEST_BEGIN("lazy reduced cyclotomic squaring is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp12_sqr_cyc(b, a);
			fp12_sqr_cyc_lazyr(c, a);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

		TEST_BEGIN("compressed squaring is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp2_zero(b[0][0]);
			fp2_zero(b[1][1]);
			fp2_zero(c[0][0]);
			fp2_zero(c[1][1]);
			fp12_sqr(b, a);
			fp12_sqr_pck(c, a);
			fp12_back_cyc(c, c);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;

#if PP_EXT == BASIC || !defined(STRIP)
		TEST_BEGIN("basic compressed squaring is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp2_zero(b[0][0]);
			fp2_zero(b[1][1]);
			fp2_zero(c[0][0]);
			fp2_zero(c[1][1]);
			fp12_sqr_pck(b, a);
			fp12_sqr_pck_basic(c, a);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

#if PP_EXT == LAZYR || !defined(STRIP)
		TEST_BEGIN("lazy reduced compressed squaring is correct") {
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp2_zero(b[0][0]);
			fp2_zero(b[1][1]);
			fp2_zero(c[0][0]);
			fp2_zero(c[1][1]);
			fp12_sqr_pck(b, a);
			fp12_sqr_pck_lazyr(c, a);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
#endif

        TEST_BEGIN("cyclotomic exponentiation is correct") {
			bn_rand(f, BN_POS, FP_BITS);
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp12_exp(b, a, f);
			fp12_exp_cyc(c, a, f);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
        } TEST_END;

		TEST_BEGIN("sparse cyclotomic exponentiation is correct") {
			int g[3] = {0, 0, FP_BITS - 1};
			do {
				bn_rand(f, BN_POS, BN_DIGIT);
				g[1] = f->dp[0] % FP_BITS;
			} while (g[1] == 0 || g[1] == FP_BITS - 1);
			bn_set_2b(f, FP_BITS - 1);
			bn_set_bit(f, g[1], 1);
			bn_set_bit(f, 0, 1);
			fp12_rand(a);
			fp12_conv_cyc(a, a);
			fp12_exp(b, a, f);
			fp12_exp_cyc_sps(c, a, g, 3);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	fp12_free(d[0]);
	fp12_free(d[1]);
	fp12_free(e[0]);
	fp12_free(e[1]);
	bn_free(f);
	return code;
}

static int inversion12(void) {
	int code = STS_ERR;
	fp12_t a, b, c;

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);

		TEST_BEGIN("inversion is correct") {
			fp12_rand(a);
			fp12_inv(b, a);
			fp12_mul(c, a, b);
			fp12_zero(b);
			fp_set_dig(b[0][0][0], 1);
			TEST_ASSERT(fp12_cmp(c, b) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("inversion of a unitary element is correct") {
			fp12_rand(a);
			fp12_conv_uni(a, a);
			fp12_inv(b, a);
			fp12_inv_uni(c, a);
			TEST_ASSERT(fp12_cmp(b, c) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	return code;
}

static int exponentiation12(void) {
	int code = STS_ERR;
	fp12_t a, b, c;
	bn_t d;

	fp12_null(a);
	fp12_null(b);
	fp12_null(c);
	bn_null(d);

	TRY {
		fp12_new(a);
		fp12_new(b);
		fp12_new(c);
		bn_new(d);

		TEST_BEGIN("frobenius and exponentiation are consistent") {
			fp12_rand(a);
			fp12_frb(b, a, 1);
			d->used = FP_DIGS;
			dv_copy(d->dp, fp_prime_get(), FP_DIGS);
			fp12_exp(c, a, d);
			TEST_ASSERT(fp12_cmp(c, b) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("frobenius and squared frobenius are consistent") {
			fp12_rand(a);
			fp12_frb(b, a, 1);
			fp12_frb(b, b, 1);
			fp12_frb(c, a, 2);
			TEST_ASSERT(fp12_cmp(c, b) == CMP_EQ, end);
		} TEST_END;

		TEST_BEGIN("frobenius and cubed frobenius are consistent") {
			fp12_rand(a);
			fp12_frb(b, a, 1);
			fp12_frb(b, b, 1);
			fp12_frb(b, b, 1);
			fp12_frb(c, a, 3);
			TEST_ASSERT(fp12_cmp(c, b) == CMP_EQ, end);
		} TEST_END;
	}
	CATCH_ANY {
		util_print("FATAL ERROR!\n");
		ERROR(end);
	}
	code = STS_OK;
  end:
	fp12_free(a);
	fp12_free(b);
	fp12_free(c);
	bn_free(d);
	return code;
}

int main(void) {
	core_init();

	if (ep_param_set_any_pairf() == STS_ERR) {
		THROW(ERR_NO_CURVE);
		core_clean();
		return 0;
	}

	util_banner("Tests for the FPX module", 0);

	util_banner("Quadratic extension:", 0);
	util_banner("Utilities:", 1);

	if (memory2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (util2() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Arithmetic:", 1);

	if (addition2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (subtraction2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (doubling2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (multiplication2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (squaring2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (inversion2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (exponentiation2() != STS_OK) {
		core_clean();
		return 1;
	}

	if (square_root2() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Sextic extension:", 0);
	util_banner("Utilities:", 1);

	if (memory6() != STS_OK) {
		core_clean();
		return 1;
	}

	if (util6() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Arithmetic:", 1);

	if (addition6() != STS_OK) {
		core_clean();
		return 1;
	}

	if (subtraction6() != STS_OK) {
		core_clean();
		return 1;
	}

	if (doubling6() != STS_OK) {
		core_clean();
		return 1;
	}

	if (multiplication6() != STS_OK) {
		core_clean();
		return 1;
	}

	if (squaring6() != STS_OK) {
		core_clean();
		return 1;
	}

	if (inversion6() != STS_OK) {
		core_clean();
		return 1;
	}

	if (exponentiation6() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Dodecic extension:", 0);
	util_banner("Utilities:", 1);

	if (memory12() != STS_OK) {
		core_clean();
		return 1;
	}

	if (util12() != STS_OK) {
		core_clean();
		return 1;
	}

	util_banner("Arithmetic:", 1);

	if (addition12() != STS_OK) {
		core_clean();
		return 1;
	}

	if (subtraction12() != STS_OK) {
		core_clean();
		return 1;
	}

	if (multiplication12() != STS_OK) {
		core_clean();
		return 1;
	}

	if (squaring12() != STS_OK) {
		core_clean();
		return 1;
	}

	if (cyclotomic12() != STS_OK) {
		core_clean();
		return 1;
	}

	if (inversion12() != STS_OK) {
		core_clean();
		return 1;
	}

	if (exponentiation12() != STS_OK) {
		core_clean();
		return 1;
	}

	core_clean();
	return 0;
}