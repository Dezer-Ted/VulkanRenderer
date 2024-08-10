#pragma once
#include <memory>

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
};
