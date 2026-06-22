#pragma once

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 20;
constexpr int FRAME = 60;

constexpr float KNOCKBACK_Y = 0.35f;
constexpr float KNOCKBACK_CONST = 6.0f;
constexpr float KNOCKBACK_MAX = 8;
constexpr int   KNOCKBACK_DURATION = 10;

// 휠/키보드로 더해지는 "표시(논리) 속도" 증감. 표시 값이라 그대로 유지된다.
constexpr float PLAYER_SPEED = 5;
constexpr float PLAYER_KEYBOARD_SPEED = 0.1f;

// --- 공유 속도/색 스케일(플레이어·적 공통) ---
// 표시(논리) 속도: 0 ~ SPEED_MAX. 1단위마다 색 단계(0~1 초, 1~2 노, 2~3 주, 3~ 빨).
// 같은 스케일을 공유하므로 "표시 속도가 같으면 색도 같다".
// 실제 화면 이동 속도 = 표시 속도 * MOVE_SCALE (표시 값은 유지하고 실제 이동만 줄인다).
constexpr float SPEED_MAX  = 4.0f;   // 모두가 공유하는 표시 속도 상한(= 색 단계 수)
constexpr float MOVE_SCALE = 1;    // 표시 속도 → 실제 이동 속도 배수(작을수록 느림)

constexpr float ENEMY_SPEED_MIN = 0.5f;   // 적 표시 속도 랜덤 하한(상한은 SPEED_MAX 공유)

constexpr int PLAYER_INVINCIBLE_FRAMES = 60;
constexpr int PLAYER_BLINK_INTERVAL    = 5;
constexpr int ENEMY_DEATH_FLASH_FRAMES = 15;

constexpr bool  USE_FERRIS_SHAKE      = true;
constexpr float VELOCITY_SHAKE_SCALE  = 2.0f;
constexpr float IMPACT_SHAKE_SCALE    = 3.0f;
constexpr int   IMPACT_SHAKE_FRAMES   = 12;  
constexpr float SHAKE_MIN_SPEED = 0.15f;
constexpr int   DEATH_SHAKE_INTENSITY = 7;   
constexpr int   DEATH_SHAKE_DURATION  = 150; 
constexpr int   DEATH_SHAKE_INTERVAL  = 12;  

constexpr float FREEZE_SHAKE_SCALE    = 2.0f;