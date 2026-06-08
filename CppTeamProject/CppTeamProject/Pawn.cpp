#include "Pawn.h"
#include "Console.h"
#include "ColliderManager.h"

Pawn::Pawn(Vector2 _pos)
	: Actor(_pos)
	, m_rigidbody(std::make_unique<Rigidbody>(&m_pos))
	, m_collider(std::make_unique<Collider>(&m_pos, 1, this))
{
	// �浹 �ݹ� ��� (OnCollisionEnter�� ���� ����)
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

// Ű �Է� �� AddForce�� �о��ֱ� (���� �̵��� �ƴ� �̲������ �̵�)
void Pawn::Move(Dir _dir)
{
	m_prevPos = m_pos;
	switch (_dir)
	{
		case Dir::LEFT:
			m_rigidbody->AddForce(-1.0f);
			break;
		case Dir::RIGHT:
			m_rigidbody->AddForce(1.0f);
			break;
		default:
			break;
	}
}

void Pawn::Tick()
{
	m_rigidbody->Tick();
}

void Pawn::Render() const
{
	SetColor(Color::SKYBLUE);
	GotoXY(m_pos.x * 2, m_pos.y);
	cout << "��";
}
