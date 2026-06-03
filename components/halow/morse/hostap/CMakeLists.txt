# Copyright 2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(ROOT_DIR ${MM_IOT_SDK}/framework/src/hostap)

set(INC
    ${ROOT_DIR}/..
    ${ROOT_DIR}
    ${ROOT_DIR}/src
    ${ROOT_DIR}/src/ap
    ${ROOT_DIR}/src/common
    ${ROOT_DIR}/src/utils
    ${ROOT_DIR}/wpa_supplicant
    ${ROOT_DIR}/src/crypto
)

set(SRCS
  ${ROOT_DIR}/drivers_morse.c
  ${ROOT_DIR}/os_mmosal.c
  ${ROOT_DIR}/src/common/dragonfly.c
  ${ROOT_DIR}/src/common/hw_features_common.c
  ${ROOT_DIR}/src/common/ieee802_11_common.c
  ${ROOT_DIR}/src/common/ptksa_cache.c
  ${ROOT_DIR}/src/common/sae.c
  ${ROOT_DIR}/src/common/wpa_common.c
  ${ROOT_DIR}/src/utils/ip_addr.c
  ${ROOT_DIR}/src/crypto/aes-unwrap.c
  ${ROOT_DIR}/src/crypto/dh_groups.c
  ${ROOT_DIR}/src/crypto/sha256-kdf.c
  ${ROOT_DIR}/src/crypto/sha384-kdf.c
  ${ROOT_DIR}/src/crypto/tls_none.c
  ${ROOT_DIR}/src/crypto/sha512-kdf.c
  ${ROOT_DIR}/src/drivers/driver_common.c
  ${ROOT_DIR}/src/eap_common/eap_common.c
  ${ROOT_DIR}/src/eap_peer/eap.c
  ${ROOT_DIR}/src/eap_peer/eap_methods.c
  ${ROOT_DIR}/src/eapol_supp/eapol_supp_sm.c
  ${ROOT_DIR}/src/rsn_supp/preauth.c
  ${ROOT_DIR}/src/rsn_supp/pmksa_cache.c
  ${ROOT_DIR}/src/rsn_supp/wpa.c
  ${ROOT_DIR}/src/rsn_supp/wpa_ie.c
  ${ROOT_DIR}/src/utils/crc32.c
  ${ROOT_DIR}/src/utils/bitfield.c
  ${ROOT_DIR}/src/utils/common.c
  ${ROOT_DIR}/src/utils/wpa_debug.c
  ${ROOT_DIR}/src/utils/wpabuf.c
  ${ROOT_DIR}/morse_dpp_s1g_shim.c
  ${ROOT_DIR}/mmint_aes_wrap_shim.c
  ${ROOT_DIR}/wpa_supplicant/bgscan.c
  ${ROOT_DIR}/wpa_supplicant/bgscan_simple.c
  ${ROOT_DIR}/wpa_supplicant/autoscan.c
  ${ROOT_DIR}/wpa_supplicant/autoscan_exponential.c
  ${ROOT_DIR}/wpa_supplicant/bss.c
  ${ROOT_DIR}/wpa_supplicant/config.c
  ${ROOT_DIR}/wpa_supplicant/eap_register.c
  ${ROOT_DIR}/wpa_supplicant/events.c
  ${ROOT_DIR}/wpa_supplicant/notify.c
  ${ROOT_DIR}/wpa_supplicant/op_classes.c
  ${ROOT_DIR}/wpa_supplicant/scan.c
  ${ROOT_DIR}/wpa_supplicant/sme.c
  ${ROOT_DIR}/wpa_supplicant/wmm_ac.c
  ${ROOT_DIR}/wpa_supplicant/wpa_supplicant.c
  ${ROOT_DIR}/wpa_supplicant/wpas_glue.c
  ${ROOT_DIR}/wpa_supplicant/bssid_ignore.c
  ${ROOT_DIR}/wpa_supplicant/wnm_sta.c
  ${ROOT_DIR}/src/common/dpp.c
  ${ROOT_DIR}/src/common/dpp_auth.c
  ${ROOT_DIR}/src/common/dpp_backup.c
  ${ROOT_DIR}/src/common/dpp_crypto.c
  ${ROOT_DIR}/src/common/dpp_pkex.c
  ${ROOT_DIR}/src/common/dpp_reconfig.c
  ${ROOT_DIR}/dpp_tcp_stubs.c
  ${ROOT_DIR}/wpa_supplicant/dpp_supplicant.c
  ${ROOT_DIR}/wpa_supplicant/offchannel.c
  ${ROOT_DIR}/src/crypto/aes-siv.c
  ${ROOT_DIR}/src/crypto/aes-ctr.c
  ${ROOT_DIR}/src/utils/json.c
  ${ROOT_DIR}/src/common/gas_server.c
  ${ROOT_DIR}/src/common/gas.c
  ${ROOT_DIR}/wpa_supplicant/gas_query.c
  ${ROOT_DIR}/src/ap/gas_serv.c
  ${ROOT_DIR}/src/ap/gas_query_ap.c
  ${ROOT_DIR}/src/ap/dpp_hostapd.c
  ${ROOT_DIR}/src/utils/base64.c
  ${ROOT_DIR}/src/tls/asn1.c
  ${ROOT_DIR}/crypto_mbedtls_dpp.c
  ${ROOT_DIR}/morse_stubs.c
  ${ROOT_DIR}/wpa_supplicant/ap.c
  ${ROOT_DIR}/src/ap/authsrv.c
  ${ROOT_DIR}/src/ap/bss_load.c
  ${ROOT_DIR}/src/ap/wmm.c
  ${ROOT_DIR}/src/ap/ap_list.c
  ${ROOT_DIR}/src/ap/comeback_token.c
  ${ROOT_DIR}/src/pasn/pasn_responder.c
  ${ROOT_DIR}/src/ap/hw_features.c
  ${ROOT_DIR}/src/ap/dfs.c
  ${ROOT_DIR}/src/ap/tkip_countermeasures.c
  ${ROOT_DIR}/src/ap/hostapd.c
  ${ROOT_DIR}/src/ap/ieee802_1x.c
  ${ROOT_DIR}/src/ap/ieee802_11_auth.c
  ${ROOT_DIR}/src/ap/ieee802_11_s1g.c
  ${ROOT_DIR}/src/ap/sta_info.c
  ${ROOT_DIR}/src/ap/ap_config.c
  ${ROOT_DIR}/src/ap/ap_drv_ops.c
  ${ROOT_DIR}/src/ap/ap_mlme.c
  ${ROOT_DIR}/src/ap/beacon.c
  ${ROOT_DIR}/src/ap/drv_callbacks.c
  ${ROOT_DIR}/src/ap/eap_user_db.c
  ${ROOT_DIR}/src/ap/ieee802_11.c
  ${ROOT_DIR}/src/ap/ieee802_11_ht.c
  ${ROOT_DIR}/src/ap/ieee802_11_shared.c
  ${ROOT_DIR}/src/ap/neighbor_db.c
  ${ROOT_DIR}/src/ap/pmksa_cache_auth.c
  ${ROOT_DIR}/src/ap/rrm.c
  ${ROOT_DIR}/src/ap/utils.c
  ${ROOT_DIR}/src/ap/wpa_auth.c
  ${ROOT_DIR}/src/ap/wpa_auth_glue.c
  ${ROOT_DIR}/src/ap/wpa_auth_ie.c
  ${ROOT_DIR}/src/eapol_auth/eapol_auth_sm.c
  ${ROOT_DIR}/src/eap_server/eap_server.c
  ${ROOT_DIR}/src/eap_server/eap_server_methods.c
)

set(BUILD_DEFINES
  IEEE8021X_EAPOL=1
  CONFIG_SME=1
  CONFIG_SAE=1
  CONFIG_OWE=1
  CONFIG_ECC=1
  CONFIG_SHA256=1
  CONFIG_SHA512=1
  CONFIG_SHA384=1
  CONFIG_FIPS=1
  CONFIG_NO_ACCOUNTING=1
  CONFIG_NO_BSS_TRANS_MGMT=1
  CONFIG_NO_CONFIG_BLOBS=1
  CONFIG_NO_RADIUS=1
  CONFIG_NO_RANDOM_POOL=1
  CONFIG_NO_RC4=1
  CONFIG_NO_ROBUST_AV=1
  CONFIG_NO_RRM=1
  CONFIG_NO_VLAN=1
  CONFIG_WNM=1
  CONFIG_IEEE80211AH=1
  CONFIG_NO_CONFIG_WRITE=1
  OS_NO_C_LIB_DEFINES=1
  CONFIG_BGSCAN=1
  CONFIG_BGSCAN_SIMPLE=1
  CONFIG_AUTOSCAN=1
  CONFIG_AUTOSCAN_EXPONENTIAL=1
  CONFIG_S1G_TWT=1
  MAX_NUM_MLD_LINKS=1
  MAX_NUM_MLO_LINKS=1
  WPA_SUPPLICANT_CLEANUP_INTERVAL=120
  MM_IOT
  CONFIG_OPENSSL_INTERNAL_AES_WRAP=1
  MM_IOT_DPP_HEADER=1
  CONFIG_OFFCHANNEL=1
  CONFIG_DPP=1
  CONFIG_DPP2=1
  CONFIG_DPP3=1
  CONFIG_JSON=1
  CONFIG_GAS_SERVER=1
  CONFIG_GAS=1
  MM_IOT_DPP_DISABLE_DOT1X=1
  MM_IOT_DPP_DISABLE_TCP=1
  MM_IOT_DPP_DISABLE_URI_HOST=1
  MM_IOT_DPP_DISABLE_PRIVATE_PEER_INTRO=1
  MM_IOT_DPP_EVENTS=1
  CONFIG_AP=1
  NEED_AP_MLME=1
)

add_library(mmhostap STATIC ${SRCS})
target_include_directories(mmhostap PUBLIC ${INC})
target_compile_definitions(mmhostap PUBLIC ${BUILD_DEFINES})
target_compile_options(mmhostap PRIVATE "-Wno-c++-compat")
target_compile_options(mmhostap PRIVATE "-Wno-unused-but-set-variable")
target_compile_options(mmhostap PRIVATE "-Wno-unused-function")
target_compile_options(mmhostap PRIVATE "-Wno-unused-parameter")
target_compile_options(mmhostap PRIVATE "-Wno-unused-variable")
target_compile_options(mmhostap PRIVATE "-Wno-format")
target_compile_options(mmhostap PRIVATE "-Wno-maybe-uninitialized")
target_compile_options(mmhostap PRIVATE "-Wno-dangling-else")
target_compile_options(mmhostap PRIVATE "-Wno-type-limits")
target_compile_options(mmhostap PRIVATE "-Wno-sign-compare")
target_compile_options(mmhostap PRIVATE "-Wno-parentheses")
target_compile_options(mmhostap PRIVATE "-Wno-packed-not-aligned")
target_compile_options(mmhostap PRIVATE "-Wno-misleading-indentation")
target_compile_options(mmhostap PRIVATE "-Wno-address")
set_source_files_properties(${ROOT_DIR}/src/common/dpp.c PROPERTIES COMPILE_FLAGS "-Wno-overflow")
add_dependencies(mmhostap idf::mbedtls)
target_link_libraries(mmhostap PUBLIC
    mmutils idf::mbedtls)
