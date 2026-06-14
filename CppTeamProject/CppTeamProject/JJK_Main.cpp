// [Rigidbody / Collider 테스트 진입점]
//
// 테스트 방법 (Visual Studio 솔루션 탐색기):
//   main.cpp     우클릭 → 속성 → 빌드에서 제외: 예
//   JJK_Main.cpp 우클릭 → 속성 → 빌드에서 제외: 아니오
//
// 테스트 화면:
//   @@  : 플레이어  (A/D 키로 힘 적용)
//   []  : 박스      (플레이어가 밀면 슬라이드)
//   ||  : 고정 벽   (부딪히면 튕김)
//   충돌 시 해당 오브젝트 빨간색으로 표시

#include "Console.h"
#include "Defines.h"
#include "Rigidbody.h"
#include "Collider.h"
#include "ColliderManager.h"
#include <sstream>
#include <iomanip>
#include <cmath>

static const int FIELD_Y  = 8;
static const int GROUND_Y = 9;
static const int LEFT_X   = 2;
static const int RIGHT_X  = 27;

enum ObjID { ID_PLAYER = 1, ID_BOX, ID_LWALL, ID_RWALL };
static void* ToVoid(ObjID id) { return reinterpret_cast<void*>(static_cast<uintptr_t>(id)); }

int main()
{
    SetCursorVisible(false);
    SetConsoleWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetConsoleWindowStyle(true);
    SetConsoleMouseInputDisabled();

    // ── 플레이어 ──────────────────────────────────────────────
    Vector2  playerPos = { 8, FIELD_Y };
    Rigidbody playerRb(&playerPos, 0.88f, 8.0f);
    Collider  playerCol(&playerPos, 1, ToVoid(ID_PLAYER));

    // ── 밀리는 박스 ───────────────────────────────────────────
    Vector2  boxPos = { 20, FIELD_Y };
    Rigidbody boxRb(&boxPos, 0.88f, 8.0f);
    Collider  boxCol(&boxPos, 1, ToVoid(ID_BOX));

    // ── 고정 벽 ───────────────────────────────────────────────
    Vector2 leftWallPos  = { LEFT_X,  FIELD_Y };
    Vector2 rightWallPos = { RIGHT_X, FIELD_Y };
    Collider leftWallCol (&leftWallPos,  1, ToVoid(ID_LWALL));
    Collider rightWallCol(&rightWallPos, 1, ToVoid(ID_RWALL));

    bool playerHit = false;
    bool boxHit    = false;

    // ── 충돌 콜백 ─────────────────────────────────────────────
    playerCol.SetOnCollision([&](Collider* other)
    {
        playerHit = true;
        float vel = playerRb.GetVelocity();
        ObjID oid = static_cast<ObjID>(reinterpret_cast<uintptr_t>(other->GetOwner()));

        switch (oid)
        {
        case ID_BOX:
            boxRb.AddForce(vel * 0.8f);                             // 박스에 운동량 전달
            playerRb.SetVelocity(-vel * 0.35f);                     // 플레이어 반동
            playerPos.x = (vel >= 0.f) ? boxPos.x - 3 : boxPos.x + 3; // 겹침 제거
            break;
        case ID_LWALL:
            playerRb.SetVelocity(std::fabs(vel) * 0.5f);
            playerPos.x = leftWallPos.x + 3;
            break;
        case ID_RWALL:
            playerRb.SetVelocity(-std::fabs(vel) * 0.5f);
            playerPos.x = rightWallPos.x - 3;
            break;
        default: break;
        }
    });

    boxCol.SetOnCollision([&](Collider* other)
    {
        boxHit    = true;
        float vel = boxRb.GetVelocity();
        ObjID oid = static_cast<ObjID>(reinterpret_cast<uintptr_t>(other->GetOwner()));

        switch (oid)
        {
        case ID_LWALL:
            boxRb.SetVelocity(std::fabs(vel) * 0.5f);
            boxPos.x = leftWallPos.x + 3;
            break;
        case ID_RWALL:
            boxRb.SetVelocity(-std::fabs(vel) * 0.5f);
            boxPos.x = rightWallPos.x - 3;
            break;
        default: break; // 플레이어와의 처리는 playerCol에서 담당
        }
    });

    ColliderManager::GetInst()->RegisterCollider(&playerCol);
    ColliderManager::GetInst()->RegisterCollider(&boxCol);
    ColliderManager::GetInst()->RegisterCollider(&leftWallCol);
    ColliderManager::GetInst()->RegisterCollider(&rightWallCol);

    // ── 메인 루프 ─────────────────────────────────────────────
    while (true)
    {
        UpdateInput();
        if (GetKey(VK_ESCAPE)) break;

        if (GetKey('A'))          playerRb.AddForce(-0.2f);
        if (GetKey('D'))          playerRb.AddForce( 0.2f);
        //if (GetKeyDown(VK_SPACE)) playerRb.StopMovement();

        playerHit = false;
        boxHit    = false;

        playerRb.Tick();
        boxRb.Tick();
        ColliderManager::GetInst()->Update();

        // ── 렌더링 ────────────────────────────────────────────
        SetColor(Color::BLACK);
        for (int y = 0; y < SCREEN_HEIGHT; ++y)
        {
            GotoXY(0, y);
            for (int x = 0; x < SCREEN_WIDTH; ++x)
                cout << ' ';
        }

        // 타이틀
        SetColor(Color::LIGHT_YELLOW);
        GotoXY(0, 0);
        cout << "===== RIGIDBODY / COLLIDER TEST =====";

        // 바닥선
        SetColor(Color::GRAY);
        GotoXY(0, GROUND_Y);
        for (int x = 0; x < SCREEN_WIDTH; ++x) cout << '-';

        // 고정 벽
        SetColor(Color::LIGHT_GRAY);
        GotoXY(leftWallPos.x  * 2, FIELD_Y); cout << "||";
        GotoXY(rightWallPos.x * 2, FIELD_Y); cout << "||";

        // 박스
        SetColor(boxHit ? Color::LIGHT_RED : Color::YELLOW);
        GotoXY(boxPos.x * 2, FIELD_Y);
        cout << "[]";

        // 플레이어
        SetColor(playerHit ? Color::LIGHT_RED : Color::SKYBLUE);
        GotoXY(playerPos.x * 2, FIELD_Y);
        cout << "@@";

        // 스탯
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);

        SetColor(Color::WHITE);
        GotoXY(0, GROUND_Y + 2);
        oss << "Player  X:" << std::setw(3) << playerPos.x
            << "  Vel:" << std::showpos << std::setw(7) << playerRb.GetVelocity() << std::noshowpos
            << (playerHit ? "  [HIT]" : "       ");
        cout << oss.str();
        oss.str("");

        GotoXY(0, GROUND_Y + 3);
        oss << "Box     X:" << std::setw(3) << boxPos.x
            << "  Vel:" << std::showpos << std::setw(7) << boxRb.GetVelocity() << std::noshowpos
            << (boxHit ? "  [HIT]" : "       ");
        cout << oss.str();

        SetColor(Color::LIGHT_GREEN);
        GotoXY(0, SCREEN_HEIGHT - 2);
        cout << "[A/D] Force   [SPACE] Stop   [ESC] Quit";

        FrameSync(60);
    }

    ColliderManager::GetInst()->DestroyInst();
    return 0;
}
