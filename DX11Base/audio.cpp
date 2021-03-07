#include "audio.h"


struct SOUNDPARAM
{
	const char *pFilename;	// path to sound file
	int nCntLoop;			// -1 == LOOP ENDLESSLY, 0 == DONT LOOP, >= 1 == LOOP
};

// sound files to load
SOUNDPARAM g_aParam[AUDIO_TYPE_MAX] =
{
	{"asset/audio/bgm/StillAlive.wav", -1},
	{"asset/audio/bgm/WantYouGone.wav", -1},
	{"asset/audio/se/GunfireBlue.wav", 0},
	{"asset/audio/se/GunfireOrange.wav", 0},
	{"asset/audio/se/PortalTravel.wav", 0}
};

IXAudio2* Audio::m_pXAudio2 = nullptr;						
IXAudio2MasteringVoice* Audio::m_pMasteringVoice = nullptr;
IXAudio2SourceVoice* Audio::m_apSourceVoice[AUDIO_TYPE_MAX] = {};
BYTE* Audio::m_apDataAudio[AUDIO_TYPE_MAX] = {};
DWORD Audio::m_aSizeAudio[AUDIO_TYPE_MAX] = {};

float Audio::m_DeltaTime;
float Audio::m_targetTime, Audio::m_targetVolume;
bool Audio::m_FadeFlag;
float Audio::m_curVolume;
AUDIO_TYPE Audio::m_curFadeSound;


HRESULT Audio::Init(HWND hWnd)
{
	m_DeltaTime = 0;
	m_FadeFlag = false;

	HRESULT hr;

	// enable multithread for sound playback
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// initialize xaudio2
	hr = XAudio2Create(&m_pXAudio2, 0);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "Failed to initialize XAudio2!", "Error!", MB_ICONWARNING);

		CoUninitialize();

		return E_FAIL;
	}
	
	// initialize master voice
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "Failed to initialize Mastering Voice!", "Error!", MB_ICONWARNING);

		if(m_pXAudio2)
		{
			m_pXAudio2->Release();
			m_pXAudio2 = NULL;
		}

		CoUninitialize();

		return E_FAIL;
	}

	// load sound files
	for(int nCntSound = 0; nCntSound < AUDIO_TYPE_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// load sound files from given path
		hFile = CreateFile(g_aParam[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "Invalid Handle Value!", "Error!", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}
		if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			MessageBox(hWnd, "Invalid Set File Pointer!", "Error!", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}
	
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "Failed at checking chunk!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "Failed to read chunk data!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}
		if(dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "Invalid file type!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}
	
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "Failed to check chunk!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "Failed to read chunk!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}

		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "Failed to check chunk!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}
		m_apDataAudio[nCntSound] = (BYTE*)malloc(m_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "Failed to read chunk!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}
	
		// create source voice
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format));
		if(FAILED(hr))
		{
			MessageBox(hWnd, "Failed at creating source voice!", "Error!", MB_ICONWARNING);
			return S_FALSE;
		}

		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags      = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount  = g_aParam[nCntSound].nCntLoop;

		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return S_OK;
}

void Audio::Uninit()
{
	for(int nCntSound = 0; nCntSound < AUDIO_TYPE_MAX; nCntSound++)
	{
		if(m_apSourceVoice[nCntSound])
		{
			m_apSourceVoice[nCntSound]->Stop(0);
	
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = NULL;
	
			free(m_apDataAudio[nCntSound]);
			m_apDataAudio[nCntSound] = NULL;
		}
	}
	
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = NULL;
	
	if(m_pXAudio2)
	{
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
	}
	
	CoUninitialize();
}

void Audio::Update()
{
	if (m_FadeFlag)
		UpdateFadeSound();
}

HRESULT Audio::PlaySound(AUDIO_TYPE type, float volume)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// init buffer
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[type];
	buffer.pAudioData = m_apDataAudio[type];
	buffer.Flags      = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount  = g_aParam[type].nCntLoop;

	m_apSourceVoice[type]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{
		m_apSourceVoice[type]->Stop(0);
		m_apSourceVoice[type]->FlushSourceBuffers();
	}

	// submit source buffer
	m_apSourceVoice[type]->SubmitSourceBuffer(&buffer);

	// play sound
	m_apSourceVoice[type]->Start(0);

	// set volume
	SetVolume(type, volume);

	return S_OK;
}

void Audio::StopAudio(AUDIO_TYPE type)
{
	XAUDIO2_VOICE_STATE xa2state;

	m_apSourceVoice[type]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{
		m_apSourceVoice[type]->Stop(0);
		m_apSourceVoice[type]->FlushSourceBuffers();
	}
}

void Audio::StopAllAudio()
{
	StopFade();
	for(int nCntSound = 0; nCntSound < AUDIO_TYPE_MAX; nCntSound++)
	{
		if(m_apSourceVoice[nCntSound])
			m_apSourceVoice[nCntSound]->Stop(0);
	}
}

HRESULT Audio::CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;
	
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return HRESULT_FROM_WIN32(GetLastError());
	
	while(hr == S_OK)
	{
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
			hr = HRESULT_FROM_WIN32(GetLastError());

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch(dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
				hr = HRESULT_FROM_WIN32(GetLastError());

			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;
		if(dwChunkType == format)
		{
			*pChunkSize         = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if(dwBytesRead >= dwRIFFDataSize)
			return S_FALSE;
	}
	
	return S_OK;
}

HRESULT Audio::ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;
	
	if(SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return HRESULT_FROM_WIN32(GetLastError());

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
		return HRESULT_FROM_WIN32(GetLastError());
	
	return S_OK;
}

HRESULT Audio::SetVolume(AUDIO_TYPE type,float volume,UINT32 OperationSet)
{
	return m_apSourceVoice[type]->SetVolume(volume * AUDIO_MASTER, OperationSet);
}

float Audio::GetVolume(AUDIO_TYPE type)
{
	float vol;
	m_apSourceVoice[type]->GetVolume(&vol);
	return vol;
}

void Audio::UpdateFadeSound()
{
	if (m_DeltaTime < m_targetTime)
	{
		SetVolume(m_curFadeSound, m_targetVolume * (m_DeltaTime / m_targetTime) + 
			m_curVolume * ((m_targetTime - m_DeltaTime ) / m_targetTime));

		m_DeltaTime += (1.0f / 60.0f);
	}
	else
	{
		SetVolume(m_curFadeSound, m_targetVolume);
		if (m_targetVolume == 0)
			StopAudio(m_curFadeSound);

		m_DeltaTime = 0;
		m_FadeFlag = false;
	}
}

void Audio::StartFade(AUDIO_TYPE type, float targetVolume, float targetTime)
{
	// return if a sound is currently fading
	if (m_FadeFlag) return;

	m_curFadeSound = type;
	m_apSourceVoice[type]->GetVolume(&m_curVolume);
	m_targetVolume = targetVolume;
	m_targetTime = targetTime;
	m_FadeFlag = true;
}

void Audio::StopFade()
{
	m_DeltaTime = 0;
	m_FadeFlag = false;
}

void Audio::SetPlaybackSpeed(AUDIO_TYPE type, float speed)
{
	// return if given speed is the same as currently set
	float value;
	m_apSourceVoice[type]->GetFrequencyRatio(&value);
	if (value == speed)
		return;

	// set playback speed and pitch
	m_apSourceVoice[type]->SetFrequencyRatio(speed, 0);
}
