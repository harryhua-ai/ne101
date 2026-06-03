/*
 * DPP TCP/relay/controller stubs for MM_IOT builds where TCP is disabled.
 *
 * The full `src/common/dpp_tcp.c` requires BSD socket APIs/types that are not
 * available in this ESP-IDF hostap port. For push-button provisioning we only
 * need DPP over action frames; provide "not supported" stubs to satisfy
 * references from `src/ap/dpp_hostapd.c` when CONFIG_DPP2/3 is enabled.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "common/dpp.h"

#ifdef MM_IOT_DPP_DISABLE_TCP

int dpp_tcp_pkex_init(struct dpp_global *dpp, struct dpp_pkex *pkex,
		      const struct hostapd_ip_addr *addr, int port,
		      void *msg_ctx, void *cb_ctx,
		      int (*pkex_done)(void *ctx, void *conn,
				       struct dpp_bootstrap_info *bi))
{
	(void) dpp;
	(void) pkex;
	(void) addr;
	(void) port;
	(void) msg_ctx;
	(void) cb_ctx;
	(void) pkex_done;
	return -1;
}

int dpp_tcp_init(struct dpp_global *dpp, struct dpp_authentication *auth,
		 const struct hostapd_ip_addr *addr, int port, const char *name,
		 enum dpp_netrole netrole, const char *mud_url,
		 const char *extra_conf_req_name,
		 const char *extra_conf_req_value,
		 void *msg_ctx, void *cb_ctx,
		 int (*process_conf_obj)(void *ctx,
					 struct dpp_authentication *auth),
		 bool (*tcp_msg_sent)(void *ctx,
				      struct dpp_authentication *auth))
{
	(void) dpp;
	(void) auth;
	(void) addr;
	(void) port;
	(void) name;
	(void) netrole;
	(void) mud_url;
	(void) extra_conf_req_name;
	(void) extra_conf_req_value;
	(void) msg_ctx;
	(void) cb_ctx;
	(void) process_conf_obj;
	(void) tcp_msg_sent;
	return -1;
}

int dpp_tcp_auth(struct dpp_global *dpp, void *_conn,
		 struct dpp_authentication *auth, const char *name,
		 enum dpp_netrole netrole, const char *mud_url,
		 const char *extra_conf_req_name,
		 const char *extra_conf_req_value,
		 int (*process_conf_obj)(void *ctx,
					 struct dpp_authentication *auth),
		 bool (*tcp_msg_sent)(void *ctx,
				      struct dpp_authentication *auth))
{
	(void) dpp;
	(void) _conn;
	(void) auth;
	(void) name;
	(void) netrole;
	(void) mud_url;
	(void) extra_conf_req_name;
	(void) extra_conf_req_value;
	(void) process_conf_obj;
	(void) tcp_msg_sent;
	return -1;
}

bool dpp_tcp_conn_status_requested(struct dpp_global *dpp)
{
	(void) dpp;
	return false;
}

void dpp_tcp_send_conn_status(struct dpp_global *dpp,
			      enum dpp_status_error result,
			      const u8 *ssid, size_t ssid_len,
			      const char *channel_list)
{
	(void) dpp;
	(void) result;
	(void) ssid;
	(void) ssid_len;
	(void) channel_list;
}

/* Relay/controller helpers - not supported when TCP is disabled */
int dpp_relay_add_controller(struct dpp_global *dpp, struct dpp_relay_config *config)
{
	(void) dpp;
	(void) config;
	return -1;
}

void dpp_relay_remove_controller(struct dpp_global *dpp, const struct hostapd_ip_addr *addr)
{
	(void) dpp;
	(void) addr;
}

int dpp_relay_listen(struct dpp_global *dpp, int port, struct dpp_relay_config *config)
{
	(void) dpp;
	(void) port;
	(void) config;
	return -1;
}

void dpp_relay_stop_listen(struct dpp_global *dpp)
{
	(void) dpp;
}

int dpp_relay_rx_action(struct dpp_global *dpp, const u8 *src, const u8 *hdr,
			const u8 *buf, size_t len, unsigned int freq,
			const u8 *i_bootstrap, const u8 *r_bootstrap,
			void *cb_ctx)
{
	(void) dpp;
	(void) src;
	(void) hdr;
	(void) buf;
	(void) len;
	(void) freq;
	(void) i_bootstrap;
	(void) r_bootstrap;
	(void) cb_ctx;
	return -1;
}

int dpp_relay_rx_gas_req(struct dpp_global *dpp, const u8 *src, const u8 *data,
			 size_t data_len)
{
	(void) dpp;
	(void) src;
	(void) data;
	(void) data_len;
	return -1;
}

bool dpp_relay_controller_available(struct dpp_global *dpp)
{
	(void) dpp;
	return false;
}

int dpp_controller_start(struct dpp_global *dpp, struct dpp_controller_config *config)
{
	(void) dpp;
	(void) config;
	return -1;
}

int dpp_controller_set_params(struct dpp_global *dpp, const char *params)
{
	(void) dpp;
	(void) params;
	return -1;
}

void dpp_controller_stop(struct dpp_global *dpp)
{
	(void) dpp;
}

void dpp_controller_stop_for_ctx(struct dpp_global *dpp, void *cb_ctx)
{
	(void) dpp;
	(void) cb_ctx;
}

struct dpp_authentication * dpp_controller_get_auth(struct dpp_global *dpp,
						    unsigned int id)
{
	(void) dpp;
	(void) id;
	return NULL;
}

void dpp_controller_new_qr_code(struct dpp_global *dpp, struct dpp_bootstrap_info *bi)
{
	(void) dpp;
	(void) bi;
}

bool dpp_controller_is_own_pkex_req(struct dpp_global *dpp,
				   const u8 *buf, size_t len)
{
	(void) dpp;
	(void) buf;
	(void) len;
	return false;
}

#endif /* MM_IOT_DPP_DISABLE_TCP */

