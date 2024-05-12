#include "DeltaTime.h"

DeltaTime::DeltaTime()
{
	
}

float DeltaTime::GetDeltaTime()
{
	return m_DeltaTime;
}

void DeltaTime::SetDeltaTime(float deltaT)
{
	m_DeltaTime = deltaT;
}
