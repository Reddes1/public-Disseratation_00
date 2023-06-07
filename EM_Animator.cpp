#include "EM_Animator.h"

void EM_Animator::Update(float dTime)
{
	//Skip out if not flagged to play
	if (!m_Data.m_Play)
		return;

	//Determine how to handle the current animation
	//This shouldnt change during a running animation, else undefined behavior
	switch (m_CurrentAnim->m_TypeID)
	{
	case(AnimationData::AnimID::LINEAR_FRAMES):
		LinearAnimation(dTime);
		break;
	case(AnimationData::AnimID::NON_LINEAR_FRAMES):
		NonLinearAnimation(dTime);
		break;
	}	
}

void EM_Animator::SetFrame(int newFrame, bool resetElapsed)
{
	m_Data.m_CurrentFrame = newFrame;
	m_Sprite->SetFrame(m_CurrentAnim->m_FrameIndexes[m_Data.m_CurrentFrame]);
	
	if(resetElapsed)
		m_Data.m_Elapsed = 0.f;
}

void EM_Animator::SetAnimation(int index, bool play, bool loop, bool reverse)
{
	assert(index >= 0 && index <= m_Sprite->GetTexturePtr()->m_Animations.size());

	//Setup shared
	m_CurrentAnim = &m_Sprite->GetTexturePtr()->m_Animations[index];
	m_Data.m_Play = play;
	m_Data.m_Loop = loop;
	m_Data.m_Reverse = reverse;
	m_Data.m_Speed = m_CurrentAnim->m_Speed;

	//Set current frame in accordance to animation type
	switch (m_CurrentAnim->m_TypeID)
	{
	case(AnimationData::AnimID::LINEAR_FRAMES):
		m_Data.m_CurrentFrame = m_CurrentAnim->m_StartFrame;
		break;
	case(AnimationData::AnimID::NON_LINEAR_FRAMES):
		m_Data.m_CurrentFrame = m_CurrentAnim->m_FrameIndexes[0];
		break;
	}

	//Set frame on sprite
	m_Sprite->SetFrame(m_Data.m_CurrentFrame);
}

void EM_Animator::LinearAnimation(float dTime)
{
	//Uptick elapsed time
	m_Data.m_Elapsed += dTime;
	//If elapsed over animation speed
	if (m_Data.m_Elapsed > (1.f / m_Data.m_Speed))
	{
		//Reset the clock
		m_Data.m_Elapsed = 0.f;

		//Determine how to manage the animation based on reverse flag
		switch (m_Data.m_Reverse)
		{
		case(false):
			//Increment the frame count
			++m_Data.m_CurrentFrame;
			//Check if out of animation range
			if (m_Data.m_CurrentFrame > m_CurrentAnim->m_EndFrame)
			{
				//If set to loop, change to first frame, else set frame to end and stop
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = m_CurrentAnim->m_StartFrame;
				else
				{
					m_Data.m_CurrentFrame = m_CurrentAnim->m_EndFrame;
					m_Data.m_Play = false;
				}
			}
			break;

		case(true):
			--m_Data.m_CurrentFrame;
			if (m_Data.m_CurrentFrame < m_CurrentAnim->m_StartFrame)
			{
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = m_CurrentAnim->m_EndFrame;
				else
				{
					m_Data.m_CurrentFrame = m_CurrentAnim->m_StartFrame;
					m_Data.m_Play = false;
				}
			}
			break;
		}


		//Class Sprite Set Frame Here
		m_Sprite->SetFrame(m_Data.m_CurrentFrame);

	}
}

void EM_Animator::NonLinearAnimation(float dTime)
{
	//Uptick elapsed time
	m_Data.m_Elapsed += dTime;

	//If elapsed over animation speed
	if (m_Data.m_Elapsed > (1.f / m_Data.m_Speed))
	{
		//Reset the clock
		m_Data.m_Elapsed = 0.f;

		//Determine how to manage the animation based on reverse flag
		switch (m_Data.m_Reverse)
		{
		case(false):
			//Increment the frame count
			++m_Data.m_CurrentFrame;
			//Check if out of animation range
			if (m_Data.m_CurrentFrame > m_CurrentAnim->m_FrameIndexes.size())
			{
				//If set to loop, change to first frame, else set frame to end and stop
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = m_CurrentAnim->m_FrameIndexes[0];
				else
				{
					m_Data.m_CurrentFrame = static_cast<short int>(m_CurrentAnim->m_FrameIndexes.size());
					m_Data.m_Play = false;
				}
			}
			break;

		case(true):
			--m_Data.m_CurrentFrame;
			if (m_Data.m_CurrentFrame < 0)
			{
				if (m_Data.m_Loop)
					m_Data.m_CurrentFrame = static_cast<short>(m_CurrentAnim->m_FrameIndexes.size());
				else
				{
					m_Data.m_CurrentFrame = m_CurrentAnim->m_FrameIndexes[0];
					m_Data.m_Play = false;
				}
			}
			break;
		}

		//Set frame for owner sprite
		m_Sprite->SetFrame(m_CurrentAnim->m_FrameIndexes[m_Data.m_CurrentFrame]);
	}
}
