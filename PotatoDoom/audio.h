// audio.c
// Used to output background music and sound effect
// Uses PWM for output and SD for input

#ifndef _AUDIO_JANKPAD_
#define _AUDIO_JANKPAD_

#define songBufferSize 2048
//#define sfxBufferSize  1024

#define maxAudioChannels 2   // The most number of sound effects we can play at once

// Sound Effect struct
typedef struct
{
  const unsigned char *audioValuesArray;
  uint32_t audioArraySize;
} sfx;

// All the sound effects we can currently play
// Actual byte values for these are in SfxData.h
extern sfx playerHurt;
extern sfx enemyHurt;
extern sfx pistol;
extern sfx enemyDeath;
extern sfx playerDeath;

// One channel of audio struct
typedef struct
{
  boolean playing;
  sfx soundEffect;
  uint32_t counter;
} Channel;

void audioInit(void);

// *****playSFX*****
// Plays sound effect, returns if we have too many sound effects playing
// Input: a sound effect struct
void playSFX(sfx soundEffect);

// *****playSong******
// Plays a song; replaces the current song.
// Input: the name of a song that corresponds to a file on the SD Card
void playSong(char *songName, boolean loop);

// *****toggleMusic*****
// Toggle background music
void toggleMusic(void);

#endif //_AUDIO_JANKPAD_
