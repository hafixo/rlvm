// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2007 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// -----------------------------------------------------------------------

#ifndef __SoundSystem_hpp__
#define __SoundSystem_hpp__

// -----------------------------------------------------------------------

#include <map>
#include <string>
#include <boost/serialization/split_member.hpp>

// -----------------------------------------------------------------------

class Gameexe;
class RLMachine;

// -----------------------------------------------------------------------

const int NUM_BASE_CHANNELS = 16;
const int NUM_EXTRA_WAVPLAY_CHANNELS = 8;

// -----------------------------------------------------------------------

struct SoundSystemGlobals
{
  SoundSystemGlobals();
  SoundSystemGlobals(Gameexe& gexe);

  /**
   * Number passed in from RealLive that represents what we want the
   * sound system to do. Right now is fairly securely set to 5 since I
   * have no idea how to change this property at runtime.
   *
   * 0 	          	11 k_hz 	          	8 bit
   * 1 	          	11 k_hz 	          	16 bit
   * 2 	          	22 k_hz 	          	8 bit
   * 3 	          	22 k_hz 	          	16 bit
   * 4 	          	44 k_hz 	          	8 bit
   * 5 	          	44 k_hz 	          	16 bit
   * 6 	          	48 k_hz 	          	8 bit
   * 7 	          	48 h_kz 	          	16 bit
   *
   */
  int sound_quality;

  /// Whether music playback is enabled
  bool bgm_enabled;

  /// Volume for the music
  int bgm_volume;

  /// Whether the Wav functions are enabled
  bool pcm_enabled;

  /**
   * Volume of wave files relative to other sound playback.
   */
  int pcm_volume;

  /// Whether the Se functions are enabled
  bool se_enabled;

  /** Volume of interface sound effects relative to other sound
   * playback.
   */
  int se_volume;

  /// boost::serialization support
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & sound_quality & bgm_enabled & bgm_volume & pcm_enabled &
      pcm_volume & se_enabled & se_volume;
  }
};

// -----------------------------------------------------------------------

/**
 *
 */
class SoundSystem
{
public:
  /**
   * Defines a piece of background music who's backed by a file,
   * usually VisualArt's nwa format.
   */
  struct DSTrack {
    DSTrack();
    DSTrack(const std::string name, const std::string file,
            int from, int to, int loop);

    std::string name;
    std::string file;
    int from;
    int to;
    int loop;
  };

  /**
   * Defines a piece of background music who's backed by a cd audio
   * track.
   */
  struct CDTrack {
    CDTrack();
    CDTrack(const std::string name, int from, int to, int loop);

    std::string name;
    int from;
    int to;
    int loop;
  };

protected:
  /// Type for a parsed \#SE table.
  typedef std::map<int, std::pair<std::string, int> > SeTable;

  /// Type for parsed \#DSTRACK entries.
  typedef std::map<std::string, DSTrack> DSTable;

  /// Type for parsed \#CDTRACK entries.
  typedef std::map<std::string, CDTrack> CDTable;

  /**
   * Stores data about an ongoing volume adjustment (such as those
   * started by fun wavSetVolume(int, int, int).)
   */
  struct VolumeAdjustTask {
    VolumeAdjustTask(unsigned int current_time, int in_start_volume,
                     int in_final_volume, int fade_time_in_ms);

    unsigned int start_time;
    unsigned int end_time;

    int start_volume;
    int final_volume;

    /// Calculate the volume for in_time
    int calculateVolumeFor(unsigned int in_time);
  };

  typedef std::map<int, VolumeAdjustTask> ChannelAdjustmentMap;

private:

  /**
   * @name Background Music data
   *
   * @{
   */
  /// Defined music tracks (files)
  DSTable ds_tracks_;

  /// Defined music tracks (cd tracks)
  CDTable cd_tracks_;

  /// @}

  // ---------------------------------------------------------------------

  /**
   * @name PCM/Wave sound effect data
   *
   * @{
   */
  /// Per channel volume
  unsigned char channel_volume_[NUM_BASE_CHANNELS];

  /**
   * Open tasks that adjust the volume of a wave channel. We do this
   * because SDL_mixer doesn't provide this functionality and I'm
   * guessing other mixers don't either.
   *
   * @note Depending on features in other systems, I may push this
   *       down to SDLSoundSystem later.
   */
  ChannelAdjustmentMap pcm_adjustment_tasks_;

  /// @}

  // ---------------------------------------------------------------------

  /**
   * @name Interface sound effect data
   *
   * @{
   */

  /**
   * Parsed \#SE.index entries. Maps a sound effect number to the
   * filename to play and the channel to play it on.
   */
  SeTable se_table_;

  /// @}

  SoundSystemGlobals globals_;

protected:
  SeTable& seTable() { return se_table_; }
  const DSTable& getDSTable() { return ds_tracks_; }
  const CDTable& getCDTable() { return cd_tracks_; }

  /**
   * Computes the actual volume for a channel based on the per channel
   * and the per system volume.
   */
  int computeChannelVolume(const int channel_volume, const int system_volume) {
    return (channel_volume * system_volume) / 255;
  }

  static void checkChannel(int channel, const char* function_name);
  static void checkVolume(int level, const char* function_name);

public:
  SoundSystem(Gameexe& gexe);
  virtual ~SoundSystem();

  /**
   * Gives the sound system a chance to run; done once per game loop.
   *
   * @note Overriders MUST call SoundSystem::execute_sound_system
   *       because we rely on it to handle volume adjustment tasks.
   */
  virtual void executeSoundSystem(RLMachine& machine);

  // ---------------------------------------------------------------------

  /**
   * Sets how much sound hertz.
   */
  virtual void setSoundQuality(const int quality)
  { globals_.sound_quality = quality; }

  int soundQuality() const { return globals_.sound_quality; }

  SoundSystemGlobals& globals() { return globals_; }

  // ---------------------------------------------------------------------
  /**
   * @name BGM functions
   *
   * @{
   */
  virtual void setBgmEnabled(const int in);

  int bgmEnabled() const;
  virtual void setBgmVolume(const int in);
  int bgmVolume() const;

  /**
   * Status of the music subsystem
   *
   * - 0 Idle
   * - 1 Playing music
   * - 2 Fading out music
   */
  virtual int bgmStatus() const = 0;

  virtual void bgmPlay(RLMachine& machine, const std::string& bgm_name, bool loop) = 0;
  virtual void bgmPlay(RLMachine& machine, const std::string& bgm_name, bool loop,
                       int fade_in_ms) = 0;
  virtual void bgmPlay(RLMachine& machine, const std::string& bgm_name, bool loop,
                       int fade_in_ms, int fade_out_ms) = 0;
  virtual void bgmStop() = 0;
  virtual void bgmPause() = 0;
  virtual void bgmUnPause() = 0;
  virtual void bgmFadeOut(int fade_out_ms) = 0;

  virtual std::string bgmName() const = 0;
  /// @}

  // ---------------------------------------------------------------------

  /**
   * @name PCM/Wave functions
   *
   * @{
   */
  /// Sets whether the wav* functions play
  virtual void setPcmEnabled(const int in);

  /// Whether the wav* functions play
  int pcmEnabled() const;

  virtual void setPcmVolume(const int in);
  int pcmVolume() const;

  /// Sets an individual channel volume
  virtual void setChannelVolume(const int channel, const int level);

  /// Change the volume smoothly; the change from the current volume
  /// to level will take fade_time_in_ms
  void setChannelVolume(RLMachine& machine, const int channel,
                        const int level, const int fade_time_in_ms);

  /// Fetches an individual channel volume
  int channelVolume(const int channel);

  virtual void wavPlay(RLMachine& machine, const std::string& wav_file,
                       bool loop) = 0;
  virtual void wavPlay(RLMachine& machine, const std::string& wav_file,
                       bool loop, const int channel) = 0;
  virtual void wavPlay(RLMachine& machine, const std::string& wav_file,
                       bool loop, const int channel, const int fadein_ms) = 0;
  virtual bool wavPlaying(RLMachine& machine, const int channel) = 0;
  virtual void wavStop(const int channel) = 0;
  virtual void wavStopAll() = 0;
  virtual void wavFadeOut(const int channel, const int fadetime) = 0;

  /// @}

  // ---------------------------------------------------------------------

  /**
   * @name Sound Effect functions
   *
   * @{
   */
  /**
   * Sets whether we should have interface sound effects
   */
  virtual void setSeEnabled(const int in);

  /**
   * Returns whether (interface) sound effects are enabled
   */
  int seEnabled() const;

  /**
   * Sets the volume of interface sound effects relative to other
   * sound playback.
   *
   * @param in Sound Effect volume (0-255)
   */
  virtual void setSeVolume(const int in);

  /**
   * Gets the current sound effect volume.
   */
  int seVolume() const;

  /**
   * Plays an interface sound effect.
   *
   * @param se_num Index into the \#SE table
   */
  virtual void playSe(RLMachine& machine, const int se_num) = 0;
  /// @}

  virtual void reset();


  template<class Archive>
  void save(Archive & ar, const unsigned int file_version) const;

  template<class Archive>
  void load(Archive& ar, const unsigned int file_version);

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};	// end of class SoundSystem

#endif
