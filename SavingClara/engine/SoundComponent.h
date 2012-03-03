/*==============================================
 * SoundComponent class for GDEV Engine
 *
 * Written by Marcus
 *==============================================*/
#pragma once

#include <xact3.h>
#include <string>
#include "GameComponent.h"
#include <vector>
#include <vector>
#include <X3DAudio.h>
#include <xact3d3.h>
#include <D3dx9math.h>

using namespace std;
/** The sound component is a game component which
wraps around XACT to play sound and music.

\par References:
* The microsoft XACT tutorials are quite simple to follow
** C:\Program Files\Microsoft DirectX SDK (November 2008)\Samples\C++\XACT\Tutorials\Tut01_PlayCue
** C:\Program Files\Microsoft DirectX SDK (November 2008)\Samples\C++\XACT\Tutorials\Tut03_Variable
* http://msdn.microsoft.com/en-us/library/ee416196%28VS.85%29.aspx
**(or just use the DirectX API documentation)
* http://cs.uccs.edu/~cs525/directx/setup.html
* Using the XACT authoring tool (yes is XNA, but XACT is for C++ and XNA)
** http://www.riemers.net/eng/Tutorials/XNA/Csharp/Series2D/Sound_in_XNA.php
* Compression Settings:
** http://blogs.msdn.com/b/mitchw/archive/2007/04/27/audio-compression-using-xact.aspx
** http://msdn.microsoft.com/en-us/library/ee415947(VS.85).aspx
** http://msdn.microsoft.com/en-us/library/ee415832(v=VS.85).aspx
** http://msdn.microsoft.com/en-us/library/ee415711(v=VS.85).aspx
*/
class CSoundComponent: public CGameComponent
{
public:
	/** Initalises the engine.
	\param engine_xgs the filename of the audio engine (.xgs)
	\param wave_xwb the filename of the wave bank (.xwb)
	\param sound_xsb the filename of the sound bank (.xsb)
	\note creation will FAIL() if unable to find the engine/banks
	*/
	CSoundComponent(const char* engine_xgs,const char* wave_xwb,const char* sound_xsb);
	void Startup();	///< inits all
	void Update(float dt);	///< Update function, call this once per cycle.
	void Shutdown();///< shuts down all

	/// Plays a single sound queue.
	/// \param cueName the cue
	/// Fails if the named cue does not exist
	void PlayCue(const char* cueName);
	IXACT3Cue* Play3DCue(const char* cueName);
	void Apply3D(vector<IXACT3Cue*>& cue, X3DAUDIO_LISTENER& mListener, vector<X3DAUDIO_EMITTER>& mEmitter);
	/// Stops all instances of the playing cue.
	/// \param cueName the cue
	/// Fails if the named cue does not exist
	void StopCue(const char* cueName);
	/// returns if a names cue is playing
	/// \param cueName the cue
	/// Fails if the named cue does not exist
	bool IsCuePlaying(const char* cueName);

	/// Stops all sound in a given catagory
	/// \param catName the category name ("Global" effects all)
	/// Fails if the named category does not exist
	void StopCategory(const char* catName="Global");
	/// Pause/unpause all sound in a given catagory
	/// there is no easy way to determine if the category is paused
	/// \param pause true to pause, false to unpause
	/// \param catName the category name ("Global" effects all)
	/// Fails if the named category does not exist
	void PauseCategory(bool pause,const char* catName="Global");
	/// Sets the sound level for a given catagory.
	/// \param volume the desired volume (usually 0..1, but can be higher)
	/// \param catName the category name ("Global" effects all)
	/// \note there is no Get sound level in XACT (you must keep track of it yourself)
	void SetVolume(float volume,const char* catName="Global");

	//bool InitializeXD3DAudioStructures();

	/// Returns the index of a cue or XACTINDEX_INVALID
	XACTINDEX FindCue(const char* cueName);
	/// Returns the index of a category or XACTCATEGORY_INVALID
	XACTCATEGORY FindCategory(const char* catName);

	// all other functions are done via the XACT directly
	// its easy enough to use, so why waste time wrapping it?
	IXACT3Engine*	GetEngine(){return mpXEngine;}///< direct access to Audio engine
	IXACT3WaveBank*	GetWaveBank(){return mpWaveBank;}///< direct access to wave bank
	IXACT3SoundBank*	GetSoundBank(){return mpSoundBank;}///< direct access to sound bank
private:
	bool InitXACT(const char* engine_xgs);	///< \internal sets up XACT
	bool LoadWaveBank(const char* wave_xwb);	///< \internal loads wavebank
	bool LoadSoundBank(const char* sound_xsb); ///< \internal loads soundbank
	void CloseXACT();	///< close all
private:
	std::string mEngName,mWaveName,mSoundName;	// names of the files
	IXACT3Engine*	mpXEngine;	// the engine
	IXACT3WaveBank*	mpWaveBank;	// the wavebank
	IXACT3SoundBank*	mpSoundBank;	// the sound band

	X3DAUDIO_HANDLE xact3dInstance;
	X3DAUDIO_DSP_SETTINGS dspSettings;
	X3DAUDIO_EMITTER emitter;
    X3DAUDIO_LISTENER listener;
	IXACT3Cue* pCue;


	VOID* mpbWaveBank; // Handle to wave bank data.  Its memory mapped so call UnmapViewOfFile() upon cleanup to release file
	VOID* mpbSoundBank; // Pointer to sound bank data.  Call delete on it when the sound bank is destroyed
};
