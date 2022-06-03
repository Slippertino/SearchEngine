/* Generated by Snowball 2.2.0 - https://snowballstem.org/ */

#include "../runtime/header.h"

#ifdef __cplusplus
extern "C" {
#endif
extern int italian_ISO_8859_1_stem(struct SN_env * z);
#ifdef __cplusplus
}
#endif
static int r_vowel_suffix(struct SN_env * z);
static int r_verb_suffix(struct SN_env * z);
static int r_standard_suffix(struct SN_env * z);
static int r_attached_pronoun(struct SN_env * z);
static int r_R2(struct SN_env * z);
static int r_R1(struct SN_env * z);
static int r_RV(struct SN_env * z);
static int r_mark_regions(struct SN_env * z);
static int r_postlude(struct SN_env * z);
static int r_prelude(struct SN_env * z);
#ifdef __cplusplus
extern "C" {
#endif


extern struct SN_env * italian_ISO_8859_1_create_env(void);
extern void italian_ISO_8859_1_close_env(struct SN_env * z);


#ifdef __cplusplus
}
#endif
static const symbol s_0_1[2] = { 'q', 'u' };
static const symbol s_0_2[1] = { 0xE1 };
static const symbol s_0_3[1] = { 0xE9 };
static const symbol s_0_4[1] = { 0xED };
static const symbol s_0_5[1] = { 0xF3 };
static const symbol s_0_6[1] = { 0xFA };

static const struct among a_0[7] =
{
{ 0, 0, -1, 7, 0},
{ 2, s_0_1, 0, 6, 0},
{ 1, s_0_2, 0, 1, 0},
{ 1, s_0_3, 0, 2, 0},
{ 1, s_0_4, 0, 3, 0},
{ 1, s_0_5, 0, 4, 0},
{ 1, s_0_6, 0, 5, 0}
};

static const symbol s_1_1[1] = { 'I' };
static const symbol s_1_2[1] = { 'U' };

static const struct among a_1[3] =
{
{ 0, 0, -1, 3, 0},
{ 1, s_1_1, 0, 1, 0},
{ 1, s_1_2, 0, 2, 0}
};

static const symbol s_2_0[2] = { 'l', 'a' };
static const symbol s_2_1[4] = { 'c', 'e', 'l', 'a' };
static const symbol s_2_2[6] = { 'g', 'l', 'i', 'e', 'l', 'a' };
static const symbol s_2_3[4] = { 'm', 'e', 'l', 'a' };
static const symbol s_2_4[4] = { 't', 'e', 'l', 'a' };
static const symbol s_2_5[4] = { 'v', 'e', 'l', 'a' };
static const symbol s_2_6[2] = { 'l', 'e' };
static const symbol s_2_7[4] = { 'c', 'e', 'l', 'e' };
static const symbol s_2_8[6] = { 'g', 'l', 'i', 'e', 'l', 'e' };
static const symbol s_2_9[4] = { 'm', 'e', 'l', 'e' };
static const symbol s_2_10[4] = { 't', 'e', 'l', 'e' };
static const symbol s_2_11[4] = { 'v', 'e', 'l', 'e' };
static const symbol s_2_12[2] = { 'n', 'e' };
static const symbol s_2_13[4] = { 'c', 'e', 'n', 'e' };
static const symbol s_2_14[6] = { 'g', 'l', 'i', 'e', 'n', 'e' };
static const symbol s_2_15[4] = { 'm', 'e', 'n', 'e' };
static const symbol s_2_16[4] = { 's', 'e', 'n', 'e' };
static const symbol s_2_17[4] = { 't', 'e', 'n', 'e' };
static const symbol s_2_18[4] = { 'v', 'e', 'n', 'e' };
static const symbol s_2_19[2] = { 'c', 'i' };
static const symbol s_2_20[2] = { 'l', 'i' };
static const symbol s_2_21[4] = { 'c', 'e', 'l', 'i' };
static const symbol s_2_22[6] = { 'g', 'l', 'i', 'e', 'l', 'i' };
static const symbol s_2_23[4] = { 'm', 'e', 'l', 'i' };
static const symbol s_2_24[4] = { 't', 'e', 'l', 'i' };
static const symbol s_2_25[4] = { 'v', 'e', 'l', 'i' };
static const symbol s_2_26[3] = { 'g', 'l', 'i' };
static const symbol s_2_27[2] = { 'm', 'i' };
static const symbol s_2_28[2] = { 's', 'i' };
static const symbol s_2_29[2] = { 't', 'i' };
static const symbol s_2_30[2] = { 'v', 'i' };
static const symbol s_2_31[2] = { 'l', 'o' };
static const symbol s_2_32[4] = { 'c', 'e', 'l', 'o' };
static const symbol s_2_33[6] = { 'g', 'l', 'i', 'e', 'l', 'o' };
static const symbol s_2_34[4] = { 'm', 'e', 'l', 'o' };
static const symbol s_2_35[4] = { 't', 'e', 'l', 'o' };
static const symbol s_2_36[4] = { 'v', 'e', 'l', 'o' };

static const struct among a_2[37] =
{
{ 2, s_2_0, -1, -1, 0},
{ 4, s_2_1, 0, -1, 0},
{ 6, s_2_2, 0, -1, 0},
{ 4, s_2_3, 0, -1, 0},
{ 4, s_2_4, 0, -1, 0},
{ 4, s_2_5, 0, -1, 0},
{ 2, s_2_6, -1, -1, 0},
{ 4, s_2_7, 6, -1, 0},
{ 6, s_2_8, 6, -1, 0},
{ 4, s_2_9, 6, -1, 0},
{ 4, s_2_10, 6, -1, 0},
{ 4, s_2_11, 6, -1, 0},
{ 2, s_2_12, -1, -1, 0},
{ 4, s_2_13, 12, -1, 0},
{ 6, s_2_14, 12, -1, 0},
{ 4, s_2_15, 12, -1, 0},
{ 4, s_2_16, 12, -1, 0},
{ 4, s_2_17, 12, -1, 0},
{ 4, s_2_18, 12, -1, 0},
{ 2, s_2_19, -1, -1, 0},
{ 2, s_2_20, -1, -1, 0},
{ 4, s_2_21, 20, -1, 0},
{ 6, s_2_22, 20, -1, 0},
{ 4, s_2_23, 20, -1, 0},
{ 4, s_2_24, 20, -1, 0},
{ 4, s_2_25, 20, -1, 0},
{ 3, s_2_26, 20, -1, 0},
{ 2, s_2_27, -1, -1, 0},
{ 2, s_2_28, -1, -1, 0},
{ 2, s_2_29, -1, -1, 0},
{ 2, s_2_30, -1, -1, 0},
{ 2, s_2_31, -1, -1, 0},
{ 4, s_2_32, 31, -1, 0},
{ 6, s_2_33, 31, -1, 0},
{ 4, s_2_34, 31, -1, 0},
{ 4, s_2_35, 31, -1, 0},
{ 4, s_2_36, 31, -1, 0}
};

static const symbol s_3_0[4] = { 'a', 'n', 'd', 'o' };
static const symbol s_3_1[4] = { 'e', 'n', 'd', 'o' };
static const symbol s_3_2[2] = { 'a', 'r' };
static const symbol s_3_3[2] = { 'e', 'r' };
static const symbol s_3_4[2] = { 'i', 'r' };

static const struct among a_3[5] =
{
{ 4, s_3_0, -1, 1, 0},
{ 4, s_3_1, -1, 1, 0},
{ 2, s_3_2, -1, 2, 0},
{ 2, s_3_3, -1, 2, 0},
{ 2, s_3_4, -1, 2, 0}
};

static const symbol s_4_0[2] = { 'i', 'c' };
static const symbol s_4_1[4] = { 'a', 'b', 'i', 'l' };
static const symbol s_4_2[2] = { 'o', 's' };
static const symbol s_4_3[2] = { 'i', 'v' };

static const struct among a_4[4] =
{
{ 2, s_4_0, -1, -1, 0},
{ 4, s_4_1, -1, -1, 0},
{ 2, s_4_2, -1, -1, 0},
{ 2, s_4_3, -1, 1, 0}
};

static const symbol s_5_0[2] = { 'i', 'c' };
static const symbol s_5_1[4] = { 'a', 'b', 'i', 'l' };
static const symbol s_5_2[2] = { 'i', 'v' };

static const struct among a_5[3] =
{
{ 2, s_5_0, -1, 1, 0},
{ 4, s_5_1, -1, 1, 0},
{ 2, s_5_2, -1, 1, 0}
};

static const symbol s_6_0[3] = { 'i', 'c', 'a' };
static const symbol s_6_1[5] = { 'l', 'o', 'g', 'i', 'a' };
static const symbol s_6_2[3] = { 'o', 's', 'a' };
static const symbol s_6_3[4] = { 'i', 's', 't', 'a' };
static const symbol s_6_4[3] = { 'i', 'v', 'a' };
static const symbol s_6_5[4] = { 'a', 'n', 'z', 'a' };
static const symbol s_6_6[4] = { 'e', 'n', 'z', 'a' };
static const symbol s_6_7[3] = { 'i', 'c', 'e' };
static const symbol s_6_8[6] = { 'a', 't', 'r', 'i', 'c', 'e' };
static const symbol s_6_9[4] = { 'i', 'c', 'h', 'e' };
static const symbol s_6_10[5] = { 'l', 'o', 'g', 'i', 'e' };
static const symbol s_6_11[5] = { 'a', 'b', 'i', 'l', 'e' };
static const symbol s_6_12[5] = { 'i', 'b', 'i', 'l', 'e' };
static const symbol s_6_13[6] = { 'u', 's', 'i', 'o', 'n', 'e' };
static const symbol s_6_14[6] = { 'a', 'z', 'i', 'o', 'n', 'e' };
static const symbol s_6_15[6] = { 'u', 'z', 'i', 'o', 'n', 'e' };
static const symbol s_6_16[5] = { 'a', 't', 'o', 'r', 'e' };
static const symbol s_6_17[3] = { 'o', 's', 'e' };
static const symbol s_6_18[4] = { 'a', 'n', 't', 'e' };
static const symbol s_6_19[5] = { 'm', 'e', 'n', 't', 'e' };
static const symbol s_6_20[6] = { 'a', 'm', 'e', 'n', 't', 'e' };
static const symbol s_6_21[4] = { 'i', 's', 't', 'e' };
static const symbol s_6_22[3] = { 'i', 'v', 'e' };
static const symbol s_6_23[4] = { 'a', 'n', 'z', 'e' };
static const symbol s_6_24[4] = { 'e', 'n', 'z', 'e' };
static const symbol s_6_25[3] = { 'i', 'c', 'i' };
static const symbol s_6_26[6] = { 'a', 't', 'r', 'i', 'c', 'i' };
static const symbol s_6_27[4] = { 'i', 'c', 'h', 'i' };
static const symbol s_6_28[5] = { 'a', 'b', 'i', 'l', 'i' };
static const symbol s_6_29[5] = { 'i', 'b', 'i', 'l', 'i' };
static const symbol s_6_30[4] = { 'i', 's', 'm', 'i' };
static const symbol s_6_31[6] = { 'u', 's', 'i', 'o', 'n', 'i' };
static const symbol s_6_32[6] = { 'a', 'z', 'i', 'o', 'n', 'i' };
static const symbol s_6_33[6] = { 'u', 'z', 'i', 'o', 'n', 'i' };
static const symbol s_6_34[5] = { 'a', 't', 'o', 'r', 'i' };
static const symbol s_6_35[3] = { 'o', 's', 'i' };
static const symbol s_6_36[4] = { 'a', 'n', 't', 'i' };
static const symbol s_6_37[6] = { 'a', 'm', 'e', 'n', 't', 'i' };
static const symbol s_6_38[6] = { 'i', 'm', 'e', 'n', 't', 'i' };
static const symbol s_6_39[4] = { 'i', 's', 't', 'i' };
static const symbol s_6_40[3] = { 'i', 'v', 'i' };
static const symbol s_6_41[3] = { 'i', 'c', 'o' };
static const symbol s_6_42[4] = { 'i', 's', 'm', 'o' };
static const symbol s_6_43[3] = { 'o', 's', 'o' };
static const symbol s_6_44[6] = { 'a', 'm', 'e', 'n', 't', 'o' };
static const symbol s_6_45[6] = { 'i', 'm', 'e', 'n', 't', 'o' };
static const symbol s_6_46[3] = { 'i', 'v', 'o' };
static const symbol s_6_47[3] = { 'i', 't', 0xE0 };
static const symbol s_6_48[4] = { 'i', 's', 't', 0xE0 };
static const symbol s_6_49[4] = { 'i', 's', 't', 0xE8 };
static const symbol s_6_50[4] = { 'i', 's', 't', 0xEC };

static const struct among a_6[51] =
{
{ 3, s_6_0, -1, 1, 0},
{ 5, s_6_1, -1, 3, 0},
{ 3, s_6_2, -1, 1, 0},
{ 4, s_6_3, -1, 1, 0},
{ 3, s_6_4, -1, 9, 0},
{ 4, s_6_5, -1, 1, 0},
{ 4, s_6_6, -1, 5, 0},
{ 3, s_6_7, -1, 1, 0},
{ 6, s_6_8, 7, 1, 0},
{ 4, s_6_9, -1, 1, 0},
{ 5, s_6_10, -1, 3, 0},
{ 5, s_6_11, -1, 1, 0},
{ 5, s_6_12, -1, 1, 0},
{ 6, s_6_13, -1, 4, 0},
{ 6, s_6_14, -1, 2, 0},
{ 6, s_6_15, -1, 4, 0},
{ 5, s_6_16, -1, 2, 0},
{ 3, s_6_17, -1, 1, 0},
{ 4, s_6_18, -1, 1, 0},
{ 5, s_6_19, -1, 1, 0},
{ 6, s_6_20, 19, 7, 0},
{ 4, s_6_21, -1, 1, 0},
{ 3, s_6_22, -1, 9, 0},
{ 4, s_6_23, -1, 1, 0},
{ 4, s_6_24, -1, 5, 0},
{ 3, s_6_25, -1, 1, 0},
{ 6, s_6_26, 25, 1, 0},
{ 4, s_6_27, -1, 1, 0},
{ 5, s_6_28, -1, 1, 0},
{ 5, s_6_29, -1, 1, 0},
{ 4, s_6_30, -1, 1, 0},
{ 6, s_6_31, -1, 4, 0},
{ 6, s_6_32, -1, 2, 0},
{ 6, s_6_33, -1, 4, 0},
{ 5, s_6_34, -1, 2, 0},
{ 3, s_6_35, -1, 1, 0},
{ 4, s_6_36, -1, 1, 0},
{ 6, s_6_37, -1, 6, 0},
{ 6, s_6_38, -1, 6, 0},
{ 4, s_6_39, -1, 1, 0},
{ 3, s_6_40, -1, 9, 0},
{ 3, s_6_41, -1, 1, 0},
{ 4, s_6_42, -1, 1, 0},
{ 3, s_6_43, -1, 1, 0},
{ 6, s_6_44, -1, 6, 0},
{ 6, s_6_45, -1, 6, 0},
{ 3, s_6_46, -1, 9, 0},
{ 3, s_6_47, -1, 8, 0},
{ 4, s_6_48, -1, 1, 0},
{ 4, s_6_49, -1, 1, 0},
{ 4, s_6_50, -1, 1, 0}
};

static const symbol s_7_0[4] = { 'i', 's', 'c', 'a' };
static const symbol s_7_1[4] = { 'e', 'n', 'd', 'a' };
static const symbol s_7_2[3] = { 'a', 't', 'a' };
static const symbol s_7_3[3] = { 'i', 't', 'a' };
static const symbol s_7_4[3] = { 'u', 't', 'a' };
static const symbol s_7_5[3] = { 'a', 'v', 'a' };
static const symbol s_7_6[3] = { 'e', 'v', 'a' };
static const symbol s_7_7[3] = { 'i', 'v', 'a' };
static const symbol s_7_8[6] = { 'e', 'r', 'e', 'b', 'b', 'e' };
static const symbol s_7_9[6] = { 'i', 'r', 'e', 'b', 'b', 'e' };
static const symbol s_7_10[4] = { 'i', 's', 'c', 'e' };
static const symbol s_7_11[4] = { 'e', 'n', 'd', 'e' };
static const symbol s_7_12[3] = { 'a', 'r', 'e' };
static const symbol s_7_13[3] = { 'e', 'r', 'e' };
static const symbol s_7_14[3] = { 'i', 'r', 'e' };
static const symbol s_7_15[4] = { 'a', 's', 's', 'e' };
static const symbol s_7_16[3] = { 'a', 't', 'e' };
static const symbol s_7_17[5] = { 'a', 'v', 'a', 't', 'e' };
static const symbol s_7_18[5] = { 'e', 'v', 'a', 't', 'e' };
static const symbol s_7_19[5] = { 'i', 'v', 'a', 't', 'e' };
static const symbol s_7_20[3] = { 'e', 't', 'e' };
static const symbol s_7_21[5] = { 'e', 'r', 'e', 't', 'e' };
static const symbol s_7_22[5] = { 'i', 'r', 'e', 't', 'e' };
static const symbol s_7_23[3] = { 'i', 't', 'e' };
static const symbol s_7_24[6] = { 'e', 'r', 'e', 's', 't', 'e' };
static const symbol s_7_25[6] = { 'i', 'r', 'e', 's', 't', 'e' };
static const symbol s_7_26[3] = { 'u', 't', 'e' };
static const symbol s_7_27[4] = { 'e', 'r', 'a', 'i' };
static const symbol s_7_28[4] = { 'i', 'r', 'a', 'i' };
static const symbol s_7_29[4] = { 'i', 's', 'c', 'i' };
static const symbol s_7_30[4] = { 'e', 'n', 'd', 'i' };
static const symbol s_7_31[4] = { 'e', 'r', 'e', 'i' };
static const symbol s_7_32[4] = { 'i', 'r', 'e', 'i' };
static const symbol s_7_33[4] = { 'a', 's', 's', 'i' };
static const symbol s_7_34[3] = { 'a', 't', 'i' };
static const symbol s_7_35[3] = { 'i', 't', 'i' };
static const symbol s_7_36[6] = { 'e', 'r', 'e', 's', 't', 'i' };
static const symbol s_7_37[6] = { 'i', 'r', 'e', 's', 't', 'i' };
static const symbol s_7_38[3] = { 'u', 't', 'i' };
static const symbol s_7_39[3] = { 'a', 'v', 'i' };
static const symbol s_7_40[3] = { 'e', 'v', 'i' };
static const symbol s_7_41[3] = { 'i', 'v', 'i' };
static const symbol s_7_42[4] = { 'i', 's', 'c', 'o' };
static const symbol s_7_43[4] = { 'a', 'n', 'd', 'o' };
static const symbol s_7_44[4] = { 'e', 'n', 'd', 'o' };
static const symbol s_7_45[4] = { 'Y', 'a', 'm', 'o' };
static const symbol s_7_46[4] = { 'i', 'a', 'm', 'o' };
static const symbol s_7_47[5] = { 'a', 'v', 'a', 'm', 'o' };
static const symbol s_7_48[5] = { 'e', 'v', 'a', 'm', 'o' };
static const symbol s_7_49[5] = { 'i', 'v', 'a', 'm', 'o' };
static const symbol s_7_50[5] = { 'e', 'r', 'e', 'm', 'o' };
static const symbol s_7_51[5] = { 'i', 'r', 'e', 'm', 'o' };
static const symbol s_7_52[6] = { 'a', 's', 's', 'i', 'm', 'o' };
static const symbol s_7_53[4] = { 'a', 'm', 'm', 'o' };
static const symbol s_7_54[4] = { 'e', 'm', 'm', 'o' };
static const symbol s_7_55[6] = { 'e', 'r', 'e', 'm', 'm', 'o' };
static const symbol s_7_56[6] = { 'i', 'r', 'e', 'm', 'm', 'o' };
static const symbol s_7_57[4] = { 'i', 'm', 'm', 'o' };
static const symbol s_7_58[3] = { 'a', 'n', 'o' };
static const symbol s_7_59[6] = { 'i', 's', 'c', 'a', 'n', 'o' };
static const symbol s_7_60[5] = { 'a', 'v', 'a', 'n', 'o' };
static const symbol s_7_61[5] = { 'e', 'v', 'a', 'n', 'o' };
static const symbol s_7_62[5] = { 'i', 'v', 'a', 'n', 'o' };
static const symbol s_7_63[6] = { 'e', 'r', 'a', 'n', 'n', 'o' };
static const symbol s_7_64[6] = { 'i', 'r', 'a', 'n', 'n', 'o' };
static const symbol s_7_65[3] = { 'o', 'n', 'o' };
static const symbol s_7_66[6] = { 'i', 's', 'c', 'o', 'n', 'o' };
static const symbol s_7_67[5] = { 'a', 'r', 'o', 'n', 'o' };
static const symbol s_7_68[5] = { 'e', 'r', 'o', 'n', 'o' };
static const symbol s_7_69[5] = { 'i', 'r', 'o', 'n', 'o' };
static const symbol s_7_70[8] = { 'e', 'r', 'e', 'b', 'b', 'e', 'r', 'o' };
static const symbol s_7_71[8] = { 'i', 'r', 'e', 'b', 'b', 'e', 'r', 'o' };
static const symbol s_7_72[6] = { 'a', 's', 's', 'e', 'r', 'o' };
static const symbol s_7_73[6] = { 'e', 's', 's', 'e', 'r', 'o' };
static const symbol s_7_74[6] = { 'i', 's', 's', 'e', 'r', 'o' };
static const symbol s_7_75[3] = { 'a', 't', 'o' };
static const symbol s_7_76[3] = { 'i', 't', 'o' };
static const symbol s_7_77[3] = { 'u', 't', 'o' };
static const symbol s_7_78[3] = { 'a', 'v', 'o' };
static const symbol s_7_79[3] = { 'e', 'v', 'o' };
static const symbol s_7_80[3] = { 'i', 'v', 'o' };
static const symbol s_7_81[2] = { 'a', 'r' };
static const symbol s_7_82[2] = { 'i', 'r' };
static const symbol s_7_83[3] = { 'e', 'r', 0xE0 };
static const symbol s_7_84[3] = { 'i', 'r', 0xE0 };
static const symbol s_7_85[3] = { 'e', 'r', 0xF2 };
static const symbol s_7_86[3] = { 'i', 'r', 0xF2 };

static const struct among a_7[87] =
{
{ 4, s_7_0, -1, 1, 0},
{ 4, s_7_1, -1, 1, 0},
{ 3, s_7_2, -1, 1, 0},
{ 3, s_7_3, -1, 1, 0},
{ 3, s_7_4, -1, 1, 0},
{ 3, s_7_5, -1, 1, 0},
{ 3, s_7_6, -1, 1, 0},
{ 3, s_7_7, -1, 1, 0},
{ 6, s_7_8, -1, 1, 0},
{ 6, s_7_9, -1, 1, 0},
{ 4, s_7_10, -1, 1, 0},
{ 4, s_7_11, -1, 1, 0},
{ 3, s_7_12, -1, 1, 0},
{ 3, s_7_13, -1, 1, 0},
{ 3, s_7_14, -1, 1, 0},
{ 4, s_7_15, -1, 1, 0},
{ 3, s_7_16, -1, 1, 0},
{ 5, s_7_17, 16, 1, 0},
{ 5, s_7_18, 16, 1, 0},
{ 5, s_7_19, 16, 1, 0},
{ 3, s_7_20, -1, 1, 0},
{ 5, s_7_21, 20, 1, 0},
{ 5, s_7_22, 20, 1, 0},
{ 3, s_7_23, -1, 1, 0},
{ 6, s_7_24, -1, 1, 0},
{ 6, s_7_25, -1, 1, 0},
{ 3, s_7_26, -1, 1, 0},
{ 4, s_7_27, -1, 1, 0},
{ 4, s_7_28, -1, 1, 0},
{ 4, s_7_29, -1, 1, 0},
{ 4, s_7_30, -1, 1, 0},
{ 4, s_7_31, -1, 1, 0},
{ 4, s_7_32, -1, 1, 0},
{ 4, s_7_33, -1, 1, 0},
{ 3, s_7_34, -1, 1, 0},
{ 3, s_7_35, -1, 1, 0},
{ 6, s_7_36, -1, 1, 0},
{ 6, s_7_37, -1, 1, 0},
{ 3, s_7_38, -1, 1, 0},
{ 3, s_7_39, -1, 1, 0},
{ 3, s_7_40, -1, 1, 0},
{ 3, s_7_41, -1, 1, 0},
{ 4, s_7_42, -1, 1, 0},
{ 4, s_7_43, -1, 1, 0},
{ 4, s_7_44, -1, 1, 0},
{ 4, s_7_45, -1, 1, 0},
{ 4, s_7_46, -1, 1, 0},
{ 5, s_7_47, -1, 1, 0},
{ 5, s_7_48, -1, 1, 0},
{ 5, s_7_49, -1, 1, 0},
{ 5, s_7_50, -1, 1, 0},
{ 5, s_7_51, -1, 1, 0},
{ 6, s_7_52, -1, 1, 0},
{ 4, s_7_53, -1, 1, 0},
{ 4, s_7_54, -1, 1, 0},
{ 6, s_7_55, 54, 1, 0},
{ 6, s_7_56, 54, 1, 0},
{ 4, s_7_57, -1, 1, 0},
{ 3, s_7_58, -1, 1, 0},
{ 6, s_7_59, 58, 1, 0},
{ 5, s_7_60, 58, 1, 0},
{ 5, s_7_61, 58, 1, 0},
{ 5, s_7_62, 58, 1, 0},
{ 6, s_7_63, -1, 1, 0},
{ 6, s_7_64, -1, 1, 0},
{ 3, s_7_65, -1, 1, 0},
{ 6, s_7_66, 65, 1, 0},
{ 5, s_7_67, 65, 1, 0},
{ 5, s_7_68, 65, 1, 0},
{ 5, s_7_69, 65, 1, 0},
{ 8, s_7_70, -1, 1, 0},
{ 8, s_7_71, -1, 1, 0},
{ 6, s_7_72, -1, 1, 0},
{ 6, s_7_73, -1, 1, 0},
{ 6, s_7_74, -1, 1, 0},
{ 3, s_7_75, -1, 1, 0},
{ 3, s_7_76, -1, 1, 0},
{ 3, s_7_77, -1, 1, 0},
{ 3, s_7_78, -1, 1, 0},
{ 3, s_7_79, -1, 1, 0},
{ 3, s_7_80, -1, 1, 0},
{ 2, s_7_81, -1, 1, 0},
{ 2, s_7_82, -1, 1, 0},
{ 3, s_7_83, -1, 1, 0},
{ 3, s_7_84, -1, 1, 0},
{ 3, s_7_85, -1, 1, 0},
{ 3, s_7_86, -1, 1, 0}
};

static const unsigned char g_v[] = { 17, 65, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 8, 2, 1 };

static const unsigned char g_AEIO[] = { 17, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 8, 2 };

static const unsigned char g_CG[] = { 17 };

static const symbol s_0[] = { 0xE0 };
static const symbol s_1[] = { 0xE8 };
static const symbol s_2[] = { 0xEC };
static const symbol s_3[] = { 0xF2 };
static const symbol s_4[] = { 0xF9 };
static const symbol s_5[] = { 'q', 'U' };
static const symbol s_6[] = { 'U' };
static const symbol s_7[] = { 'I' };
static const symbol s_8[] = { 'i' };
static const symbol s_9[] = { 'u' };
static const symbol s_10[] = { 'e' };
static const symbol s_11[] = { 'i', 'c' };
static const symbol s_12[] = { 'l', 'o', 'g' };
static const symbol s_13[] = { 'u' };
static const symbol s_14[] = { 'e', 'n', 't', 'e' };
static const symbol s_15[] = { 'a', 't' };
static const symbol s_16[] = { 'a', 't' };
static const symbol s_17[] = { 'i', 'c' };

static int r_prelude(struct SN_env * z) {
    int among_var;
    {   int c_test1 = z->c;
        while(1) {
            int c2 = z->c;
            z->bra = z->c;
            among_var = find_among(z, a_0, 7);
            if (!(among_var)) goto lab0;
            z->ket = z->c;
            switch (among_var) {
                case 1:
                    {   int ret = slice_from_s(z, 1, s_0);
                        if (ret < 0) return ret;
                    }
                    break;
                case 2:
                    {   int ret = slice_from_s(z, 1, s_1);
                        if (ret < 0) return ret;
                    }
                    break;
                case 3:
                    {   int ret = slice_from_s(z, 1, s_2);
                        if (ret < 0) return ret;
                    }
                    break;
                case 4:
                    {   int ret = slice_from_s(z, 1, s_3);
                        if (ret < 0) return ret;
                    }
                    break;
                case 5:
                    {   int ret = slice_from_s(z, 1, s_4);
                        if (ret < 0) return ret;
                    }
                    break;
                case 6:
                    {   int ret = slice_from_s(z, 2, s_5);
                        if (ret < 0) return ret;
                    }
                    break;
                case 7:
                    if (z->c >= z->l) goto lab0;
                    z->c++;
                    break;
            }
            continue;
        lab0:
            z->c = c2;
            break;
        }
        z->c = c_test1;
    }
    while(1) {
        int c3 = z->c;
        while(1) {
            int c4 = z->c;
            if (in_grouping(z, g_v, 97, 249, 0)) goto lab2;
            z->bra = z->c;
            {   int c5 = z->c;
                if (z->c == z->l || z->p[z->c] != 'u') goto lab4;
                z->c++;
                z->ket = z->c;
                if (in_grouping(z, g_v, 97, 249, 0)) goto lab4;
                {   int ret = slice_from_s(z, 1, s_6);
                    if (ret < 0) return ret;
                }
                goto lab3;
            lab4:
                z->c = c5;
                if (z->c == z->l || z->p[z->c] != 'i') goto lab2;
                z->c++;
                z->ket = z->c;
                if (in_grouping(z, g_v, 97, 249, 0)) goto lab2;
                {   int ret = slice_from_s(z, 1, s_7);
                    if (ret < 0) return ret;
                }
            }
        lab3:
            z->c = c4;
            break;
        lab2:
            z->c = c4;
            if (z->c >= z->l) goto lab1;
            z->c++;
        }
        continue;
    lab1:
        z->c = c3;
        break;
    }
    return 1;
}

static int r_mark_regions(struct SN_env * z) {
    z->I[2] = z->l;
    z->I[1] = z->l;
    z->I[0] = z->l;
    {   int c1 = z->c;
        {   int c2 = z->c;
            if (in_grouping(z, g_v, 97, 249, 0)) goto lab2;
            {   int c3 = z->c;
                if (out_grouping(z, g_v, 97, 249, 0)) goto lab4;
                {   
                    int ret = out_grouping(z, g_v, 97, 249, 1);
                    if (ret < 0) goto lab4;
                    z->c += ret;
                }
                goto lab3;
            lab4:
                z->c = c3;
                if (in_grouping(z, g_v, 97, 249, 0)) goto lab2;
                {   
                    int ret = in_grouping(z, g_v, 97, 249, 1);
                    if (ret < 0) goto lab2;
                    z->c += ret;
                }
            }
        lab3:
            goto lab1;
        lab2:
            z->c = c2;
            if (out_grouping(z, g_v, 97, 249, 0)) goto lab0;
            {   int c4 = z->c;
                if (out_grouping(z, g_v, 97, 249, 0)) goto lab6;
                {   
                    int ret = out_grouping(z, g_v, 97, 249, 1);
                    if (ret < 0) goto lab6;
                    z->c += ret;
                }
                goto lab5;
            lab6:
                z->c = c4;
                if (in_grouping(z, g_v, 97, 249, 0)) goto lab0;
                if (z->c >= z->l) goto lab0;
                z->c++;
            }
        lab5:
            ;
        }
    lab1:
        z->I[2] = z->c;
    lab0:
        z->c = c1;
    }
    {   int c5 = z->c;
        {   
            int ret = out_grouping(z, g_v, 97, 249, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        {   
            int ret = in_grouping(z, g_v, 97, 249, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        z->I[1] = z->c;
        {   
            int ret = out_grouping(z, g_v, 97, 249, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        {   
            int ret = in_grouping(z, g_v, 97, 249, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        z->I[0] = z->c;
    lab7:
        z->c = c5;
    }
    return 1;
}

static int r_postlude(struct SN_env * z) {
    int among_var;
    while(1) {
        int c1 = z->c;
        z->bra = z->c;
        if (z->c >= z->l || (z->p[z->c + 0] != 73 && z->p[z->c + 0] != 85)) among_var = 3; else
        among_var = find_among(z, a_1, 3);
        if (!(among_var)) goto lab0;
        z->ket = z->c;
        switch (among_var) {
            case 1:
                {   int ret = slice_from_s(z, 1, s_8);
                    if (ret < 0) return ret;
                }
                break;
            case 2:
                {   int ret = slice_from_s(z, 1, s_9);
                    if (ret < 0) return ret;
                }
                break;
            case 3:
                if (z->c >= z->l) goto lab0;
                z->c++;
                break;
        }
        continue;
    lab0:
        z->c = c1;
        break;
    }
    return 1;
}

static int r_RV(struct SN_env * z) {
    if (!(z->I[2] <= z->c)) return 0;
    return 1;
}

static int r_R1(struct SN_env * z) {
    if (!(z->I[1] <= z->c)) return 0;
    return 1;
}

static int r_R2(struct SN_env * z) {
    if (!(z->I[0] <= z->c)) return 0;
    return 1;
}

static int r_attached_pronoun(struct SN_env * z) {
    int among_var;
    z->ket = z->c;
    if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((33314 >> (z->p[z->c - 1] & 0x1f)) & 1)) return 0;
    if (!(find_among_b(z, a_2, 37))) return 0;
    z->bra = z->c;
    if (z->c - 1 <= z->lb || (z->p[z->c - 1] != 111 && z->p[z->c - 1] != 114)) return 0;
    among_var = find_among_b(z, a_3, 5);
    if (!(among_var)) return 0;
    {   int ret = r_RV(z);
        if (ret <= 0) return ret;
    }
    switch (among_var) {
        case 1:
            {   int ret = slice_del(z);
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, s_10);
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int r_standard_suffix(struct SN_env * z) {
    int among_var;
    z->ket = z->c;
    among_var = find_among_b(z, a_6, 51);
    if (!(among_var)) return 0;
    z->bra = z->c;
    switch (among_var) {
        case 1:
            {   int ret = r_R2(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_del(z);
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = r_R2(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_del(z);
                if (ret < 0) return ret;
            }
            {   int m1 = z->l - z->c; (void)m1;
                z->ket = z->c;
                if (!(eq_s_b(z, 2, s_11))) { z->c = z->l - m1; goto lab0; }
                z->bra = z->c;
                {   int ret = r_R2(z);
                    if (ret == 0) { z->c = z->l - m1; goto lab0; }
                    if (ret < 0) return ret;
                }
                {   int ret = slice_del(z);
                    if (ret < 0) return ret;
                }
            lab0:
                ;
            }
            break;
        case 3:
            {   int ret = r_R2(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_from_s(z, 3, s_12);
                if (ret < 0) return ret;
            }
            break;
        case 4:
            {   int ret = r_R2(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_from_s(z, 1, s_13);
                if (ret < 0) return ret;
            }
            break;
        case 5:
            {   int ret = r_R2(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_from_s(z, 4, s_14);
                if (ret < 0) return ret;
            }
            break;
        case 6:
            {   int ret = r_RV(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_del(z);
                if (ret < 0) return ret;
            }
            break;
        case 7:
            {   int ret = r_R1(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_del(z);
                if (ret < 0) return ret;
            }
            {   int m2 = z->l - z->c; (void)m2;
                z->ket = z->c;
                if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((4722696 >> (z->p[z->c - 1] & 0x1f)) & 1)) { z->c = z->l - m2; goto lab1; }
                among_var = find_among_b(z, a_4, 4);
                if (!(among_var)) { z->c = z->l - m2; goto lab1; }
                z->bra = z->c;
                {   int ret = r_R2(z);
                    if (ret == 0) { z->c = z->l - m2; goto lab1; }
                    if (ret < 0) return ret;
                }
                {   int ret = slice_del(z);
                    if (ret < 0) return ret;
                }
                switch (among_var) {
                    case 1:
                        z->ket = z->c;
                        if (!(eq_s_b(z, 2, s_15))) { z->c = z->l - m2; goto lab1; }
                        z->bra = z->c;
                        {   int ret = r_R2(z);
                            if (ret == 0) { z->c = z->l - m2; goto lab1; }
                            if (ret < 0) return ret;
                        }
                        {   int ret = slice_del(z);
                            if (ret < 0) return ret;
                        }
                        break;
                }
            lab1:
                ;
            }
            break;
        case 8:
            {   int ret = r_R2(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_del(z);
                if (ret < 0) return ret;
            }
            {   int m3 = z->l - z->c; (void)m3;
                z->ket = z->c;
                if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((4198408 >> (z->p[z->c - 1] & 0x1f)) & 1)) { z->c = z->l - m3; goto lab2; }
                if (!(find_among_b(z, a_5, 3))) { z->c = z->l - m3; goto lab2; }
                z->bra = z->c;
                {   int ret = r_R2(z);
                    if (ret == 0) { z->c = z->l - m3; goto lab2; }
                    if (ret < 0) return ret;
                }
                {   int ret = slice_del(z);
                    if (ret < 0) return ret;
                }
            lab2:
                ;
            }
            break;
        case 9:
            {   int ret = r_R2(z);
                if (ret <= 0) return ret;
            }
            {   int ret = slice_del(z);
                if (ret < 0) return ret;
            }
            {   int m4 = z->l - z->c; (void)m4;
                z->ket = z->c;
                if (!(eq_s_b(z, 2, s_16))) { z->c = z->l - m4; goto lab3; }
                z->bra = z->c;
                {   int ret = r_R2(z);
                    if (ret == 0) { z->c = z->l - m4; goto lab3; }
                    if (ret < 0) return ret;
                }
                {   int ret = slice_del(z);
                    if (ret < 0) return ret;
                }
                z->ket = z->c;
                if (!(eq_s_b(z, 2, s_17))) { z->c = z->l - m4; goto lab3; }
                z->bra = z->c;
                {   int ret = r_R2(z);
                    if (ret == 0) { z->c = z->l - m4; goto lab3; }
                    if (ret < 0) return ret;
                }
                {   int ret = slice_del(z);
                    if (ret < 0) return ret;
                }
            lab3:
                ;
            }
            break;
    }
    return 1;
}

static int r_verb_suffix(struct SN_env * z) {

    {   int mlimit1;
        if (z->c < z->I[2]) return 0;
        mlimit1 = z->lb; z->lb = z->I[2];
        z->ket = z->c;
        if (!(find_among_b(z, a_7, 87))) { z->lb = mlimit1; return 0; }
        z->bra = z->c;
        {   int ret = slice_del(z);
            if (ret < 0) return ret;
        }
        z->lb = mlimit1;
    }
    return 1;
}

static int r_vowel_suffix(struct SN_env * z) {
    {   int m1 = z->l - z->c; (void)m1;
        z->ket = z->c;
        if (in_grouping_b(z, g_AEIO, 97, 242, 0)) { z->c = z->l - m1; goto lab0; }
        z->bra = z->c;
        {   int ret = r_RV(z);
            if (ret == 0) { z->c = z->l - m1; goto lab0; }
            if (ret < 0) return ret;
        }
        {   int ret = slice_del(z);
            if (ret < 0) return ret;
        }
        z->ket = z->c;
        if (z->c <= z->lb || z->p[z->c - 1] != 'i') { z->c = z->l - m1; goto lab0; }
        z->c--;
        z->bra = z->c;
        {   int ret = r_RV(z);
            if (ret == 0) { z->c = z->l - m1; goto lab0; }
            if (ret < 0) return ret;
        }
        {   int ret = slice_del(z);
            if (ret < 0) return ret;
        }
    lab0:
        ;
    }
    {   int m2 = z->l - z->c; (void)m2;
        z->ket = z->c;
        if (z->c <= z->lb || z->p[z->c - 1] != 'h') { z->c = z->l - m2; goto lab1; }
        z->c--;
        z->bra = z->c;
        if (in_grouping_b(z, g_CG, 99, 103, 0)) { z->c = z->l - m2; goto lab1; }
        {   int ret = r_RV(z);
            if (ret == 0) { z->c = z->l - m2; goto lab1; }
            if (ret < 0) return ret;
        }
        {   int ret = slice_del(z);
            if (ret < 0) return ret;
        }
    lab1:
        ;
    }
    return 1;
}

extern int italian_ISO_8859_1_stem(struct SN_env * z) {
    {   int c1 = z->c;
        {   int ret = r_prelude(z);
            if (ret < 0) return ret;
        }
        z->c = c1;
    }
    
    {   int ret = r_mark_regions(z);
        if (ret < 0) return ret;
    }
    z->lb = z->c; z->c = z->l;

    {   int m2 = z->l - z->c; (void)m2;
        {   int ret = r_attached_pronoun(z);
            if (ret < 0) return ret;
        }
        z->c = z->l - m2;
    }
    {   int m3 = z->l - z->c; (void)m3;
        {   int m4 = z->l - z->c; (void)m4;
            {   int ret = r_standard_suffix(z);
                if (ret == 0) goto lab2;
                if (ret < 0) return ret;
            }
            goto lab1;
        lab2:
            z->c = z->l - m4;
            {   int ret = r_verb_suffix(z);
                if (ret == 0) goto lab0;
                if (ret < 0) return ret;
            }
        }
    lab1:
    lab0:
        z->c = z->l - m3;
    }
    {   int m5 = z->l - z->c; (void)m5;
        {   int ret = r_vowel_suffix(z);
            if (ret < 0) return ret;
        }
        z->c = z->l - m5;
    }
    z->c = z->lb;
    {   int c6 = z->c;
        {   int ret = r_postlude(z);
            if (ret < 0) return ret;
        }
        z->c = c6;
    }
    return 1;
}

extern struct SN_env * italian_ISO_8859_1_create_env(void) { return SN_create_env(0, 3); }

extern void italian_ISO_8859_1_close_env(struct SN_env * z) { SN_close_env(z, 0); }

