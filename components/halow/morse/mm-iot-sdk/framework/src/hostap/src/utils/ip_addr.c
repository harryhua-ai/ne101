/*
 * IP address processing
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "ip_addr.h"

/*
 * ESP-IDF/MM_IOT hostap port does not provide the usual BSD socket headers
 * (inet_aton/inet_ntoa) and also defines `struct in_addr` in
 * `hostap_morse_common.h`, which conflicts with lwIP's inet.h.
 *
 * Keep this file self-contained by implementing minimal IPv4 text conversion.
 */

static int hostapd_ipv4_aton(const char *txt, struct in_addr *out)
{
	unsigned int a, b, c, d;
	u32 v;

	if (!txt || !out)
		return 0;

	/* Strict dotted-quad; hostapd only needs basic parsing. */
	if (sscanf(txt, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
		return 0;
	if (a > 255 || b > 255 || c > 255 || d > 255)
		return 0;

	v = ((u32) a << 24) | ((u32) b << 16) | ((u32) c << 8) | (u32) d;
	out->s_addr = htonl(v);
	return 1;
}

static const char * hostapd_ipv4_ntoa_r(const struct in_addr *in, char *buf,
					size_t buflen)
{
	u32 v;

	if (!in || !buf || buflen == 0)
		return NULL;

	v = ntohl(in->s_addr);
	os_snprintf(buf, buflen, "%u.%u.%u.%u",
		    (unsigned int) ((v >> 24) & 0xff),
		    (unsigned int) ((v >> 16) & 0xff),
		    (unsigned int) ((v >> 8) & 0xff),
		    (unsigned int) (v & 0xff));
	return buf;
}

const char * hostapd_ip_txt(const struct hostapd_ip_addr *addr, char *buf,
			    size_t buflen)
{
	if (buflen == 0 || addr == NULL)
		return NULL;

	if (addr->af == AF_INET) {
		if (hostapd_ipv4_ntoa_r(&addr->u.v4, buf, buflen) == NULL)
			buf[0] = '\0';
	} else {
		buf[0] = '\0';
	}
#ifdef CONFIG_IPV6
	if (addr->af == AF_INET6) {
		if (inet_ntop(AF_INET6, &addr->u.v6, buf, buflen) == NULL)
			buf[0] = '\0';
	}
#endif /* CONFIG_IPV6 */

	return buf;
}


int hostapd_parse_ip_addr(const char *txt, struct hostapd_ip_addr *addr)
{
#ifndef CONFIG_NATIVE_WINDOWS
	if (hostapd_ipv4_aton(txt, &addr->u.v4)) {
		addr->af = AF_INET;
		return 0;
	}

#ifdef CONFIG_IPV6
	if (inet_pton(AF_INET6, txt, &addr->u.v6) > 0) {
		addr->af = AF_INET6;
		return 0;
	}
#endif /* CONFIG_IPV6 */
#endif /* CONFIG_NATIVE_WINDOWS */

	return -1;
}


bool hostapd_ip_equal(const struct hostapd_ip_addr *a,
		      const struct hostapd_ip_addr *b)
{
	if (a->af != b->af)
		return false;

	if (a->af == AF_INET && a->u.v4.s_addr == b->u.v4.s_addr)
		return true;

#ifdef CONFIG_IPV6
	if (a->af == AF_INET6 &&
	    os_memcmp(&a->u.v6, &b->u.v6, sizeof(a->u.v6)) == 0)
		return true;
#endif /* CONFIG_IPV6 */

	return false;
}
