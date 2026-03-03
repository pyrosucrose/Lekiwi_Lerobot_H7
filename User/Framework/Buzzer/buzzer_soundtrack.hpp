//
// Created by Glucose_carbide on 25-8-8.
//

#ifndef BUZZER_SOUNDTRACK_HPP
#define BUZZER_SOUNDTRACK_HPP

// =============================== 调用库 ===============================
#include "buzzer_pitch.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================
class cSoundTrackUnit
{
public:
    /** 若period为0，则该单元用于配置bpm(用pitch参数)，亦即一分钟会有几个period */
    cSoundTrackUnit(const uint16_t pitch, const uint8_t period = 1, const uint8_t on_ratio = 15, const uint8_t duty = 8)
    {
        data = static_cast<uint32_t>(pitch)  << 16 |
               static_cast<uint32_t>(period) << 8  |
               (static_cast<uint32_t>(on_ratio) & 0x0F) << 4 |
               static_cast<uint32_t>(duty) & 0x0F;
    }
    [[nodiscard]] uint16_t GetPitch() const { return (data >> 16) & 0xFFFF; }   // 音高(Hz)
    [[nodiscard]] uint8_t GetPeriod() const { return (data >> 8) & 0xFF; }      // 持续时间(beat)
    [[nodiscard]] uint8_t GetOnRatio() const { return (data >> 4) & 0x0F; }     // 鸣起时间(0~15 -> 0~100)
    [[nodiscard]] uint8_t GetDuty() const { return (data >> 0) & 0x0F; }        // PWM波占空比(0~15 -> 0~100)
private:
    uint32_t data;
};

enum class eSoundtrack : uint16_t
{
    MUTE = 0,
    KEIL,
    PURE_FURIES,
    WATERY_GRAVES,
};

inline cSoundTrackUnit soundtrack_pure_fries[] = {
    {440,0,0},
    {la_4,1,13}, {la_4,1,13},
    {re_5,8}, {mi_5,8}, {fa_5,8}, {so_5,4},
    {la_5,2}, {do_6,2}, {re_6,4},
    {do_6,2}, {re_6,2}, {fa_6,2}, {mi_6,2},
    {so_5,2}, {do_6,2}, {la_5,12},

    {do_5,4},
    {re_5,8}, {mi_5,8}, {fa_5,8}, {so_5,4},
    {la_5,2}, {do_6,2}, {re_6,4},
    {do_6,2}, {re_6,2}, {fa_6,2}, {mi_6,2},
    {do_6,4}, {re_6,16}

    // {200,0,0},
    // {so_4,4}, {la_4,4}, {lap4,4}, {do_5,2},
    // {re_5,1}, {fa_5,1}, {so_5,2},
    // {fa_5,1}, {so_5,1}, {lap5,1}, {la_5,1},
    // {do_5,1}, {fa_5,1}, {re_5,6},
    //
    // {fa_4,2},
    //
    // {so_4,4}, {la_4,4}, {lap4,4}, {do_5,2},
    // {re_5,1}, {fa_5,1}, {so_5,2},
    // {fa_5,1}, {so_5,1}, {lap5,1}, {la_5,1},
    // {fa_5,2}, {so_5,8}
};

inline uint16_t soundtrack_keil1[] = { 180,
    do_3, do_3, do_3, do_3, do_3, do_3, do_3, do_3,
    fa_3, fa_3, fa_3, fa_3, fa_3, fa_3, fa_3, fa_3,
    rep3, rep3, rep3, rep3, rep3, rep3, rep3, lap2,
    do_3, do_3, do_3, do_3, do_3, do_3, do_3, do_3,
    dop3, dop3, dop3, dop3, dop3, dop3, dop3, do_3,
    lap2, lap2, lap2, lap2, lap2, lap2, lap2, fa_2,
    do_3, do_3, do_3, do_3, do_3, do_3, do_3, do_3,
    do_3, do_3, do_3, do_3, do_3, do_3, do_3, do_3,
};

inline uint16_t soundtrack_spring_shadow[] = {360,
    mi_4, mi_4, mi_4, mi_4, re_4, re_4, do_4, do_4,
    do_4, do_4, re_4, re_4, mi_4, mi_4, mi_4, fa_4,
    mi_4, mi_4, re_4, re_4, re_4, re_4, re_4, re_4
};

inline uint8_t DUTY = 2;
inline cSoundTrackUnit soundtrack_immortal_ash[] = {
    {483, 0, 0}, // {330,0,0}

    // {mi_4,4,15,DUTY},{fap4,1,15,DUTY},{sop4,1,15,DUTY},{si_4,2,15,DUTY},{rep4,5,15,DUTY},
    // {si_4,1,15,DUTY},{rep5,2,14,DUTY},{rep5,2,15,DUTY},{si_4,2,15,DUTY},{dop5,2,15,DUTY},{lap4,2,15,DUTY},{so_4,8,15,DUTY},
    // {mi_4,4,15,DUTY},{fap4,1,15,DUTY},{sop4,1,15,DUTY},{lap4,2,15,DUTY},{si_3,4,15,DUTY},{lap3,4,15,DUTY},
    // {si_3,1,15,DUTY},{dop4,1,15,DUTY},{rep4,1,15,DUTY},{sop4,1,15,DUTY},{rep4,1,15,DUTY},{dop4,1,15,DUTY},{si_3,1,15,DUTY},{dop4,1,15,DUTY},{rep4,1,15,DUTY},{sop4,1,15,DUTY},{lap4,1,15,DUTY},{dop5,1,15,DUTY},{rep5,4,15,DUTY},
    //
    // {sop4,4,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{rep5,2,15,DUTY},{sop4,5,15,DUTY},
    // {rep5,1,15,DUTY},{sop5,1,15,DUTY},{lap5,1,15,DUTY},{si_5,2,15,DUTY},{sop5,2,15,DUTY},{lap5,2,15,DUTY},{fap5,2,15,DUTY},{rep5,8,15,DUTY},
    // {sop4,4,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{rep5,2,15,DUTY},{sop4,4,15,DUTY},{so_4,4,15,DUTY},
    // {sop4,1,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{rep5,1,15,DUTY},{si_4,1,15,DUTY},{lap4,1,15,DUTY},{sop4,1,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{dop5,1,15,DUTY},{rep5,1,15,DUTY},{fap5,1,15,DUTY},{sop5,4,15,DUTY},


    {mi_4,4,15,DUTY},{fap4,1,15,DUTY},{sop4,1,15,DUTY},{si_4,2,15,DUTY},{rep4,5,15,DUTY},
    {si_4,1,15,DUTY},{rep5,2,14,DUTY},{rep5,2,15,DUTY},{si_4,2,15,DUTY},{dop5,2,15,DUTY},{lap4,2,15,DUTY},{so_4,8,15,DUTY},
    {mi_4,4,15,DUTY},{fap4,1,15,DUTY},{sop4,1,15,DUTY},{lap4,2,15,DUTY},{si_3,4,15,DUTY},{lap3,4,15,DUTY},
    {si_3,1,15,DUTY},{dop4,1,15,DUTY},{rep4,1,15,DUTY},{sop4,1,15,DUTY},{rep4,1,15,DUTY},{dop4,1,15,DUTY},{si_3,1,15,DUTY},{dop4,1,15,DUTY},{rep4,1,15,DUTY},{sop4,1,15,DUTY},{lap4,1,15,DUTY},{dop5,1,15,DUTY},{rep5,4,15,DUTY},

    {sop4,4,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{rep5,2,15,DUTY},{sop4,5,15,DUTY},
    {rep5,1,15,DUTY},{sop5,1,15,DUTY},{lap5,1,15,DUTY},{si_5,2,15,DUTY},{sop5,2,15,DUTY},{lap5,2,15,DUTY},{fap5,2,15,DUTY},{rep5,8,15,DUTY},
    {sop4,4,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{rep5,2,15,DUTY},{sop4,4,15,DUTY},{so_4,4,15,DUTY},
    {sop4,1,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{rep5,1,15,DUTY},{si_4,1,15,DUTY},{lap4,1,15,DUTY},{sop4,1,15,DUTY},{lap4,1,15,DUTY},{si_4,1,15,DUTY},{dop5,1,15,DUTY},{rep5,1,15,DUTY},{fap5,1,15,DUTY},{sop5,4,15,DUTY},

    {468,0},

    // {fap3,4},{sop3},{la_3},{dop4,2},{fap3,5},
    // {dop4},{fap4},{sop4},{la_4,2},{fap4,2},{sop4,2},{mi_4,2},{dop4,8},
    // {fap3,4},{sop3},{la_3},{dop4,2},{fap3,4},{fa_3,4},
    // {fap3},{sop3},{la_3},{dop4},{la_3},{sop3},{fap3},{sop3},{la_3},{si_3},{dop4},{mi_4},{fap4,4},

    {fap3},{la_2},{re_3},{fap3},{sop3},{la_3},{dop4},{sop3},{fap3},{dop3},{fap3},{dop4,2,14},
    {dop4},{fap4},{sop4},{la_4},{re_3},{fap4},{re_4},{sop4},{mi_3},{mi_4,2},{dop4},{sop2},{dop3},{sop3},{dop3},{sop2},{dop2},{sop2},
    {fap3},{fap2},{re_3},{fap3},{sop3},{la_3},{dop4},{sop3},{fap3},{la_2},{re_3},{la_3},{fa_3},{sop2},{dop3},{sop3},
    {fap3},{sop3},{la_3},{dop4},{la_3},{sop3},{fap3},{sop3},{la_3},{si_3},{dop4},{mi_4},{fap4,4},

    {mute, 8}
};

inline cSoundTrackUnit soundtrack_watery_graves[] = {
    {660,0,0},

    {so_3,2},{so_4,2},{fap4,2},{re_4,2},{fa_4,2},{mi_4,2},{mute,2},{mi_4,2},
    {so_3,1,13},{so_3,1,13},{re_4,1,13},{re_4,1,13},{so_4,1,13},{so_4,1,13},{re_4,1,13},{re_4,1,13},
    {so_3,1,13},{so_3,1,13},{re_4,1,13},{re_4,1,13},{so_4,1,13},{so_4,1,13},{re_4,1,13},{re_4,1,13},

    {so_3,2},{so_4,2},{fap4,2},{re_4,2},{fa_4,2},{mi_4,2},{mute,2},{mi_4,2},
    {so_3,1,13},{so_3,1,13},{re_4,1,13},{re_4,1,13},{so_4,1,13},{so_4,1,13},{re_4,1,13},{re_4,1,13},
    {so_3,1,13},{so_3,1,13},{re_4,1,13},{re_4,1,13},{so_4,1,13},{so_4,1,13},{re_4,1,13},{re_4,1,13},

    {so_3, 2}, {so_4, 2}, {fap4, 2}, {re_4, 2}, {fa_4, 2}, {mi_4, 2}, {mute, 2}, {mi_4, 2},
    {do_4, 2}, {rep4, 2}, {mute, 1}, {fa_4, 2}, {mute, 1}, {re_4, 2}, {mute, 1}, {lap3, 2}, {mute, 1}, {so_3, 1}, {mute, 1},
    {so_3, 2}, {so_4, 2}, {mute, 1}, {re_4, 2}, {mute, 1}, {fa_4, 2}, {mute, 1}, {rep4, 2}, {mute, 1}, {do_4, 1}, {mute, 1},
    {do_4, 2}, {rep4, 2}, {mute, 1}, {fa_4, 2}, {mute, 1}, {re_4, 1}, {mute, 1}, {re_4, 2}, {dop4, 2}, {fa_4, 2},

    {so_3, 6}, {la_3, 1}, {si_3, 1}, {do_4, 12},
    {re_4, 4},
    {lap3, 4},
    {so_3, 3}, {mute, 1},
    {so_3, 6}, {la_3, 1}, {si_3, 1},
    {do_4, 6},
    {rep4, 4},
    {fa_4, 4},
    {so_4, 4},
    {la_4, 4},

    {so_4, 8}, {mute, 2},
    {la_4, 2}, {so_4, 2}, {rep4, 2},
    {re_4, 8}, {mute, 2},
    {re_4, 2}, {dop4, 2}, {fa_4, 2}, {mute, 1},
    {re_4, 1}, {rep4, 1}, {fa_4, 1}, {so_4, 1}, {la_4, 1}, {lap4, 1}, {do_5, 1}, {dop5, 1}, {re_5, 1}, {lap4, 1}, {so_4, 1}, {re_4, 1}, {do_4, 1}, {lap3, 1}, {so_3, 1}, {re_4, 1}, {mute, 1},
    {re_4, 2}, {do_4, 2}, {re_4, 1}, {so_3, 2}, {re_4, 1}, {so_4, 1}, {mute, 1},
    {so_4, 1}, {mute, 1}, {so_4, 2}, {rep3, 1}, {mute, 1}, {rep3, 1}, {mute, 1},
    {fa_3, 1}, {so_3, 2}, {fap3, 2}, {mute, 1}, {fap3, 2}, {so_3, 2},
    {la_3, 2}, {lap3, 2}, {la_3, 2}, {lap3, 1},
    {re_4, 2}, {so_3, 2}, {so_3, 1}, {so_4, 1}, {mute, 1}, {so_4, 2}, {rep3, 1},
    {fa_3, 1}, {so_3, 1}, {mute, 1}, {so_3, 1}, {mute, 1},
    {so_3, 1}, {la_3, 2}, {lap3, 2}, {do_4, 2}, {re_4, 2}, {rep4, 1}, {do_4, 1}, {lap3, 1}, {la_3, 1},
    {dop3, 1}, {re_3, 1}, {mi_3, 1}, {fap3, 1}, {re_3, 1}, {fap3, 1}, {la_3, 1}, {re_4, 1}, {mute, 1},
    {re_4, 2}, {dop4, 2}, {fa_4, 2},

    {re_4, 8}, {mute, 2},
    {la_4, 2}, {so_4, 2}, {rep4, 2}, {re_4, 1}, {mute, 1}, {re_4, 1}, {mute, 1},
    {re_4, 1}, {rep4, 1}, {mute, 1}, {re_4, 2}, {mute, 1},
    {re_4, 2}, {dop4, 2}, {fa_4, 2}, {mute, 1},
    {re_4, 1}, {rep4, 1}, {fa_4, 1}, {so_4, 1}, {la_4, 1}, {lap4, 1}, {do_5, 1}, {dop5, 1}, {re_5, 1}, {lap4, 1}, {so_4, 1}, {re_4, 1}, {do_4, 1}, {lap3, 1}, {so_3, 1}, {re_4, 1}, {mute, 1},
    {re_4, 2}, {do_4, 2}, {re_4, 1}, {so_3, 2}, {re_4, 1}, {so_4, 1}, {mute, 1},
    {so_4, 1}, {mute, 1}, {so_4, 2}, {rep3, 1}, {mute, 1}, {rep3, 1}, {mute, 1},
    {fa_3, 1}, {so_3, 2}, {fap3, 2}, {mute, 1}, {fap3, 2}, {so_3, 2},
    {la_3, 2}, {lap3, 2}, {la_3, 2}, {lap3, 1},
    {re_4, 2}, {so_4, 2}, {mute, 1}, {so_4, 1}, {mute, 1},
    {so_4, 2}, {rep3, 1}, {fa_3, 1}, {so_3, 1}, {mute, 1}, {so_3, 1}, {mute, 1},
    {so_3, 1}, {la_3, 2}, {lap3, 2}, {do_4, 2}, {re_4, 2}, {rep4, 1}, {do_4, 1}, {lap3, 1}, {re_4, 2}, {la_3, 2}, {la_2, 2}, {la_3, 2}, {re_3, 2}, {la_3, 2}, {mute, 4},

    {so_5, 4, 15, 2}, {fa_5, 3, 15, 2}, {re_5, 3, 15, 2}, {re_4, 2}, {so_4, 2}, {re_4, 2},
    {rep4, 1}, {mute, 1}, {rep4, 1}, {do_4, 2}, {lap3, 1}, {re_4, 2}, {so_3, 2}, {re_4, 2}, {so_4, 2}, {re_4, 2},

    {so_5, 4, 15, 2}, {fa_5, 3, 15, 2}, {re_5, 3, 15, 2}, {re_4, 2}, {so_4, 2}, {re_4, 2},
    {re_4, 1}, {mute, 1}, {do_4, 1}, {lap3, 2}, {do_4, 1}, {re_4, 2}, {so_3, 2}, {re_4, 2}, {so_4, 2}, {re_4, 2},

    {so_5, 4, 15, 2}, {fa_5, 3, 15, 2}, {re_5, 3, 15, 2}, {re_4, 2}, {so_4, 2}, {re_4, 2},
    {rep4, 1}, {mute, 1}, {rep4, 1}, {do_4, 2}, {lap3, 1}, {re_4, 2}, {so_3, 1}, {re_4, 1}, {so_4, 1}, {mute, 1}, {so_4, 1}, {mute, 1}, {so_4, 2},

    {so_5, 4, 15, 2}, {fa_5, 3, 15, 2}, {re_5, 3, 15, 2}, {re_4, 2}, {so_4, 2}, {re_4, 2},
    {do_4, 1}, {rep4, 1}, {so_4, 1}, {lap4, 1}, {so_4, 1}, {mute, 1}, {do_4, 2}, {mute, 1}, {fa_3, 1}, {so_3, 1}, {la_3, 1}, {lap3, 1}, {do_4, 1}, {re_4, 1}, {rep4, 1},

    {do_4, 1}, {rep4, 1}, {so_4, 1}, {lap4, 1}, {so_4, 1}, {mute, 1}, {do_4, 2}, {mute, 1}, {fa_3, 1}, {so_3, 1}, {la_3, 1}, {lap3, 1}, {mute, 1}, {so_3, 1}, {mute, 1},

    {re_5, 2}, {lap4, 2}, {so_4, 3}, {do_5, 3}, {so_4, 2}, {do_5, 2}, {so_4, 1}, {mute, 1},
    {do_5, 1}, {mute, 1}, {do_5, 1}, {lap4, 1}, {do_5, 1}, {re_5, 2}, {lap4, 2}, {la_4, 2}, {so_4, 2}, {re_4, 1}, {la_4, 1}, {lap4, 3},

    {re_5, 2}, {fa_5, 1}, {re_5, 2}, {do_5, 3}, {fa_4, 2}, {fa_5, 2}, {do_5, 1}, {mute, 1},
    {do_5, 2}, {la_4, 2}, {fa_4, 2}, {lap4, 4}, {mute, 1}, {so_4, 1}, {lap4, 1}, {so_4, 1}, {lap4, 1}, {do_5, 1},

    {re_5, 2}, {lap4, 2}, {so_4, 3}, {do_5, 3}, {so_4, 2}, {do_5, 2}, {so_4, 1}, {mute, 1},
    {do_5, 1}, {mute, 1}, {do_5, 1}, {lap4, 1}, {do_5, 1}, {re_5, 2}, {lap4, 2}, {la_4, 2}, {so_4, 2}, {re_4, 1}, {la_4, 1}, {lap4, 3},

    {re_5, 2}, {fa_5, 1}, {re_5, 2}, {do_5, 3}, {fa_4, 2}, {fa_5, 2}, {do_5, 1}, {mute, 1},
    {do_5, 2}, {fa_4, 2}, {fa_5, 2}, {lap4, 6}, {mute, 4}

    // {so_3,2},{so_4,2},{fap4,2},{re_4,2},{fa_4,2},{mi_4,2},{mute,2},{mi_4,2},
    // {do_4,2},{rep4,3,12},{fa_4,3,12},{re_4,3,12},{lap3,3,12},{so_3,2,12},
};

inline cSoundTrackUnit oriental_dream_1[] =
{
    {200,0,0},
    // Arpeggio & Final Cadence
    {so_4, 2}, {fa_4, 1}, {do_5, 1},
    {so_4, 2}, {fa_4, 1}, {do_5, 1},
    {so_4, 2}, {fa_4, 1}, {do_5, 1},
    {so_4, 2}, {fa_4, 1}, {do_5, 1},
    {so_4, 2}, {fa_4, 1}, {do_5, 1},
    {so_4, 2}, {fa_4, 1}, {do_5, 1},
    {so_4, 2}, {fa_4, 1}, {do_5, 1},
    {so_4, 2}, {fa_4, 1}, {do_5, 1},

    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {dop4, 1}, {do_5, 1}, {fa_4, 1},
    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {dop4, 1}, {do_5, 1}, {fa_4, 1},
    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {dop4, 1}, {do_5, 1}, {fa_4, 1},
    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {dop4, 1}, {do_5, 1}, {fa_4, 1},
    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {dop4, 1}, {do_5, 1}, {fa_4, 1},
    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {dop4, 1}, {do_5, 1}, {fa_4, 1},
    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {dop4, 1}, {do_5, 1}, {fa_4, 1},
    {so_4, 1}, {dop4, 1}, {fa_4, 1}, {do_5, 1},
    {so_4, 1}, {re_5, 1}, {rep5, 1}, {lap4, 1},

    {do_5, 1}, {so_4, 1}, {lap3, 1}, {do_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_4, 1}, {fa_4, 1}, {do_4, 1}, {fa_4, 1},
    {so_3, 1}, {fa_3, 1}, {so_3, 1}, {la_3, 1},
    {lap3, 1}, {la_3, 2}, {lap3, 1}, {do_4, 1},
    {mute, 5},
    {rep5, 1}, {fa_5, 1}, {re_5, 1}, {rep5, 1}, {re_5, 1}, {lap4, 1},

    {do_5, 10}, {fa_5, 1}, {rep5, 1}, {fa_5, 1}, {lap5, 1}, {so_5, 2},
    {so_5, 10}, {rep5, 1}, {do_5, 1}, {re_5, 1}, {rep5, 1}, {re_5, 1}, {do_5, 1},

    {so_4, 2}, {do_5, 1}, {lap4, 1}, {do_5, 3}, {rep5, 1}, {fa_5, 1}, {rep5, 1}, {fa_5, 1}, {so_5, 1}, {do_5, 4},

    {do_5, 10}, {rep5, 1}, {fa_5, 1}, {re_5, 1}, {rep5, 1}, {re_5, 1}, {lap4, 1},
    {do_5, 10}, {fa_5, 1}, {rep5, 1}, {fa_5, 1}, {lap5, 1}, {so_5, 2},
    {so_5, 10}, {rep5, 1}, {do_5, 1}, {re_5, 1}, {rep5, 1}, {re_5, 1}, {do_5, 1},

    {so_4, 2}, {do_5, 1}, {lap4, 1}, {do_5, 2}, {so_4, 1}, {lap4, 1}, {do_5, 2}, {fa_5, 1}, {so_5, 1}, {do_5, 4},

    {do_4, 1}, {lap3, 1}, {so_3, 1}, {rep3, 1}, {mute, 1}, {do_3, 1}, {rep3, 1}, {sop3, 1}, {mute, 1},
    {rep3, 1}, {so_3, 1}, {lap3, 1}, {mute, 1}, {rep3, 1}, {so_3, 1}, {lap3, 1},
    {do_4, 1}, {lap3, 1}, {so_3, 1}, {rep3, 1}, {mute, 1}, {do_3, 1}, {rep3, 1}, {sop3, 1}, {mute, 1},
    {rep3, 1}, {so_3, 1}, {lap3, 1}, {mute, 1}, {rep3, 1}, {so_3, 1}, {lap3, 1},

    {do_4, 4}};

// inline uint16_t csoundtrack_watery_graves[] = {426,
//     so_3, so_3, so_4, so_4, fap4, fap4, re_4, re_4, fa_4, fa_4, mi_4, mi_4, mute, mute, mi_4, mi_4,
//     so_3, so_3, re_4, re_4, so_4, so_4, re_4, re_4, so_3, so_3, re_4, re_4, so_4, so_4, re_4, re_4,
//     so_3, so_3, so_4, so_4, fap4, fap4, re_4, re_4, fa_4, fa_4, mi_4, mi_4, mute, mute, mi_4, mi_4,
//     so_3, so_3, re_4, re_4, so_4, so_4, re_4, re_4, so_3, so_3, re_4, re_4, so_4, so_4, re_4, re_4, /* NoHorn */
//
//     so_3, so_3, so_4, so_4, fap4, fap4, re_4, re_4, fa_4, fa_4, mi_4, mi_4, mute, mute, mi_4, mi_4,
//     do_4, do_4, rep4, rep4, mute, fa_4, fa_4, mute, re_4, re_4, mute, lap3, lap3, mute, so_3, mute,
//     so_3, so_3, so_4, so_4, mute, re_4, re_4, mute, fa_4, fa_4 ,mute, rep4, rep4, mute, do_4, mute,
//     do_4, do_4, rep4, rep4, mute, fa_4, fa_4, mute, re_4, mute, re_4, re_4, dop4, dop4, fa_4, fa_4, /* WithHorn */
//
//     so_3, so_3, so_3, so_3, so_3, so_3, la_3, si_3, do_4, do_4, do_4, do_4, do_4, do_4, do_4, do_4,
//     do_4, do_4, do_4, do_4, re_4, re_4, re_4, re_4, lap3, lap3, lap3, lap3, so_3, so_3, so_3, mute,
//     so_3, so_3, so_3, so_3, so_3, so_3, la_3, si_3, do_4, do_4, do_4, do_4, do_4, do_4, do_4, do_4,
//     rep4, rep4, rep4, rep4, fa_4, fa_4, fa_4, fa_4, so_4, so_4, so_4, so_4, la_4, la_4, la_4, la_4, /* Violin */
//
//     so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, mute, mute, la_4, la_4, so_4, so_4, rep4, rep4,
//     re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, mute, mute, re_4, re_4, dop4, dop4, fa_4, fa_4,
//     mute, re_4, rep4, fa_4, so_4, la_4, lap4, do_5, dop5, re_5, lap4, so_4, re_4, do_4, lap3, so_3,
//     re_4, mute, re_4, re_4, do_4, do_4, re_4, so_3, so_3, re_4, so_4, mute, so_4, mute, so_4, so_4,
//
//     rep3, mute, rep3, mute, fa_3, so_3, so_3, fap3, fap3, mute, fap3, fap3, so_3, so_3, la_3, la_3,
//     lap3, lap3, la_3, la_3, lap3, re_4, re_4, so_3, so_3, so_3, so_4, mute, so_4, so_4, rep3, fa_3,
//     so_3, mute, so_3, mute, so_3, la_3, la_3, lap3, lap3, do_4, do_4, re_4, re_4, rep4, do_4, lap3,
//     la_3, dop3, re_3, mi_3, fap3, re_3, fap3, la_3, re_4, mute, re_4, re_4, dop4, dop4, fa_4, fa_4, /* Mountain */
//
//     re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, mute, mute, la_4, la_4, so_4, so_4, rep4, rep4,
//     re_4, mute, re_4, mute, re_4, rep4, mute, re_4, re_4, mute, re_4, re_4, dop4, dop4, fa_4, fa_4,
//     mute, re_4, rep4, fa_4, so_4, la_4, lap4, do_5, dop5, re_5, lap4, so_4, re_4, do_4, lap3, so_3,
//     re_4, mute, re_4, re_4, do_4, do_4, re_4, so_3, so_3, re_4, so_4, mute, so_4, mute, so_4, so_4,
//
//     rep3, mute, rep3, mute, fa_3, so_3, so_3, fap3, fap3, mute, fap3, fap3, so_3, so_3, la_3, la_3,
//     lap3, lap3, la_3, la_3, lap3, re_4, re_4, so_4, so_4, mute, so_4, mute, so_4, so_4, rep3, fa_3,
//     so_3, mute, so_3, mute, so_3, la_3, la_3, lap3, lap3, do_4, do_4, re_4, re_4, rep4, do_4, lap3,
//     re_4, re_4, la_3, la_3, la_2, la_2, la_3, la_3, re_3, re_3, la_3, la_3, mute, mute, mute, mute, /* Mountain */
//
//     so_5, so_5, so_5, so_5, fa_5, fa_5, fa_5, re_5, re_5, re_5, re_4, re_4, so_4, so_4, re_4, re_4,
//     rep4, mute, rep4, do_4, do_4, lap3, re_4, re_4, so_3, so_3, re_4, re_4, so_4, so_4, re_4, re_4, /* Ring! */
//     so_5, so_5, so_5, so_5, fa_5, fa_5, fa_5, re_5, re_5, re_5, re_4, re_4, so_4, so_4, re_4, re_4,
//     re_4, mute, do_4, lap3, lap3, do_4, re_4, re_4, so_3, so_3, re_4, re_4, so_4, so_4, re_4, re_4, /* Ring! */
//     so_5, so_5, so_5, so_5, fa_5, fa_5, fa_5, re_5, re_5, re_5, re_4, re_4, so_4, so_4, re_4, re_4,
//     rep4, mute, rep4, do_4, do_4, lap3, re_4, re_4, so_3, re_4, so_4, mute, so_4, mute, so_4, so_4, /* Ring! */
//     so_5, so_5, so_5, so_5, fa_5, fa_5, fa_5, re_5, re_5, re_5, re_4, re_4, so_4, so_4, re_4, re_4,
//     do_4, rep4, so_4, lap4, so_4, mute, do_4, do_4, mute, fa_3, so_3, la_3, lap3, do_4, re_4, rep4, /* Ring! */
//
//     do_4, rep4, so_4, lap4, so_4, mute, do_4, do_4, mute, fa_3, so_3, la_3, lap3, mute, so_3, mute,
//
//     re_5, re_5, lap4, lap4, so_4, so_4, so_4, do_5, do_5, do_5, so_4, so_4, do_5, do_5, so_4, mute,
//     do_5, mute, do_5, lap4, do_5, re_5, re_5, lap4, lap4, la_4, la_4, so_4, so_4, re_4, la_4, lap4,
//     lap4, lap4, re_5, re_5, fa_5, re_5, re_5, do_5, do_5, do_5, fa_4, fa_4, fa_5, fa_5, do_5, mute,
//     do_5, do_5, la_4, la_4, fa_4, fa_4, lap4, lap4, lap4, lap4, mute, so_4, lap4, so_4, lap4, do_5,
//
//     re_5, re_5, lap4, lap4, so_4, so_4, so_4, do_5, do_5, do_5, so_4, so_4, do_5, do_5, so_4, mute,
//     do_5, mute, do_5, lap4, do_5, re_5, re_5, lap4, lap4, la_4, la_4, so_4, so_4, re_4, la_4, lap4,
//     lap4, lap4, re_5, re_5, fa_5, re_5, re_5, do_5, do_5, do_5, fa_4, fa_4, fa_5, fa_5, do_5, mute,
//     do_5, do_5, fa_4, fa_4, fa_5, fa_5, lap4, lap4, lap4, lap4, lap4, lap4, mute, mute, mute, mute
// };

inline uint16_t soundtrack_oriental_dream_1[] = {
    so_4, so_4, fa_4, do_5, so_4, so_4, fa_4, do_5, so_4, so_4, fa_4, do_5, so_4, so_4, fa_4, do_5,
    so_4, so_4, fa_4, do_5, so_4, so_4, fa_4, do_5, so_4, so_4, fa_4, do_5, so_4, so_4, fa_4, do_5,
    so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4, so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4,
    so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4, so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4, // 16
    so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4, so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4,
    so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4, so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4,
    so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4, so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4,
    so_4, dop4, fa_4, do_5, so_4, dop4, do_5, fa_4, so_4, dop4, fa_4, do_5, so_4, re_5, rep5, lap4, // 32
    do_5, so_4, lap3, do_4, so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4,
    so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4,
    so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4,
    so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4, so_4, fa_4, do_4, fa_4, so_3, fa_3, so_3, la_3, // 48
    lap3, la_3, la_3, lap3, do_4, mute, mute, mute, mute, mute, rep5, fa_5, re_5, rep5, re_5, lap4,
    do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, fa_5, rep5, fa_5, lap5, so_5, so_5,
    so_5, so_5, so_5, so_5, so_5, so_5, so_5, so_5, so_5, so_5, rep5, do_5, re_5, rep5, re_5, do_5,
    so_4, so_4, do_5, lap4, do_5, do_5, do_5, rep5, fa_5, rep5, fa_5, so_5, do_5, do_5, do_5, do_5, // 64
    do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, rep5, fa_5, re_5, rep5, re_5, lap4,
    do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, do_5, fa_5, rep5, fa_5, lap5, so_5, so_5,
    so_5, so_5, so_5, so_5, so_5, so_5, so_5, so_5, so_5, so_5, rep5, do_5, re_5, rep5, re_5, do_5,
    so_4, so_4, do_5, lap4, do_5, do_5, so_4, lap4, do_5, do_5, fa_5, so_5, do_5, do_5, do_5, do_5, // 80
    do_4, lap3, so_3, rep3, mute, do_3, rep3, sop3, mute, rep3, so_3, lap3, mute, rep3, so_3, lap3,
    do_4, lap3, so_3, rep3, mute, do_3, rep3, sop3, mute, rep3, so_3, lap3, mute, rep3, so_3, lap3,
    do_4, do_4, do_4, do_4
};

inline uint16_t soundtrack_oriental_dream_2[] = {
    mute, mute, mute, mute, mute, mute, mute, mute, mute, mute, mute, mute, mute, mute, mute, mute,
    rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5,
    re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, re_5, do_5,
    rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, rep5, lap4, fa_4, // 16
    so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4,
    so_4, so_4, so_4, so_4, so_4, so_4, lap4, fa_4, rep4, rep4, rep4, rep4, fa_4, fa_4, fa_4, fa_4,
    re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, re_4, so_3, so_3,
    fa_3, fa_3, so_3, so_3, lap3, lap3, do_4, do_4, fa_4, fa_4, fa_4, fa_4, fa_4, fa_4, fa_4, fa_4, // 32
    so_4, so_4, so_4, so_4, rep4, rep4, mute, do_4, so_3, so_3, mute, do_4, re_4, do_4, fa_4, mi_4,
    re_4, mute, do_4, re_4, rep4, mute, do_4, rep4, sop4, sop4, mute, so_4, fa_4, fa_4, fa_4, fa_4,
    fa_4, mute, rep4, do_4, re_4, re_4, mute, do_4, so_3, so_3, mute, do_4, re_4, do_4, fa_4, so_4,
    re_4, mute, rep4, fa_4, so_4, fa_4, so_4, la_4, lap4, la_4, so_4, fa_4, so_4, so_4, so_4, so_4, // 48
    so_4, so_4, fa_4, so_4, lap4, do_5, so_4, fa_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4,
    so_4, mute, so_4, lap4, do_5, rep5, fa_5, rep5, fa_5, fa_5, fa_5, fa_5, fa_5, fa_5, fa_5, fa_5,
    fa_5, fa_5, so_4, lap4, do_5, do_5, lap4, do_5, so_4, mute, so_4, rep4, fa_4, so_4, fa_4, rep4,
    do_4, mute, do_4, lap3, do_4, mute, do_4, rep4, fa_4, rep4, fa_4, so_4, do_4, do_4, do_4, do_4, // 64
    do_4, do_4, fa_4, so_4, lap4, do_5, so_4, fa_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4, so_4,
    so_4, mute, so_4, lap4, do_5, rep5, fa_5, rep5, fa_5, fa_5, fa_5, fa_5, fa_5, fa_5, fa_5, fa_5,
    fa_5, fa_5, so_4, lap4, do_5, do_5, lap4, do_5, so_4, mute, so_4, rep4, fa_4, so_4, fa_4, rep4,
    do_4, mute, do_4, lap3, do_4, mute, so_3, lap3, do_4, do_4, so_3, lap3, do_4, do_4, do_4, do_4, // 80
    do_4, do_4, so_4, lap4, do_5, mute, so_4, lap4, do_5, mute, so_4, lap4, do_5, mute, mute, mute,
    mute, mute, so_4, lap4, do_5, mute, so_4, lap4, do_5, mute, fa_5, so_5, do_5, mute, mute, mute,
    do_5, do_5, do_5, do_5
};


// =============================== 函数声明 ===============================

// =============================== 类声明 ===============================

#endif // BUZZER_SOUNDTRACK_HPP
