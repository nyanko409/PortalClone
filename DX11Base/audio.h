#pragma once

#include <XAudio2.h>

#define AUDIO_MASTER 0.5f


enum AUDIO_TYPE
{
	AUDIO_BGM_GAME,
	AUDIO_SE_FIREBLUE,
	AUDIO_SE_FIREORANGE,
	AUDIO_SE_PORTALTRAVEL,

	AUDIO_TYPE_MAX
};

static class Audio
{
public:
	static HRESULT Init(HWND hWnd);
	static void Uninit();
	static void Update();

	static HRESULT PlaySound(AUDIO_TYPE type, float volume = 1.0F);
	static void StopAudio(AUDIO_TYPE type);
	static void StopAllAudio();
	 
	static HRESULT SetVolume(AUDIO_TYPE type, float volume, UINT32 OperationSet = 0);
	static float GetVolume(AUDIO_TYPE type);
	 
	static void StartFade(AUDIO_TYPE type, float targetVolume, float targetTime);
	static void StopFade();
	 
	static void SetPlaybackSpeed(AUDIO_TYPE type, float speed);

private:
	static IXAudio2* m_pXAudio2;									// XAudio2 interface
	static IXAudio2MasteringVoice* m_pMasteringVoice;				// master voice
	static IXAudio2SourceVoice* m_apSourceVoice[AUDIO_TYPE_MAX];	// source voice
	static BYTE* m_apDataAudio[AUDIO_TYPE_MAX];
	static DWORD m_aSizeAudio[AUDIO_TYPE_MAX];

	static float m_DeltaTime;
	static float m_targetTime, m_targetVolume;
	static bool m_FadeFlag;
	static float m_curVolume;
	static AUDIO_TYPE m_curFadeSound;

	static HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	static HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	static void UpdateFadeSound();
};
