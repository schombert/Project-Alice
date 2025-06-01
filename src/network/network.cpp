#ifdef _WIN64 // WINDOWS
#ifndef _MSC_VER
#include <unistd.h>
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <windows.h>
#include <natupnp.h>
#include <iphlpapi.h>
#include <Mstcpip.h>
#include <ip2string.h>
#include "pcp.h"
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#endif // ...
#include <string_view>
#include "system_state.hpp"
#include "commands.hpp"
#include "SPSCQueue.h"
#include "network.hpp"
#include "serialization.hpp"
#include "gui_error_window.hpp"
#include "persistent_server_extensions.hpp"

#define ZSTD_STATIC_LINKING_ONLY
#define XXH_NAMESPACE ZSTD_
#include "zstd.h"

#ifdef _WIN64
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ntdll.lib")
#endif

#include <webapi/json.hpp>
using json = nlohmann::json;

namespace network {

//
// platform specific
//

struct local_addresses {
	std::string address;
	bool ipv6 = false;
};

port_forwarder::port_forwarder() { }

inline static SHA512 sha512;

void port_forwarder::start_forwarding() {
#ifdef _WIN64
	if(started)
		return;

	internal_wait.lock();
	started = true;
	do_forwarding = std::thread{ [&]() {
		//setup forwarding
		if(!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
			return;


		std::vector<local_addresses> found_locals;

		// try to figure out what the computer's local address is
		{
			IP_ADAPTER_ADDRESSES* adapter_addresses(NULL);
			IP_ADAPTER_ADDRESSES* adapter(NULL);

			// Start with a 16 KB buffer and resize if needed -
			// multiple attempts in case interfaces change while
			// we are in the middle of querying them.
			DWORD adapter_addresses_buffer_size = 16 * 1024;
			for(int attempts = 0; attempts != 3; ++attempts) {
				adapter_addresses = (IP_ADAPTER_ADDRESSES*)malloc(adapter_addresses_buffer_size);
				assert(adapter_addresses);

				DWORD error = GetAdaptersAddresses(
				  AF_UNSPEC,
				  GAA_FLAG_SKIP_ANYCAST |
					GAA_FLAG_SKIP_MULTICAST |
					GAA_FLAG_SKIP_DNS_SERVER |
					GAA_FLAG_SKIP_FRIENDLY_NAME,
				  NULL,
				  adapter_addresses,
				  &adapter_addresses_buffer_size);

				if(ERROR_SUCCESS == error) {
					// We're done here, people!
					break;
				} else if(ERROR_BUFFER_OVERFLOW == error) {
					// Try again with the new size
					free(adapter_addresses);
					adapter_addresses = NULL;

					continue;
				} else {
					// Unexpected error code - log and throw
					free(adapter_addresses);
					adapter_addresses = NULL;

					// @todo
				}
			}

			// Iterate through all of the adapters
			for(adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next) {
				// Skip loopback adapters
				if(IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType) {
					continue;
				}

				// Parse all IPv4 and IPv6 addresses
				for(
				  IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress;
				  NULL != address;
				  address = address->Next) {
					auto family = address->Address.lpSockaddr->sa_family;
					if(address->DadState != NldsPreferred && address->DadState != IpDadStatePreferred)
						continue;

					if(AF_INET == family) {
						// IPv4
						SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);

						char str_buffer[INET_ADDRSTRLEN] = { 0 };
						inet_ntop(AF_INET, &(ipv4->sin_addr), str_buffer, INET_ADDRSTRLEN);
						found_locals.push_back(local_addresses{ std::string(str_buffer),  false});
					} else if(AF_INET6 == family) {
						// IPv6
						SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(address->Address.lpSockaddr);

						char str_buffer[INET6_ADDRSTRLEN] = { 0 };
						inet_ntop(AF_INET6, &(ipv6->sin6_addr), str_buffer, INET6_ADDRSTRLEN);

						std::string ipv6_str(str_buffer);

						// Detect and skip non-external addresses
						bool is_link_local(false);
						bool is_special_use(false);

						if(0 == ipv6_str.find("fe")) {
							char c = ipv6_str[2];
							if(c == '8' || c == '9' || c == 'a' || c == 'b') {
								is_link_local = true;
							}
						} else if(0 == ipv6_str.find("2001:0:")) {
							is_special_use = true;
						}

						if(!(is_link_local || is_special_use)) {
							found_locals.push_back(local_addresses{ std::string(ipv6_str), true });
						}
					} else {
						// Skip all other types of addresses
						continue;
					}
				}
			}

			// Cleanup
			free(adapter_addresses);
			adapter_addresses = NULL;
		}

		// try to add port mapping
		bool mapped_ports_with_upnp = false;
		if(found_locals.size() != 0) {
			IUPnPNAT* nat_interface = nullptr;
			IStaticPortMappingCollection* port_mappings = nullptr;
			IStaticPortMapping* opened_port = nullptr;

			BSTR proto = SysAllocString(L"TCP");
			BSTR desc = SysAllocString(L"Project Alice Host");
			auto tmpwstr = simple_fs::utf8_to_native(found_locals[0].address);
			BSTR local_host = SysAllocString(tmpwstr.c_str());
			VARIANT_BOOL enabled = VARIANT_TRUE;

			if(SUCCEEDED(CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_ALL, __uuidof(IUPnPNAT), (void**)&nat_interface)) && nat_interface) {
				if(SUCCEEDED(nat_interface->get_StaticPortMappingCollection(&port_mappings)) && port_mappings) {
					if(SUCCEEDED(port_mappings->Add(default_server_port, proto, default_server_port, local_host, enabled, desc, &opened_port)) && opened_port) {
						mapped_ports_with_upnp = true;
					}
				}
			}

			pcp_ctx_t* pcp_obj = nullptr;
			if(!mapped_ports_with_upnp) {
				pcp_obj = pcp_init(ENABLE_AUTODISCOVERY, NULL);

				sockaddr_storage source_ip;
				sockaddr_storage ext_ip;

				WORD wVersionRequested;
				WSADATA wsaData;

				wVersionRequested = MAKEWORD(2, 2);
				auto err = WSAStartup(wVersionRequested, &wsaData);

				memset(&source_ip, 0, sizeof(source_ip));
				memset(&ext_ip, 0, sizeof(ext_ip));

				if(found_locals[0].ipv6 == false) {
					((sockaddr_in*)(&source_ip))->sin_port = 1984;
					((sockaddr_in*)(&source_ip))->sin_addr.s_addr = inet_addr(found_locals[0].address.c_str());
					((sockaddr_in*)(&source_ip))->sin_family = AF_INET;

					((sockaddr_in*)(&ext_ip))->sin_port = 1984;
					((sockaddr_in*)(&ext_ip))->sin_family = AF_INET;
				} else {
					((sockaddr_in6*)(&source_ip))->sin6_port = 1984;
					PCSTR term = nullptr;
					RtlIpv6StringToAddressA(found_locals[0].address.c_str(), &term, &(((sockaddr_in6*)(&source_ip))->sin6_addr));
					((sockaddr_in6*)(&source_ip))->sin6_family = AF_INET6;

					((sockaddr_in6*)(&ext_ip))->sin6_port = 1984;
					((sockaddr_in6*)(&ext_ip))->sin6_family = AF_INET6;
				}

				auto flow = pcp_new_flow(pcp_obj, (sockaddr*)&source_ip, nullptr, (sockaddr*)&ext_ip, IPPROTO_TCP, 3600, nullptr);
				if(flow)
					pcp_wait(flow, 10000, 0);

			}

			// wait for destructor
			internal_wait.lock();

			if(pcp_obj) {
				pcp_terminate(pcp_obj, 0);
			}

			//cleanup forwarding
			if(port_mappings)
				port_mappings->Remove(default_server_port, proto);

			if(opened_port)
				opened_port->Release();
			if(port_mappings)
				port_mappings->Release();
			if(nat_interface)
				nat_interface->Release();

			SysFreeString(proto);
			SysFreeString(local_host);
			SysFreeString(desc);

			internal_wait.unlock();
		}
		CoUninitialize();
	} };
#else
#endif
}

port_forwarder::~port_forwarder() {
#ifdef _WIN64
	if(started) {
		internal_wait.unlock();
		do_forwarding.join();
	}
#else
#endif
}

std::string get_last_error_msg() {
#ifdef _WIN64
	auto err = WSAGetLastError();
	LPTSTR err_buf = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, (LPTSTR)&err_buf, 0, nullptr);
	native_string err_text = err_buf;
	LocalFree(err_buf);

	std::string err_msg;
	if(err == WSAECONNRESET) {
		err_msg += "Host was lost ";
	} else if(err == WSAEHOSTDOWN) {
		err_msg += "Host is down ";
	} else if(err == WSAEHOSTUNREACH) {
		err_msg += "Host is unreachable ";
	} else {
		err_msg += "Network issue ocurred ";
	}
	err_msg += "Technical details: ";
	err_msg += std::to_string(err);
	err_msg += " => ";
	err_msg += simple_fs::native_to_utf8(err_text);
	return err_msg;
#else
	return std::string("Dummy");
#endif
}

static int internal_socket_recv(socket_t socket_fd, void *data, size_t n) {
#ifdef _WIN64
	u_long has_pending = 0;
	auto r = ioctlsocket(socket_fd, FIONREAD, &has_pending);
	if(has_pending)
		return static_cast<int>(recv(socket_fd, reinterpret_cast<char *>(data), static_cast<int>(n), 0));
	return 0;
#else
	return recv(socket_fd, data, n, MSG_DONTWAIT);
#endif
}

static int internal_socket_send(socket_t socket_fd, const void *data, size_t n) {
#ifdef _WIN64
	return static_cast<int>(send(socket_fd, reinterpret_cast<const char *>(data), static_cast<int>(n), 0));
#else
	return send(socket_fd, data, n, MSG_NOSIGNAL);
#endif
}

template<typename F>
static int socket_recv(socket_t socket_fd, void* data, size_t len, size_t* m, F&& func) {
	while(*m < len) {
		int r = internal_socket_recv(socket_fd, reinterpret_cast<uint8_t*>(data) + *m, len - *m);
		if(r > 0) {
			*m += static_cast<size_t>(r);
		} else if(r < 0) { // error
#ifdef _WIN32
			int err = WSAGetLastError();
			if(err == WSAENOBUFS || err == WSAEWOULDBLOCK) {
				return 0;
			}
			return err;
#else
			return r;
#endif
		} else if(r == 0) {
			break;
		}
	}
	// Did we receive a command?
	if(*m >= len) {
		assert(*m <= len);
		*m = 0; // reset
		func();
		return 0;
	}
	// No data received
	return -1;
}

static int socket_send(socket_t socket_fd, std::vector<char>& buffer) {
	while(!buffer.empty()) {
		int r = internal_socket_send(socket_fd, buffer.data(), buffer.size());
		if(r > 0) {
			buffer.erase(buffer.begin(), buffer.begin() + static_cast<size_t>(r));
		} else if(r < 0) {
#ifdef _WIN32
			int err = WSAGetLastError();
			if(err == WSAENOBUFS || err == WSAEWOULDBLOCK) {
				return 0;
			}
			return err;
#else
			return r;
#endif
		} else if(r == 0) {
			break;
		}
	}
	return 0;
}

static void socket_add_to_send_queue(std::vector<char>& buffer, const void *data, size_t n) {
	buffer.resize(buffer.size() + n);
	std::memcpy(buffer.data() + buffer.size() - n, data, n);
}

static void socket_shutdown(socket_t socket_fd) {
	if(socket_fd > 0) {
#ifdef _WIN64
		shutdown(socket_fd, SD_BOTH);
		closesocket(socket_fd);
#else
		shutdown(socket_fd, SHUT_RDWR);
		close(socket_fd);
#endif
	}
}

static socket_t socket_init_server(bool as_v6, struct sockaddr_storage& server_address) {
	socket_t socket_fd = static_cast<socket_t>(socket(as_v6 ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP));
#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET)) {
		window::emit_error_message("Network socket error: " + get_last_error_msg(), true);
	}
#else
	if(socket_fd < 0)
		std::abort();
#endif
	struct timeval timeout;
	timeout.tv_sec = 60;
	timeout.tv_usec = 0;
	int opt = 1;
#ifdef _WIN64
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
		window::emit_error_message("Network setsockopt [reuseaddr] error: " + get_last_error_msg(), true);
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		window::emit_error_message("Network setsockopt [rcvtimeo] error: " + get_last_error_msg(), true);
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
		window::emit_error_message("Network setsockopt [sndtimeo] error: " + get_last_error_msg(), true);
	}
#else
	if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		std::abort();
	}
	if(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
		std::abort();
	}
#endif
	if(as_v6) {
		struct sockaddr_in6 v6_server_address;
		v6_server_address.sin6_addr = IN6ADDR_ANY_INIT;
		v6_server_address.sin6_family = AF_INET6;
		v6_server_address.sin6_port = htons(default_server_port);
		std::memcpy(&server_address, &v6_server_address, sizeof(v6_server_address));
	} else {
		struct sockaddr_in v4_server_address;
		v4_server_address.sin_addr.s_addr = INADDR_ANY;
		v4_server_address.sin_family = AF_INET;
		v4_server_address.sin_port = htons(default_server_port);
		std::memcpy(&server_address, &v4_server_address, sizeof(v4_server_address));
	}
	if(bind(socket_fd, (struct sockaddr*)&server_address, as_v6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in)) < 0) {
		window::emit_error_message("Network bind error: " + get_last_error_msg(), true);
	}
	if(listen(socket_fd, SOMAXCONN) < 0) {
		window::emit_error_message("Network listen error: " + get_last_error_msg(), true);
	}
#ifdef _WIN64
	u_long mode = 1; // 1 to enable non-blocking socket
	ioctlsocket(socket_fd, FIONBIO, &mode);
#endif
	return socket_fd;
}

static socket_t socket_init_client(bool& as_v6, struct sockaddr_storage& client_address, const char *ip_address) {
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo* result = NULL;
	if(getaddrinfo(ip_address, "1984", &hints, &result) != 0) {
		window::emit_error_message("Network getaddrinfo error: " + get_last_error_msg(), true);
	}
	as_v6 = false;
	bool found = false;
	for(struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		if(ptr->ai_socktype == SOCK_STREAM && ptr->ai_protocol == IPPROTO_TCP) {
			if(ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6) {
				as_v6 = ptr->ai_family == AF_INET6;
				std::memcpy(&client_address, ptr->ai_addr, sizeof(sockaddr));
				found = true;
			}
		}
	}
	freeaddrinfo(result);
	if(!found) {
		window::emit_error_message("No suitable host found", true);
	}
	socket_t socket_fd = static_cast<socket_t>(socket(as_v6 ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP));
#ifdef _WIN64
	if(socket_fd == static_cast<socket_t>(INVALID_SOCKET)) {
		window::emit_error_message("Network socket error: " + get_last_error_msg(), true);
	}
#else
	if(socket_fd < 0) {
		window::emit_error_message("Network socket error: " + get_last_error_msg(), true);
	}
#endif
	if(connect(socket_fd, (const struct sockaddr*)&client_address, as_v6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in)) < 0) {
		window::emit_error_message("Network connect error: " + get_last_error_msg(), true);
	}
	return socket_fd;
}

//
// non-platform specific
//

void clear_socket(sys::state& state, client_data& client) {
	socket_shutdown(client.socket_fd);
	client.socket_fd = 0;
	client.send_buffer.clear();
	client.early_send_buffer.clear();
	client.total_sent_bytes = 0;
	client.save_stream_size = 0;
	client.save_stream_offset = 0;
	client.playing_as = dcon::nation_id{};
	client.recv_count = 0;
	client.handshake = true;
	client.last_seen = sys::date{};
}

static void disconnect_client(sys::state& state, client_data& client, bool graceful) {
	if(command::can_notify_player_leaves(state, client.playing_as, graceful)) {
		command::notify_player_leaves(state, client.playing_as, graceful);
	}
#ifndef NDEBUG
	state.console_log("server:disconnectclient | country:" + std::to_string(client.playing_as.index()));
	log_player_nations(state);
#endif
	clear_socket(state, client);
}

void log_player_nations(sys::state& state) {
	auto msg = std::string{};
	for(const auto n : state.world.in_nation)
		if(state.world.nation_get_is_player_controlled(n))
			state.console_log("player controlled: " + std::to_string(n.id.index()));

	for(const auto p : state.world.in_mp_player) {
		auto data = p.get_nickname();
		auto nickname = sys::player_name{ data };
		auto nation = p.get_player_nation().get_nation();

		state.console_log("player id: " + std::to_string(p.id.index()) + " nickname: " + nickname.to_string() + " nation:" + std::to_string(nation.id.index()));
	}
}

static std::map<int, std::string> readableCommandTypes = {
	{0,"invalid"},
{1,"change_nat_focus"},
{2,"start_research"},
{3,"make_leader"},
{4,"begin_province_building_construction"},
{5,"increase_relations"},
{6,"decrease_relations"},
{7,"begin_factory_building_construction"},
{8,"begin_naval_unit_construction"},
{9,"cancel_naval_unit_construction"},
{10,"change_factory_settings"},
{11,"delete_factory"},
{12,"make_vassal"},
{13,"release_and_play_nation"},
{14,"war_subsidies"},
{15,"cancel_war_subsidies"},
{16,"change_budget"},
{17,"start_election"},
{18,"change_influence_priority"},
{19,"discredit_advisors"},
{20,"expel_advisors"},
{21,"ban_embassy"},
{22,"increase_opinion"},
{23,"decrease_opinion"},
{24,"add_to_sphere"},
{25,"remove_from_sphere"},
{26,"upgrade_colony_to_state"},
{27,"invest_in_colony"},
{28,"abandon_colony"},
{29,"finish_colonization"},
{30,"intervene_in_war"},
{31,"suppress_movement"},
{32,"civilize_nation"},
{33,"appoint_ruling_party"},
{34,"change_issue_option"},
{35,"change_reform_option"},
{36,"become_interested_in_crisis"},
{37,"take_sides_in_crisis"},
{38,"begin_land_unit_construction"},
{39,"cancel_land_unit_construction"},
{40,"change_stockpile_settings"},
{41,"take_decision"},
{42,"make_n_event_choice"},
{43,"make_f_n_event_choice"},
{44,"make_p_event_choice"},
{45,"make_f_p_event_choice"},
{46,"fabricate_cb"},
{47,"cancel_cb_fabrication"},
{48,"ask_for_military_access"},
{49,"ask_for_alliance"},
{50,"call_to_arms"},
{51,"respond_to_diplomatic_message"},
{52,"cancel_military_access"},
{53,"cancel_alliance"},
{54,"cancel_given_military_access"},
{55,"declare_war"},
{56,"add_war_goal"},
{58,"start_peace_offer"},
{59,"add_peace_offer_term"},
{60,"send_peace_offer"},
{61,"move_army"},
{62,"move_navy"},
{63,"embark_army"},
{64,"merge_armies"},
{65,"merge_navies"},
{66,"split_army"},
{67,"split_navy"},
{68,"delete_army"},
{69,"delete_navy"},
{70,"designate_split_regiments"},
{71,"designate_split_ships"},
{72,"naval_retreat"},
{73,"land_retreat"},
{74,"start_crisis_peace_offer"},
{75,"invite_to_crisis"},
{76,"add_wargoal_to_crisis_offer"},
{77,"send_crisis_peace_offer"},
{78,"change_admiral"},
{79,"change_general"},
{80,"toggle_mobilization"},
{81,"give_military_access"},
{82,"set_rally_point"},
{83,"save_game"},
{84,"cancel_factory_building_construction"},
{85,"disband_undermanned"},
{86,"even_split_army"},
{87,"even_split_navy"},
{88,"toggle_hunt_rebels"},
{89,"toggle_select_province"},
{90,"toggle_immigrator_province"},
{91,"state_transfer"},
{92,"release_subject"},
{93,"enable_debt"},
{94,"move_capital"},
{95,"toggle_unit_ai_control"},
{96,"toggle_mobilized_is_ai_controlled"},
{97,"toggle_interested_in_alliance"},
{98,"pbutton_script"},
{99,"nbutton_script"},
{100,"set_factory_type_priority"},
{101,"crisis_add_wargoal" },
{102,"change_unit_type" },
{ 102,"change_unit_type" },
{ 103,"take_province" },
{ 104,"grant_province" },
{ 105,"ask_for_free_trade_agreement" },
{ 106,"switch_embargo_status" },
{ 107,"revoke_trade_rights" },
{ 108,"toggle_local_administration" },
{ 110,"notify_player_ban" },
{ 111,"notify_player_kick" },
{ 112,"notify_player_picks_nation" },
{ 113,"notify_player_joins" },
{ 114,"notify_player_leaves" },
{ 115,"notify_player_oos" },
{ 116,"notify_save_loaded" },
{ 117,"notify_start_game" },
{ 118,"notify_stop_game" },
{ 118,"notify_stop_game" },
{ 119,"notify_pause_game" },
{ 120,"notify_reload" },
{ 121,"advance_tick" },
{ 122,"chat_message" },
{ 123,"network_inactivity_ping" },
{ 255,"console_command" },
};

dcon::mp_player_id create_mp_player(sys::state& state, sys::player_name& name, sys::player_password_raw& password, bool fully_loaded, bool is_oos) {
	auto p = state.world.create_mp_player();
	state.world.mp_player_set_nickname(p, name.data);

	auto salt = sys::player_password_salt{}.from_string_view(std::to_string(int32_t(rand())));
	auto hash = sys::player_password_hash{}.from_string_view(sha512.hash(password.to_string() + salt.to_string()));
	state.world.mp_player_set_password_hash(p, hash.data);
	state.world.mp_player_set_password_salt(p, salt.data);
	state.world.mp_player_set_fully_loaded(p, fully_loaded);
	state.world.mp_player_set_is_oos(p, is_oos);

	return p;
}

dcon::mp_player_id load_mp_player(sys::state& state, sys::player_name& name, sys::player_password_hash& password_hash, sys::player_password_salt& password_salt) {
	auto p = state.world.create_mp_player();
	state.world.mp_player_set_nickname(p, name.data);
	state.world.mp_player_set_password_hash(p, password_hash.data);
	state.world.mp_player_set_password_salt(p, password_salt.data);

	return p;
}

void update_mp_player_password(sys::state& state, dcon::mp_player_id player_id, sys::player_password_raw& password) {
	auto salt = sys::player_password_salt{}.from_string_view(std::to_string(int32_t(rand())));
	auto hash = sys::player_password_hash{}.from_string_view(sha512.hash(password.to_string() + salt.to_string()));
	state.world.mp_player_set_password_hash(player_id, hash.data);
	state.world.mp_player_set_password_salt(player_id, salt.data);
}

dcon::mp_player_id find_mp_player(sys::state& state, sys::player_name name) {
	for(const auto p : state.world.in_mp_player) {
		auto nickname = p.get_nickname();

		if(std::equal(std::begin(nickname), std::end(nickname), std::begin(name.data))) {
			return p;
		}
	}

	return dcon::mp_player_id{};
}

dcon::mp_player_id find_country_player(sys::state& state, dcon::nation_id nation) {
	return state.world.nation_get_mp_player_from_player_nation(nation);
}

// Reassign player to his previous nation if any
static dcon::nation_id get_player_nation(sys::state& state, sys::player_name name) {

	auto p = find_mp_player(state, name);
	if(p) {
		return state.world.mp_player_get_nation_from_player_nation(p);
	}

	return dcon::nation_id{ };
}

static dcon::nation_id choose_nation_for_player(sys::state& state) {
	// On join or hotjoin give the client a "joining" nation, basically a temporal nation choosen
	// "randomly" that is tied to the client iself

	for(auto n : state.nations_by_rank)
		if(!state.world.nation_get_is_player_controlled(n) && state.world.nation_get_owned_province_count(n) > 0) {
			bool is_taken = false;
			for(auto& client : state.network_state.clients) {
				if(client.playing_as == n) {
					is_taken = true;
					break;
				}
			}
			if(!is_taken) {
				return n;
			}
		}
	return dcon::nation_id{ };
}

dcon::nation_id get_first_available_ai_nation(sys::state& state) {
	for(auto nation : state.world.in_nation) {
		if(nation && !nation.get_is_player_controlled()) {
			return nation;
		}
	}

	// if there are no nations available
	return dcon::nation_id{ };
}


void place_players_after_reload(sys::state& state, std::vector<dcon::nation_id>& players, dcon::nation_id old_local_player_nation) {
	for(auto playernation : players) {
		if(state.world.nation_is_valid(playernation)) {
			state.world.nation_set_is_player_controlled(playernation, true);
			if(playernation == old_local_player_nation) {
				state.local_player_nation = playernation;
			}
		}
		else {
			auto new_nation = network::get_first_available_ai_nation(state);
			if(new_nation) {
				state.world.nation_set_is_player_controlled(new_nation, true);
				if(playernation == old_local_player_nation) {
					state.local_player_nation = playernation;
				}
			}
			else {
				// refactor this when co-op becomes possible
				ui::popup_error_window(state, "Too many players", "There are not enough nations for the amount of players");
			}
		}
	}
}

bool any_player_oos(sys::state& state) {
	for(auto player : state.world.in_mp_player) {
		if(player && player.get_is_oos()) {
			return true;
		}
	}
	return false;
}

void client_send_handshake(sys::state& state) {
	/* Send our client handshake back */
	client_handshake_data hshake;
	hshake.nickname = state.network_state.nickname;
	hshake.player_password = state.network_state.player_password;
	std::memcpy(hshake.lobby_password, state.network_state.lobby_password, sizeof(hshake.lobby_password));
	socket_add_to_send_queue(state.network_state.send_buffer, &hshake, sizeof(hshake));

#ifndef NDEBUG
	state.console_log("client:send:handshake");
#endif
}

int client_process_handshake(sys::state& state) {
	int r = socket_recv(state.network_state.socket_fd, &state.network_state.s_hshake, sizeof(state.network_state.s_hshake), &state.network_state.recv_count, [&]() {
		if(!state.scenario_checksum.is_equal(state.network_state.s_hshake.scenario_checksum)) {
			bool found_match = false;
			// Find a scenario with a matching checksum
			auto dir = simple_fs::get_or_create_scenario_directory();
			for(const auto& uf : simple_fs::list_files(dir, NATIVE(".bin"))) {
				auto f = simple_fs::open_file(uf);
				if(f) {
					auto contents = simple_fs::view_contents(*f);
					sys::scenario_header scen_header;
					if(contents.file_size > sizeof(sys::scenario_header)) {
						sys::read_scenario_header(reinterpret_cast<const uint8_t*>(contents.data), scen_header);
						if(!scen_header.checksum.is_equal(state.network_state.s_hshake.scenario_checksum))
							continue; // Same checksum
						if(scen_header.version != sys::scenario_file_version)
							continue; // Same version of scenario
						if(sys::try_read_scenario_and_save_file(state, simple_fs::get_file_name(uf))) {
							state.fill_unsaved_data();
							found_match = true;
							break;
						}
					}
				}
			}
			if(!found_match) {
				std::string msg = "Could not find a scenario with a matching checksum!"
					"This is most likely a false positive, so just ask the host for their"
					"scenario file and it should work. Or you haven't clicked on 'Make scenario'!";
				msg += "\n";
				msg += "Host should give you the scenario from:\n"
					"'My Documents\\Project Alice\\scenarios\\<Most recent scenario>'";
				msg += "And you place it on:\n"
					"'My Documents\\Project Alice\\scenarios\\'\n";

				window::emit_error_message(msg.c_str(), true);
			}
		}
		state.session_host_checksum = state.network_state.s_hshake.save_checksum;
		state.game_seed = state.network_state.s_hshake.seed;
		state.local_player_nation = state.network_state.s_hshake.assigned_nation;
		state.world.nation_set_is_player_controlled(state.local_player_nation, true);

#ifndef NDEBUG
		state.console_log("client:recv:handshake");
#endif

		state.network_state.handshake = false;

		//update map
		state.map_state.set_selected_province(dcon::province_id{});
		state.map_state.unhandled_province_selection = true;
			});

	return r;
}

void server_send_handshake(sys::state& state, network::client_data& client) {
	/* Tell the client their assigned nation */
	auto plnation = get_player_nation(state, client.hshake_buffer.nickname);
	if(plnation) {
		client.playing_as = plnation;
	} else {
		client.playing_as = choose_nation_for_player(state);
	}
	assert(client.playing_as);

	/* Send it data so the client is in sync with everyone else! */
	server_handshake_data hshake;
	hshake.seed = state.game_seed;
	hshake.assigned_nation = client.playing_as;
	hshake.scenario_checksum = state.scenario_checksum;
	hshake.save_checksum = state.get_save_checksum();
	socket_add_to_send_queue(client.early_send_buffer, &hshake, sizeof(hshake));
#ifndef NDEBUG
	state.console_log("host:send:handshake | assignednation:" + std::to_string(client.playing_as.index()));
#endif

	/* Exit from handshake mode */
	client.handshake = false;
}

void init(sys::state& state) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer

	state.network_state.finished = false;
#ifdef _WIN64
	WSADATA data;
	if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		window::emit_error_message("WSA startup error: " + get_last_error_msg(), true);
	}
#endif
	if(state.network_mode == sys::network_mode_type::host) {
		state.network_state.socket_fd = socket_init_server(state.network_state.as_v6, state.network_state.address);
	} else {
		assert(state.network_state.ip_address.size() > 0);
		state.network_state.socket_fd = socket_init_client(state.network_state.as_v6, state.network_state.address, state.network_state.ip_address.c_str());
	}

	// Host must have an already selected nation, to prevent issues...
	if(state.network_mode == sys::network_mode_type::host) {
		load_player_nations(state);

		auto nid = get_player_nation(state, state.network_state.nickname);
		state.local_player_nation = nid ? nid : choose_nation_for_player(state);

		assert(bool(state.local_player_nation));

		/* Materialize it into a command we send to new clients who connect and have to replay everything... */
		command::payload c;
		memset(&c, 0, sizeof(c));
		c.type = command::command_type::notify_player_joins;
		c.source = state.local_player_nation;
		c.data.notify_join.player_name = state.network_state.nickname;
		c.data.notify_join.player_password = state.network_state.player_password;
		state.network_state.outgoing_commands.push(c);
	}
	else if(state.network_mode == sys::network_mode_type::client) {
		/* Send our client's handshake */
		client_send_handshake(state);
	}
}

static uint8_t* write_network_compressed_section(uint8_t* ptr_out, uint8_t const* ptr_in, uint32_t uncompressed_size) {
	uint32_t decompressed_length = uncompressed_size;
	uint32_t section_length = uint32_t(ZSTD_compress(ptr_out + sizeof(uint32_t) * 2, ZSTD_compressBound(uncompressed_size), ptr_in,
		uncompressed_size, ZSTD_maxCLevel())); // write compressed data
	memcpy(ptr_out, &section_length, sizeof(uint32_t));
	memcpy(ptr_out + sizeof(uint32_t), &decompressed_length, sizeof(uint32_t));
	return ptr_out + sizeof(uint32_t) * 2 + section_length;
}

template<typename T>
static uint8_t const* with_network_decompressed_section(uint8_t const* ptr_in, T const& function) {
	uint32_t section_length = 0;
	uint32_t decompressed_length = 0;
	memcpy(&section_length, ptr_in, sizeof(uint32_t));
	memcpy(&decompressed_length, ptr_in + sizeof(uint32_t), sizeof(uint32_t));
	auto temp_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[decompressed_length]);
	ZSTD_decompress(temp_buffer.get(), decompressed_length, ptr_in + sizeof(uint32_t) * 2, section_length);
	function(temp_buffer.get(), decompressed_length);
	return ptr_in + sizeof(uint32_t) * 2 + section_length;
}



void notify_player_is_loading(sys::state& state, sys::player_name name, dcon::nation_id nation, bool execute_self) {
	command::payload c;
	memset(&c, 0, sizeof(c));
	c.type = command::command_type::notify_player_is_loading;
	c.source = nation;
	c.data.notify_player_is_loading.name = name;

	for(auto& cl : state.network_state.clients) {
		if(!cl.is_active()) {
			continue;
		}
		socket_add_to_send_queue(cl.send_buffer, &c, sizeof(c));
	}
	if(execute_self) {
		command::execute_command(state, c);
	}
#ifndef NDEBUG
	state.console_log("host:broadcast:cmd | type:notify_player_is_loading nation:" + std::to_string(nation.index()));
#endif
	write_player_nations(state);
}

void notify_player_joins(sys::state& state, sys::player_name name, dcon::nation_id nation, sys::player_password_raw password, bool needs_loading) {
	// Tell all clients about this client
	command::payload c;
	memset(&c, 0, sizeof(c));
	c.type = command::command_type::notify_player_joins;

	c.source = nation;
	c.data.notify_join.player_name = name;
	c.data.notify_join.player_password = password;
	c.data.notify_join.needs_loading = needs_loading;

	for(auto& cl : state.network_state.clients) {
		if(!cl.is_active() || cl.playing_as == nation) {
			continue;
		}
		socket_add_to_send_queue(cl.send_buffer, &c, sizeof(c));
	}
	command::execute_command(state, c);
#ifndef NDEBUG
	state.console_log("host:broadcast:cmd | type:notify_player_joins nation:" + std::to_string(nation.index()));
#endif
	write_player_nations(state);
}

void notify_player_joins(sys::state& state, network::client_data& client, bool needs_loading) {
	notify_player_joins(state, client.hshake_buffer.nickname, client.playing_as, client.hshake_buffer.player_password, needs_loading);
}

void notify_player_joins_discovery(sys::state& state, network::client_data& client) {
	for(const auto n : state.world.in_nation) {
		if(n.get_is_player_controlled()) {
			// Tell new client about all other clients

			command::payload c;
			memset(&c, 0, sizeof(c));
			c.type = command::command_type::notify_player_joins;
			c.source = n;
			auto p = find_country_player(state, n);
			assert(p);
			auto nickname = state.world.mp_player_get_nickname(p);
			c.data.notify_join.player_name = sys::player_name{ nickname };
			// if the player in question is not fully loaded, tell the other clients they have to load first
			c.data.notify_join.needs_loading = !state.world.mp_player_get_fully_loaded(p);
			socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
			state.console_log("host:send:cmd | type:notify_player_joins | to:" + std::to_string(client.playing_as.index()) + " | target nation:" + std::to_string(n.id.index())
			+ " | nickname: " + c.data.notify_join.player_name.to_string());
#endif
		}
	}
}
// loads the save from network which is currently in the save buffer
void load_network_save(sys::state& state, const uint8_t* save_buffer) {
	state.render_semaphore.acquire();
	std::vector<dcon::nation_id> players;
	for(const auto n : state.world.in_nation)
		if(state.world.nation_get_is_player_controlled(n))
			players.push_back(n);
	dcon::nation_id old_local_player_nation = state.local_player_nation;
	state.local_player_nation = dcon::nation_id{ };
	// Then reload from network
	state.reset_state();
	with_network_decompressed_section(save_buffer, [&state](uint8_t const* ptr_in, uint32_t length) {
		read_save_section(ptr_in, ptr_in + length, state);
	});
	network::place_players_after_reload(state, players, old_local_player_nation);
	state.fill_unsaved_data();
	state.render_semaphore.release();
	assert(state.world.nation_get_is_player_controlled(state.local_player_nation));
}

void send_savegame(sys::state& state, network::client_data& client, bool hotjoin = false) {
	/*std::vector<char> tmp = client.send_buffer;
	client.send_buffer.clear();*/

	/* Send the savefile to the newly connected client (if not a new game) */
	{
		command::payload c;
		memset(&c, 0, sizeof(command::payload));
		c.type = command::command_type::notify_save_loaded;
		c.source = state.local_player_nation;
		c.data.notify_save_loaded.target = client.playing_as;
		network::broadcast_save_to_clients(state, c, state.network_state.current_save_buffer.get(), state.network_state.current_save_length, state.network_state.current_mp_state_checksum);
#ifndef NDEBUG

		state.console_log("host:broadcast:cmd | (new->save_loaded) | checksum: " + sha512.hash(state.network_state.current_mp_state_checksum.to_char())
		+ " | target: " + std::to_string(c.data.notify_save_loaded.target.index()));
		log_player_nations(state);
#endif
	}

	if(hotjoin) {
		// we expect the save has been reloaded by now
		{ /* Reload all the other clients except the newly connected one */
			command::payload c;
			memset(&c, 0, sizeof(command::payload));
			c.type = command::command_type::notify_reload;
			c.source = state.local_player_nation;
			c.data.notify_reload.checksum = state.get_mp_state_checksum();
			for(auto& other_client : state.network_state.clients) {
				if(other_client.playing_as != client.playing_as && other_client.is_active()) {
					// for each client that must reload, notify every other client that they are loading
					network::notify_player_is_loading(state, other_client.hshake_buffer.nickname, other_client.playing_as, true);

					// then send the actual reload notification
					socket_add_to_send_queue(other_client.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
					state.console_log("host:send:cmd: (new->reload) | to:" + std::to_string(other_client.playing_as.index()));
#endif
				}
			}
		}
	}

	/*auto old_size = client.send_buffer.size();
	client.send_buffer.resize(old_size + tmp.size());
	std::memcpy(client.send_buffer.data() + old_size, tmp.data(), tmp.size());*/
}

void notify_start_game(sys::state& state, network::client_data& client) {
	// notify_start_game
	command::payload c;
	memset(&c, 0, sizeof(c));
	c.type = command::command_type::notify_start_game;
	c.source = state.local_player_nation;
	socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
	state.console_log("host:send:cmd | (new->start_game) to:" + std::to_string(client.playing_as.index()));
#endif
}

bool client_data::is_banned(sys::state& state) const {
	if(state.network_state.as_v6) {
		auto sa = (struct sockaddr_in6 const*)&address;
		return std::find_if(state.network_state.v6_banlist.begin(), state.network_state.v6_banlist.end(), [&](auto const& a) {
			return std::memcmp(&sa->sin6_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v6_banlist.end();
	} else {
		auto sa = (struct sockaddr_in const*)&address;
		return std::find_if(state.network_state.v4_banlist.begin(), state.network_state.v4_banlist.end(), [&](auto const& a) {
			return std::memcmp(&sa->sin_addr, &a, sizeof(a)) == 0;
		}) != state.network_state.v4_banlist.end();
	}
}

static void send_post_handshake_commands(sys::state& state, network::client_data& client) {
	std::vector<char> tmp = client.send_buffer;
	client.send_buffer.clear();

	bool paused = false;

	if(state.current_scene.starting_scene) {
		/* Lobby - existing savegame */
		notify_player_joins(state, client, !state.network_state.is_new_game);
		notify_player_joins_discovery(state, client);
		if(!state.network_state.is_new_game) {
			// compare the last save checksum with the current one, if it dosent match, write new save into the buffer
			if(state.network_state.last_save_checksum.to_string() != state.get_save_checksum().to_string()) {
				network::write_network_save(state);
			}
			// load the save which was just written
			load_network_save(state, state.network_state.current_save_buffer.get());
			// generate checksum for the entire mp state
			state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();
			send_savegame(state, client, true);
		}

	} else if(state.current_scene.game_in_progress) {
		notify_player_joins(state, client, !state.network_state.is_new_game);
		notify_player_joins_discovery(state, client);
			if(!state.network_state.is_new_game) {
				paused = pause_game(state);
				// compare the last save checksum with the current one, if it dosent match, write new save into the buffer
				if(state.network_state.last_save_checksum.to_string() != state.get_save_checksum().to_string()) {
					network::write_network_save(state);
				}
				// load the save which was just written
				load_network_save(state, state.network_state.current_save_buffer.get());
				// generate checksum for the entire mp state
				state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();
				send_savegame(state, client, true);
			}
		
		notify_start_game(state, client);
	}
	
	auto old_size = client.send_buffer.size();
	client.send_buffer.resize(old_size + tmp.size());
	std::memcpy(client.send_buffer.data() + old_size, tmp.data(), tmp.size());

	if(paused) {
		unpause_game(state);
	}
}

void full_reset_after_oos(sys::state& state) {
	pause_game(state);
#ifndef NDEBUG
	state.console_log("host:full_reset_after_oos");
	network::log_player_nations(state);
#endif
	/* Reload clients */
	if(!state.network_state.is_new_game) {
		// notfy every client that every client is now loading (reloading or loading the save)
		for(auto& loading_client : state.network_state.clients) {
			if(loading_client.is_active()) {
				network::notify_player_is_loading(state, loading_client.hshake_buffer.nickname, loading_client.playing_as, true);
			}
		}
		// if the save state has changed, write a new network save
		if(state.network_state.last_save_checksum.to_string() != state.get_save_checksum().to_string()) {
			network::write_network_save(state);
		}
		/* Then reload as if we loaded the save data */
		load_network_save(state, state.network_state.current_save_buffer.get());
		// generate checksum for the entire mp state
		state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();
		{ // set up commands, one for reload, one for save load
			command::payload reload_cmd;
			memset(&reload_cmd, 0, sizeof(command::payload));
			reload_cmd.type = command::command_type::notify_reload;
			reload_cmd.source = state.local_player_nation;
			reload_cmd.data.notify_reload.checksum = state.network_state.current_mp_state_checksum;


			command::payload save_load_cmd;
			memset(&save_load_cmd, 0, sizeof(command::payload));
			save_load_cmd.type = command::command_type::notify_save_loaded;
			save_load_cmd.source = state.local_player_nation;
			save_load_cmd.data.notify_save_loaded.checksum = state.network_state.current_mp_state_checksum;



			// iterate over clients and send command to the non-oos ones to reload their own gamestate, and send a save load command to each of the oos'd clients
			for(auto& other_client : state.network_state.clients) {
				for(auto player : state.world.in_mp_player) {
					// if the client is active, has the same nation as the player, and said player is NOT oos, then notify reload
					if(other_client.is_active() && other_client.playing_as == player.get_nation_from_player_nation()) {
						// if the client is oos, send the save load command
						if(player.get_is_oos()) {
							broadcast_save_to_single_client(state, save_load_cmd, other_client, state.network_state.current_save_buffer.get(), state.network_state.current_save_length);
#ifndef NDEBUG
							state.console_log("host:broadcast save to player " + other_client.hshake_buffer.nickname.to_string() + ":cmd | (new->save_loaded)");
#endif
						}
						// if not oos, send reload command
						else {

							socket_add_to_send_queue(other_client.send_buffer, &reload_cmd, sizeof(reload_cmd));
#ifndef NDEBUG
							state.console_log("host:send:cmd | (new->reload) to:" + std::to_string(other_client.playing_as.index()) +
							"| checksum: " + sha512.hash(reload_cmd.data.notify_reload.checksum.to_char()));
#endif
						}

						// for each client that must reload, notify every other client that they are loading
						/*command::payload p;
						memset(&p, 0, sizeof(p));
						p.type = command::command_type::notify_player_is_loading;
						p.source = other_client.playing_as;
						p.data.notify_player_is_loading.name = other_client.hshake_buffer.nickname;
						command::execute_command(state, p);
						for(auto& to_be_notified : state.network_state.clients) {
							if(to_be_notified.is_active()) {
								socket_add_to_send_queue(to_be_notified.send_buffer, &p, sizeof(p));
							}
						}*/


					}
				}
				
			}
		}
		/*{ 
			command::payload c;
			memset(&c, 0, sizeof(command::payload));
			c.type = command::command_type::notify_save_loaded;
			c.source = state.local_player_nation;
			c.data.notify_save_loaded.checksum = state.network_state.current_mp_state_checksum;

			for(auto& other_client : state.network_state.clients) {
				for(auto player : state.world.in_mp_player) {
					if(other_client.playing_as == player.get_nation_from_player_nation() && player.get_is_oos()) {
						broadcast_save_to_single_client(state, c, other_client, state.network_state.current_save_buffer.get(), state.network_state.current_save_length);
					}
				}
			}

		}*/
	}
	{
		command::payload c;
		memset(&c, 0, sizeof(c));
		c.type = command::command_type::notify_start_game;
		c.source = state.local_player_nation;

		broadcast_to_clients(state, c);

		// send message to everyone letting them know that the lobby has been resync'd
		command::chat_message(state, state.local_player_nation, text::produce_simple_string(state, "alice_host_has_resync"), dcon::nation_id{ });
#ifndef NDEBUG
		state.console_log("host:broadcast:cmd | (new->start_game)");
#endif
	}
}

int server_process_handshake(sys::state& state, network::client_data& client) {
	auto r = socket_recv(client.socket_fd, &client.hshake_buffer, sizeof(client.hshake_buffer), &client.recv_count, [&]() {
#ifndef NDEBUG
		state.console_log("host:recv:handshake | nickname: " + client.hshake_buffer.nickname.to_string());
#endif
		// Check lobby password
		if(std::memcmp(client.hshake_buffer.lobby_password, state.network_state.lobby_password, sizeof(state.network_state.lobby_password)) != 0) {
			disconnect_client(state, client, false);
			return;
		}

		// Don't allow two players with the same nickname
		for(auto& c : state.network_state.clients) {
			if(!c.is_active()) {
				continue;
			}

			if(c.hshake_buffer.nickname.to_string_view() == client.hshake_buffer.nickname.to_string_view() && c.socket_fd != client.socket_fd) {
				disconnect_client(state, client, false);
				return;
			}
		}

		// Check player password
		for(auto pl : state.world.in_mp_player) {
			auto nickname_1 = sys::player_name{ pl.get_nickname() }.to_string();
			auto nickname_2 = client.hshake_buffer.nickname.to_string();
			auto hash_1 = sys::player_password_hash{ pl.get_password_hash() };
			auto password_2 = client.hshake_buffer.player_password.to_string();
			auto salt = sys::player_password_salt{ pl.get_password_salt() }.to_string();
			auto hash_2 = sys::player_password_hash{}.from_string_view(sha512.hash(password_2 + salt));

			// If no password is set we allow player to set new password with this connection
			if(nickname_1 == nickname_2 && !hash_1.empty() && hash_1.to_string() != hash_2.to_string()) {
				disconnect_client(state, client, false);
				return;
			}
			else if(nickname_1 == nickname_2) {
				break;
			}
		}

		server_send_handshake(state, client);
		send_post_handshake_commands(state, client);
		state.game_state_updated.store(true, std::memory_order::release);
			});

	return r;
}

int server_process_commands(sys::state& state, network::client_data& client) {
	int r = socket_recv(client.socket_fd, &client.recv_buffer, sizeof(client.recv_buffer), &client.recv_count, [&]() {
		switch(client.recv_buffer.type) {
			// client can notify the host that they are loaded without needing to check the num of clients loading
		case command::command_type::notify_player_fully_loaded:
			if(client.recv_buffer.source == client.playing_as
			&& command::can_perform_command(state, client.recv_buffer)) {
				state.network_state.outgoing_commands.push(client.recv_buffer);
			}
			break;
		case command::command_type::invalid:
		case command::command_type::notify_player_ban:
		case command::command_type::notify_player_kick:
		case command::command_type::notify_save_loaded:
		case command::command_type::notify_reload:
		case command::command_type::advance_tick:
		case command::command_type::notify_start_game:
		case command::command_type::notify_stop_game:
		case command::command_type::notify_pause_game:
		case command::command_type::notify_player_joins:
		case command::command_type::save_game:
			break; // has to be valid/sendable by client
		default:
			/* Has to be from the nation of the client proper - and early
			discard invalid commands, and no clients must be currently loading */
			if(client.recv_buffer.source == client.playing_as
			&& command::can_perform_command(state, client.recv_buffer)
			&& state.network_state.num_client_loading == 0  ) {
				state.network_state.outgoing_commands.push(client.recv_buffer);
			}
			break;
		}
#ifndef NDEBUG
		state.console_log("host:recv:client_cmd | from:" + std::to_string(client.playing_as.index()) + " type:" + readableCommandTypes[uint32_t(client.recv_buffer.type)]);
#endif
			});

	return r;
}

static void receive_from_clients(sys::state& state) {

	for(auto& client : state.network_state.clients) {
		if(!client.is_active())
			continue;
		int r = 0;
		if(client.handshake) {
			r = server_process_handshake(state, client);
			if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
				state.console_log("host:disconnect | in-receive err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg() + " from:" + std::to_string(client.playing_as.index()));
#endif
				network::disconnect_client(state, client, false);
			}

			return;
		}

		int commandspertick = 0;
		while(r == 0 && commandspertick < 10) {
			r = server_process_commands(state, client);
			commandspertick++;
		}

		if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
			state.console_log("host:disconnect | in-receive err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg() + " from:" + std::to_string(client.playing_as.index()));
#endif
			network::disconnect_client(state, client, false);
		}
	}
}

bool pause_game(sys::state& state) {
	state.console_log("Pausing the game");

	if(state.actual_game_speed == 0) {
		return false;
	}

	state.ui_state.held_game_speed = state.actual_game_speed.load();
	state.actual_game_speed = 0;
	if(state.network_mode == sys::network_mode_type::host) {
		command::notify_pause_game(state, state.local_player_nation);
	}
	return true;
}

bool unpause_game(sys::state& state) {
	state.console_log("Unpausing the game");

	state.actual_game_speed = state.ui_state.held_game_speed;
	return true;
}

void write_network_save(sys::state& state) {
	/* A save lock will be set when we load a save, naturally loading a save implies
	that we have done preload/fill_unsaved so we will skip doing that again, to save a
	bit of sanity on our miserable CPU */
	size_t length = sizeof_save_section(state);
	auto save_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[length]);
	/* Clear the player nation since it is part of the savegame */
	write_save_section(save_buffer.get(), state); //writeoff data
	// this is an upper bound, since compacting the data may require less space
	state.network_state.current_save_buffer.reset(new uint8_t[ZSTD_compressBound(length) + sizeof(uint32_t) * 2]);
	auto buffer_position = write_network_compressed_section(state.network_state.current_save_buffer.get(), save_buffer.get(), uint32_t(length));
	state.network_state.current_save_length = uint32_t(buffer_position - state.network_state.current_save_buffer.get());
	state.network_state.last_save_checksum = state.get_save_checksum();
	//state.network_state.current_mp_state_checksum = state.get_mp_state_checksum();

}

void broadcast_save_to_clients(sys::state& state, command::payload& c, uint8_t const* buffer, uint32_t length, sys::checksum_key const& k) {
	assert(length > 0);
	assert(c.type == command::command_type::notify_save_loaded);
	c.data.notify_save_loaded.checksum = k;
	for(auto& client : state.network_state.clients) {
		if(!client.is_active())
			continue;
		bool send_full = (client.playing_as == c.data.notify_save_loaded.target) || (!c.data.notify_save_loaded.target);
		if(send_full && !state.network_state.is_new_game) {
			broadcast_save_to_single_client(state, c, client, buffer, length);
		}
	}
}
void broadcast_save_to_single_client(sys::state& state, command::payload& c, client_data& client, uint8_t const* buffer, uint32_t length) {
	/* And then we have to first send the command payload itself */
	client.save_stream_size = size_t(length);
	c.data.notify_save_loaded.length = size_t(length);
	socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
	/* And then the bulk payload! */
	client.save_stream_offset = client.total_sent_bytes + client.send_buffer.size();
	socket_add_to_send_queue(client.send_buffer, buffer, size_t(length));
#ifndef NDEBUG
	state.console_log("host:send:save | to" + std::to_string(client.playing_as.index()) + " len: " + std::to_string(uint32_t(length)));
#endif
}

void broadcast_to_clients(sys::state& state, command::payload& c) {
	if(c.type == command::command_type::save_game)
		return;
	assert(c.type != command::command_type::notify_save_loaded);

	if(c.type == command::command_type::notify_player_joins) {
		c.data.notify_join.player_password = sys::player_password_raw{}; // Never send password to clients
	}
	/* Propagate to all the clients */
	for(auto& client : state.network_state.clients) {
		if(client.is_active()) {
			socket_add_to_send_queue(client.send_buffer, &c, sizeof(c));
		}
	}
}

static void accept_new_clients(sys::state& state) {
	/* Check if any new clients are to join us */
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(state.network_state.socket_fd, &rfds);
	struct timeval tv{};
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	if(select(socket_t(int(state.network_state.socket_fd) + 1), &rfds, nullptr, nullptr, &tv) <= 0)
		return;
	
	// Find available slot for client
	for(auto& client : state.network_state.clients) {
		if(client.is_active())
			continue;
		socklen_t addr_len = state.network_state.as_v6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
		client.socket_fd = accept(state.network_state.socket_fd, (struct sockaddr*)&client.address, &addr_len);
		client.last_seen = state.current_date;
		if(client.is_banned(state)) {
			disconnect_client(state, client, false);
			break;
		}
		if(state.current_scene.final_scene) {
			disconnect_client(state, client, false);
			break;
		}
		return;
	}
}

void send_and_receive_commands(sys::state& state) {
	/* An issue that arose in multiplayer is that the UI was loading the savefile
	   directly, while the game state loop was running, this was fine with the
	   assumption that commands weren't executed while the save was being loaded
	   HOWEVER in multiplayer this is often the case, so we have to block all
	   commands until the savefile is finished loading
	   This way, we're able to effectively and safely queue commands until we
	   can receive them AFTER loading the savefile. */
	if(state.network_state.save_slock.load(std::memory_order::acquire) == true)
		return;

	if(state.network_state.finished)
		return;

	bool command_executed = false;
	if(state.network_mode == sys::network_mode_type::host) {
		accept_new_clients(state); // accept new connections
		receive_from_clients(state); // receive new commands

		// send the commands of the server to all the clients
		auto* c = state.network_state.outgoing_commands.front();
		while(c) {
			if(!command::is_console_command(c->type)) {
				// Generate checksum on the spot
				if(c->type == command::command_type::advance_tick) {
					if(state.current_date.to_ymd(state.start_date).day == 1 || state.cheat_data.daily_oos_check) {
						c->data.advance_tick.checksum = state.get_mp_state_checksum();
					}
				}
#ifndef NDEBUG
				state.console_log("host:send:cmd | from " + std::to_string(c->source.index()) + " type:" + readableCommandTypes[(uint32_t(c->type))]);
#endif
				// if the command could not be performed on the host, don't bother sending it to the clients
				if(command::execute_command(state, *c)) {
					broadcast_to_clients(state, *c);
				}
				command_executed = true;
			}
			state.network_state.outgoing_commands.pop();
			c = state.network_state.outgoing_commands.front();
		}

		// Clear lost sockets
		if(state.current_date.to_ymd(state.start_date).day == 1 || state.cheat_data.daily_oos_check) {
			for(auto& client : state.network_state.clients) {
				if(!client.is_active())
					continue;

				// Drop lost clients
				if(state.current_scene.game_in_progress && state.current_date.value > state.host_settings.alice_lagging_behind_days_to_drop && state.current_date.value - client.last_seen.value > state.host_settings.alice_lagging_behind_days_to_drop) {
					disconnect_client(state, client, true);
				}
				// Slow down for the lagging ones
				else if (state.current_scene.game_in_progress && state.current_date.value > state.host_settings.alice_lagging_behind_days_to_slow_down && state.current_date.value - client.last_seen.value > state.host_settings.alice_lagging_behind_days_to_slow_down) {
					state.actual_game_speed = std::clamp(state.actual_game_speed - 1, 1, 4);
				}
			}
		}

		for(auto& client : state.network_state.clients) {
			if(!client.is_active())
				continue;
			if(client.early_send_buffer.size() > 0) {
				size_t old_size = client.early_send_buffer.size();
				int r = socket_send(client.socket_fd, client.early_send_buffer);
				if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
					state.console_log("host:disconnect | in-send-EARLY err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg());
#endif
					disconnect_client(state, client, false);
					continue;
				}
				client.total_sent_bytes += old_size - client.early_send_buffer.size();
#ifndef NDEBUG
				if(old_size != client.early_send_buffer.size())
					state.console_log("host:send:stats | [EARLY] to:" + std::to_string(client.playing_as.index()) + "total:" + std::to_string(uint32_t(client.total_sent_bytes)) + " bytes");
#endif
		} else if(client.send_buffer.size() > 0) {
				size_t old_size = client.send_buffer.size();
				int r = socket_send(client.socket_fd, client.send_buffer);
				if(r > 0) { // error
#if !defined(NDEBUG) && defined(_WIN32)
					state.console_log("host:disconnect | in-send-INGAME err=" + std::to_string(int32_t(r)) + "::" + get_last_error_msg());
#endif
					disconnect_client(state, client, false);
					continue;
				}
				client.total_sent_bytes += old_size - client.send_buffer.size();
#ifndef NDEBUG
				if(old_size != client.send_buffer.size())
					state.console_log("host:send:stats | [SEND] to:" +std::to_string(client.playing_as.index()) + "total:" + std::to_string(uint32_t(client.total_sent_bytes)) + " bytes");
#endif
			}
		}
	} else if(state.network_mode == sys::network_mode_type::client) {
		if(state.network_state.handshake) {
			int r = client_process_handshake(state);
			if(r > 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client handshake receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		} else if(state.network_state.save_stream) {
			int r = socket_recv(state.network_state.socket_fd, state.network_state.save_data.data(), state.network_state.save_data.size(),
				&state.network_state.recv_count, [&]() {
#ifndef NDEBUG
				state.console_log("client:recv:save | len=" + std::to_string(uint32_t(state.network_state.save_data.size())));
#endif
				window::change_cursor(state, window::cursor_type::busy);
				load_network_save(state, state.network_state.save_data.data());

#ifndef NDEBUG
				auto mp_state_checksum = state.get_mp_state_checksum();
				assert(mp_state_checksum.is_equal(state.session_host_checksum));
				state.console_log("client:loadsave | checksum:" + sha512.hash(state.session_host_checksum.to_char()) + "| localchecksum: " + sha512.hash(mp_state_checksum.to_char()));
				log_player_nations(state);
#endif

				state.railroad_built.store(true, std::memory_order::release);
				state.game_state_updated.store(true, std::memory_order::release);
				state.network_state.save_data.clear();
				state.network_state.save_stream = false; // go back to normal command loop stuff
				window::change_cursor(state, window::cursor_type::normal);
				command::notify_player_fully_loaded(state, state.local_player_nation, state.network_state.nickname); // notify that we are loaded and ready to start
			});
			if(r > 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client save stream receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		} else {
			// receive commands from the server and immediately execute them
			int r = socket_recv(state.network_state.socket_fd, &state.network_state.recv_buffer, sizeof(state.network_state.recv_buffer), &state.network_state.recv_count, [&]() {

#ifndef NDEBUG
				state.console_log("client:recv:cmd | from:" + std::to_string(state.network_state.recv_buffer.source.index()) + "type:" + readableCommandTypes[uint32_t(state.network_state.recv_buffer.type)]);
#endif

				command::execute_command(state, state.network_state.recv_buffer);
				command_executed = true;
				// start save stream!
				if(state.network_state.recv_buffer.type == command::command_type::notify_save_loaded) {
					uint32_t save_size = state.network_state.recv_buffer.data.notify_save_loaded.length;
					state.network_state.save_stream = true;
					assert(save_size > 0);
					if(save_size >= 32 * 1000 * 1000) { // 32 MB
						ui::popup_error_window(state, "Network Error", "Network client save stream too big: " + get_last_error_msg());
						network::finish(state, false);
						return;
					}
					state.network_state.save_data.resize(static_cast<size_t>(save_size));
				}

			});
			if(r > 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client command receive error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
			// send the outgoing commands to the server and flush the entire queue
			auto* c = state.network_state.outgoing_commands.front();
			while(c) {
#ifndef NDEBUG
				state.console_log("client:send:cmd | type:" + readableCommandTypes[uint32_t(c->type)]);
#endif
				if(c->type == command::command_type::save_game) {
					command::execute_command(state, *c);
					command_executed = true;
				} else {
					socket_add_to_send_queue(state.network_state.send_buffer, c, sizeof(*c));
				}
				state.network_state.outgoing_commands.pop();
				c = state.network_state.outgoing_commands.front();
			}
		}
		/* Do not send commands while we're on save stream mode! */
		if(!state.network_state.save_stream) {
			if(socket_send(state.network_state.socket_fd, state.network_state.send_buffer) != 0) { // error
				ui::popup_error_window(state, "Network Error", "Network client command send error: " + get_last_error_msg());
				network::finish(state, false);
				return;
			}
		}
		assert(state.network_state.early_send_buffer.empty()); //do not use the early send buffer
	}

	if(command_executed) {
		if(state.network_state.out_of_sync && !state.network_state.reported_oos) {
			command::notify_player_oos(state, state.local_player_nation);
			state.network_state.reported_oos = true;
		}
		state.game_state_updated.store(true, std::memory_order::release);
	}
}

void finish(sys::state& state, bool notify_host) {
	if(state.network_mode == sys::network_mode_type::single_player)
		return; // Do nothing in singleplayer

	state.network_state.finished = true;
	if(notify_host && state.network_mode == sys::network_mode_type::client) {
		if(!state.network_state.save_stream) {
			// send the outgoing commands to the server and flush the entire queue
			{
				auto* c = state.network_state.outgoing_commands.front();
				while(c) {
					if(c->type == command::command_type::save_game) {
						command::execute_command(state, *c);
					} else {
						socket_add_to_send_queue(state.network_state.send_buffer, c, sizeof(*c));
					}
					state.network_state.outgoing_commands.pop();
					c = state.network_state.outgoing_commands.front();
				}
			}
			command::payload c;
			memset(&c, 0, sizeof(c));
			c.type = command::command_type::notify_player_leaves;
			c.source = state.local_player_nation;
			c.data.notify_leave.make_ai = (state.host_settings.alice_place_ai_upon_disconnection == 1);
			socket_add_to_send_queue(state.network_state.send_buffer, &c, sizeof(c));
#ifndef NDEBUG
			state.console_log("client:send:cmd | type:notify_player_leaves");
#endif
			while(state.network_state.send_buffer.size() > 0) {
				if(socket_send(state.network_state.socket_fd, state.network_state.send_buffer) != 0) { // error
					state.console_log("Network client command send error: " + get_last_error_msg());
					//ui::popup_error_window(state, "Network Error", "Network client command send error: " + get_last_error_msg());
					break;
				}
			}
		}
	}
	
	socket_shutdown(state.network_state.socket_fd);
#ifdef _WIN64
	WSACleanup();
#endif
}

void remove_player(sys::state& state, sys::player_name name) {
	auto p = find_mp_player(state, name);
	if(p) {

		auto c = state.world.mp_player_get_nation_from_player_nation(p);

		if(c) {
			state.world.nation_set_is_player_controlled(c, false);
		}
		auto rel = state.world.mp_player_get_player_nation(p);
		state.world.delete_player_nation(rel);
		state.world.delete_mp_player(p);
	}
}

void kick_player(sys::state& state, client_data& client) {
	socket_shutdown(client.socket_fd);

	clear_socket(state, client);
}

void ban_player(sys::state& state, client_data& client) {
	socket_shutdown(client.socket_fd);

	remove_player(state, client.hshake_buffer.nickname);

	clear_socket(state, client);

	if(state.network_state.as_v6) {
		auto sa = (struct sockaddr_in6*)&client.address;
		state.network_state.v6_banlist.push_back(sa->sin6_addr);
	} else {
		auto sa = (struct sockaddr_in*)&client.address;
		state.network_state.v4_banlist.push_back(sa->sin_addr);
	}
}

void switch_player(sys::state& state, dcon::nation_id new_n, dcon::nation_id old_n) {
	auto p = find_country_player(state, old_n);
	state.world.force_create_player_nation(new_n, p);

	if(state.network_mode == sys::network_mode_type::host) {
		for(auto& client : state.network_state.clients) {
			if(!client.is_active())
				continue;
			if(client.playing_as == old_n) {
				client.playing_as = new_n;
			}
		}

		write_player_nations(state);
	}
}

void place_host_player_after_saveload(sys::state& state) {
	load_player_nations(state);

	auto n = choose_nation_for_player(state);
	state.local_player_nation = n;
	assert(bool(state.local_player_nation));
	state.world.nation_set_is_player_controlled(n, true);

	command::payload c;
	memset(&c, 0, sizeof(c));
	c.type = command::command_type::notify_player_joins;
	c.source = n;
	c.data.notify_join.player_name = state.network_state.nickname;
	state.local_player_nation = c.source;
	state.network_state.outgoing_commands.push(c);

	log_player_nations(state);
}

void load_host_settings(sys::state& state) {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	auto settings_file = open_file(settings_location, NATIVE("host_settings.json"));
	if(settings_file) {
		auto content = view_contents(*settings_file);
		json data = json::parse(content.data);

		if(!data["alice_expose_webui"].empty()) \
			state.host_settings.alice_expose_webui = data["alice_expose_webui"];

#define HS_LOAD(x, y) \
if(!data[x].empty()) \
state.host_settings.y = data[x]

		HS_LOAD("alice_expose_webui", alice_expose_webui);
		HS_LOAD("alice_lagging_behind_days_to_drop", alice_lagging_behind_days_to_drop);
		HS_LOAD("alice_persistent_server_mode", alice_persistent_server_mode);
		HS_LOAD("alice_place_ai_upon_disconnection", alice_place_ai_upon_disconnection);
		HS_LOAD("alice_persistent_server_pause", alice_persistent_server_pause);
		HS_LOAD("alice_persistent_server_unpause", alice_persistent_server_unpause);
	}
}

// Used primarily to create JSON file with default values
void save_host_settings(sys::state& state) {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	auto settings_file = open_file(settings_location, NATIVE("host_settings.json"));
	if(!settings_file) {

#define HS_SAVE(x, y) \
data[x] = state.host_settings.y

		json data = json::object();

		HS_SAVE("alice_expose_webui", alice_expose_webui);
		HS_SAVE("alice_lagging_behind_days_to_drop", alice_lagging_behind_days_to_drop);
		HS_SAVE("alice_persistent_server_mode", alice_persistent_server_mode);
		HS_SAVE("alice_place_ai_upon_disconnection", alice_place_ai_upon_disconnection);
		HS_SAVE("alice_persistent_server_pause", alice_persistent_server_pause);
		HS_SAVE("alice_persistent_server_unpause", alice_persistent_server_unpause);

		std::string res = data.dump();

		simple_fs::write_file(settings_location, NATIVE("host_settings.json"), res.data(), uint32_t(res.length()));
	}
}

}
