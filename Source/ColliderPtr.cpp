#include "ColliderPtr.h"
#include "Physics.h"

void ColliderPtr::SetScale(DirectX::XMVECTOR scale)
{
	m_collider->setLocalScaling(Physics::VecFromDX(scale));
}