#ifndef BLENDMODES_H
#define BLENDMODES_H

#ifdef TARGET_PC
// Material blend modes.
enum
{
	KRBM_OPAQUE,
	KRBM_PUNCHTHROUGH,

	KRBM_BLEND,
	KRBM_ADDITIVE,
	KRBM_SUBTRACTIVE,

	KRBM_CONST_BLEND,
	KRBM_CONST_ADDITIVE,
	KRBM_CONST_SUBTRACTIVE,

	KRBM_MAX_BLEND_MODES
};

#endif

#endif