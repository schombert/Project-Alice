#ifdef _WIN64

extern "C" {
#include "network/pcp_api.c"
#include "network/pcp_client_db.c"
#include "network/pcp_event_handler.c"
#include "network/pcp_logger.c"
#include "network/pcp_msg.c"
#include "network/pcp_server_discovery.c"
#include "network/windows/pcp_gettimeofday.c"
#include "network/net/pcp_socket.c"
#include "network/net/sock_ntop.c"
#include "network/net/gateway.c"
#include "network/net/findsaddr-udp.c"
};

#endif
