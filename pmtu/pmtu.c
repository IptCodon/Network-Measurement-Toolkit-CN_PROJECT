
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

#include "pmtu.h"

uint16_t calculate_checksum(const void *data, int length)
{
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t sum = 0;

    if (data == NULL || length < 0) {
        return 0;
    }

    while (length >= 2) {
        sum += ((uint16_t)bytes[0] << 8) | bytes[1];
        bytes += 2;
        length -= 2;
    }

    if (length == 1) {
        sum += (uint16_t)bytes[0] << 8;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFFU) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

void build_ipv4_header(
    ipv4_header *header,
    uint16_t total_length,
    uint32_t source_ip,
    uint32_t destination_ip)
{
    if (header == NULL) {
        return;
    }

    memset(header, 0, sizeof(*header));

    header->version_ihl =
        (uint8_t)((IPV4_VERSION << 4) | IPV4_IHL);

    header->type_of_service = 0;
    header->total_length = htons(total_length);
    header->identification = htons(0);
    header->flags_fragment_offset = htons(0);
    header->ttl = DEFAULT_TTL;
    header->protocol = IPV4_PROTOCOL_ICMP;
    header->header_checksum = 0;
    header->source_ip = source_ip;
    header->destination_ip = destination_ip;
}

void set_df_flag(ipv4_header *header)
{
    uint16_t flags;

    if (header == NULL) {
        return;
    }

    flags = ntohs(header->flags_fragment_offset);
    flags |= 0x4000U;

    header->flags_fragment_offset = htons(flags);
}

int discover_pmtu(const char *destination)
{
    if (destination == NULL || destination[0] == '\0') {
        fprintf(stderr, "Error: destination is required.\n");
        return 1;
    }

    printf("PMTU discovery target: %s\n", destination);
    printf("Initial probe range: %d - %d bytes\n",
           MIN_PROBE_SIZE,
           MAX_PROBE_SIZE);

    printf("PMTU discovery is not implemented yet.\n");

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination-ip>\n", argv[0]);
        return 1;
    }

    return discover_pmtu(argv[1]);
}
