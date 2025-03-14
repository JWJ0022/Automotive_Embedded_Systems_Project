#include "LCD.h"
#include "ASCLIN_UART.h"
#include "GlobalVar.h"
#include <string.h>

#define LCD_COLS 16  // LCD 한 줄의 문자 수

/* 1μs 단위 지연 함수 */
void delay_us (uint32_t microseconds)
{
    uint32 stmFrequency = IfxStm_getFrequency(STM_MODULE); // Hz
    uint32 ticksPerMicrosecond = stmFrequency / 1000000;     // 1us에 해당하는 Tick

    // 현재 STM 하위 32비트 값 읽기
    uint32 startTime = IfxStm_getLower(STM_MODULE);

    // 목표 Tick 계산
    uint32 targetTicks = microseconds * ticksPerMicrosecond;

    // 딜레이 루프
    while ((IfxStm_getLower(STM_MODULE) - startTime) < targetTicks)
    {
        // Busy-wait loop
    }
}

/* Enable 펄스 생성 */
static void pulse_en (void)
{
    IfxPort_setPinHigh(E_PIN);
    delay_us(20); // 최소 450ns 필요
    IfxPort_setPinLow(E_PIN);
    delay_us(20);
}

/* 4비트 데이터 전송 */
static void lcd_send_4bit (char data)
{
    if ((data >> 3) & 0x01 == 1)
    {
        IfxPort_setPinHigh(D7_PIN);
    }
    else
    {
        IfxPort_setPinLow(D7_PIN);
    }

    if ((data >> 2) & 0x01 == 1)
    {
        IfxPort_setPinHigh(D6_PIN);
    }
    else
    {
        IfxPort_setPinLow(D6_PIN);
    }

    if ((data >> 1) & 0x01 == 1)
    {
        IfxPort_setPinHigh(D5_PIN);
    }
    else
    {
        IfxPort_setPinLow(D5_PIN);
    }

    if ((data & 0x01 == 1))
    {
        IfxPort_setPinHigh(D4_PIN);
    }
    else
    {
        IfxPort_setPinLow(D4_PIN);
    }
    pulse_en();
}

/* 8비트 데이터 전송 */
static void lcd_send_8bit (char data)
{
    lcd_send_4bit((data >> 4) & 0x0F); // 상위 4비트 전송
    lcd_send_4bit(data & 0x0F);        // 하위 4비트 전송
}

/* GPIO 초기화 */
static void gpio_init (void)
{
    IfxPort_setPinModeOutput(RS_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(E_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(D4_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(D5_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(D6_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(D7_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
}

void int_to_string (int value, char *buffer)
{
    int isNegative = 0;
    int index = 0;

    // 음수 처리
    if (value < 0)
    {
        isNegative = 1;
        value = -value; // 절대값으로 변환
    }

    // 0 처리
    if (value == 0)
    {
        buffer[index++] = '0';
    }
    else
    {
        // 값 추출 (10진수로 변환)
        while (value > 0)
        {
            buffer[index++] = (value % 10) + '0'; // ASCII 숫자 값으로 변환
            value /= 10;
        }

        // 음수 기호 추가
        if (isNegative)
        {
            buffer[index++] = '-';
        }

        // 문자열 뒤집기
        for (int i = 0; i < index / 2; i++)
        {
            char temp = buffer[i];
            buffer[i] = buffer[index - i - 1];
            buffer[index - i - 1] = temp;
        }
    }

    buffer[index] = '\0'; // 문자열 끝 표시
}

void lcd_send_int (int value)
{
    char buffer[16]; // 정수를 저장할 문자열 버퍼 (최대 16자리)
    int_to_string(value, buffer); // 정수를 문자열로 변환
    lcd_send_string(buffer);      // 변환된 문자열을 LCD에 출력
}

/* LCD 초기화 */
void lcd_init (void)
{
    gpio_init();

    IfxPort_setPinLow(RS_PIN); // 설정 모드
    delay_us(50000);           // 부팅 대기 15ms 이상
    lcd_send_4bit(0x03);
    delay_us(5000);
    lcd_send_4bit(0x03);
    delay_us(1000);
    lcd_send_4bit(0x03);
    delay_us(1000);
    lcd_send_4bit(0x02); // 4비트 모드
    delay_us(1000);
    lcd_send_8bit(0x28); // Function set: 4비트, 2줄, 5x7 도트
    delay_us(1000);
    lcd_send_8bit(0x08); // Display OFF
    delay_us(1000);
    lcd_send_8bit(0x01); // Clear display
    delay_us(1000);
    lcd_send_8bit(0x06); // Entry mode: 증가, 시프트 없음
    delay_us(1000);
    lcd_send_8bit(0x0C); // Display ON, 커서 OFF
    delay_us(1000);
}

/* LCD 커서 설정 */
void lcd_set_cur (int row, int col)
{
    IfxPort_setPinLow(RS_PIN);
    switch (row)
    {
        case 0 :
            col |= 0x80;
            break;
        case 1 :
            col |= 0xC0;
            break;
    }
    lcd_send_8bit((char)col);
}

/* 문자열 출력 */
void lcd_send_string (char *str)
{
    IfxPort_setPinHigh(RS_PIN);
    while (*str)
    {
        lcd_send_8bit(*str++);
    }
}

void asepa (char *str)
{
    int strLength = strlen(str); // 문자열 길이
    int displayPos = 0;          // 현재 디스플레이 시작 위치
    char displayBuffer[LCD_COLS + 1] = {0}; // 화면에 출력할 버퍼

    while (startflag)
    {
        lcd_typewriter_effect("Asepa! *^^*", 100);
        // 현재 디스플레이 위치에서 LCD_COLS만큼 복사
        for (int i = 0; i < LCD_COLS; i++)
        {
            int idx = (displayPos + i) % strLength; // 원형 슬라이딩 효과
            displayBuffer[i] = str[idx];
        }

        // 버퍼 null 종료
        displayBuffer[LCD_COLS] = '\0';

        // 첫 번째 줄 커서를 시작 위치로 설정
        lcd_set_cur(0, 0);

        // 첫 번째 줄로 출력
        lcd_send_string(displayBuffer);

        // 디스플레이 위치 이동
        displayPos = (displayPos + 1) % strLength;

        // 일정 시간 대기 (속도 조절)
        delay_us(200000); // 약 200ms 딜레이 (원하는 속도로 조정 가능)

    }

    lcd_clear_line(0);
    lcd_clear_line(1);
}

void Day6 (char *str)
{
    int strLength = strlen(str); // 문자열 길이
    int displayPos = 0;          // 현재 디스플레이 시작 위치
    char displayBuffer[LCD_COLS + 1] = {0}; // 화면에 출력할 버퍼

    while (SongFlag % 5 == 1)
    {
        lcd_typewriter_effect("Day6!", 100);
        // 현재 디스플레이 위치에서 LCD_COLS만큼 복사
        for (int i = 0; i < LCD_COLS; i++)
        {
            int idx = (displayPos + i) % strLength; // 원형 슬라이딩 효과
            displayBuffer[i] = str[idx];
        }

        // 버퍼 null 종료
        displayBuffer[LCD_COLS] = '\0';

        // 첫 번째 줄 커서를 시작 위치로 설정
        lcd_set_cur(0, 0);

        // 첫 번째 줄로 출력
        lcd_send_string(displayBuffer);

        // 디스플레이 위치 이동
        displayPos = (displayPos + 1) % strLength;

        // 일정 시간 대기 (속도 조절)
        delay_us(200000); // 약 200ms 딜레이 (원하는 속도로 조정 가능)

    }

    lcd_clear_line(0);
    lcd_clear_line(1);
}

void NMIX (char *str, int delayMs)
{
    int strLength = strlen(str); // 문자열 길이
    int displayPos = 0;          // 현재 디스플레이 시작 위치
    char displayBuffer[LCD_COLS + 1] = {0}; // 화면에 출력할 버퍼

    while (SongFlag % 5 == 4)
    {
        lcd_typewriter_effect("NMIX!", 100);
        // 현재 디스플레이 위치에서 LCD_COLS만큼 복사
        for (int i = 0; i < LCD_COLS; i++)
        {
            int idx = (displayPos + i) % strLength; // 원형 슬라이딩 효과
            displayBuffer[i] = str[idx];
        }

        // 버퍼 null 종료
        displayBuffer[LCD_COLS] = '\0';

        // 첫 번째 줄 커서를 시작 위치로 설정
        lcd_set_cur(0, 0);

        // 첫 번째 줄로 출력
        lcd_send_string(displayBuffer);

        // 디스플레이 위치 이동
        displayPos = (displayPos + 1) % strLength;

        // 일정 시간 대기 (속도 조절)
        delay_us(200000); // 약 200ms 딜레이 (원하는 속도로 조정 가능)

    }

    lcd_clear_line(0);
    lcd_clear_line(1);
}

void Day7 (char *str)
{
    int strLength = strlen(str); // 문자열 길이
    int displayPos = 0;          // 현재 디스플레이 시작 위치
    char displayBuffer[LCD_COLS + 1] = {0}; // 화면에 출력할 버퍼

    while (SongFlag % 5 == 2)
    {
        lcd_typewriter_effect("Day6!", 100);
        // 현재 디스플레이 위치에서 LCD_COLS만큼 복사
        for (int i = 0; i < LCD_COLS; i++)
        {
            int idx = (displayPos + i) % strLength; // 원형 슬라이딩 효과
            displayBuffer[i] = str[idx];
        }

        // 버퍼 null 종료
        displayBuffer[LCD_COLS] = '\0';

        // 첫 번째 줄 커서를 시작 위치로 설정
        lcd_set_cur(0, 0);

        // 첫 번째 줄로 출력
        lcd_send_string(displayBuffer);

        // 디스플레이 위치 이동
        displayPos = (displayPos + 1) % strLength;

        // 일정 시간 대기 (속도 조절)
        delay_us(200000); // 약 200ms 딜레이 (원하는 속도로 조정 가능)

    }

    lcd_clear_line(0);
    lcd_clear_line(1);
}

void New (char *str)
{
    int strLength = strlen(str); // 문자열 길이
    int displayPos = 0;          // 현재 디스플레이 시작 위치
    char displayBuffer[LCD_COLS + 1] = {0}; // 화면에 출력할 버퍼

    while (SongFlag % 5 == 3)
    {
        lcd_typewriter_effect("New Jeans! *^^*", 100);
        // 현재 디스플레이 위치에서 LCD_COLS만큼 복사
        for (int i = 0; i < LCD_COLS; i++)
        {
            int idx = (displayPos + i) % strLength; // 원형 슬라이딩 효과
            displayBuffer[i] = str[idx];
        }

        // 버퍼 null 종료
        displayBuffer[LCD_COLS] = '\0';

        // 첫 번째 줄 커서를 시작 위치로 설정
        lcd_set_cur(0, 0);

        // 첫 번째 줄로 출력
        lcd_send_string(displayBuffer);

        // 디스플레이 위치 이동
        displayPos = (displayPos + 1) % strLength;

        // 일정 시간 대기 (속도 조절)
        delay_us(200000); // 약 200ms 딜레이 (원하는 속도로 조정 가능)

    }

    lcd_clear_line(0);
    lcd_clear_line(1);
}


void lcd_scroll_text (char *str)
{
    int strLength = strlen(str); // 문자열 길이
    int displayPos = 0;          // 현재 디스플레이 시작 위치
    char displayBuffer[LCD_COLS + 1] = {0}; // 화면에 출력할 버퍼

    while (1)
    {

        // 현재 디스플레이 위치에서 LCD_COLS만큼 복사
        for (int i = 0; i < LCD_COLS; i++)
        {
            int idx = (displayPos + i) % strLength; // 원형 슬라이딩 효과
            displayBuffer[i] = str[idx];
        }

        // 버퍼 null 종료
        displayBuffer[LCD_COLS] = '\0';

        // 첫 번째 줄 커서를 시작 위치로 설정
        lcd_set_cur(1, 0);

        // 첫 번째 줄로 출력
        lcd_send_string(displayBuffer);

        // 디스플레이 위치 이동
        displayPos = (displayPos + 1) % strLength;

        // 일정 시간 대기 (속도 조절)
        delay_us(500000); // 약 200ms 딜레이 (원하는 속도로 조정 가능)
    }
}

/* LCD 줄 지우기 */
void lcd_clear_line (int row)
{
    lcd_set_cur(row, 0); // 해당 줄의 첫 번째 위치로 이동
    for (int i = 0; i < 16; i++) // 최대 16칸
    {
        lcd_send_string(" "); // 공백 출력
    }
    lcd_set_cur(row, 0); // 다시 시작 위치로 이동
}

void lcd_scroll_horizontal (char *str, int leftToRight, int delayMs)
{
    int strLength = strlen(str);
    char displayBuffer[LCD_COLS + 1] = {0};
    int start = leftToRight ? LCD_COLS - 1 : 0;
    int end = leftToRight ? -1 : strLength;

    for (int pos = start; leftToRight ? pos >= end : pos <= end; leftToRight ? pos-- : pos++)
    {
        memset(displayBuffer, ' ', LCD_COLS); // 초기화 (공백으로 채움)
        for (int i = 0; i < LCD_COLS; i++)
        {
            int idx = leftToRight ? pos + i - LCD_COLS + 1 : pos + i;
            if (idx >= 0 && idx < strLength)
            {
                displayBuffer[i] = str[idx];
            }
        }

        displayBuffer[LCD_COLS] = '\0'; // null 종료
        lcd_set_cur(0, 0);
        lcd_send_string(displayBuffer);
        delay_us(delayMs * 1000); // 딜레이
    }
}

void lcd_blink_text (char *str, int blinkCount, int delayMs)
{
    for (int i = 0; i < blinkCount; i++)
    {
        lcd_set_cur(1, 0);
        lcd_send_string(str); // 텍스트 출력
        delay_us(delayMs * 1000); // 대기
        lcd_clear_line(0); // 텍스트 지움
        delay_us(delayMs * 1000); // 대기
    }
}

void lcd_typewriter_effect (char *str, int delayMs)
{
    lcd_set_cur(1, 0);
    for (int i = 0; i < strlen(str); i++)
    {
        char buffer[2] = {str[i], '\0'}; // 현재 문자 출력
        lcd_send_string(buffer);
        delay_us(delayMs * 1000); // 딜레이
    }
}

void lcd_scroll_vertical (char *str, int delayMs)
{
    char displayBuffer[LCD_COLS + 1] = {0};

    for (int i = 0; i < strlen(str); i++)
    {
        memset(displayBuffer, ' ', LCD_COLS); // 초기화
        for (int j = 0; j <= i && j < LCD_COLS; j++)
        {
            displayBuffer[j] = str[j];
        }
        lcd_set_cur(0, 0);
        lcd_send_string(displayBuffer);
        delay_us(delayMs * 1000); // 딜레이
    }
}

void lcd_expand_effect (char *str, int delayMs)
{
    int strLength = strlen(str);
    char displayBuffer[LCD_COLS + 1] = {0};
    int mid = strLength / 2;

    for (int i = 0; i <= mid; i++)
    {
        int start = mid - i;
        int end = mid + i;

        memset(displayBuffer, ' ', LCD_COLS); // 초기화
        for (int j = start; j <= end && j < strLength; j++)
        {
            if (j >= 0)
            {
                displayBuffer[j] = str[j];
            }
        }

        lcd_set_cur(0, 0);
        lcd_send_string(displayBuffer);
        delay_us(delayMs * 1000); // 딜레이
    }
}

void lcd_wave_effect (char *str, int delayMs)
{
    char displayBuffer[LCD_COLS + 1] = {0};
    int waveHeight = 2;

    for (int phase = 0; phase < LCD_COLS + waveHeight; phase++)
    {
        memset(displayBuffer, ' ', LCD_COLS); // 초기화
        for (int i = 0; i < LCD_COLS; i++)
        {
            int wave = (i + phase) % waveHeight; // 물결 패턴 계산
            if (wave == 0 && i < strlen(str))
            {
                displayBuffer[i] = str[i];
            }
        }

        lcd_set_cur(0, 0);
        lcd_send_string(displayBuffer);
        delay_us(delayMs * 1000); // 딜레이
    }
}

