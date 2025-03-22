#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/init.h"
#include "lwipopts.h"
#include "thingspeak.h"

#define WIFI_SSID "TPN"
#define WIFI_PASS "121727tp"

#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_PORT 80

#define API_KEY "FML2RX1KKHDQUCCO"  // Chave de escrita do ThingSpeak


struct tcp_pcb *tcp_client_pcb;
ip_addr_t server_ip;


static int luminosidade_global; // Variável global para armazenar a luminosidade


// Função para conectar ao Wi-Fi
void connect_wifi() {
    // Tenta conectar com timeout de 10 segundos
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
         printf("Erro: não foi possível conectar ao Wi-Fi.\n");
    } else {
         printf("Conectado ao Wi-Fi!\n");
    }
}


// Callback quando recebe resposta do ThingSpeak
static err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    printf("Resposta do ThingSpeak: %.*s\n", p->len, (char *)p->payload);
    pbuf_free(p);
    return ERR_OK;
}

// Callback quando a conexão TCP é estabelecida
static err_t http_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Erro na conexão TCP\n");
        return err;
    }

    printf("Conectado ao ThingSpeak!\n");

    char request[256];
    
    snprintf(request, sizeof(request),
        "GET /update?api_key=%s&field1=%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        API_KEY, luminosidade_global, THINGSPEAK_HOST);

    tcp_write(tpcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_recv(tpcb, http_recv_callback);

    return ERR_OK;
}

// Resolver DNS e conectar ao servidor
static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr) {
        printf("Endereço IP do ThingSpeak: %s\n", ipaddr_ntoa(ipaddr));
        tcp_client_pcb = tcp_new();
        tcp_connect(tcp_client_pcb, ipaddr, THINGSPEAK_PORT, http_connected_callback);
    } else {
        printf("Falha na resolução de DNS\n");
    }
}

// Função pública para enviar dados ao ThingSpeak
void enviar_luminosidade_ao_thingspeak(int luminosidade) {
    luminosidade_global = luminosidade; // Armazena a luminosidade na variável global
    printf("Enviando luminosidade %d para ThingSpeak...\n", luminosidade);
    dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_callback, NULL);
}