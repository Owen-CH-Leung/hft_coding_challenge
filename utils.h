#ifndef HFT_CODING_CHALLENGE_UTILS_H
#define HFT_CODING_CHALLENGE_UTILS_H

#include <stddef.h>
#include "message.h"

uint16_t get_checksum16(const uint8_t* buf, uint32_t len);
uint64_t get_current_timestamp();
int send_http_request(int socket_descriptor, void* msg, size_t size);
bool receive_and_verify_chksum(int socket_descriptor, size_t buffer_size, char* buffer, int max_attempt);
int determine_response_and_output_valuable_info(char buffer[]);
#endif