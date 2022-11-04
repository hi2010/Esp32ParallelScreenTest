#pragma once

typedef uint8_t byte;

enum class COM_STATUS {
    OK = 0x00,
    ERR = 0xFF
};

enum class COM_CMD {
    SET_PARAM = 0x01,
    GET_PARAM = 0x11
};
