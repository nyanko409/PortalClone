#pragma once

#include "gameObject.h"
#include "rangeshader.h"


class EnemySpawner : public GameObject
{
public:
	EnemySpawner() {}
	~EnemySpawner() {}

	void Init() override;
	void Uninit() override;
	void Update() override;

private:
	float m_nextMoveSpeed;
	float m_spawnRange;
	float m_timeBetweenSpawn;
	float m_timeTillNextSpawn;

	void SpawnEnemy();
};
