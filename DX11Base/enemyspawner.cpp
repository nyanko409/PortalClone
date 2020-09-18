#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "main.h"
#include "model.h"
#include "enemyspawner.h"
#include "enemy.h"
#include "player.h"


void EnemySpawner::Init()
{
	GameObject::Init();

	m_timeBetweenSpawn = 5;
	m_timeTillNextSpawn = 0;
	m_nextMoveSpeed = 0.05F;
	m_spawnRange = 20;
}

void EnemySpawner::Uninit()
{
	GameObject::Uninit();
}

void EnemySpawner::Update()
{
	GameObject::Update();

	// spawn when ready
	m_timeTillNextSpawn += TIME_PER_FRAME;
	if (m_timeTillNextSpawn >= m_timeBetweenSpawn)
	{
		m_timeTillNextSpawn -= m_timeBetweenSpawn;
		SpawnEnemy();
	}
}

void EnemySpawner::SpawnEnemy()
{
	// check for player
	auto player = CManager::GetActiveScene()->GetGameObjects<Player>(0);
	if (player.empty())
		return;

	// calculate the spawn position of enemy
	dx::XMVECTOR origin = player.front()->GetPosition();

	dx::XMVECTOR direction = dx::XMVectorZero();
	direction = dx::XMVectorSetX(direction, ((rand() % 2001) / 1000.0F) - 1.0F);
	direction = dx::XMVectorSetZ(direction, ((rand() % 2001) / 1000.0F) - 1.0F);
	direction = dx::XMVector3Normalize(direction);
	direction = dx::XMVectorScale(direction, m_spawnRange);
	direction = dx::XMVectorAdd(direction, origin);
	direction = dx::XMVectorSetY(direction, 1);

	// spawn the enemy at calculated position
	auto enemy = CManager::GetActiveScene()->AddGameObject<Enemy>(1);
	enemy->SetPosition(direction);
	enemy->SetMoveSpeed(m_nextMoveSpeed);

	// increase the difficulty
	m_nextMoveSpeed += 0.01F;
	m_spawnRange += 2;
	
}
