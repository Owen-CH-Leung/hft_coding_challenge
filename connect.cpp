#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include "utils.h"
#include "message.h"
#include <cstring>

/**
 * Resolves DNS for the given domain and port and returns address information.
 *
 * @param domain The domain name to resolve.
 * @param port The port number as a string.
 *
 * @return Pointer to addrinfo structure containing resolved DNS information.
 *         Returns nullptr if there is an error in resolving the DNS.
 *
 */
addrinfo* resolve_dns(const char* domain, const char* port) {
    struct addrinfo hints, *res;
    int status;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(domain, port, &hints, &res)) != 0) {
        std::cerr << "Getaddrinfo error: " << gai_strerror(status) << std::endl;
        return nullptr;
    }

    return res;
}

/**
 * Establishes a connection to a server specified in the addrinfo structure.
 *
 * @param res Pointer to the addrinfo structure containing the server addresses.
 * @param port The port number to connect to as a string.
 *
 * @return The socket file descriptor on successful connection, or 1 on failure.
 *
 * @note The function tries to connect using each address in the addrinfo until
 *       a successful connection is made or all addresses are tried.
 */
int connect_and_get_socket_descriptor(addrinfo* res, const char* port) {
    int sock;
    char ipstr[INET_ADDRSTRLEN];

    for (addrinfo* p = res; p != nullptr; p = p->ai_next) {
        void *addr;
        std::string ipver;

        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &ipv4->sin_addr;
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &ipv6->sin6_addr;
            ipver = "IPv6";
        }

        // Convert IP to a string and print it out
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << "Attempting to connect " << ipver << " " << ipstr << " on port " << port << std::endl;

        // Create a socket using the found addrinfo
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1) {
            std::cerr << "Socket error: " << errno << " " << strerror(errno) << std::endl;
            continue;
        }

        // Set the port for the sockaddr structure
        if (p->ai_family == AF_INET) { // For IPv4
            ((struct sockaddr_in *)p->ai_addr)->sin_port = htons(atoi(port));
        } else { // For IPv6
            ((struct sockaddr_in6 *)p->ai_addr)->sin6_port = htons(atoi(port));
        }

        // Attempt to connect
        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            std::cerr << "Connect error: " << errno << " " << strerror(errno) << std::endl;
            continue; // If the connection failed, try with the next address
        }

        std::cout << "Connection established" << std::endl;
        return sock;
    }
    return 1;
}

/**
 * Sends a login request over the given socket and processes the response.
 *
 * This function sends a login request message over a socket and then waits
 * to receive a response. The response is verified for a valid checksum
 *
 * @param msg Pointer to the login request message.
 * @param sock The socket file descriptor over which to send the request.
 * @param max_attempt The maximum number of attempts for receiving a response.
 *
 * @return 0 on successful login, 1 on failure or error.
 */
int send_login_request(void* msg, int sock, int max_attempt) {
    std::cout << "Sending Login request......" << std::endl;

    int status = send_http_request(sock, msg, sizeof(LoginRequest));
    if (status == 1) {
        std::cerr << "Unable to send http request. " << std::endl;
        return 1;
    }

    char buffer[sizeof(LoginResponse)];
    if (receive_and_verify_chksum(sock, sizeof(LoginResponse), buffer, max_attempt)) {
        int status = determine_response_and_output_valuable_info(buffer);
        if (status == 1) {
            std::cout << "Receive unknown response" << std::endl;
            return 1;
        }
        return 0;
    }

    return 1;
}

/**
 * Sends a submission request over the given socket and processes the response.
 *
 * This function sends a submission request message (submission) over a socket
 * and then waits to receive a response. The response is verified for a valid
 * checksum.
 *
 * @param submission Pointer to the submission request message.
 * @param sock The socket file descriptor over which to send the request.
 * @param max_attempt The maximum number of attempts for receiving a response.
 *
 * @return 0 on successful submission, 1 on failure or error.
 */
int send_submission(SubmissionRequest* submission, int sock, int max_attempt) {
    std::cout << "Sending Submission request......" << std::endl;

    int status = send_http_request(sock, submission, sizeof(SubmissionRequest));
    if (status == 1) {
        std::cerr << "Unable to send request. Quitting the program. " << std::endl;
        return 1;
    }
    std::cout << "SubmissionRequest Status: " << status << std::endl;
    char buffer[sizeof(SubmissionResponse)];
    if (receive_and_verify_chksum(sock, sizeof(SubmissionResponse), buffer, max_attempt)) {
        int status = determine_response_and_output_valuable_info(buffer);
        if (status == 1) {
            std::cout << "Receive unknown response" << std::endl;
            return 1;
        }
        return 0;
    }
    return 1;
}

/**
 * Sends a logout request over the given socket and processes the response.
 *
 * This function sends a logout request message (request) over a socket
 * and then waits to receive a response. The response is verified for a valid
 * checksum.
 *
 * @param request Pointer to the logout request message.
 * @param sock The socket file descriptor over which to send the request.
 * @param max_attempt The maximum number of attempts for receiving a response.
 *
 * @return 0 on successful logout, 1 on failure or error.
 */
int send_logout(LogoutRequest* request, int sock, int max_attempt) {
    std::cout << "Sending Logout request......" << std::endl;

    int status = send_http_request(sock, request, sizeof(LogoutRequest));
    if (status == 1) {
        std::cerr << "Unable to send request. Quitting the program. " << std::endl;
        return 1;
    }

    char buffer[sizeof(LogoutResponse)];
    if (receive_and_verify_chksum(sock, sizeof(LogoutResponse), buffer, max_attempt)) {
        int status = determine_response_and_output_valuable_info(buffer);
        if (status == 1) {
            std::cout << "Receive unknown response" << std::endl;
            return 1;
        }
        return 0;
    }
    return 0;
}
