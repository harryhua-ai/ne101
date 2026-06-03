/*
 * The ESP-IDF/Morse build mangles crypto symbol names (see hostap_morse_common.h),
 * mapping aes_wrap() -> mmint_aes_wrap(). Some crypto backends (e.g. mbedtls DPP)
 * provide aes_wrap(), but not the mangled alias. Provide a thin wrapper.
 */

#include "utils/includes.h"
#include "utils/common.h"

/*
 * This build mangles aes_wrap() -> mmint_aes_wrap via a macro in
 * `hostap_morse_common.h` (pulled in via common headers). Undefine it in this
 * translation unit so we can call the real backend `aes_wrap()` symbol.
 */
#ifdef aes_wrap
#undef aes_wrap
#endif

/* Declare the backend implementation symbol explicitly. */
int aes_wrap(const u8 *kek, size_t kek_len, int n, const u8 *plain, u8 *cipher);

int mmint_aes_wrap(const u8 *kek, size_t kek_len, int n, const u8 *plain, u8 *cipher)
{
	return aes_wrap(kek, kek_len, n, plain, cipher);
}

