#ifndef BUTIINPUT_RESOURCE_INTERFACE_H
#define BUTIINPUT_RESOURCE_INTERFACE_H

#ifdef BUTIINPUT_EXPORTS
#define BUTIINPUT_API __declspec(dllexport)
#else
#define BUTIINPUT_API __declspec(dllimport)
#endif
#pragma once
#include<cstdint>
#include"ButiMath/ButiMath.h"
#include"ButiMemorySystem/ButiMemorySystem/ButiPtr.h"
#include"ButiMemorySystem/ButiMemorySystem/ButiList.h"
namespace ButiEngine {
class IBinaryReader;
}
namespace ButiInput {

enum class PadButtons {
	XBOX_A = 12, XBOX_B = 13, XBOX_X = 14, XBOX_Y = 15,
	XBOX_UP = 0, XBOX_DOWN = 1, XBOX_LEFT = 2, XBOX_RIGHT = 3,
	XBOX_START = 4, XBOX_BACK = 5,
	XBOX_STICK_LEFT = 6, XBOX_STICK_RIGHT = 7,
	XBOX_BUTTON_LEFT = 8, XBOX_BUTTON_RIGHT = 9,
};

enum  class MouseButtons
{
	LeftClick, RightClick, WheelButton
};

enum class Keys
{
	None,//0
	Esc, One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Zero, Minus, Equarl, BackSpace,//1~14
	Tab, Q, W, E, R, T, Y, U, I, O, P, LeftBlackect, RightBlackect, Enter, LeftCtrl,//15~29
	A, S, D, F, G, H, J, K, L, Plus, Apostrophe, Grave, LeftShift, BackSlash,//30~43
	Z, X, C, V, B, N, M, LessThan, MoreThan, Slash, RightShift,//44~54
	Asterisk, Alt, Space, Capital, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NumLock, ScrollLock, //55~70
	Num7, Num8, Num9, NumMinus, Num4, Num5, Num6, NumPlus, Num1, Num2, Num3, Num0, NumDot,//71~83
	F11, F12, F13, F14, F15, KANA, Convert, NonConvert, Yen, NanEqual, CircumFlex, At, Colon, UnderLine, Kanji, Stop,//84~99
	Ax, Unlabebel, NumEnter, RightCtrl, NumComma, NumSlash, SysRq, RightAlt, Pause, Home, Up,//100~110
	PageUp, Left, Right, End, Down, PageDown, Insert, Delete, LWindow, RWindow, Menu, Power, NWindow//111~123
};
class IInputManager {
public:
	virtual void Release() = 0;
	virtual void Initialize(const void* arg_hwnd)= 0;
	virtual bool CheckKey(const std::uint32_t arg_index)const= 0;
	virtual bool TriggerKey(const std::uint32_t arg_index)const= 0;
	virtual bool ReleaseKey(const std::uint32_t arg_index)const= 0;
	virtual bool CheckKey(const Keys arg_index)const = 0;
	virtual bool TriggerKey(const Keys arg_index)const= 0;
	virtual bool ReleaseKey(const Keys arg_index)const= 0;
	virtual bool CheckAnyKey()const = 0;
	virtual bool TriggerAnyKey()const = 0;
	virtual bool ReleaseAnyKey()const = 0;
	virtual void Update()= 0;
	virtual bool GetAnyButton()const = 0;
	virtual bool GetAnyButtonTrigger()const = 0;
	virtual bool GetPadButton(const PadButtons arg_button)const =0;
	virtual bool GetPadButtonTrigger(const PadButtons arg_button)const= 0;
	virtual bool GetPadButtonRelease(const PadButtons arg_button)const= 0;
	virtual bool GetMouseButton(const MouseButtons arg_button)const = 0;
	virtual bool GetMouseTrigger(const MouseButtons arg_button)const = 0;
	virtual bool GetMouseRelease(const MouseButtons arg_button)const = 0;
	virtual bool GetMouseWheel()const = 0;
	virtual float GetMouseWheelMove()const = 0;
	virtual void SetCursorHide(const bool arg_isCursorHide)= 0;
	virtual void Vibration_L(const float arg_power)= 0;
	virtual void Vibration_R(const float arg_power)= 0;
	virtual ButiEngine::Vector2 GetLeftStick()const = 0;
	virtual ButiEngine::Vector2 GetRightStick()const = 0;
	virtual float GetLeftTrigger()const = 0;
	virtual float GetRightTrigger()const = 0;
	virtual ButiEngine::Vector2 GetMouseMove()const = 0;
	virtual ButiEngine::Vector2 GetMousePos()const = 0;
	virtual void SetIsWindowActive(const bool arg_windowActive) = 0;
	virtual void SetMouseCursor(const ButiEngine::Vector2& arg_position)= 0;
	virtual void StartRec() = 0;
	virtual void StopRec() = 0;
	virtual void Replay(const ButiEngine::List< const void* >&arg_list_data,const ButiEngine::List<std::int64_t>& arg_list_dataSize) = 0;
	virtual void Replay(const std::string& arg_logDir) = 0;
	virtual void SetLogFileDir(const std::string& arg_fileDir) = 0;
	virtual void ReplayStop() = 0;
	virtual bool IsReplay() = 0;
private:
};
BUTIINPUT_API ButiEngine::Value_ptr<IInputManager> CreateInputManager(const void* arg_hwnd);
}

#endif // !BUTIINPUT_RESOURCE_INTERFACE_H