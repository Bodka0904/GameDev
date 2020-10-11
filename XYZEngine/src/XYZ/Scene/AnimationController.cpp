#include "stdafx.h"
#include "AnimationController.h"


namespace XYZ {
	void AnimationController::AddAnimation(const std::string& name, const Ref<Animation>& animation)
	{
		if (!m_CurrentAnimation)
			m_CurrentAnimation = animation;
		State state = m_StateMachine.CreateState(name);
		m_Animations.insert({ state.GetID(), animation });
		m_StatesMap.insert({ name,state.GetID() });
	}
	void AnimationController::RemoveAnimation(const std::string& name)
	{
		auto it = m_StatesMap.find(name);
		if (it != m_StatesMap.end())
		{
			m_Animations.erase(it->second);
			m_StatesMap.erase(it);
		}
	}
	void AnimationController::SetDefaultAnimation(const std::string& name)
	{
		m_CurrentAnimation = m_Animations[m_StatesMap[name]];
	}
	void AnimationController::Update(Timestep ts)
	{
		XYZ_ASSERT(m_CurrentAnimation, "No animation exists");
		m_CurrentAnimation->Update(ts);
	}
	const Ref<Animation>& AnimationController::GetCurrentAnimation() const
	{
		XYZ_ASSERT(m_CurrentAnimation, "No animation exists");
		return m_CurrentAnimation;
	}
}