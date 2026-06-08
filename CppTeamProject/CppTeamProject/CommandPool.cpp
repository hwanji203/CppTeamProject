#include "CommandPool.h"

CommandPool::CommandPool()
{
	m_velocityCommand[-2] = VelocityCommand(-2);
	m_velocityCommand[-1] = VelocityCommand(-1);
	m_velocityCommand[1] = VelocityCommand(1);
	m_velocityCommand[2] = VelocityCommand(2);
}