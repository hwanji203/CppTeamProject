#include "Pawn.h"
#include "Console.h"
#include "ColliderManager.h"

Pawn::Pawn(Vector2 _pos)
	: Actor(_pos)
	, m_rigidbody(std::make_unique<Rigidbody>(&m_pos))
	, m_collider(std::make_unique<Collider>(&m_pos, 1, this))
{
	m_collider->SetOnCollision([this](Collider* other)
	{
		int tileTop = other->GetTop();    // 타일 y
		if (m_pos.y >= tileTop - 1)        // 발이 타일에 닿거나 박혔으면
		{
			m_pos.y = tileTop - 1;         // 타일 바로 위 칸에 발 붙이기
			m_rigidbody->SetGrounded(true);// 중력 멈춤
		}
	});

	ColliderManager::GetInst()->RegisterCollider(m_collider.get());
}

Pawn::~Pawn()
{
	ColliderManager::GetInst()->UnregisterCollider(m_collider.get());
}

void Pawn::SetVelocity(float velocityX)
{
	m_prevPos = m_pos;
	m_rigidbody->AddForce(velocityX);
}

void Pawn::Tick()
{
	m_rigidbody->SetGrounded(false); // 일단 공중 가정, 충돌 시 다시 true
	m_rigidbody->Tick();
}

void Pawn::Render() const
{
	SetColor(Color::SKYBLUE);
	GotoXY(3, 3);
	cout << m_rigidbody->GetVelocity();

	SetColor(Color::SKYBLUE);
	GotoXY(m_pos.x, m_pos.y);
	cout << "§";
}
