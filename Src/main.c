#include "stm32f446xx.h"

// LCD Pins (PA0–PA5)
#define D4 0
#define D5 1
#define D6 2
#define D7 3
#define RS 4
#define EN 5

// Keypad Rows PB0–PB3
#define ROW_A 0
#define ROW_B 1
#define ROW_C 2
#define ROW_D 3

// Keypad Cols PB4–PB6
#define COL_1 4
#define COL_2 5
#define COL_3 6

// Simple delay
void delay(volatile int x){ for(volatile int i=0;i<x;i++); }

// LCD low-level (keeps your PIC-style)
void lcd_pulse(){
    GPIOA->BSRR = (1<<EN);
    delay(2000);
    GPIOA->BSRR = (1<<(EN+16));
    delay(2000);
}
void lcd_nibble(char d){
    GPIOA->ODR &= ~((1<<D4)|(1<<D5)|(1<<D6)|(1<<D7));
    if(d & 0x01) GPIOA->ODR |= (1<<D4);
    if(d & 0x02) GPIOA->ODR |= (1<<D5);
    if(d & 0x04) GPIOA->ODR |= (1<<D6);
    if(d & 0x08) GPIOA->ODR |= (1<<D7);
    lcd_pulse();
}
void lcd_cmd(char c){
    GPIOA->ODR &= ~(1<<RS);
    lcd_nibble(c >> 4);
    lcd_nibble(c & 0x0F);
    delay(2000);
}
void lcd_data(char d){
    GPIOA->ODR |= (1<<RS);
    lcd_nibble(d >> 4);
    lcd_nibble(d & 0x0F);
    delay(2000);
}
void lcd_print(const char *s){
    while(*s) lcd_data(*s++);
}
void lcd_init(){
    delay(50000);
    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

// Map (row,col) -> key char (no function, inline mapping used below)

/* Main */
int main(void){
    // enable GPIOA, GPIOB
    RCC->AHB1ENR |= (1<<0) | (1<<1);

    // PA0..PA5 outputs (LCD)
    GPIOA->MODER &= ~(0x3FFF);
    GPIOA->MODER |=  0x1555;

    // PB0..PB3 outputs (rows)
    GPIOB->MODER &= ~(0xFF);
    GPIOB->MODER |=  0x55;

    // PB4..PB6 inputs with pull-ups (cols)
    GPIOB->MODER &= ~(0x3F << 8);
    GPIOB->PUPDR &=  ~(0x3F << 8);
    GPIOB->PUPDR |=   (0x15 << 8);

    // state variables
    int password = 1234;   // current password
    int input = 0;         // accumulate digits for normal entry
    int new_pass = 0;      // accumulating new password
    int in_change = 0;     // 0 = normal mode, 1 = entering new password
    int digits = 0;        // how many digits entered (for both modes)
    int door_state = 0;    // 0 = closed, 1 = open

    lcd_init();
    lcd_print("Welcome");
    delay(800000);
    lcd_cmd(0x01);
    lcd_print("Password:");
    lcd_cmd(0x89); // set cursor roughly after

    while(1){
        char key = 0;
        int detected_col_pin = 0;

        // scan rows
        for(int row = 0; row < 4; row++){
            // set all rows HIGH then pull current LOW
            GPIOB->ODR |= 0x0F;
            GPIOB->ODR &= ~(1<<row);
            delay(500); // allow lines to settle

            if(!(GPIOB->IDR & (1<<COL_1))){ key = (row==3? '*' : (row==0? '1' : row==1? '4' : '7')); detected_col_pin = COL_1; }
            else if(!(GPIOB->IDR & (1<<COL_2))){ key = (row==3? '0' : (row==0? '2' : row==1? '5' : '8')); detected_col_pin = COL_2; }
            else if(!(GPIOB->IDR & (1<<COL_3))){ key = (row==3? '#' : (row==0? '3' : row==1? '6' : '9')); detected_col_pin = COL_3; }

            if(key) break;
        }

        if(key){
            // debounce + wait release
            delay(20000);
            while(!(GPIOB->IDR & (1<<detected_col_pin)));
            delay(20000);

            // Process single key press
            if(key >= '0' && key <= '9'){
                // print '*' instead of digit
                lcd_data('*');

                // accumulate password numbers
                if(in_change){
                    new_pass = new_pass * 10 + (key - '0');
                } else {
                    input = input * 10 + (key - '0');
                }
                digits++;
                if(digits > 6) digits = 6;   // limit digits
            }

            else if(key == '#'){
                // enter/change password mode
                if(!in_change){
                    lcd_cmd(0x01);
                    lcd_print("Enter New Pass:");
                    lcd_cmd(0xC0);
                    new_pass = 0;
                    digits = 0;
                    in_change = 1;
                } else {
                    if(digits > 0){
                        password = new_pass;
                        lcd_cmd(0x01);
                        lcd_print("Password Saved");
                        delay(80000);
                    }
                    // reset back to normal
                    in_change = 0;
                    new_pass = 0;
                    digits = 0;
                    lcd_cmd(0x01);
                    lcd_print("Password:");
                    lcd_cmd(0x89);
                }
            }

            else if(key == '*'){
                // check password in normal mode
                if(!in_change){
                    lcd_cmd(0xC0); // second line
                    if(input == password && digits>0){
                        if(door_state == 0){
                            lcd_print("OPEN");
                            door_state = 1;
                        } else {
                            lcd_print("CLOSED");
                            door_state = 0;
                        }
                    } else {
                        lcd_print("Wrong Try");
                    }
                    delay(90000);
                    input = 0;
                    digits = 0;
                    lcd_cmd(0x01);
                    lcd_print("Password:");
                    lcd_cmd(0x89);
                }
            }
        }
    }
}

