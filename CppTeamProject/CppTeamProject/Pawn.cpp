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
		// �浹 �� �ӵ� ���� �� ���� (ƨ��� ����)
		m_rigidbody->SetVelocity(-m_rigidbody->GetVelocity() * 0.5f);
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
