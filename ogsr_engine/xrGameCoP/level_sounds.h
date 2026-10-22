#pragma once

struct SStaticSound
{
    ref_sound m_Source;
    Ivector2 m_ActiveTime;
    Ivector2 m_PlayTime;
    Ivector2 m_PauseTime;
    u32 m_NextTime;
    u32 m_StopTime;
    Fvector m_Position;
    float m_Volume;
    float m_Freq;

public:
    void Load(IReader& F);
    void Update(u32 gt, u32 rt);
};

// music interface
struct SMusicTrack
{
#ifdef DEBUG
    shared_str m_DbgName;
#endif
    ref_sound m_SourceStereo;
    Ivector2 m_ActiveTime;
    Ivector2 m_PauseTime;
    float m_Volume;

public:
    void Load(LPCSTR fn, LPCSTR params);
    BOOL in(u32 game_time);
    BOOL IsPlaying();
    void Play();
    void Stop();
    void SetVolume(float volume);
};

class CLevelSoundManager
{
    using StaticSoundsVec = xr_vector<SStaticSound>;
    using StaticSoundsVecIt = StaticSoundsVec::iterator;

    StaticSoundsVec m_StaticSounds;

    using MusicTrackVec = xr_vector<SMusicTrack>;
    using MusicTrackVecIt = MusicTrackVec::iterator;

    MusicTrackVec m_MusicTracks;
    u32 m_NextTrackTime{}, m_NextTrackTimeInterval{10000};
    bool TrackTimeIntervalUsePause{true};
    int m_CurrentTrack{-1};

public:
    CLevelSoundManager() = default;
    void Load();
    void Unload();
    void Update();

    void SetMusicInterval(const u32 val) { m_NextTrackTimeInterval = val; }
    u32 GetMusicInterval() const { return m_NextTrackTimeInterval; }

    void SetMusicUsePause(const bool val) { TrackTimeIntervalUsePause = val; }
    bool GetMusicUsePause() const { return TrackTimeIntervalUsePause; }
};
