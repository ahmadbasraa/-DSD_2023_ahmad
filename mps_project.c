#include "TM4C123.h"
#include <stdint.h>
#include <string.h>

void SystemInit(void) {
#if (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
#endif
}

// --- REGISTERS ---
#define SYSCTL_RCGCGPIO_R     (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCUART_R     (*((volatile unsigned long *)0x400FE618)) 

// UART7 (Speaker/MP3) - Pins PE0, PE1                                    // Page 904 for UART section 
#define UART7_DR_R            (*((volatile unsigned long *)0x40013000))                  
#define UART7_FR_R            (*((volatile unsigned long *)0x40013018))
#define UART7_IBRD_R          (*((volatile unsigned long *)0x40013024))
#define UART7_FBRD_R          (*((volatile unsigned long *)0x40013028))
#define UART7_LCRH_R          (*((volatile unsigned long *)0x4001302C))
#define UART7_CTL_R           (*((volatile unsigned long *)0x40013030))
#define UART7_CC_R            (*((volatile unsigned long *)0x40013FC8))

// ADC REGISTERS (LDR)
#define SYSCTL_RCGCADC_R      (*((volatile unsigned long *)0x400FE638))
#define ADC0_ACTSS_R          (*((volatile unsigned long *)0x40038000))
#define ADC0_EMUX_R           (*((volatile unsigned long *)0x40038014))
#define ADC0_SSMUX3_R         (*((volatile unsigned long *)0x400380A0))
#define ADC0_SSCTL3_R         (*((volatile unsigned long *)0x400380A4))
#define ADC0_PSSI_R           (*((volatile unsigned long *)0x40038028))
#define ADC0_RIS_R            (*((volatile unsigned long *)0x40038004))
#define ADC0_ISC_R            (*((volatile unsigned long *)0x4003800C))
#define ADC0_SSFIFO3_R        (*((volatile unsigned long *)0x400380A8))
#define ADC0_PC_R             (*((volatile unsigned long *)0x40038FC4))

// PORT A (Keypad Cols: PA5, PA6, PA7)
#define GPIO_PORTA_DIR_R      (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_DEN_R      (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AFSEL_R    (*((volatile unsigned long *)0x40004420)) 
#define GPIO_PORTA_AMSEL_R    (*((volatile unsigned long *)0x40004528)) 
#define GPIO_PORTA_PIN5       (*((volatile unsigned long *)0x40004080)) 
#define GPIO_PORTA_PIN6       (*((volatile unsigned long *)0x40004100)) 
#define GPIO_PORTA_PIN7       (*((volatile unsigned long *)0x40004200)) 

// PORT B (LCD Data)
#define GPIO_PORTB_DATA_R     (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R      (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_DEN_R      (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AFSEL_R    (*((volatile unsigned long *)0x40005420))

// PORT C (Keypad Rows)
#define GPIO_PORTC_DATA_R     (*((volatile unsigned long *)0x400063FC))
#define GPIO_PORTC_DIR_R      (*((volatile unsigned long *)0x40006400))
#define GPIO_PORTC_DEN_R      (*((volatile unsigned long *)0x4000651C))
#define GPIO_PORTC_PDR_R      (*((volatile unsigned long *)0x40006514)) 

// PORT E (UART7 + ADC + LCD Control)
#define LCD_CONTROL_PORT      (*((volatile unsigned long *)0x400240E0)) // PE3-PE5
#define GPIO_PORTE_DIR_R      (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_DEN_R      (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AFSEL_R    (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_AMSEL_R    (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R     (*((volatile unsigned long *)0x4002452C)) 

// PORT F (Keypad Col 1 + Buzzer + Servo)
#define GPIO_PORTF_DIR_R      (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_DEN_R      (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_PIN1       (*((volatile unsigned long *)0x40025008)) // Col 1
#define GPIO_PORTF_PIN2       (*((volatile unsigned long *)0x40025010)) // Buzzer
#define SERVO_PIN             (*((volatile unsigned long *)0x40025020)) // PF3 Servo

// --- DEFINITIONS ---
#define LCD_E_PIN   0x08 
#define LCD_RW_PIN  0x10 
#define LCD_RS_PIN  0x20 
#define LCD_DATA_BUS    GPIO_PORTB_DATA_R
#define LCD_CONTROL_BUS LCD_CONTROL_PORT

#define ROW0 (GPIO_PORTC_DATA_R & 0x80) 
#define ROW1 (GPIO_PORTC_DATA_R & 0x40) 
#define ROW2 (GPIO_PORTC_DATA_R & 0x20) 
#define ROW3 (GPIO_PORTC_DATA_R & 0x10) 

// --- LDR THRESHOLDS ---
#define LDR_THRESHOLD_BRIGHT 300
#define LDR_THRESHOLD_DARK   290

// --- PROTOTYPES ---
void Init_Hardware(void);
void ADC0_Init(void);
unsigned long ADC0_Read(void);
void UART7_Init(void);
void UART7_WriteChar(unsigned char data);
void MP3_Cmd(uint8_t command, uint16_t parameter);

// Sound Helper Functions
void Play_Welcome(void);        // Track 001
void Play_DoorLocked(void);     // Track 002
void Play_WrongPass(void);      // Track 003
void Play_SecurityAlert(void);  // Track 004

void LCD_Init(void);
void LCD_Clear(void);
void LCD_Send_Command(unsigned char command);
void LCD_Send_Data(unsigned char data);
void LCD_Send_String(char *ptr);
void LCD_Goto_XY(unsigned char x, unsigned char y);
unsigned char Scan_Keypad(void);

void Delay_us(unsigned long counter);
void Delay_ms(unsigned long milliseconds);
void Buzzer_Beep(int count, int duration_ms);

void Servo_Go_28_Degrees(void);
void Go_0_Degrees(void);

// --- MAIN FUNCTION ---
int main(void)
{
    // --- CONFIGURATION ---
    char correct_pass[] = "123A";  
    int pass_len = strlen(correct_pass); 
    char input_buffer[15] = {0}; 
    
    int count = 0;
    int wrong_attempts = 0; // New variable to track wrong tries
    unsigned char key = 0;
    unsigned long light_level = 0;
    int i = 0;

    Init_Hardware();
    UART7_Init(); 
    ADC0_Init();  
    LCD_Init();
    LCD_Clear();
    
    // --- STARTUP DELAY ---
    for(i = 0; i < 4000000; i++) { }

    
    
    LCD_Send_String("System Ready");
		Delay_ms(400);
    Servo_Go_28_Degrees(); // Locked
    
		Delay_ms(500);
    
    LCD_Clear();
    LCD_Send_String("Enter Pass:");
    LCD_Goto_XY(1, 2); 
        
    while(1)
    {
        key = Scan_Keypad();
        
        if (key != 0) 
        {
            input_buffer[count] = key;
            count++;
            
            Buzzer_Beep(1, 50); 
            LCD_Send_Data('*'); 
            Delay_ms(300);      
            
            if (count == pass_len)
            {
                Delay_ms(200); 
                LCD_Clear();
                input_buffer[count] = '\0'; 

                if (strcmp(input_buffer, correct_pass) == 0)
                {
                    // --- CORRECT PASS ---
                    wrong_attempts = 0; // Reset counter
                    LCD_Send_String("Correct!");
									  MP3_Cmd(0x06, 25); // Set Volume
                    Play_Welcome(); // Track 001
                    
                    LCD_Goto_XY(1, 2);
                    LCD_Send_String("Opening...");
									  Delay_ms(4000);
                    Go_0_Degrees(); // OPEN
                    
                    Delay_ms(100); 
                    LCD_Clear();
                    LCD_Send_String("Waiting Light...");
                    
                    // 1. WAIT FOR LIGHT
                    while (light_level < LDR_THRESHOLD_BRIGHT){
                        light_level = ADC0_Read();
                        Delay_ms(100);
                    } 
                    
                    LCD_Clear();
                    LCD_Send_String("Ready to Lock");
                    LCD_Goto_XY(1, 2);
                    LCD_Send_String("Waiting Dark...");

                    // 2. WAIT FOR DARKNESS
                     while (light_level > LDR_THRESHOLD_DARK){
                        light_level = ADC0_Read();
                        Delay_ms(100);
                    } 

                    // 3. LOCK THE DOOR
                    LCD_Clear();
                    LCD_Send_String("Closing...");
										Play_DoorLocked();
										Delay_ms(5500);
                    Servo_Go_28_Degrees(); // CLOSE
                    
                    // --- NEW: Play Track 002 after locking ---
                    Delay_ms(500); // Small delay to let servo finish
                    
                }
                else 
                {
                    // --- WRONG PASS ---
                    wrong_attempts++; // Increment counter
                    
                    if(wrong_attempts >= 3) {
                        LCD_Send_String("SECURITY ALERT!");
											  MP3_Cmd(0x06, 100); // Set Volume
                        Play_SecurityAlert(); // Track 004
                        Buzzer_Beep(5, 100); // 5 fast beeps
                    } else {
                        LCD_Send_String("Wrong Pass!");
                        Play_WrongPass(); // Track 003
                        Buzzer_Beep(3, 100); // 3 beeps
                    }
                    
                    Delay_ms(1500);
                }
                
                // --- RESET ---
                count = 0;
                memset(input_buffer, 0, sizeof(input_buffer));
                LCD_Clear();
                LCD_Send_String("Enter Pass:");
                LCD_Goto_XY(1, 2);
            }
        }
        Delay_ms(20);
    }
}

// --- HARDWARE INIT ---
void Init_Hardware(void)
{
    volatile unsigned long delay;
    SYSCTL_RCGCGPIO_R |= 0x3F; 
    delay = SYSCTL_RCGCGPIO_R;

    // PORT A
    GPIO_PORTA_AMSEL_R &= ~0xE0; 
    GPIO_PORTA_AFSEL_R &= ~0xE0; 
    GPIO_PORTA_DIR_R |= 0xE0;    
    GPIO_PORTA_DEN_R |= 0xE0;     

    // PORT B
    GPIO_PORTB_DIR_R |= 0xFF;    
    GPIO_PORTB_DEN_R |= 0xFF; 
    GPIO_PORTB_AFSEL_R = 0;

    // PORT C
    GPIO_PORTC_DIR_R &= ~0xF0;   
    GPIO_PORTC_DEN_R |= 0xF0; 
    GPIO_PORTC_PDR_R |= 0xF0;    
    
    // PORT E
    GPIO_PORTE_AMSEL_R &= ~0x38;   
    GPIO_PORTE_DIR_R |= 0x38;  
    GPIO_PORTE_DEN_R |= 0x38;    
    GPIO_PORTE_AFSEL_R &= ~0x38;

    // PORT F
    GPIO_PORTF_DIR_R |= 0x0E; 
    GPIO_PORTF_DEN_R |= 0x0E;
}

// --- KEYPAD SCANNING ---
unsigned char Scan_Keypad(void)
{
    GPIO_PORTF_PIN1 = 0; GPIO_PORTA_PIN6 = 0;
    GPIO_PORTA_PIN7 = 0; GPIO_PORTA_PIN5 = 0;

    // Col 1 (PF1)
    GPIO_PORTF_PIN1 = 0x02; 
    if (ROW0) { while(ROW0); return '1'; }
    if (ROW1) { while(ROW1); return '4'; }
    if (ROW2) { while(ROW2); return '7'; }
    if (ROW3) { while(ROW3); return '*'; }
    GPIO_PORTF_PIN1 = 0; 

    // Col 2 (PA6)
    GPIO_PORTA_PIN6 = 0x40; 
    if (ROW0) { while(ROW0); return '2'; }
    if (ROW1) { while(ROW1); return '5'; }
    if (ROW2) { while(ROW2); return '8'; }
    if (ROW3) { while(ROW3); return '0'; }
    GPIO_PORTA_PIN6 = 0; 

    // Col 3 (PA7)
    GPIO_PORTA_PIN7 = 0x80; 
    if (ROW0) { while(ROW0); return '3'; }
    if (ROW1) { while(ROW1); return '6'; }
    if (ROW2) { while(ROW2); return '9'; }
    if (ROW3) { while(ROW3); return '#'; }
    GPIO_PORTA_PIN7 = 0; 

    // Col 4 (PA5)
    GPIO_PORTA_PIN5 = 0x20; 
    if (ROW0) { while(ROW0); return 'A'; }
    if (ROW1) { while(ROW1); return 'B'; }
    if (ROW2) { while(ROW2); return 'C'; }
    if (ROW3) { while(ROW3); return 'D'; }
    GPIO_PORTA_PIN5 = 0; 

    return 0; 
}

// --- ADC FUNCTIONS ---
void ADC0_Init(void) {
    volatile unsigned long delay;
    SYSCTL_RCGCADC_R |= 0x01;  
    delay = SYSCTL_RCGCGPIO_R; 
    
    GPIO_PORTE_AFSEL_R |= 0x04; 
    GPIO_PORTE_DEN_R &= ~0x04;  
    GPIO_PORTE_AMSEL_R |= 0x04; 
    
    ADC0_ACTSS_R &= ~0x0008;        
    ADC0_EMUX_R &= ~0xF000;         
    ADC0_SSMUX3_R = 1;             
    ADC0_SSCTL3_R = 0x0006;        
    ADC0_ACTSS_R |= 0x0008;        
}

unsigned long ADC0_Read(void) {
    unsigned long result;
    ADC0_PSSI_R = 0x0008;           
    while((ADC0_RIS_R & 0x08) == 0){}; 
    result = ADC0_SSFIFO3_R & 0xFFF; 
    ADC0_ISC_R = 0x0008;            
    return result;
}

// --- UART7 FUNCTIONS ---
volatile unsigned long delay;
void UART7_Init(void) {
    SYSCTL_RCGCUART_R |= (1<<7);
    delay = SYSCTL_RCGCUART_R;
    
    GPIO_PORTE_AFSEL_R |= 0x03; 
    GPIO_PORTE_DEN_R   |= 0x03; 
    GPIO_PORTE_PCTL_R  &= ~0x000000FF; 
    GPIO_PORTE_PCTL_R  |=  0x00000011; 
    
    UART7_CTL_R &= ~0x01;        
    UART7_IBRD_R = 104; UART7_FBRD_R = 11; 
    UART7_LCRH_R = 0x70; 
    UART7_CC_R = 0;
    UART7_CTL_R |= 0x01;        
}

void UART7_WriteChar(unsigned char data) {
    while((UART7_FR_R & 0x20) != 0); 
    UART7_DR_R = data;
}

void MP3_Cmd(uint8_t command, uint16_t parameter) {
    uint8_t packet[10];
    uint16_t checksum;
    uint8_t i;
    packet[0] = 0x7E; packet[1] = 0xFF; packet[2] = 0x06; 
    packet[3] = command; packet[4] = 0x00; 
    packet[5] = (uint8_t)(parameter >> 8); 
    packet[6] = (uint8_t)(parameter);        
    checksum = 0xFFFF - (packet[1] + packet[2] + packet[3] + packet[4] + packet[5] + packet[6]) + 1;
    packet[7] = (uint8_t)(checksum >> 8); packet[8] = (uint8_t)(checksum);        
    packet[9] = 0xEF; 
    for(i = 0; i < 10; i++) UART7_WriteChar(packet[i]);
}

// --- SOUND HANDLERS (UPDATED) ---
void Play_Welcome(void) {
    MP3_Cmd(0x0F, 0x0101); // Play Folder 01, File 001
}
void Play_DoorLocked(void) {
    MP3_Cmd(0x0F, 0x0102); // Play Folder 01, File 002 (Door Closed)
}
void Play_WrongPass(void) {
    MP3_Cmd(0x0F, 0x0103); // Play Folder 01, File 003 (Wrong Pass)
}
void Play_SecurityAlert(void) {
    MP3_Cmd(0x0F, 0x0104); // Play Folder 01, File 004 (Alert)
}

// --- SERVO & BUZZER ---
void Go_0_Degrees(void) {
    int i;
    for(i = 0; i < 50; i++) {
        SERVO_PIN = 0x08; Delay_us(755); 
			  SERVO_PIN = 0x00; Delay_us(19245);
    }
}
void Servo_Go_28_Degrees(void) {
    int i;
    for(i = 0; i < 50; i++) {
        SERVO_PIN = 0x08; Delay_us(1003);  //28 degrees because required rotation = 755 + (chosen angle * 7.77)
			  SERVO_PIN = 0x00; Delay_us(18997);
    }
}
void Buzzer_Beep(int count, int duration_ms) {
    int i;
    for(i=0; i<count; i++) {
        GPIO_PORTF_PIN2 = 0x04; Delay_ms(duration_ms);
        GPIO_PORTF_PIN2 = 0x00; Delay_ms(duration_ms);         
    }
}

// --- LCD DRIVERS ---
void LCD_Send_Command(unsigned char command) {
    LCD_DATA_BUS = command; LCD_CONTROL_BUS = 0; Delay_us(100);
    LCD_CONTROL_BUS |= LCD_E_PIN; Delay_us(100); LCD_CONTROL_BUS &= ~LCD_E_PIN; Delay_us(2000); 
}
void LCD_Send_Data(unsigned char data) {
    LCD_DATA_BUS = data; LCD_CONTROL_BUS = LCD_RS_PIN; Delay_us(100);
    LCD_CONTROL_BUS |= LCD_E_PIN; Delay_us(100); LCD_CONTROL_BUS &= ~LCD_E_PIN; Delay_us(2000); 
}
void LCD_Init(void) {
    LCD_CONTROL_BUS = 0; Delay_ms(200); 
    LCD_Send_Command(0x38); LCD_Send_Command(0x0C); LCD_Send_Command(0x06); LCD_Send_Command(0x01); Delay_ms(20);
}
void LCD_Clear(void) { LCD_Send_Command(0x01); Delay_ms(20); }
void LCD_Send_String(char *ptr) { while (*ptr) LCD_Send_Data(*ptr++); }
void LCD_Goto_XY(unsigned char x, unsigned char y) {
    unsigned char row_start[] = {0x80, 0xC0};
    if (y >= 1 && y <= 2 && x >= 1 && x <= 16) { LCD_Send_Command(row_start[y-1] + (x-1)); Delay_us(200); }
}

// --- DELAYS ---
void Delay_us(unsigned long counter) {
    unsigned long i, j;
    for (i = 0; i < counter; i++) for (j = 0; j < 3; j++); 
}
void Delay_ms(unsigned long milliseconds) {
    unsigned long i;
    for(i = 0; i < milliseconds; i++) Delay_us(1000); 
}
