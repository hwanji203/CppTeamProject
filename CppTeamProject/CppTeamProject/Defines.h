#pragma once

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 20;
constexpr int FRAME = 60;

constexpr float KNOCKBACK_X = 1.2f;        // 수평 넉백 임펄스(초기 속도)
constexpr float KNOCKBACK_Y = 0.35f;       // 수직 넉백 임펄스(위로)
constexpr float KNOCKBACK_MAX = 3.0f;      // 넉백 중 허용 최대 수평 속도(maxSpeed 무시)
constexpr int   KNOCKBACK_DURATION = 10;   // 넉백 1회당 무적/재적용 차단 프레임 수

constexpr float PLAYER_SPEED = 0.25f;
constexpr float PLAYER_KEYBOARD_SPEED = 0.1f;
constexpr float ENEMY_SPEED = 0.5f;