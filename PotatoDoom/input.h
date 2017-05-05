// input.h
// Runs on LM4F120/TM4C123
// input module for PotatoDoom
// Reads data from joysticks and buttons
// Steven Zhu

#define deadZoneStart 120
#define deadZoneEnd		136

#define maxSpeed			5
#define maxAngularSpeed			0.08f

extern Player player;
extern uint8_t shooting;

void inputInit(void);

void updateSpeed(void);

void disableInput(void);

void enableInput(void);
