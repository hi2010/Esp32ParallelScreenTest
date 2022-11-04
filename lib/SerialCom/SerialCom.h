#pragma once
#include <type_traits>
#include <PARAMETERS.h>
#include <Arduino.h>

struct Param {
    uint8_t paramId;
    uint8_t nBytes;
    uint8_t *data;

    // ! only use full data types like float, uint16_t, ... not arrays or pointers
    template <typename T>
    Param data2param(T dataVal, const ParameterType pt) {
        // will likely not work

        uint8_t dataBuff[pt.nBytes];
        for (auto i = pt.nBytes; i > 0; i--) {
            dataBuff[i] = 0xff && dataVal;
            dataVal >>= 8;
        }

        Param p;
        p.nBytes = pt.nBytes;
        p.data = dataBuff;
        p.paramId = pt.paramId;
        return p;
    };

    Param data2param(float dataVal, const ParameterType pt) {
        // max 4 bytes data
        // convert float to binary 4 bytes (remove type info)
        uint32_t rawData = (uint32_t) dataVal;

        uint8_t dataBuff[pt.nBytes];
        for (auto i = pt.nBytes; i > 0; i--) {
            dataBuff[i] = 0xff && rawData;
            rawData >>= 8;
        }

        Param p;
        p.nBytes = pt.nBytes;
        p.data = dataBuff;
        p.paramId = pt.paramId;
        return p;
    }

};



class SerialCom {
    // bytes
    static const uint8_t arduinoTxBufferSize = 64;

    // from the view of the communicator / pc

    // the idea is only write to the serial buffer if enough space is free otherweise it is blocking
    // if the msg is important put it in some buffer otherwhise think about ignoring it
    bool fitsInWriteBuffer(uint16_t nBytes) {
        return (Serial.availableForWrite() + nBytes) < 64;
    }

    // this part is using persistent variables

    // get a parameter value and report it back using serial
    Param getParam(uint8_t paramId);

    // set a parameter from the values gotten via serial
    void setParam(const Param &param) {
        // assumption: the command got in and got reckoned as set param cmd
        // this means the following bytes are the param id and the param data
        // total msg: 
        // CMD | TOPIC/ID | CONTENT ...
        // SET_PARAM | PARAM_ID | N_BYTES ...
        ParameterId paramId = Serial.read();
        // check if valid
        auto paramT = Parameters::getParameterById(paramId);
        if (paramT.paramId == Parameters::InvalidParam.paramId) {
            return;
        }

        uint8_t paramBuff[MAX_PARAM_SIZE];
        Serial.readBytes(paramBuff, paramT.nBytes);
        
        setMemParamByComId(paramT.paramId, paramBuff);

        // command +1, id +1 -> +2
        /*
        if (fitsInWriteBuffer(paramT.nBytes + 2)) {
            sendParamResponse(paramId);
        } else {
            sendNexTime;
        }
        */
    }
    
};