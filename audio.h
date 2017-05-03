// audio.c
// Used to output background music and sound effect
// Uses PWM for output and SD for input

#define audioBufferSize 2048

#define playerHurt		0
#define enemyHurt 		1
#define pistol				2
#define enemyDeath		3

void audioInit(void);

//*****playSound*****
// Plays sound effect, return if already playing one
// Input: 0 for player hurt, 1 for enemy hurt, 2 for pistol, 3 for monster death
void playSound(uint8_t sound);

// *****startMusic*****
// Start background music
void startMusic(void);

// *****stopMusic*****
// Stop background music
void stopMusic(void);
