#ifndef THINGSPEAK_CLIENT_H
#define THINGSPEAK_CLIENT_H

#include "lwip/tcp.h"
#include "lwip/dns.h"

// Funções
static err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t http_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err);
static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
void enviar_luminosidade_ao_thingspeak(int luminosidade);
void connect_wifi();

#endif // THINGSPEAK_CLIENT_H
