
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY   0
#define ICMP_HEADER_SIZE  8
#define PAYLOAD_SIZE      1000
#define PACKET_SIZE       (ICMP_HEADER_SIZE + PAYLOAD_SIZE)
#define PAIR_COUNT        10
#define TIMEOUT_SECONDS   2

typedef struct {
    uint16_t sequence;
    struct timespec arrival;
    int received;
} reply_record;

/* Calculate the Internet checksum over bytes in network order. */
static uint16_t checksum(const uint8_t *data, size_t length)
{
    uint32_t sum = 0;

    while (length >= 2) {
        sum += ((uint16_t)data[0] << 8) | data[1];
        data += 2;
        length -= 2;
    }

    if (length == 1)
        sum += (uint16_t)data[0] << 8;

    while (sum >> 16)
        sum = (sum & 0xffffU) + (sum >> 16);

    return (uint16_t)(~sum);
}

static uint16_t read_be16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static double seconds_between(struct timespec start, struct timespec end)
{
    return (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
}

/* Send one manually constructed ICMP Echo Request. */
static int send_echo(int sock, const struct sockaddr_in *target, uint16_t identifier, uint16_t sequence)
{
    uint8_t packet[PACKET_SIZE];
    memset(packet, 0, sizeof(packet));

    packet[0] = ICMP_ECHO_REQUEST;
    packet[1] = 0;

    packet[4] = (uint8_t)(identifier >> 8);
    packet[5] = (uint8_t)(identifier & 0xff);
    packet[6] = (uint8_t)(sequence >> 8);
    packet[7] = (uint8_t)(sequence & 0xff);

    for (size_t i = ICMP_HEADER_SIZE; i < sizeof(packet); i++)
        packet[i] = (uint8_t)((i + sequence) & 0xff);

    uint16_t sum = checksum(packet, sizeof(packet));
    packet[2] = (uint8_t)(sum >> 8);
    packet[3] = (uint8_t)(sum & 0xff);

    ssize_t sent = sendto(sock, packet, sizeof(packet), 0, (const struct sockaddr *)target, sizeof(*target));

    if (sent < 0) {
        perror("sendto");
        return -1;
    }

    if ((size_t)sent != sizeof(packet)) {
        fprintf(stderr, "Incomplete packet send.\n");
        return -1;
    }

    return 0;
}

/*
 * Receive matching Echo Replies until both pair members arrive
 * or the timeout expires. Replies are matched by sequence number,
 * so their arrival order does not matter.
 *
 * Returns:
 *   1 = both replies received
 *   0 = timeout / incomplete pair
 *  -1 = system error
 */
static int receive_pair(int sock, uint16_t identifier,
                        reply_record *first, reply_record *second)
{
    uint8_t response[65536];

    while (!first->received || !second->received) {
        struct sockaddr_in from;
        socklen_t from_len = sizeof(from);

        ssize_t n = recvfrom(sock, response, sizeof(response), 0,
                             (struct sockaddr *)&from, &from_len);

        if (n < 0) {
            if (errno == EINTR)
                continue;

            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return 0;

            perror("recvfrom");
            return -1;
        }

        if ((size_t)n < 20)
            continue;

        size_t ip_header_len = (size_t)(response[0] & 0x0f) * 4U;

        if ((response[0] >> 4) != 4 ||
            ip_header_len < 20 ||
            (size_t)n < ip_header_len + ICMP_HEADER_SIZE)
            continue;

        const uint8_t *icmp = response + ip_header_len;

        if (icmp[0] != ICMP_ECHO_REPLY || icmp[1] != 0)
            continue;

        if (read_be16(icmp + 4) != identifier)
            continue;

        uint16_t seq = read_be16(icmp + 6);
        reply_record *record = NULL;

        if (seq == first->sequence)
            record = first;
        else if (seq == second->sequence)
            record = second;
        else
            continue;

        if (record->received)
            continue;

        /*
         * Timestamp as soon as this process receives the matching
         * reply. This is not a hardware/kernel arrival timestamp.
         */
        if (clock_gettime(CLOCK_MONOTONIC, &record->arrival) < 0) {
            perror("clock_gettime");
            return -1;
        }

        record->received = 1;
    }

    return 1;
}

/* Sort rates in ascending order for median calculation. */
static void sort_values(double values[], int count)
{
    for (int i = 1; i < count; i++) {
        double value = values[i];
        int j = i - 1;

        while (j >= 0 && values[j] > value) {
            values[j + 1] = values[j];
            j--;
        }

        values[j + 1] = value;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination-ipv4>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in target;
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;

    if (inet_pton(AF_INET, argv[1], &target.sin_addr) != 1) {
        fprintf(stderr, "Invalid IPv4 address: %s\n", argv[1]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sock < 0) {
        perror("Raw ICMP socket (try sudo)");
        return 1;
    }

    struct timeval timeout = {
        .tv_sec = TIMEOUT_SECONDS,
        .tv_usec = 0
    };

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        close(sock);
        return 1;
    }

    uint16_t identifier = (uint16_t)(getpid() & 0xffff);
    uint16_t sequence = 1;

    double estimates[PAIR_COUNT];
    int valid = 0;

    printf("Packet-pair bandwidth experiment\n");
    printf("Destination: %s\n", argv[1]);
    printf("ICMP payload: %d bytes; %d pairs\n\n", PAYLOAD_SIZE, PAIR_COUNT);

    for (int pair = 0; pair < PAIR_COUNT; pair++) {
        reply_record first = {
            .sequence = sequence++,
            .received = 0
        };

        reply_record second = {
            .sequence = sequence++,
            .received = 0
        };

        printf("Pair %2d: sending sequences %u and %u...\n", pair + 1, first.sequence, second.sequence);

        /*
         * Send both requests before waiting for replies.
         */
        if (send_echo(sock, &target, identifier, first.sequence) < 0 ||
            send_echo(sock, &target, identifier, second.sequence) < 0) {
            close(sock);
            return 1;
        }

        int result = receive_pair(sock, identifier, &first, &second);

        if (result < 0) {
            close(sock);
            return 1;
        }

        if (result == 0) {
            printf("  Timeout: received %s reply; pair skipped.\n",first.received && !second.received ? "first only" :!first.received && second.received ? "second only" : "no");
            continue;
        }

        /*
         * Replies may arrive out of order. Use the absolute arrival
         * gap so the calculation remains non-negative.
         */
        double gap = seconds_between(first.arrival, second.arrival);
        if (gap < 0.0)
            gap = -gap;

        if (gap <= 0.0) {
            printf("  Invalid arrival gap; pair skipped.\n");
            continue;
        }

        double bits = (double)PACKET_SIZE * 8.0;
        double mbps = bits / gap / 1e6;

        estimates[valid++] = mbps;

        printf("  Reply gap: %.9f seconds; rough rate: %.3f Mbps\n", gap, mbps);
    }

    close(sock);

    if (valid == 0) {
        printf("\nNo valid pairs. No estimate available.\n");
        return 0;
    }

    double sum = 0.0;

    for (int i = 0; i < valid; i++)
        sum += estimates[i];

    double mean = sum / valid;

    sort_values(estimates, valid);

    double median;
    if (valid % 2 == 1) {
        median = estimates[valid / 2];
    } else {
        median = (estimates[valid / 2 - 1] + estimates[valid / 2]) / 2.0;
    }

    printf("\n========== SUMMARY ==========\n");
    printf("Valid pairs: %d / %d\n", valid, PAIR_COUNT);
    printf("Mean reply-gap rate:   %.3f Mbps\n", mean);
    printf("Median reply-gap rate: %.3f Mbps\n", median);
    printf("Measurement: approximate ICMP reply-gap estimate.\n");
    printf("It is not a direct measurement of one-way link capacity.\n");
    printf("==============================\n");

    return 0;
}
