/*
 * Minimal 802.11ah S1G helpers for DPP (MM_IOT / ESP-IDF hostap build).
 *
 * Full morse.c is not built here: it depends on struct ah_class and hostapd
 * fields that differ from this trimmed port. DPP hostapd only needs channel
 * mapping and country -> global operating class for presence announcement.
 */

#include "utils/includes.h"
#include "utils/morse.h"

struct s1g_ht_chan_pair {
	int s1g_channel;
	int ht_channel;
	int bw;
};

#define S1G_CHAN_COUNT (52)
#define S1G_CHAN_MIN (1)
#define S1G_CHAN_MAX (S1G_CHAN_COUNT - 1)

/* Default channelisation (AU/US/etc.) from morse.c */
static const struct s1g_ht_chan_pair s1g_ht_chan_pairs_default[] = {
	{-1, -1, -1},
	{1, 132, 1},
	{2, 134, 2},
	{3, 136, 1},
	{4, -1, -1},
	{5, 36, 1},
	{6, 38, 2},
	{7, 40, 1},
	{8, 42, 4},
	{9, 44, 1},
	{10, 46, 2},
	{11, 48, 1},
	{12, 50, 8},
	{13, 52, 1},
	{14, 54, 2},
	{15, 56, 1},
	{16, 58, 4},
	{17, 60, 1},
	{18, 62, 2},
	{19, 64, 1},
	{20, -1, 16},
	{21, 100, 1},
	{22, 102, 2},
	{23, 104, 1},
	{24, 106, 4},
	{25, 108, 1},
	{26, 110, 2},
	{27, 112, 1},
	{28, 114, 8},
	{29, 116, 1},
	{30, 118, 2},
	{31, 120, 1},
	{32, 122, 4},
	{33, 124, 1},
	{34, 126, 2},
	{35, 128, 1},
	{36, -1, -1},
	{37, 149, 1},
	{38, 151, 2},
	{39, 153, 1},
	{40, 155, 4},
	{41, 157, 1},
	{42, 159, 2},
	{43, 161, 1},
	{44, 163, 8},
	{45, 165, 1},
	{46, 167, 2},
	{47, 169, 1},
	{48, 171, 4},
	{49, 173, 1},
	{50, 175, 2},
	{51, 177, 1},
};

static bool cc_is(const char *cc, char c0, char c1)
{
	return cc && cc[0] == c0 && cc[1] == c1;
}

int morse_s1g_chan_to_ht_chan(int s1g_chan)
{
	if (s1g_chan < S1G_CHAN_MIN || s1g_chan > S1G_CHAN_MAX)
		return MORSE_S1G_RETURN_ERROR;

	return s1g_ht_chan_pairs_default[s1g_chan].ht_channel;
}

int morse_s1g_country_to_global_op_class(char *cc)
{
	if (!cc || !cc[0] || !cc[1])
		return MORSE_S1G_RETURN_ERROR;

	/* Global op class 66 (Europe, etc.) */
	if (cc_is(cc, 'E', 'U') || cc_is(cc, 'G', 'B') || cc_is(cc, 'I', 'N'))
		return 66;

	/* Global op class 68 (ITU region 2, AU, NZ, SG, US, CA, ...) */
	if (cc_is(cc, 'U', 'S') || cc_is(cc, 'C', 'A') ||
	    cc_is(cc, 'A', 'U') || cc_is(cc, 'N', 'Z') || cc_is(cc, 'S', 'G'))
		return 68;

	return MORSE_S1G_RETURN_ERROR;
}
