
#ifndef PMTU_H
#define PMTU_H

#include <stdint.h>

#define MIN_PROBE_SIZE 576
#define MAX_PROBE_SIZE 1500
#define IPV4_HEADER_SIZE 20
#define IPV4_VERSION 4
#define IPV4_IHL 5
#define IPV4_PROTOCOL_ICMP 1
#define DEFAULT_TTL 64

/*
 * Manually defined IPv4 header.
 * Fields are stored as individual wire-format bytes
 * where byte order matters.
 */
typedef struct {
    uint8_t version_ihl;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t source_ip;
    uint32_t destination_ip;
} ipv4_header;

/* Calculate the Internet checksum. */
uint16_t calculate_checksum(const void *data, int length);

/* Initialise a manually constructed IPv4 header. */
void build_ipv4_header(
    ipv4_header *header,
    uint16_t total_length,
    uint32_t source_ip,
    uint32_t destination_ip
);

/* Set the IPv4 Don't Fragment flag. */
void set_df_flag(ipv4_header *header);

/* Current PMTU module entry point. */
int discover_pmtu(const char *destination);

#endif
