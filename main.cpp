#include "connect.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

void showHelp() {
    std::cout << "Usage: client --domain <domain_name> --port <port>\n";
}

int main(int argc, char* argv[]) {
    // Check for 5 arguments (including the program name)
    if (argc != 5) {
        std::cerr << "Error: Incorrect number of arguments." << std::endl;
        showHelp();
        return 1;
    }

    std::string domain, port;

    // Parse arguments
    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--domain") {
            domain = argv[i + 1];
        } else if (arg == "--port") {
            port = argv[i + 1];
        } else {
            std::cerr << "Unknown or repeated argument: " << arg << std::endl;
            showHelp();
            return 1;
        }
    }

    if (domain.empty() || port.empty()) {
        std::cerr << "Error: Both --domain and --port must be provided.\n";
        showHelp();
        return 1;
    }

    // At this point, domain and ip are populated with the provided arguments
    std::cout << "Attempt to connect to " << domain << "with port " << port << std::endl;

    std::string email = "owen.leung2@gmail.com";
    std::string pw = "pwd123";
    std::string name = "Owen-Leung";
    std::string repo = "myrepo";

    int max_attempt = 5;

    addrinfo* res = resolve_dns(domain.c_str(), port.c_str());
    if (res == nullptr) {
        std::cerr << "Unable to resolve " << domain << std::endl;
        return 1;
    }

    int sock = connect_and_get_socket_descriptor(res, port.c_str());
    if (sock == 1) {
        std::cerr << "Socket descriptor is not valid." << std::endl;
        return 1;
    }
    LoginRequest* login = new LoginRequest('L', sizeof(LoginRequest), email.c_str(), pw.c_str());
    int login_result = send_login_request(login, sock, max_attempt);
    if (login_result == 1) {
        std::cerr << "Error when sending login request." << std::endl;
        return 1;
    }

    SubmissionRequest* submission = new SubmissionRequest('S', sizeof(SubmissionRequest), name.c_str(), email.c_str(), repo.c_str());
    int submission_result = send_submission(submission, sock, max_attempt);
    if (submission_result == 1) {
        std::cerr << "Error when sending submission request." << std::endl;
        return 1;
    }

    LogoutRequest* logout = new LogoutRequest('O', sizeof(LogoutRequest));
    int logout_result = send_logout(logout, sock, max_attempt);
    if (logout_result == 1) {
        std::cerr << "Error when sending logout request." << std::endl;
        return 1;
    }

    freeaddrinfo(res);
    close(sock);
    delete login;
    delete submission;
    delete logout;

    return 0;
}
