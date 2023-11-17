#include <chrono>
#include <iostream>
#include <netdb.h>
#include <cstring>
#include "message.h"

/**
 * Calculates a 16-bit checksum for a given buffer.
 *
 * @param buf A pointer to the buffer containing the data over which
 *            the checksum is to be calculated.
 * @param len The length of the buffer in bytes.
 *
 * @return The calculated 16-bit checksum as an unsigned short.
 */
uint16_t get_checksum16(const uint8_t* buf, uint32_t len) {
    uint32_t sum = 0;
    for (uint32_t j = 0; j < len - 1; j += 2) {
        sum += *((uint16_t*)(&buf[j]));
    }
    if ((len & 1) != 0) {
        sum += buf[len - 1];
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum = (sum >> 16) + (sum & 0xFFFF);

    return uint16_t(~sum);
}

/**
 * Retrieves the current timestamp in nanoseconds since the Unix epoch.
 *
 * @return The current timestamp as a 64-bit unsigned integer, representing
 *         the number of nanoseconds since the Unix epoch.
 */
uint64_t get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    auto nano_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
    return nano_since_epoch.count();
}

/**
 * Sends an http request over a specified socket.
 *
 * This function sends data over a socket described by `socket_descriptor`.
 * The data to be sent is pointed to by `msg`, and its size is specified by `size`.
 *
 * @param socket_descriptor The file descriptor representing the socket.
 * @param msg A pointer to the data to be sent.
 * @param size The size of the data to be sent in bytes.
 *
 * @return Returns 0 on successful sending of the data, and 1 if there is an error.
 */
int send_http_request(int socket_descriptor, void* msg, size_t size) {
    int bytes_sent = send(socket_descriptor, msg, size, 0);
    if (bytes_sent == -1) {
        std::cerr << "Send error " << errno << ": " << strerror(errno) << std::endl;
        return 1;
    }
    return 0;
}

/**
 * Receives data over a socket and verifies its checksum.
 *
 * This function receives data from a socket described by `socket_descriptor` into
 * a buffer `buffer` of size `buffer_size`. It then verifies the checksum of the
 * received data.
 *
 * @param socket_descriptor The file descriptor representing the socket.
 * @param buffer_size The size of the buffer into which the data is received.
 * @param buffer The buffer into which the data is received.
 * @param max_attempt The maximum number of attempts to receive and verify data.
 * @return Returns `true` if data is received and its checksum is valid, otherwise `false`.
 *
 * @note The function assumes that the checksum field in the data is always at
 *       offset 11 and is 2 bytes long. It temporarily zeroes out these bytes
 *       for checksum calculation and then restores them.
 */
bool receive_and_verify_chksum(int socket_descriptor, size_t buffer_size, char* buffer, int max_attempt) {
    uint16_t received_chksum, expected_chksum;
    bool chksum_is_valid = false;
    int attempt = 0;
    do {
        std::memset(buffer, 0, buffer_size);
        int bytes_received = recv(socket_descriptor, buffer, buffer_size, 0);
        if (bytes_received == -1) {
            std::cerr << "Recv error " << errno << ": " << strerror(errno) << std::endl;
            break;
        }

        // Save the original checksum bytes
        char original_chksum_bytes[2];
        std::memcpy(original_chksum_bytes, buffer + 11, 2);

        // Assumption: CheckSum Field is always at offset 11 with 2 bytes
        received_chksum = *(uint16_t*)(buffer + 11);
        buffer[11] = 0;
        buffer[12] = 0;

        uint8_t* buffer_ptr = (uint8_t*)(buffer);
        expected_chksum = get_checksum16(buffer_ptr, buffer_size);
        std::cout << "received chksum: " << received_chksum << ", expected chksum: " << expected_chksum << std::endl;

        // Restore the original checksum bytes
        std::memcpy(buffer + 11, original_chksum_bytes, 2);

        if(received_chksum == expected_chksum) {
            std::cout << "Valid chksum received." << std::endl;
            chksum_is_valid = true;
            break;  // Valid checksum, break the loop
        }
    } while (attempt < max_attempt);

    return chksum_is_valid;
}

int determine_response_and_output_valuable_info(char buffer[]) {
    switch (buffer[0]) {
        case 'E': //LoginResponse
        {
            LoginResponse* res = new LoginResponse();
            std::memcpy(res, buffer, sizeof(LoginResponse));
            std::cout << "Receive valid LoginResponse at timestamp " << res->Timestamp << std::endl;
            std::cout << "Code = " << res->Code << ", Reason = " << res->Reason << std::endl;
            delete res;
            return 0;
        }
        case 'R': //SubmissionResponse
        {
            SubmissionResponse* res = new SubmissionResponse();
            std::memcpy(res, buffer, sizeof(SubmissionResponse));
            std::cout << "Receive valid SubmissionResponse at timestamp " << res->Timestamp << std::endl;
            std::cout << "Please use this token: " << res->Token << std::endl;
            delete res;
            return 0;
        }

        case 'G': //LogoutResponse
        {
            LogoutResponse* res = new LogoutResponse();
            std::memcpy(res, buffer, sizeof(LogoutResponse));
            std::cout << "Receive valid LogoutResponse at timestamp " << res->Timestamp << std::endl;
            std::cout << "Logout Reason: " << res->Reason << std::endl;
            delete res;
            return 0;
        }
        default:
            std::cout << "Unknown message type " << buffer[0] << std::endl;
            return 1;
    }
}