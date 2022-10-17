#include"stdafx.h"

#include<dinput.h>
#include <xinput.h>
#include"ButiUtil/ButiUtil/Util.h"
#include"ButiUtil/ButiUtil/Helper/StringHelper.h"
#include "ButiCereal/include/cereal/cereal.hpp"
#include "ButiCereal/include/cereal/archives/portable_binary.hpp"
#include "ButiCereal/include/cereal/types/ButiContaier.hpp"
#include<filesystem>
#include<fstream>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

constexpr std::int32_t KEY_MAX = 256;
constexpr std::int32_t CONTROLLERS_MAX = 1;
constexpr std::uint16_t ushort_max = 0xffff;
constexpr float shortDiv = 1.0f / 32767;
const unsigned char char_max = 128;	
constexpr float ucharDiv = 1.0f / 255;
constexpr std::int32_t logFileDataSize = 60*60;
BYTE emptyKeyBoard[256];
namespace ButiInput {
struct _XINPUT_GAMEPAD_cerealize
{
	WORD                                wButtons;
	BYTE                                bLeftTrigger;
	BYTE                                bRightTrigger;
	SHORT                               sThumbLX;
	SHORT                               sThumbLY;
	SHORT                               sThumbRX;
	SHORT                               sThumbRY;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(wButtons);
		archive(bLeftTrigger);
		archive(bRightTrigger);
		archive(sThumbLX);
		archive(sThumbLY);
		archive(sThumbRX);
		archive(sThumbRY);
	}
};

struct _XINPUT_STATE_cerealize
{
	DWORD                               dwPacketNumber;
	_XINPUT_GAMEPAD_cerealize                      Gamepad;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(dwPacketNumber);
		archive(Gamepad);
	}
};
struct _DIMOUSESTATE_cerealize {
	LONG    lX;
	LONG    lY;
	LONG    lZ;
	BYTE    rgbButtons[4];
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(lX);
		archive(lY);
		archive(lZ);
		archive(rgbButtons);
	}
};
class InputData {
public:
	InputData& operator=(const InputData& arg_other) {
		this->pad = arg_other.pad;
		this->mouseState = arg_other.mouseState;
		memcpy_s(this->keys, KEY_MAX, arg_other.keys, KEY_MAX);
		return *this;
	}
	void Clear() {
		memset(&pad, 0, sizeof(pad));
		memcpy_s(keys, KEY_MAX, keys, KEY_MAX);
		memset(&keys, 0, sizeof(keys));
	}
	BYTE keys[KEY_MAX];
	union _pad
	{
		XINPUT_STATE pad;
		_XINPUT_STATE_cerealize cereal;
	} pad;
	union _mouse {
		DIMOUSESTATE mouseState;
		_DIMOUSESTATE_cerealize cereal;
	} mouseState;
	ButiEngine::Vector2 mousePos;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(keys);
		archive(pad.cereal);
		archive(mouseState.cereal);
		archive(mousePos);
	}
};
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
	void Update() override;
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
	void StartRec() override {
		if (m_isReplay) { return; }
		m_isInputRec = true; m_inputFileIndex = 0; m_list_input.Clear();
		m_list_input.Add(m_currentInput);
		m_logDataFilePath = m_logDataDir + std::to_string(m_inputFileIndex) + ".ilog";
	}
	void SetLogFileDir(const std::string& arg_fileDir)override {
		m_logDataDir = arg_fileDir;
	}
	void StopRec() override;
	void Replay(const ButiEngine::List< const void* >& arg_list_data, const ButiEngine::List<std::int64_t>& arg_list_dataSize)override;
	void Replay(const std::string& arg_logDir) override;
	void ReplayStop() override { m_isReplay = false; }
	bool IsReplay() override { return m_isReplay; }
private:
	HRESULT CreateInput();
	HRESULT CreateKey();
	HRESULT CreateMouse();
	HRESULT SetKeyFormat();
	HRESULT SetMouseFormat();
	HRESULT SetKeyCooperative(HWND arg_hwnd);
	HRESULT SetMouseCooperative(HWND arg_hwnd);
	void RecInput();
	void PadUpdate();
	void MouseUpdate();
	HRESULT m_result;
	LPDIRECTINPUT8 m_input;
	LPDIRECTINPUTDEVICE8 m_key;
	LPDIRECTINPUTDEVICE8 m_mouse;
	InputData m_currentInput, m_befInput;
	ButiEngine::Vector2 m_mouseMove = ButiEngine::Vector2(0, 0);
	POINT m_mousePoint;
	bool m_isMouseCenterKeep,m_isWindowActive,m_isInputRec,m_isReplay;
	float m_vib_L = 0.0f, m_vib_R = 0.0f;
	std::int32_t m_inputFileIndex=0,m_replayIndex=0;
	ButiEngine::List<InputData> m_list_input;
	std::string m_logDataDir,m_logDataFilePath;
};
void InputManager::StopRec() { 
	m_isInputRec = false;
}
void InputManager::Replay(const ButiEngine::List< const void* >& arg_list_data, const ButiEngine::List<std::int64_t>& arg_list_dataSize)
{
	m_replayIndex = 0;
	m_list_input.Clear();
	m_isReplay = true;
	std::stringstream stream; std::int32_t index = 0;
	for (auto data : arg_list_data) {
		ButiEngine::List<InputData> list_data;
		stream <<std::string(reinterpret_cast<const char*>( data),arg_list_dataSize[ index]);
		cereal::PortableBinaryInputArchive binInputARCHIVE_BUTI(stream);
		binInputARCHIVE_BUTI(list_data);
		stream.clear();
		m_list_input.Add(list_data); index++;
	}
}
void InputManager::Replay(const std::string& arg_dir)
{
	m_replayIndex = 0;
	m_list_input.Clear();
	m_isReplay = true;
	std::stringstream stream;
	std::filesystem::directory_iterator itr(arg_dir), end;
	std::error_code err;
	for (; itr != end && !err; itr.increment(err)) {
		ButiEngine::List<InputData> list_data;

		if (StringHelper::GetExtension(itr->path().string()) != "ilog") { continue; }
		std::ifstream inputFile(itr->path().string(),std::ios::binary);
		stream << inputFile.rdbuf();
		cereal::PortableBinaryInputArchive binInputARCHIVE_BUTI(stream);
		binInputARCHIVE_BUTI(list_data);
		stream.clear();
		m_list_input.Add(list_data);
	}

}
void InputManager::Release()
{
	if (m_key) {
		m_key->Release();
	}
	if (m_mouse) {
		m_mouse->Release();
	}
	if (m_input) {
		m_input->Release();
	}
}
void InputManager::Initialize(const void* arg_hwnd)
{
	memset(&emptyKeyBoard, 0, sizeof(emptyKeyBoard));
	memset(&m_currentInput.keys, 0, sizeof(m_currentInput.keys));
	memset(&m_befInput.keys, 0, sizeof(m_befInput.keys));
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
	return m_currentInput.keys[arg_index] & 0x80;
}
bool InputManager::TriggerKey(const std::uint32_t arg_index) const
{
	return (m_currentInput.keys[arg_index] & 0x80) && !(m_befInput.keys[arg_index] & 0x80);
}
bool InputManager::ReleaseKey(const std::uint32_t arg_index) const
{
	return !(m_currentInput.keys[arg_index] & 0x80) && (m_befInput.keys[arg_index] & 0x80);
}
bool InputManager::CheckKey(const Keys arg_index) const
{
	return m_currentInput.keys[static_cast<std::int32_t>(arg_index)] & 0x80;
}
bool InputManager::TriggerKey(const Keys arg_index) const
{
	return (m_currentInput.keys[static_cast<std::int32_t>(arg_index)] & 0x80) && !(m_befInput.keys[static_cast<std::int32_t>(arg_index)] & 0x80);
}
bool InputManager::ReleaseKey(const Keys arg_index) const
{
	return !(m_currentInput.keys[static_cast<std::int32_t>(arg_index)] & 0x80) && (m_befInput.keys[static_cast<std::int32_t>(arg_index)] & 0x80);
}
bool InputManager::CheckAnyKey() const
{
	return !memcmp(m_currentInput.keys, emptyKeyBoard, KEY_MAX);
}
bool InputManager::TriggerAnyKey() const
{
	return memcmp(m_currentInput.keys, m_befInput.keys, KEY_MAX) && (!memcmp(m_befInput.keys, emptyKeyBoard, KEY_MAX));

}
bool InputManager::ReleaseAnyKey() const
{
	return !memcmp(m_currentInput.keys, m_befInput.keys, KEY_MAX) && (memcmp(m_befInput.keys, emptyKeyBoard, KEY_MAX));
}

void InputManager::RecInput(){
	m_list_input.Add(m_currentInput);

	ButiEngine::Util::MakeDirectory(m_logDataDir);
	std::stringstream stream;
	{
		cereal::PortableBinaryOutputArchive binOutARCHIVE_BUTI(stream);
		binOutARCHIVE_BUTI(m_list_input);
	}
	std::ofstream outputFile(m_logDataFilePath,std::ios::binary);
	outputFile << stream.str();
	outputFile.close();
	stream.clear();
	if (m_list_input.GetSize()>logFileDataSize) {
		m_list_input.Clear(); 
		m_inputFileIndex++;
		m_logDataFilePath = m_logDataDir + std::to_string(m_inputFileIndex) + ".ilog";		
	}
}
void InputManager::Update() {

	if (!m_isWindowActive) {
		m_currentInput.Clear();
		return;
	}
	m_befInput= m_currentInput;
	if (m_isReplay&&m_replayIndex<m_list_input.GetSize()) {
		m_currentInput = m_list_input[m_replayIndex];
		m_replayIndex++;
	}
	else {
		PadUpdate();
		MouseUpdate();
	}
	if (m_isInputRec) {
		RecInput();
	}
}
void InputManager::PadUpdate()
{
	DWORD dwResult;
	for (DWORD i = 0; i < CONTROLLERS_MAX; i++)
	{
		std::memset(&m_currentInput.pad, 0, sizeof(m_currentInput.pad));

		dwResult = XInputGetState(i, &m_currentInput.pad.pad);
		if (dwResult == ERROR_SUCCESS)
		{
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed = static_cast<WORD>(ushort_max *  m_vib_L);
			vibration.wRightMotorSpeed = static_cast<WORD>(ushort_max * m_vib_R);
			XInputSetState(i, &vibration);
			m_vib_L = 0;
			m_vib_R = 0;
		}
		else
		{

		}
	}
}
void InputManager::MouseUpdate()
{
	auto hr = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_currentInput.mouseState);

	if (hr != S_OK) {
		m_mouse->Acquire();
		hr = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_currentInput.mouseState);
	}
	GetCursorPos(&m_mousePoint);
	m_mouseMove.x = m_currentInput.mousePos.x - m_mousePoint.x;
	m_mouseMove.y = m_currentInput.mousePos.y - m_mousePoint.y;

	m_currentInput.mousePos.x =static_cast<float>( m_mousePoint.x); m_currentInput.mousePos.y =static_cast<float> (m_mousePoint.y);

	m_key->GetDeviceState(sizeof(m_currentInput.keys), &m_currentInput.keys);
}
bool InputManager::GetAnyButton() const
{
	return(m_currentInput.pad.pad.Gamepad.wButtons != 0);
}
bool InputManager::GetAnyButtonTrigger() const
{
	return (m_currentInput.pad.pad.Gamepad.wButtons != 0 && m_befInput.pad.pad.Gamepad.wButtons == 0);
}
bool InputManager::GetPadButton(const PadButtons arg_button) const
{
	return ButiEngine::Util::GetBitFlag(m_currentInput.pad.pad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button));
}
bool InputManager::GetPadButtonTrigger(const PadButtons arg_button) const
{
	return ButiEngine::Util::GetBitFlag(m_currentInput.pad.pad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button)) && !ButiEngine::Util::GetBitFlag(m_befInput.pad.pad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button));

}
bool InputManager::GetPadButtonRelease(const PadButtons arg_button) const
{
	return !ButiEngine::Util::GetBitFlag(m_currentInput.pad.pad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button)) && ButiEngine::Util::GetBitFlag(m_befInput.pad.pad.Gamepad.wButtons, static_cast<std::int32_t>(arg_button));

}
bool InputManager::GetMouseButton(const MouseButtons arg_button) const
{
	return m_currentInput.mouseState.mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] == char_max;
}
bool InputManager::GetMouseTrigger(const MouseButtons arg_button) const
{
	return (m_befInput.mouseState.mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] != char_max) && m_currentInput.mouseState.mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] == char_max;

}
bool InputManager::GetMouseReleaseTrigger(const MouseButtons arg_button) const
{
	return (m_befInput.mouseState.mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] == char_max) && m_currentInput.mouseState.mouseState.rgbButtons[static_cast<std::int32_t>(arg_button)] != char_max;

}
bool InputManager::GetMouseWheel() const
{
	if (m_currentInput.mouseState.mouseState.lZ != 0)
		return true;
	else {
		return false;
	}
}
float InputManager::GetMouseWheelMove() const
{
	return static_cast<float>( m_currentInput.mouseState.mouseState.lZ);
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
	m_vib_L = arg_power;
}
void InputManager::Vibration_R(const float arg_power)
{
	m_vib_R = arg_power;
}
ButiEngine::Vector2 InputManager::GetLeftStick() const
{
	float x = m_currentInput.pad.pad.Gamepad.sThumbLX;
	float y = m_currentInput.pad.pad.Gamepad.sThumbLY;

	if (x < 0)x += 1;
	if (y < 0)y += 1;
	return ButiEngine::Vector2(x * shortDiv, y * shortDiv);
}
ButiEngine::Vector2 InputManager::GetRightStick() const
{
	float x = m_currentInput.pad.pad.Gamepad.sThumbRX;
	float y = m_currentInput.pad.pad.Gamepad.sThumbRY;

	if (x < 0)x += 1;
	if (y < 0)y += 1;
	return ButiEngine::Vector2(x * shortDiv, y * shortDiv);
}
float InputManager::GetLeftTrigger() const
{
	float t = m_currentInput.pad.pad.Gamepad.bLeftTrigger * ucharDiv;
	return t;
}
float InputManager::GetRightTrigger() const
{
	float t = m_currentInput.pad.pad.Gamepad.bRightTrigger * ucharDiv;
	return t;
}
ButiEngine::Vector2 InputManager::GetMouseMove() const
{
	return m_mouseMove;
}
ButiEngine::Vector2 InputManager::GetMousePos() const
{
	return m_currentInput.mousePos;
}
void InputManager::SetMouseCursor(const ButiEngine::Vector2& arg_position)
{
	SetCursorPos(static_cast<std::int32_t>( arg_position.x), static_cast<std::int32_t>(arg_position.y));
	m_currentInput.mousePos = arg_position;
}
void InputManager::SetIsWindowActive(const bool arg_windowActive)
{
	m_isWindowActive = arg_windowActive;
	if (!arg_windowActive) {
		std::int32_t i = 0;
	}
}
HRESULT InputManager::CreateInput()
{
	m_result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8,reinterpret_cast<void**>(&m_input), NULL);
	return m_result;
}
HRESULT InputManager::CreateKey()
{
	m_result = m_input->CreateDevice(GUID_SysKeyboard, &m_key, NULL);
	return m_result;
}
HRESULT InputManager::CreateMouse()
{
	m_result = m_input->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	return m_result;
}
HRESULT InputManager::SetKeyFormat()
{
	m_result = m_key->SetDataFormat(&c_dfDIKeyboard);
	return m_result;
}
HRESULT InputManager::SetMouseFormat()
{
	m_result = m_mouse->SetDataFormat(&c_dfDIMouse);
	return m_result;
}
HRESULT InputManager::SetKeyCooperative(HWND arg_hwnd)
{
	m_result = m_key->SetCooperativeLevel(arg_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	m_key->Acquire();
	return m_result;
}
HRESULT InputManager::SetMouseCooperative(HWND arg_hwnd)
{
	m_result = m_mouse->SetCooperativeLevel(arg_hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	DIPROPDWORD diProperty;
	diProperty.diph.dwSize = sizeof(diProperty);
	diProperty.diph.dwHeaderSize = sizeof(diProperty.diph);
	diProperty.diph.dwObj = 0;
	diProperty.diph.dwHow = DIPH_DEVICE;
	diProperty.dwData = DIPROPAXISMODE_REL;
	m_result = m_mouse->SetProperty(DIPROP_AXISMODE, &diProperty.diph);
	m_mouse->Acquire();

	return m_result;
}
}

ButiEngine::Value_ptr<ButiInput::IInputManager> ButiInput::CreateInputManager(const void* arg_hwnd)
{
	auto output = ButiEngine::make_value<ButiInput::InputManager>();
	output->Initialize(arg_hwnd);
	return output;
}