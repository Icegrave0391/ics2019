#include "rtl/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
			rtl_get_OF(dest);
			break;
		case CC_B:         // Jump near if below (CF=1)
			rtl_get_CF(dest);
			break;
    case CC_E:
			rtl_get_ZF(dest);
			break;
    case CC_BE:  // CF or OF
			rtl_get_CF(&s0);
			rtl_get_ZF(&s1);
			rtl_or(dest, &s0, &s1);
			break;
    case CC_S:
			rtl_get_SF(dest);
			break;
    case CC_L:  // SF != OF
			rtl_get_SF(&s0);
			rtl_get_OF(&s1);
			rtl_xor(dest, &s0, &s1);
			break;
    case CC_LE:  // ZF or SF != OF
			rtl_get_SF(&s0);
			rtl_get_OF(&s1);
			rtl_xor(&s0, &s0, &s1);
			rtl_get_ZF(&s1);
			rtl_or(dest, &s0, &s1);
			break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}
