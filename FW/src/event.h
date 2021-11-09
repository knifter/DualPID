#ifndef __EVENT_H
#define __EVENT_H

#include <stdint.h>
#include <tools-keys.h>

typedef enum : uint32_t
{
	KEY_NONE = 0x00,
	KEY_A = 0x01,
	KEY_B = 0x02,
	KEY_C = 0x04,
	KEY_E1 = 0x08,       // External 1
	KEY_E2 = 0x10,       // External 2
	KEY_P = 0x20,
	KEY_AB = KEY_A | KEY_B,
	KEY_AC = KEY_A | KEY_C,
	KEY_BC = KEY_B | KEY_C,
	KEY_ABC = KEY_A | KEY_B | KEY_C,
	KEY_A_LONG = KEY_A | KEYTOOL_LONG_MASK,
	KEY_B_LONG = KEY_B | KEYTOOL_LONG_MASK,
	KEY_C_LONG = KEY_C | KEYTOOL_LONG_MASK,
	KEY_P_LONG = KEY_P | KEYTOOL_LONG_MASK,
	KEY_AB_LONG = KEY_AB | KEYTOOL_LONG_MASK,
	KEY_AC_LONG = KEY_AC | KEYTOOL_LONG_MASK,
	KEY_BC_LONG = KEY_BC | KEYTOOL_LONG_MASK,
	KEY_ABC_LONG = KEY_ABC | KEYTOOL_LONG_MASK
} event_t;

#endif // __EVENT_H
