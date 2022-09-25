#include"stdafx.h"

#include<dinput.h>
#include <xinput.h>
#include"ButiUtil/ButiUtil/Util.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

constexpr std::int32_t KEY_MAX = 256;
constexpr std::int32_t CONTROLLERS_MAX = 1;
constexpr std::uint16_t ushort_max = 0xffff;
constexpr float shortDiv = 1.0f / 32767;
const unsigned char char_max = 128;
constexpr float ucharDiv = 1.0f / 255;
BYTE emptyKeyBoard[256];
namespace ButiInput {
class InputManager:public IInputManager {
public:
	void Release()override;
	void Initialize(const void* arg_hwnd) override;
	bool CheckKey(const std::uint32_t arg_index)const override;
	bool TriggerKey(const std::uint32_t arg_index)const override;
	bool ReleaseKey(const std::uint32_t arg_index)const override;
	bool CheckKey(const Keys arg_index)const override;
	bool TriggerKey(const Keys arg_index)const override;
	bool ReleaseKey(const Keys arg_index)const override;
	bool CheckAnyKey()const override;
	bool TriggerAnyKey()const override;
	bool ReleaseAnyKey()const override;
	void PadUpdate() override;
	void MouseUpdate() override;
	bool GetAnyButton()const override;
	bool GetAnyButtonTrigger()const override;
	bool GetPadButton(const PadButtons arg_button)const override;
	bool GetPadButtonTrigger(const PadButtons arg_button)const override;
	bool GetPadButtonRelease(const PadButtons arg_button)const override;
	bool GetMouseButton(const MouseButtons arg_button)const override;
	bool GetMouseTrigger(const MouseButtons arg_button)const override;
	bool GetMouseReleaseTrigger(const MouseButtons arg_button)const override;
	bool GetMouseWheel()const override;
	float GetMouseWheelMove()const override;
	void SetCursorHide(const bool arg_isCursorHide)override;
	void Vibration_L(const float arg_power)override;
	void Vibration_R(const float arg_power)override;
	ButiEngine::Vector2 GetLeftStick()const override;
	ButiEngine::Vector2 GetRightStick()const override;
	float GetLeftTrigger()const override;
	float GetRightTrigger()const override;
	ButiEngine::Vector2 GetMouseMove()const override;
	ButiEngine::Vector2 GetMousePos()const override;
	void SetMouseCursor(const ButiEngine::Vector2& arg_position) override;
	void SetIsWindowActive(const bool arg_windowActive)override;
private:
	HRESULT CreateInput();
	HRESULT CreateKey();
	HRESULT CreateMouse();
	HRESULT SetKeyFormat();
	HRESULT SetMouseFormat();
	HRESULT SetKeyCooperative(HWND arg_hwnd);
	HRESULT SetMouseCooperative(HWND arg_hwnd);

	HRESULT m_result;
	LPDIRECTINPUT8 input;

	LPDIRECTINPUTDEVICE8 key;
	LPDIRECTINPUTDEVICE8 mouse;

	BYTE currentKeys[KEY_MAX];

	BYTE beffores[KEY_MAX];
	XINPUT_STATE currentPad;
	XINPUT_STATE befforePad;

	DIMOUSESTATE mouseState;
	DIMOUSESTATE beforeMouseState;
	ButiEngine::Vector2 mousePos;
	ButiEngine::Vector2 mouseMove = ButiEngine::Vector2(0, 0);
	POINT mousePoint;
	bool isMouseCenterKeep,isWindowActive;
	float vib_L = 0.0f, vib_R = 0.0f;

};
void InputManager::Release()
{
	if (key) {
		key->Release();
	}
	if (mouse) {
		mouse->Release();
	}
	if (input) {
		input->Release();
	}
}
void InputManager::Initialize(const void* arg_hwnd)
{
	memset(&emptyKeyBoard, 0, sizeof(emptyKeyBoard));
	memset(&currentKeys, 0, sizeof(currentKeys));
	memset(&beffores, 0, sizeof(beffores));
	CreateInput();
	CreateKey();
	CreateMouse();
	SetKeyFormat();
	SetMouseFormat();
	SetKeyCooperative(* reinterpret_cast<const HWND*>( arg_hwnd));
	SetMouseCooperative(*reinterpret_cast<const HWND*>(arg_hwnd));

	MouseUpdate();
}
bool InputManager::CheckKey(const std::uint32_t arg_index) const
{
	return currentKeys[arg_index] & 0x80;
}
bool InputManager::TriggerKey(const std::uint32_t arg_index) const
{
	return (currentKeys[arg_index] & 0x80) && !(beffores[arg_index] & 0x80);
}
bool InputManager::ReleaseKey(const std::uint32_t arg_index) const
{
	return !(currentKeys[arg_index] & 0x80) && (beffores[arg_index] & 0x80);
}
bool InputManager::CheckKey(const Keys arg_index) const
{
	return currentKeys[static_cast<std::int32_t>(arg_index)] & 0x80;
}
bool InputManager::TriggerKey(const Keys arg_index) const
{
	return (currentKeys[static_cast<std::int32_t>(arg_index)] & 0x80) && !(beffores[static_cast<std::int32_t>(arg_index)] & 0x80);
}
bool InputManager::ReleaseKey(const Keys arg_index) const
{
	return !(currentKeys[static_cast<std::int32_t>(arg_index)] & 0x80) && (beffores[static_cast<std::int32_t>(arg_index)] & 0x80);
}
bool InputManager::CheckAnyKey() const
{
	return !memcmp(currentKeys, emptyKeyBoard, KEY_MAX);
}
bool InputManager::TriggerAnyKey() const
{
	return memcmp(currentKeys, beffores, KEY_MAX) && (!memcmp(beffores, emptyKeyBoard, KEY_MAX));

}
bool InputManager::ReleaseAnyKey() const
{
	return !memcmp(currentKeys, beffores, KEY_MAX) && (memcmp(beffores, emptyKeyBoard, KEY_MAX));
}
void InputManager::PadUpdate()
{
	DWORD dwResult;

	befforePad = currentPad;
	if (!isWindowActive) {
		memset(&currentPad, 0, sizeof(currentPad));
		return;
	}
	for (DWORD i = 0; i < CONTROLLERS_MAX; i++)
	{
		std::memset(&currentPad, 0, sizeof(currentPad));

		dwResult = XInputGetState(i, &currentPad);
		if (dwResult == ERROR_SUCCESS)
		{
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed = static_cast<WORD>(ushort_max * vib_L);
			vibration.wRightMotorSpeed = static_cast<WORD>(ushort_max * vib_R);
			XInputSetState(i, &vibration);
			vib_L = 0;
			vib_R = 0;
		}
		else
		{

		}
	}
}
void InputManager::MouseUpdate()
{
	if (!isWindowActive) {
		memcpy_s(beffores, KEY_MAX, currentKeys, KEY_MAX);
		memset(&currentKeys, 0, sizeof(currentKeys));
		return;
	}
	beforeMouseState = mouseState;
	auto hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);

	if (hr != S_OK) {
		mouse->Acquire();
		hr = mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	}
	GetCursorPos(&mousePoint);
	mouseMove.x = mousePos.x - mousePoint.x;
	mouseMove.y = mousePos.y - mousePoint.y;

	mousePos.x =static_cast<float>( mousePoint.x); mousePos.y =static_cast<float> (mousePoint.y);

	memcpy_s(beffores, KEY_MAX, currentKeys, KEY_MAX);


	key->GetDeviceState(sizeof(currentKeys), &currentKeys);
}
bool InputManager::GetAnyButton() const
{
	return(currentPad.Gamepad.wButtons != 0);
}
bool InputManager::GetAnyButtonTrigger() const
{
	return (currentPad.Gamepad.wButtons != 0 && befforePad.Gamepad.wButtons == 0);
}
bool InputManager::GetPadButton(const PadButtons arg_button) const
{
	return ButiEngine::Util::GetBitFlag(currentPad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button));
}
bool InputManager::GetPadButtonTrigger(const PadButtons arg_button) const
{
	return ButiEngine::Util::GetBitFlag(currentPad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button)) && !ButiEngine::Util::GetBitFlag(befforePad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button));

}
bool InputManager::GetPadButtonRelease(const PadButtons arg_button) const
{
	return !ButiEngine::Util::GetBitFlag(currentPad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button)) && ButiEngine::Util::GetBitFlag(befforePad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button));

}
bool InputManager::GetMouseButton(const MouseButtons arg_button) const
{
	return mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] == char_max;
}
bool InputManager::GetMouseTrigger(const MouseButtons arg_button) const
{
	return (beforeMouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] != char_max) && mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] == char_max;

}
bool InputManager::GetMouseReleaseTrigger(const MouseButtons arg_button) const
{
	return (beforeMouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] == char_max) && mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] != char_max;

}
bool InputManager::GetMouseWheel() const
{
	if (mouseState.lZ != 0)
		return true;
	else {
		return false;
	}
}
float InputManager::GetMouseWheelMove() const
{
	return static_cast<float>( mouseState.lZ);
}
void InputManager::SetCursorHide(const bool arg_isCursorHide)
{
	if (arg_isCursorHide) {
		ShowCursor(false);
	}
	else {
		ShowCursor(true);
	}
}
void InputManager::Vibration_L(const float arg_power)
{
	vib_L = arg_power;
}
void InputManager::Vibration_R(const float arg_power)
{
	vib_R = arg_power;
}
ButiEngine::Vector2 InputManager::GetLeftStick() const
{
	float x = currentPad.Gamepad.sThumbLX;
	float y = currentPad.Gamepad.sThumbLY;

	if (x < 0)x += 1;
	if (y < 0)y += 1;
	return ButiEngine::Vector2(x * shortDiv, y * shortDiv);
}
ButiEngine::Vector2 InputManager::GetRightStick() const
{
	float x = currentPad.Gamepad.sThumbRX;
	float y = currentPad.Gamepad.sThumbRY;

	if (x < 0)x += 1;
	if (y < 0)y += 1;
	return ButiEngine::Vector2(x * shortDiv, y * shortDiv);
}
float InputManager::GetLeftTrigger() const
{
	float t = currentPad.Gamepad.bLeftTrigger * ucharDiv;
	return t;
}
float InputManager::GetRightTrigger() const
{
	float t = currentPad.Gamepad.bRightTrigger * ucharDiv;
	return t;
}
ButiEngine::Vector2 InputManager::GetMouseMove() const
{
	return mouseMove;
}
ButiEngine::Vector2 InputManager::GetMousePos() const
{
	return mousePos;
}
void InputManager::SetMouseCursor(const ButiEngine::Vector2& arg_position)
{
	SetCursorPos(static_cast<std::int32_t>( arg_position.x), static_cast<std::int32_t>(arg_position.y));
	mousePos = arg_position;
}
void InputManager::SetIsWindowActive(const bool arg_windowActive)
{
	isWindowActive = arg_windowActive;
	if (!arg_windowActive) {
		std::int32_t i = 0;
	}
}
HRESULT InputManager::CreateInput()
{
	m_result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8,reinterpret_cast<void**>(&input), NULL);
	return m_result;
}
HRESULT InputManager::CreateKey()
{
	m_result = input->CreateDevice(GUID_SysKeyboard, &key, NULL);
	return m_result;
}
HRESULT InputManager::CreateMouse()
{
	m_result = input->CreateDevice(GUID_SysMouse, &mouse, NULL);
	return m_result;
}
HRESULT InputManager::SetKeyFormat()
{
	m_result = key->SetDataFormat(&c_dfDIKeyboard);
	return m_result;
}
HRESULT InputManager::SetMouseFormat()
{
	m_result = mouse->SetDataFormat(&c_dfDIMouse);
	return m_result;
}
HRESULT InputManager::SetKeyCooperative(HWND arg_hwnd)
{
	m_result = key->SetCooperativeLevel(arg_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	key->Acquire();
	return m_result;
}
HRESULT InputManager::SetMouseCooperative(HWND arg_hwnd)
{
	m_result = mouse->SetCooperativeLevel(arg_hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	DIPROPDWORD diProperty;
	diProperty.diph.dwSize = sizeof(diProperty);
	diProperty.diph.dwHeaderSize = sizeof(diProperty.diph);
	diProperty.diph.dwObj = 0;
	diProperty.diph.dwHow = DIPH_DEVICE;
	diProperty.dwData = DIPROPAXISMODE_REL;
	m_result = mouse->SetProperty(DIPROP_AXISMODE, &diProperty.diph);
	mouse->Acquire();

	return m_result;
}
}

ButiEngine::Value_ptr<ButiInput::IInputManager> ButiInput::CreateInputManager(const void* arg_hwnd)
{
	auto output = ButiEngine::make_value<ButiInput::InputManager>();
	output->Initialize(arg_hwnd);
	return output;
}