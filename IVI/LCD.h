#ifndef LCD_H_
#define LCD_H_

#include "IfxPort.h"
#include "IfxStm.h"
#include <stdint.h>

/* LCD 핀 매핑 */
#define D4_PIN &MODULE_P10, 4
#define D5_PIN &MODULE_P02, 3
#define D6_PIN &MODULE_P02, 4
#define D7_PIN &MODULE_P02, 5
#define RS_PIN &MODULE_P02, 6
#define E_PIN  &MODULE_P02, 7

/* STM 모듈 설정 */
#define STM_MODULE &MODULE_STM1

/* Function Prototypes */
extern void delay_us(uint32_t microseconds);
extern void lcd_init(void);
extern void lcd_set_cur(int row, int col);
extern void lcd_send_string(char *str);
extern void lcd_clear_line(int row);
extern void lcd_scroll_horizontal(char *str, int leftToRight, int delayMs);
extern void lcd_blink_text(char *str, int blinkCount, int delayMs);
extern void lcd_typewriter_effect(char *str, int delayMs);
extern void lcd_scroll_vertical(char *str, int delayMs);
extern void lcd_expand_effect(char *str, int delayMs);
extern void lcd_wave_effect(char *str, int delayMs);

extern void delay_us(uint32_t microseconds);


#endif /* LCD_H_ */
