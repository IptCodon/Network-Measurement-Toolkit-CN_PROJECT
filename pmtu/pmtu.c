#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "pmtu.h"

#define PROBE_SIZE 1200
#define ICMP_HEADER_SIZE 8
#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACHABLE 3
#define ICMP_FRAG_NEEDED 4
#define REPLY_TIMEOUT_SECONDS 2

_Static_assert(sizeof(ipv4_header) == IPV4_HEADER_SIZE,
            "IPv4 header must be exactly 20 bytes");

static uint16_t read_be16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}


uint16_t calculate_checksum(const void *data, int length)
{
    if (data == NULL || length < 0)
        return 0;

    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t sum = 0;

    while (length >= 2) {
        sum += ((uint16_t)bytes[0] << 8) | bytes[1];
        bytes += 2;
        length -= 2;
    }

    if (length == 1)
        sum += (uint16_t)bytes[0] << 8;

    while (sum >> 16)
        sum = (sum & 0xFFFFU) + (sum >> 16);

    return (uint16_t)(~sum);
}

void build_ipv4_header(ipv4_header *header, uint16_t total_length, uint32_t source_ip, uint32_t destination_ip)
{
    if (header == NULL)
        return;

    memset(header, 0, sizeof(*header));

    header->version_ihl =
        (uint8_t)((IPV4_VERSION << 4) | IPV4_IHL);

    header->total_length = htons(total_length);
    header->identification = htons((uint16_t)(getpid() & 0xFFFF));
    header->flags_fragment_offset = 0;
    header->ttl = DEFAULT_TTL;
    header->protocol = IPV4_PROTOCOL_ICMP;
    header->header_checksum = 0;
    header->source_ip = source_ip;
    header->destination_ip = destination_ip;
}

void set_df_flag(ipv4_header *header)
{
    if (header == NULL)
        return;

    uint16_t flags = ntohs(header->flags_fragment_offset);
    flags |= 0x4000U;

    header->flags_fragment_offset = htons(flags);
}


static int get_source_address(const struct in_addr *destination, struct in_addr *source)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0)
        return -1;

    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));

    remote.sin_family = AF_INET;
    remote.sin_port = htons(33434);
    remote.sin_addr = *destination;

    if (connect(fd, (struct sockaddr *)&remote,
                sizeof(remote)) < 0) {
        close(fd);
        return -1;
    }

    struct sockaddr_in local;
    socklen_t local_length = sizeof(local);

    if (getsockname(fd, (struct sockaddr *)&local,
                    &local_length) < 0) {
        close(fd);
        return -1;
    }

    *source = local.sin_addr;

    close(fd);
    return 0;
}

static int matches_probe(const uint8_t *icmp, size_t length, uint16_t identifier, uint16_t sequence)
{
    if (length < ICMP_HEADER_SIZE)
        return 0;

    return read_be16(icmp + 4) == identifier &&
            read_be16(icmp + 6) == sequence;
}


static int send_probe(const struct in_addr *destination,
                        size_t probe_size)
{
    if (probe_size < IPV4_HEADER_SIZE + ICMP_HEADER_SIZE ||
        probe_size > PROBE_SIZE) {
        fprintf(stderr, "Invalid probe size for this version.\n");
        return -1;
    }

    struct in_addr source;

    if (get_source_address(destination, &source) < 0) {
        perror("Could not determine source address");
        return -1;
    }

    uint8_t packet[PROBE_SIZE];
    memset(packet, 0, sizeof(packet));


    ipv4_header ip;

    build_ipv4_header(&ip, (uint16_t)probe_size, source.s_addr, destination->s_addr);

    set_df_flag(&ip);

    ip.header_checksum = 0;
    ip.header_checksum =
        htons(calculate_checksum(&ip, (int)sizeof(ip)));

    memcpy(packet, &ip, sizeof(ip));


    size_t icmp_length = probe_size - sizeof(ipv4_header);
    uint8_t *icmp = packet + sizeof(ipv4_header);

    uint16_t identifier = (uint16_t)(getpid() & 0xFFFF);
    uint16_t sequence = 1;

    icmp[0] = ICMP_ECHO_REQUEST;
    icmp[1] = 0;
    icmp[2] = 0;
    icmp[3] = 0;

    icmp[4] = (uint8_t)(identifier >> 8);
    icmp[5] = (uint8_t)(identifier & 0xFF);
    icmp[6] = (uint8_t)(sequence >> 8);
    icmp[7] = (uint8_t)(sequence & 0xFF);

    for (size_t i = ICMP_HEADER_SIZE; i < icmp_length; i++)
        icmp[i] = (uint8_t)(i & 0xFF);

    uint16_t icmp_checksum =
        calculate_checksum(icmp, (int)icmp_length);

    icmp[2] = (uint8_t)(icmp_checksum >> 8);
    icmp[3] = (uint8_t)(icmp_checksum & 0xFF);


    int send_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    if (send_fd < 0) {
        perror("Raw send socket (try sudo)");
        return -1;
    }

    int enabled = 1;

    if (setsockopt(send_fd, IPPROTO_IP, IP_HDRINCL,
                    &enabled, sizeof(enabled)) < 0) {
        perror("setsockopt IP_HDRINCL");
        close(send_fd);
        return -1;
    }


    int recv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (recv_fd < 0) {
        perror("Raw ICMP receive socket (try sudo)");
        close(send_fd);
        return -1;
    }

    struct timeval timeout = {.tv_sec = REPLY_TIMEOUT_SECONDS,.tv_usec = 0
    };

    if (setsockopt(recv_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt receive timeout");
        close(recv_fd);
        close(send_fd);
        return -1;
    }

    struct sockaddr_in target;
    memset(&target, 0, sizeof(target));

    target.sin_family = AF_INET;
    target.sin_addr = *destination;

    printf("Sending %zu-byte IPv4 probe to %s (DF set)...\n",
            probe_size, inet_ntoa(*destination));

    ssize_t sent = sendto(send_fd, packet, probe_size, 0,
                          (struct sockaddr *)&target,
                        sizeof(target));

    close(send_fd);

    if (sent < 0) {
        perror("sendto");
        close(recv_fd);
        return -1;
    }

    uint8_t response[65536];

    for (;;) {
        struct sockaddr_in responder;
        socklen_t responder_length = sizeof(responder);

        ssize_t received = recvfrom(
            recv_fd, response, sizeof(response), 0,
            (struct sockaddr *)&responder, &responder_length);

        if (received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("No matching ICMP response within %d seconds.\n",
                    REPLY_TIMEOUT_SECONDS);
                printf("Timeout is inconclusive; it does not prove "
                    "fragmentation was required.\n");
            } else {
                perror("recvfrom");
            }

            close(recv_fd);
            return 0;
        }

        if ((size_t)received < IPV4_HEADER_SIZE)
            continue;

        size_t outer_ihl = (size_t)(response[0] & 0x0F) * 4U;

        if ((response[0] >> 4) != 4 ||
            outer_ihl < IPV4_HEADER_SIZE ||
            (size_t)received < outer_ihl + ICMP_HEADER_SIZE)
            continue;

        uint8_t *reply = response + outer_ihl;
        size_t reply_length = (size_t)received - outer_ihl;

        if (reply[0] == ICMP_ECHO_REPLY &&
            reply[1] == 0 &&
            matches_probe(reply, reply_length,
                        identifier, sequence)) {
            printf("Matching Echo Reply received from %s.\n",
                        inet_ntoa(responder.sin_addr));
            printf("Probe succeeded at %zu bytes.\n", probe_size);

            close(recv_fd);
            return 1;
        }

        
        if (reply[0] == ICMP_DEST_UNREACHABLE &&
            reply[1] == ICMP_FRAG_NEEDED &&
            reply_length >= ICMP_HEADER_SIZE + IPV4_HEADER_SIZE) {

            const uint8_t *inner_ip = reply + ICMP_HEADER_SIZE;
            size_t inner_length = reply_length - ICMP_HEADER_SIZE;

            if ((inner_ip[0] >> 4) != 4)
                continue;

            size_t inner_ihl = (size_t)(inner_ip[0] & 0x0F) * 4U;

            if (inner_ihl < IPV4_HEADER_SIZE || inner_length < inner_ihl + ICMP_HEADER_SIZE || inner_ip[9] != IPV4_PROTOCOL_ICMP)
                continue;

            const uint8_t *inner_icmp = inner_ip + inner_ihl;
            size_t inner_icmp_length = inner_length - inner_ihl;

            if (inner_icmp[0] != ICMP_ECHO_REQUEST ||
                !matches_probe(inner_icmp, inner_icmp_length,
                            identifier, sequence))
                continue;

            uint16_t next_hop_mtu = read_be16(reply + 6);

            printf("Matching ICMP Fragmentation Needed received.\n");

            if (next_hop_mtu != 0)
                printf("Reported next-hop MTU: %u bytes\n", next_hop_mtu);
            else
                printf("Router did not report a next-hop MTU.\n");

            printf("Probe cannot pass at this size without " "fragmentation.\n");

            close(recv_fd);
            return 2;
        }


    }
}

int discover_pmtu(const char *destination)
{
    if (destination == NULL || destination[0] == '\0') {
        fprintf(stderr, "Error: destination IPv4 address is required.\n");
        return 1;
    }

    struct in_addr target;

    if (inet_pton(AF_INET, destination, &target) != 1) {
        fprintf(stderr, "Invalid IPv4 address: %s\n", destination);
        return 1;
    }

    printf("PMTU discovery target: %s\n", destination);
    printf("Current stage: single-probe test\n");

    int result = send_probe(&target, PROBE_SIZE);

    if (result == 1)
        printf("Stage result: probe succeeded.\n");
    else if (result == 2)
        printf("Stage result: fragmentation was required.\n");
    else if (result == 0)
        printf("Stage result: inconclusive.\n");
    else
        return 1;

    printf("Next step: implement the multi-size PMTU search.\n");
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination-ipv4>\n", argv[0]);
        return 1;
    }

    return discover_pmtu(argv[1]);
}
