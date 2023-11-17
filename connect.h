#ifndef HFT_CODING_CHALLENGE_CONNECT_H
#define HFT_CODING_CHALLENGE_CONNECT_H

#include <netdb.h>
#include "message.h"

addrinfo* resolve_dns(const char* domain, const char* port);
int connect_and_get_socket_descriptor(addrinfo* res, const char* port);
int send_login_request(void* msg, int sock, int max_attempt);
int send_submission(SubmissionRequest* submission, int sock, int max_attempt);
int send_logout(LogoutRequest* request, int sock, int max_attempt);

#endif //HFT_CODING_CHALLENGE_CONNECT_H
