//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "usartio.hpp"
#include "typedef.hpp"
#include "config.hpp"

class cRemoteData
{
public:
	void RcUpdateValue(const uint8_t *data);

	[[nodiscard]] int16_t GetRcLeftHorizontalRaw() const { return rc_left_horizontal; }
	[[nodiscard]] int16_t GetRcRightHorizontalRaw() const { return rc_right_horizontal; }
	[[nodiscard]] int16_t GetRcLeftVerticalRaw() const { return rc_left_vertical; }
	[[nodiscard]] int16_t GetRcRightVerticalRaw() const { return rc_right_vertical; }
	[[nodiscard]] float   GetRcLeftHorizontal() const { return rc_left_horizontal_float; }
	[[nodiscard]] float   GetRcRightHorizontal() const { return rc_right_horizontal_float; }
	[[nodiscard]] float   GetRcLeftVertical() const { return rc_left_vertical_float; }
	[[nodiscard]] float   GetRcRightVertical() const { return rc_right_vertical_float; }
	[[nodiscard]] int16_t GetRcSwitchA() const { return rc_switch_A; }
	[[nodiscard]] int16_t GetRcSwitchB() const { return rc_switch_B; }
	[[nodiscard]] int16_t GetRcSwitchC() const { return rc_switch_C; }
	[[nodiscard]] int16_t GetRcSwitchD() const { return rc_switch_D; }
	[[nodiscard]] int16_t GetRcKnobLeft() const { return rc_knob_left; }
	[[nodiscard]] int16_t GetRcKnobRight() const { return rc_knob_right; }
	[[nodiscard]] int16_t GetMouseX() const { return mouse_x; }
	[[nodiscard]] int16_t GetMouseY() const { return mouse_y; }
	[[nodiscard]] int16_t GetMouseZ() const { return mouse_z; }
	[[nodiscard]] uint8_t IsMousePressed_Left() const { return mouse_pressed_left; }
	[[nodiscard]] uint8_t IsMousePressed_Right() const { return mouse_pressed_right; }
	[[nodiscard]] uint8_t IsKeyPressed_W() const { return pressed_W; }
	[[nodiscard]] uint8_t IsKeyPressed_S() const { return pressed_S; }
	[[nodiscard]] uint8_t IsKeyPressed_A() const { return pressed_A; }
	[[nodiscard]] uint8_t IsKeyPressed_D() const { return pressed_D; }
	[[nodiscard]] uint8_t IsKeyPressed_Q() const { return pressed_Q; }
	[[nodiscard]] uint8_t IsKeyPressed_E() const { return pressed_E; }
	[[nodiscard]] uint8_t IsKeyPressed_Shift() const { return pressed_Shift; }
	[[nodiscard]] uint8_t IsKeyPressed_Ctrl() const { return pressed_Ctrl; }

	[[nodiscard]] bool IsRcOnline() const { return rc_online; }

private:
	int16_t rc_left_horizontal = 0;
	int16_t rc_left_vertical = 0;
	int16_t rc_right_horizontal = 0;
	int16_t rc_right_vertical = 0;
	float rc_left_horizontal_float = 0.0f;
	float rc_left_vertical_float = 0.0f;
	float rc_right_horizontal_float = 0.0f;
	float rc_right_vertical_float = 0.0f;
	int16_t rc_switch_A = eRemoteSwitchValue::HIGH;
	int16_t rc_switch_B = eRemoteSwitchValue::HIGH;
	int16_t rc_switch_C = eRemoteSwitchValue::HIGH;
	int16_t rc_switch_D = eRemoteSwitchValue::HIGH;
	int16_t rc_knob_left = 0;
	int16_t rc_knob_right = 0;

	int16_t mouse_x = 0;
	int16_t mouse_y = 0;
	int16_t mouse_z = 0;
	uint8_t mouse_pressed_left = 0;
	uint8_t mouse_pressed_right = 0;

	uint16_t keyboard_values = 0;
	uint8_t pressed_W = 0;
	uint8_t pressed_S = 0;
	uint8_t pressed_A = 0;
	uint8_t pressed_D = 0;
	uint8_t pressed_Q = 0;
	uint8_t pressed_E = 0;
	uint8_t pressed_Shift = 0;
	uint8_t pressed_Ctrl = 0;

	int16_t counter = 1145;
	int16_t rc_online = false;
};

inline cRemoteData rc_data;
