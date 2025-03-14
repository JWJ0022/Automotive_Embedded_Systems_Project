#ifndef MEMORYSHARED_H
#define MEMORYSHARED_H

#include <stdint.h>

#define SHARED_MEMORY_BASE 0x70000000 // 공유 메모리 주소 (SRAM)
#define LCD_COLS 16                  // LCD의 열 수

// 공유 변수 선언 (정의가 아님)
extern volatile uint16_t *sharedTrackNumber; // 공유 트랙 번호
extern volatile int titleUpdated;           // 제목 갱신 플래그

// 트랙 번호와 제목 매핑
extern const char *trackTitleMap[];

#endif // MEMORYSHARED_H
