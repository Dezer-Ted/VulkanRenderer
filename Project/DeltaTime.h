#pragma once
#include <memory>
#include "../../Minigin/Minigin/Engine/Minigin.h"

class DeltaTime {
public:
	DeltaTime();
	DeltaTime(const DeltaTime& time) = delete;
	DeltaTime(DeltaTime&& time) = delete;
	DeltaTime& operator=(const DeltaTime& time) = delete;
	DeltaTime& operator=(DeltaTime&& time) = delete;
	~DeltaTime() = default;
	float GetDeltaTime();
    void SetDeltaTime(float deltaT);
private:
	float m_DeltaTime {};
	friend class dae::Minigin;
	
};
