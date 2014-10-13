#ifdef __cplusplus
extern "C" {
#endif 

#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6

extern char b1,b2,b3;

#define PUSHED 		0
#define NOT_PUSHED 	1

void Button1_isr();
void Button2_isr();
void Button3_isr();

void button_init();
void button_read();
void button_Timeslice();


#ifdef __cplusplus
 }
#endif 
