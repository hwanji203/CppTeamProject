#pragma once

constexpr int SCREEN_WIDTH  = 160;  // 콘솔 가로 칸 수.
constexpr int SCREEN_HEIGHT = 23;   // 콘솔 세로 칸 수.
constexpr int FRAME         = 60;   // 목표 FPS(물리 적분/프레임 동기화).

constexpr float KNOCKBACK_Y        = 0.35f;  // 넉백 시 위로 뜨는 수직 세기.
constexpr float KNOCKBACK_CONST    = 4.0f;   // 넉백 수평 세기(이동 반대 방향).
constexpr float KNOCKBACK_MAX      = 1000;   // 넉백 중 속도 상한.
constexpr int   KNOCKBACK_DURATION = 4;      // 넉백 윈도우 길이(프레임).

constexpr float PLAYER_SPEED          = 0.2f;  // 휠 한 칸이 더하는 표시 속도.
constexpr float PLAYER_KEYBOARD_SPEED = 0.2f;  // 키보드 A/D가 프레임마다 더하는 표시 속도.
constexpr float PLAYER_FRICTION       = 0.92f; // 플레이어 마찰(1에 가까울수록 더 미끄러짐).
constexpr int   CHARGE_BOOST_FRAMES   = 13;    // 차징 발사 후 마찰을 멈추는 프레임 수.

// 속도가 이 값보다 작으면 0으로 스냅(미세 드리프트 정지).
// 키보드 한 프레임 힘(PLAYER_KEYBOARD_SPEED * MOVE_SCALE)보다 작아야 입력이 쌓여 움직인다.
constexpr float MOVE_STOP_THRESHOLD = 0.03f;

// 표시(논리) 속도 0~SPEED_MAX. 1단위마다 색 단계가 바뀐다.
// 실제 이동 속도 = 표시 속도 * MOVE_SCALE (표시 값은 유지, 실제 이동만 줄임).
constexpr float SPEED_MAX  = 3.0f;  // 공유 표시 속도 상한(= 색 단계 수).
constexpr float MOVE_SCALE = 0.75f;  // 표시 속도를 실제 이동 속도로 바꾸는 배수(작을수록 느림).

// 적은 색 구간의 정중앙 값(0.5 / 1.5 / 2.5 ...)으로만 스폰된다.
constexpr int   ENEMY_SPEED_BANDS = 3;     // 색 구간(밴드) 수 = 스폰되는 적 종류 수.
constexpr float ENEMY_SPAWN_DELAY     = 2.5f;  // 적 스폰 간격 초기값(초).
constexpr float ENEMY_SPAWN_DELAY_MIN = 0.7f;  // 시간이 지나도 이 밑으론 안 내려가는 간격 하한(초).
constexpr float ENEMY_SPAWN_DELAY_RAMP= 60.f;  // 이 시간(초) 동안 간격이 초기값에서 하한까지 선형 감소.
constexpr int   ENEMY_MAX_ALIVE   = 3;     // 살아있는 적이 이 수 이하일 때만 새로 스폰.

constexpr int PLAYER_MAX_HP = 3;  // 색이 다른 적과 충돌 시 1 감소, 0이면 패배.

constexpr int PLAYER_INVINCIBLE_FRAMES = 60;  // 넉백 후 무적 지속 프레임.
constexpr int PLAYER_BLINK_INTERVAL    = 1;   // 무적 중 깜빡임 간격(프레임).
constexpr int ENEMY_DEATH_FLASH_FRAMES = 0;   // 크리티컬 처치 시 빨갛게 멈추는 프레임.

constexpr float VELOCITY_SHAKE_SCALE       = 2.0f; // 평상시 속도를 진동 세기로 바꾸는 배수.
constexpr float FREEZE_SHAKE_SCALE         = 2.0f; // 차징 중 모은 속도를 진동 세기로 바꾸는 배수.
constexpr float IMPACT_SHAKE_SCALE         = 3.0f; // 넉백 세기를 진동 세기로 바꾸는 배수.
constexpr int   IMPACT_SHAKE_FRAMES        = 12;   // 넉백 진동 지속 프레임.
constexpr float SHAKE_MIN_SPEED            = 0.15f;// 이 속도 미만이면 진동하지 않음.

// 크리티컬 처치 시 차단형(Sleep 기반) 진동: 세기 / 총 지속(ms) / 흔드는 간격(ms).
constexpr int CRIT_SHAKE_INTENSITY = 20;
constexpr int CRIT_SHAKE_DURATION  = 70;
constexpr int CRIT_SHAKE_INTERVAL  = 10;

// 사망 시 차단형(Sleep 기반) 진동: 세기 / 총 지속(ms) / 흔드는 간격(ms).
constexpr int DEATH_SHAKE_INTENSITY = 7;
constexpr int DEATH_SHAKE_DURATION  = 150;
constexpr int DEATH_SHAKE_INTERVAL  = 12;

// ── 아이템 시스템 ─────────────────────────────────────────────
// 스폰 간격은 매번 [MIN, MAX] 초 사이 랜덤. 프레임 기반이라 오버레이 중 자동 정지.
constexpr float ITEM_SPAWN_DELAY_MIN = 5.0f;   // 아이템 최소 스폰 간격(초).
constexpr float ITEM_SPAWN_DELAY_MAX = 7.0f;  // 아이템 최대 스폰 간격(초).

constexpr int ITEM_LIFETIME_FRAMES   = 480;    // 아이템 총 수명(프레임, 60fps=8초).
constexpr int ITEM_BLINK_SLOW_FRAMES = 30;     // 수명 많을 때 깜빡임 주기(느림).
constexpr int ITEM_BLINK_FAST_FRAMES = 6;      // 수명 적을 때 깜빡임 주기(빠름).

constexpr int STAR_DURATION_FRAMES   = 300;    // 별 효과 지속(프레임, 5초).
constexpr int STAR_BLINK_WARN_FRAMES = 60;     // 별 종료 전 깜빡임 시작(프레임, 1초).

constexpr int GUN_DURATION_FRAMES    = 360;    // 총 효과 지속(프레임, 6초).
constexpr int GUN_FIRE_INTERVAL      = 36;     // 총알 발사 간격(프레임, 0.6초).
constexpr int GUN_BLINK_WARN_FRAMES  = 60;     // 총 종료 전 깜빡임 시작(프레임, 1초).

constexpr float BULLET_SPEED = 3.0f;           // 총알 수평 속도(셀/프레임, 플레이어보다 빠름).
