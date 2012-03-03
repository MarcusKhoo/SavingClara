/*==============================================
* SoundComponent class for GDEV Engine
*
* Written by <YOUR NAME HERE>
*==============================================*/

#include "SoundComponent.h"
#include "Fail.h"

// handy trick to auto link the libraries
#pragma comment(lib, "x3daudio.lib") // instruction to the linker


CSoundComponent::CSoundComponent(const char* engine_xgs,const char* wave_xwb,const char* sound_xsb)
{
	mpXEngine=NULL;
	mpWaveBank=NULL;
	mpSoundBank=NULL;
	mpbWaveBank=NULL;
	mpbSoundBank=NULL;
	mEngName=engine_xgs;
	mWaveName=wave_xwb;
	mSoundName=sound_xsb;
}

void CSoundComponent::Startup()
{
	if (InitXACT(mEngName.c_str())==false ||
		LoadWaveBank(mWaveName.c_str())==false ||
		LoadSoundBank(mSoundName.c_str())==false)
	{
		CloseXACT();	// give up
	}
}
void CSoundComponent::Update(float dt)
{
	mpXEngine->DoWork();	// this is a _really_ stupid fn name
}
void CSoundComponent::Shutdown()
{
	delete dspSettings.pMatrixCoefficients;
	CloseXACT();	// close all
}

bool CSoundComponent::InitXACT(const char* engine_xgs)
{
	HRESULT hr;
	CoInitializeEx( NULL, COINIT_MULTITHREADED );  // COINIT_APARTMENTTHREADED will work too
	//TODO("Create the XACT engine");
	hr=XACT3CreateEngine(0, &mpXEngine);

	if( FAILED( hr ) || mpXEngine == NULL )
	{
		FAIL("Problems creating XACT engine","InitXACT Failed");
		return false;
	}

	// Load the global settings file and pass it into XACTInitialize
	VOID* pGlobalSettingsData = NULL;
	DWORD dwGlobalSettingsFileSize = 0;
	bool bSuccess = false;
	HANDLE hFile = CreateFile( engine_xgs, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( hFile!=INVALID_HANDLE_VALUE )
	{
		dwGlobalSettingsFileSize = GetFileSize( hFile, NULL );
		if( dwGlobalSettingsFileSize != INVALID_FILE_SIZE )
		{
			pGlobalSettingsData = CoTaskMemAlloc( dwGlobalSettingsFileSize );
			if( pGlobalSettingsData )
			{
				DWORD dwBytesRead;
				if( 0 != ReadFile( hFile, pGlobalSettingsData, dwGlobalSettingsFileSize, &dwBytesRead, NULL ) )
				{
					bSuccess = true;
				}
			}
		}
		CloseHandle( hFile );
	}
	if( !bSuccess )
	{
		if( pGlobalSettingsData )
			CoTaskMemFree( pGlobalSettingsData );
		pGlobalSettingsData = NULL;
		dwGlobalSettingsFileSize = 0;
		FAIL(engine_xgs,"Problems opening engine file");
		return false;
	}

	// Initialize & create the XACT runtime 
	XACT_RUNTIME_PARAMETERS xrParams = {0};
	xrParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
	xrParams.pGlobalSettingsBuffer = pGlobalSettingsData;
	xrParams.globalSettingsBufferSize = dwGlobalSettingsFileSize;
	xrParams.globalSettingsFlags = XACT_FLAG_GLOBAL_SETTINGS_MANAGEDATA; // this will tell XACT to delete[] the buffer when its unneeded
	//TODO(Initalise the XACT engine);
	hr=mpXEngine->Initialize(&xrParams);
	if( FAILED( hr ) )
	{
		FAIL(engine_xgs,"Problems Initalising XACT engine");
		return false;
	}

	X3DAUDIO_EMITTER emitter = {0};
	X3DAUDIO_LISTENER listener = {0};

	listener.OrientFront = D3DXVECTOR3( 0, 0, 1 );
	listener.OrientTop = D3DXVECTOR3( 0, 1, 0 );
	listener.Position = D3DXVECTOR3( 0, 0, 0 );
	listener.Velocity = D3DXVECTOR3( 0, 0, 0 );

	// the following need to be orthonormal
	emitter.OrientFront = D3DXVECTOR3( 0, 0, 1 );
	emitter.OrientTop = D3DXVECTOR3( 0, 1, 0 );
	emitter.Position = D3DXVECTOR3( 0, 0, 0 );
	emitter.Velocity = D3DXVECTOR3( 0, 0, 0 ); // needs to not be zero if you want to hear Doppler effect

	// emitter ChannelCount and DSP Setting's SrcChannelCount must match
	emitter.ChannelCount = 2;   

	// this will be set by XACT3DCalculate if ChannelCount > 1.
	emitter.ChannelRadius = 1.0f;   

	// will be set by XACT3DCalculate
	emitter.pChannelAzimuths = NULL;

	// will be set by XACT3DCalculate
	emitter.pVolumeCurve = emitter.pLFECurve
		= emitter.pLPFDirectCurve
		= emitter.pLPFReverbCurve
		= emitter.pReverbCurve
		= NULL;

	emitter.CurveDistanceScaler = 1.0;
	emitter.DopplerScaler = 1.0f;

	hr = XACT3DInitialize(mpXEngine, xact3dInstance);

	if( FAILED( hr ) )
	{
		FAIL(engine_xgs,"Problems Initalising XACT 3D engine");
		return false;
	}

	// check how many output channels are supported
	WAVEFORMATEXTENSIBLE format;
	hr = mpXEngine->GetFinalMixFormat(&format);
	if(FAILED(hr))
	{
		FAIL("BLABLBA");
	}
	// fill the DSP
	ZeroMemory(&dspSettings,sizeof(dspSettings));
	// different code's seem to suggest 1 or 2 channels for the emitter
	dspSettings.SrcChannelCount = 1;
	dspSettings.DstChannelCount = format.Format.nChannels;  // as supported 
	dspSettings.pMatrixCoefficients = new FLOAT32[dspSettings.SrcChannelCount * dspSettings.DstChannelCount];
	ZeroMemory(dspSettings.pMatrixCoefficients ,sizeof(FLOAT32)*dspSettings.SrcChannelCount * dspSettings.DstChannelCount);

	return true;
}

//bool CSoundComponent::InitializeXD3DAudioStructures()
//{
//	X3DAUDIO_DSP_SETTINGS DSPSettings = {0};
//
//    WAVEFORMATEXTENSIBLE format;
//    mpXEngine->GetFinalMixFormat(&format);
//
//    DSPSettings.SrcChannelCount = srcChannelCount;
//    DSPSettings.DstChannelCount = format.Format.nChannels;    
//    DSPSettings.pMatrixCoefficients = new FLOAT32[DSPSettings.SrcChannelCount * DSPSettings.DstChannelCount];
//}

bool CSoundComponent::LoadWaveBank(const char* wave_xwb)
{
	DWORD dwFileSize;
	HANDLE hMapFile;
	HANDLE hFile;

	bool ok=false;	// assume fail
	// Create an "in memory" XACT wave bank file using memory mapped file IO
	// Memory mapped files tend to be the fastest for most situations assuming you
	// have enough virtual address space for a full map of the file
	hFile = CreateFile( wave_xwb, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		dwFileSize = GetFileSize( hFile, NULL );
		if( dwFileSize != -1 )
		{
			hMapFile = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL );
			if( hMapFile )
			{
				mpbWaveBank = MapViewOfFile( hMapFile, FILE_MAP_READ, 0, 0, 0 );   
				if( mpbWaveBank )
				{
					HRESULT hr;
					//TODO(create the wave bank);
					hr=mpXEngine->CreateInMemoryWaveBank(mpbWaveBank, dwFileSize, 0,0,&mpWaveBank);
					if(SUCCEEDED(hr))
						ok=true;
				}               
				CloseHandle( hMapFile ); // mpWaveBank maintains a handle on the file so close this unneeded handle
			}
		}
		CloseHandle( hFile ); // mpWaveBank maintains a handle on the file so close this unneeded handle
	}
	if (ok==false)
		FAIL(wave_xwb,"Problems loading Wave Bank");
	return ok;
}

bool CSoundComponent::LoadSoundBank(const char* sound_xsb)
{
	DWORD dwFileSize;
	DWORD dwBytesRead;
	HANDLE hFile;

	bool ok=false;	// assume fail

	// Read and register the sound bank file with XACT.  Do not use memory mapped file IO because the 
	// memory needs to be read/write and the working set of sound banks are small.
	hFile = CreateFile( sound_xsb, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		dwFileSize = GetFileSize( hFile, NULL );
		if( dwFileSize != -1 )
		{
			// Allocate the data here and free the data when recieving the sound bank destroyed notification
			mpbSoundBank = new BYTE[dwFileSize];
			if( mpbSoundBank )
			{
				if( 0 != ReadFile( hFile, mpbSoundBank, dwFileSize, &dwBytesRead, NULL ) )
				{
					HRESULT hr;
					//TODO(create the sound bank);
					hr=mpXEngine->CreateSoundBank(mpbSoundBank, dwFileSize, 0, 0, &mpSoundBank);
					if (SUCCEEDED(hr))
						ok=true;
				}
			}
		}
		CloseHandle( hFile );
	}
	if (ok==false)
		FAIL(sound_xsb,"Problems loading Sound Bank");
	return ok;
}

void CSoundComponent::CloseXACT()
{
	// Shutdown XACT
	//
	// Note that mpXEngine->ShutDown is synchronous and will take some time to complete
	// if there are still playing cues.  Also mpXEngine->ShutDown() is generally only
	// called when a game exits and is not the preferred method of changing audio
	// resources. To know when it is safe to free wave/sound bank data without
	// shutting down the XACT engine, use the XACTNOTIFICATIONTYPE_SOUNDBANKDESTROYED
	// or XACTNOTIFICATIONTYPE_WAVEBANKDESTROYED notifications
	if( mpXEngine )
	{
		mpXEngine->ShutDown();
		mpXEngine->Release();
	}

	if( mpbSoundBank )
		delete[] mpbSoundBank;
	mpbSoundBank = NULL;

	// After mpXEngine->ShutDown() returns it is safe to release memory mapped files
	if( mpWaveBank )
		UnmapViewOfFile( mpWaveBank );
	mpWaveBank = NULL;

	CoUninitialize();
}

void CSoundComponent::PlayCue(const char* cueName)
{
	XACTINDEX cueIdx=FindCue(cueName);
	if (cueIdx==XACTINDEX_INVALID)
		FAIL(cueName,"No such XACT cue name");
	//TODO(Play the cue);
	cueIdx=mpSoundBank->GetCueIndex(cueName);
	mpSoundBank->Play(cueIdx, 0,0,NULL);
}

IXACT3Cue* CSoundComponent::Play3DCue(const char* cueName)
{
	XACTINDEX cueIdx=FindCue(cueName);
	if (cueIdx==XACTINDEX_INVALID)
		FAIL(cueName,"No such XACT cue name");
	//TODO(Play the cue);
	mpSoundBank->Play(cueIdx, 0,0,&pCue);
	return pCue;
}

void CSoundComponent::Apply3D(vector<IXACT3Cue*>& cue, X3DAUDIO_LISTENER& mListener, vector<X3DAUDIO_EMITTER>& mEmitter)
{
	// Set the listener information
	ZeroMemory(&listener,sizeof(listener));
	// set the XYZ of the camera
	listener.Position = mListener.Position;
	// set the direction the camera looks in
	// this must be a unit vector
	listener.OrientFront = mListener.OrientFront;
	// set the up-direction of the camera
	// again, this must be a unit vector and orthogonal to camLook
	listener.OrientTop = mListener.OrientFront;
	// assuming no doppler effect
	listener.Velocity = D3DXVECTOR3(0,0,0);

	// emitter ChannelCount and DSP Setting's SrcChannelCount must match
	for(int i = 0; i<mEmitter.size(); i++)
	{
		mEmitter[i].ChannelCount = dspSettings.SrcChannelCount;
	}

	const float MIN_SOUND_DIST = 3.0f;
	const float MAX_SOUND_DIST = 15.0f;
	// the falloff curve:
	// at range 0=>100%, MIN_SOUND_DIST=>100%, MAX_SOUND_DIST=>0%
	X3DAUDIO_DISTANCE_CURVE_POINT soundFalloffPt[] = { 0.0f, 1.0f, MIN_SOUND_DIST, 1.0f, MAX_SOUND_DIST, 0.0f  };
	X3DAUDIO_DISTANCE_CURVE       soundFalloff          = { &soundFalloffPt[0], 3 };
	
	for(int i = 0; i<mEmitter.size(); i++)
	{
		// set the falloff curve
		mEmitter[i].pVolumeCurve =(X3DAUDIO_DISTANCE_CURVE*)&soundFalloff;
		mEmitter[i].CurveDistanceScaler = 1.0; // this is a multiplier on all distances
		// computer the effects on the sound and apply it
		XACT3DCalculate(xact3dInstance, &listener, &mEmitter[i], &dspSettings );
		if(i < cue.size())
			XACT3DApply( &dspSettings, cue[i]);
		else
			FAILED("Cue not found");
	}
}

void CSoundComponent::StopCue(const char* cueName)
{
	XACTINDEX cueIdx=FindCue(cueName);
	if (cueIdx==XACTINDEX_INVALID)
		FAIL(cueName,"No such XACT cue name");
	//TODO(Stop the cue);
	cueIdx=mpSoundBank->GetCueIndex(cueName);
	mpSoundBank->Stop(cueIdx,0);
}

bool CSoundComponent::IsCuePlaying(const char* cueName)
{
	XACTINDEX cueIdx=FindCue(cueName);
	if (cueIdx==XACTINDEX_INVALID)
		FAIL(cueName,"No such XACT cue name");
	XACT_CUE_PROPERTIES prop;
	mpSoundBank->GetCueProperties(cueIdx,&prop);
	return (prop.currentInstances>0);
}

void CSoundComponent::StopCategory(const char* catName)
{
	XACTCATEGORY cat=FindCategory( catName );
	if (cat==XACTINDEX_INVALID)
		FAIL(catName,"No such XACT category name");
	//TODO(Stop the category);
	mpXEngine->Stop(cat, 0);
}
void CSoundComponent::PauseCategory(bool pause,const char* catName)
{
	XACTCATEGORY cat=FindCategory( catName );
	if (cat==XACTINDEX_INVALID)
		FAIL(catName,"No such XACT category name");
	//TODO(Pause the category);
	mpXEngine->Pause(cat, pause);
}

void CSoundComponent::SetVolume(float volume,const char* catName)
{
	XACTCATEGORY cat=FindCategory( catName );
	if (cat==XACTINDEX_INVALID)
		FAIL(catName,"No such XACT category name");
	//TODO(Set the volume);
	mpXEngine->SetVolume(cat, volume);
}

XACTINDEX CSoundComponent::FindCue(const char* cueName)
{
	XACTINDEX cueIdx = mpSoundBank->GetCueIndex(cueName);
	if(cueIdx == XACTINDEX_INVALID)
		return XACTINDEX_INVALID;
	else
		return cueIdx;
}

XACTCATEGORY CSoundComponent::FindCategory(const char* catName)
{
	//TODO(find the category); return XACTINDEX_INVALID;
	XACTINDEX catIdx = mpXEngine->GetCategory(catName);
	if(catIdx == XACTINDEX_INVALID)
		return XACTINDEX_INVALID;
	else
		return catIdx;
}