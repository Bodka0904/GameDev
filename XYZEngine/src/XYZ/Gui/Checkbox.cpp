#include "stdafx.h"
#include "Checkbox.h"


namespace XYZ {
	Checkbox::Checkbox(const glm::vec4& hooverColor)
		:
		HooverColor(hooverColor)
	{
		State<CheckboxState::NumStates>& checkedState = Machine.CreateState();
		State<CheckboxState::NumStates>& unCheckedState = Machine.CreateState();
		State<CheckboxState::NumStates>& hooverState = Machine.CreateState();
		State<CheckboxState::NumStates>& unHooverState = Machine.CreateState();

		checkedState.AllowTransition(unCheckedState.GetID());
		unCheckedState.AllowTransition(checkedState.GetID());
		unCheckedState.AllowTransition(hooverState.GetID());
		unCheckedState.AllowTransition(unHooverState.GetID());
		hooverState.AllowTransition(checkedState.GetID());
		hooverState.AllowTransition(unHooverState.GetID());
		hooverState.AllowTransition(hooverState.GetID());
		unHooverState.AllowTransition(hooverState.GetID());
		Machine.SetDefaultState(unHooverState.GetID());
	}
	void Checkbox::OnUpdate(Timestep ts)
	{
		if (Checked)
			Execute(CheckedEvent{});
	}
}