#ifndef HFT_CODING_CHALLENGE_MESSAGE_H
#define HFT_CODING_CHALLENGE_MESSAGE_H

#include <cstdint>

#pragma pack(push, 1)
class _MessageBase {
public:
    char MsgType;
    uint16_t MsgLen;
    uint64_t Timestamp;
    uint16_t ChkSum;

    ~_MessageBase() = default;
};
#pragma pack(pop)

#pragma pack(push, 1)
class LoginRequest: public _MessageBase {
public:
    char User[64];
    char Password[32];
    LoginRequest(char msg_type, uint16_t msg_len, const char* user, const char* password);

};
#pragma pack(pop)

#pragma pack(push, 1)
class LoginResponse: public _MessageBase {
public:
    char Code;
    char Reason[32];
    LoginResponse();
};
#pragma pack(pop)

#pragma pack(push, 1)
class SubmissionRequest: public _MessageBase {
public:
    char Name[64];
    char Email[64];
    char Repo[64];
    SubmissionRequest(char msg_type, uint16_t msg_len, const char* name, const char* email, const char* repo);
};
#pragma pack(pop)

#pragma pack(push, 1)
class SubmissionResponse: public _MessageBase {
public:
    char Token[32];
    SubmissionResponse();
};
#pragma pack(pop)

#pragma pack(push, 1)
class LogoutRequest: public _MessageBase {
public:
    LogoutRequest(char msg_type, uint16_t msg_len);
};
#pragma pack(pop)

#pragma pack(push, 1)
class LogoutResponse: public _MessageBase {
public:
    char Reason[32];
    LogoutResponse();
};
#pragma pack(pop)

#endif
