#pragma once
#include <TYPEDEFS.h>

static constexpr const int MAX_PARAM_SIZE = 4; // bytes

struct ParameterType
{
    ParameterId paramId;
    /**
     * for uint (float = 4 bytes, int -1 /2)
     * 1 -> 255
     * 2 -> 65 535
     * 3 -> 16 777 216
     * 4 -> 4 294 967 296
     */
    uint8_t nBytes;
};

// the typedefs for communication
class Parameters
{
    public:

    // this ones for invalid comm and not in mem
    static constexpr ParameterType InvalidParam = {
        0,
        0};

    // time between full pressure and beer in ms
    static constexpr ParameterType Co22BeerTime = {
        0x01,
        2};

    // counter value at which to stop the filling process / beer
    static constexpr ParameterType BeerStopValue = {
        0x02,
        4};

    // time after beer has stopped to push co2 in to the hose to rinse it
    static constexpr ParameterType PostBeerCo2Time = {
        0x03,
        2};

    // allow fast access if one is sure or dosn't care if violation occurs (segf)
    static constexpr const ParameterType ParamById[]{
        InvalidParam,
        Co22BeerTime,
        BeerStopValue,
        PostBeerCo2Time};

    static const ParameterType getParameterById(ParameterId pId) {
        if (pId > (sizeof(ParamById) / sizeof(ParameterType*))) {
            return InvalidParam;
        }
        return ParamById[pId];
    }

    

};

// the mem struct to store / retrieve the params to / from eeprom using put n get
struct MemParameters {
    uint16_t Co22BeerTime;
    uint32_t BeerStopValue;
    uint16_t PostBeerCo2Time;
};

static MemParameters memParamInst;

/*
template <typename T>
static T getMemParamByComId(ParameterId pId) {
    switch(pId) {
        case 0:
            return NULL;
        case 1:
            return memParamInst.Co22BeerTime;
        case 2:
            return memParamInst.BeerStopValue;
        case 3:
            return memParamInst.PostBeerCo2Time;
    }
    return NULL;
}*/

template <typename T, size_t sz>
static uint8_t uint8_tArr2uint8_t(T (&data)[sz]) {
    static_assert(sz >= 1);
    return data[0];
}
template <typename T, size_t sz>
static uint16_t uint8_tArr2uint16_t(T (&data)[sz]) {
    static_assert(sz >= 2);
    return (uint16_t) (data[1] << 8) + data[0];
}
template <typename T, size_t sz>
static uint32_t uint8_tArr2uint32_t(T (&data)[sz]) {
    static_assert(sz >= 4);
    return (uint32_t) (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + (data[0]);
}

template <typename T, size_t N>
static void setMemParamByComId(ParameterId pId, T (&data)[N]) {
    // memVar = static_cast<decltype(memVar)>(paramBuff);

    //auto memVar = getMemParamByComId(paramId);
    //    memVar = static_cast<decltype(memVar)>(paramBuff);
    switch(pId) {
        case 0:
            break;
        case 1:
            memParamInst.Co22BeerTime = uint8_tArr2uint16_t(data);
            break;
        case 2:
            memParamInst.BeerStopValue = uint8_tArr2uint32_t(data);
            break;
        case 3:
            memParamInst.PostBeerCo2Time = uint8_tArr2uint16_t(data);
            break;
    }
}

// update the struct in mem
static void updateMemParams() {
    ;
}