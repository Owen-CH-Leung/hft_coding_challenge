#include "message.h"
#include "utils.h"
#include <cstring>

LoginRequest::LoginRequest(char msg_type, uint16_t msg_len, const char *user, const char *password) {
    std::memset(this, 0, sizeof(LoginRequest));

    this->MsgType = msg_type;
    this->MsgLen = msg_len;
    strncpy(this->User, user, sizeof(this->User));
    strncpy(this->Password, password, sizeof(this->Password));
    this->Timestamp = get_current_timestamp();
    uint8_t* ptr = (uint8_t*)(this);
    uint32_t size = sizeof(LoginRequest);
    this->ChkSum = get_checksum16(ptr, size);
}

SubmissionRequest::SubmissionRequest(char msg_type, uint16_t msg_len, const char *name, const char *email, const char *repo) {
    std::memset(this, 0, sizeof(SubmissionRequest));

    this->MsgType = msg_type;
    this->MsgLen = msg_len;
    strncpy(this->Name, name, sizeof(this->Name));
    strncpy(this->Email, email, sizeof(this->Email));
    strncpy(this->Repo, repo, sizeof(this->Repo));
    this->Timestamp = get_current_timestamp();

    uint8_t* ptr = (uint8_t*)(this);
    uint32_t size = sizeof(SubmissionRequest);
    this->ChkSum = get_checksum16(ptr, size);
}

LogoutRequest::LogoutRequest(char msg_type, uint16_t msg_len) {
    std::memset(this, 0, sizeof(LogoutRequest));

    this->MsgType = msg_type;
    this->MsgLen = msg_len;
    this->Timestamp = get_current_timestamp();
    uint8_t* ptr = (uint8_t*)(this);
    uint32_t size = sizeof(LogoutRequest);
    this->ChkSum = get_checksum16(ptr, size);
}

LoginResponse::LoginResponse() {
    std::memset(this, 0, sizeof(LoginResponse));
}

SubmissionResponse::SubmissionResponse() {
    std::memset(this, 0, sizeof(SubmissionResponse));
}

LogoutResponse::LogoutResponse() {
    std::memset(this, 0, sizeof(LogoutResponse));
}