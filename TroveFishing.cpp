#include "stdafx.h"
#include <mmdeviceapi.h>
#include "EndpointVolume.h"
#include <iostream>

using namespace std;

/*
Does no error handling
Gets current output volume level from currently playing audio.
*/
class Audio
{
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	IMMDevice *playbackDevice;
	HRESULT hr;
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	IAudioMeterInformation *meterInfo = NULL;
	float soundLevel = 0;
public:
	Audio()
	{
		//Initialize COM initializer
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&deviceEnumerator);

		//Get the sound device device
		hr = deviceEnumerator->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &playbackDevice);
		//Activate chosen interface with device, in this case IAudioMeterInformation
		hr = playbackDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&meterInfo));
	}

	float getOutputVolumeLevel()
	{
		//Get current sound level from IAudioMeterInformation
		hr = meterInfo->GetPeakValue(&soundLevel);

		return soundLevel * 100.0f;
	}

};

/* 
Old-style depreciated
keybd_event(VkKeyScan('F'), 0, 0, 0);
keybd_event(VkKeyScan('F'), 0, KEYEVENTF_KEYUP, 0);
*/

/*
Accept a WCHAR as the key to be pressed in constructor has functions "down" and "release"
*/
class SendKey {

	INPUT input;

public:
	SendKey(WCHAR keyToPress)
	{
		const SHORT key = VkKeyScan(keyToPress);
		const UINT mappedKey = MapVirtualKey(LOBYTE(key), 0);
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = mappedKey;
	}
	void down() {
		input.ki.dwFlags = KEYEVENTF_SCANCODE;		
		SendInput(1, &input, sizeof(input));
	}

	void release() {
		input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(input));
	}

	void press() {
		down();
		Sleep(400);
		release();
	}
};

int main()
{
	SendKey *key = new SendKey('f');

	Audio *audio = new Audio();

	//Find a particular window that we want to send our key presses to
	HWND GameWindow = FindWindow(0, TEXT("Trove"));
	SetForegroundWindow(GameWindow);
	

	key->down();
	Sleep(500);
	key->release();

	Sleep(2000);
	while (true)
	{
		Sleep(10);
		float volume = audio->getOutputVolumeLevel();

		if (volume > 7.4f)
		{
			Sleep(500);
			key->press();

			cout << volume << endl;
			Sleep(5000);

			key->press();

			Sleep(2000);
		}
	}

	return 0;
}

