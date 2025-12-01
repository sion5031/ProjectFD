// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/CharacterJumpAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GAS/GA/AT/Player/AT_JumpAndWaitForLanding.h"


UCharacterJumpAbility::UCharacterJumpAbility()
{
}

bool UCharacterJumpAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                           const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (!bResult)
	{
		return false;
	}

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	return (Character && Character->CanJump());
}

void UCharacterJumpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->JumpMaxCount = FMath::Max(Character->JumpMaxCount, MaxJumpCount);

	Character->Jump();
	CurrentJumpCount = 1;
	
	UAT_JumpAndWaitForLanding* JumpAndWaitForLandingTask = UAT_JumpAndWaitForLanding::CreateTask(this);
	JumpAndWaitForLandingTask->OnComplete.AddDynamic(this, &UCharacterJumpAbility::OnLandedCallback);
	JumpAndWaitForLandingTask->ReadyForActivation();
}

void UCharacterJumpAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	// 점프 가능하면 처리
	if (CurrentJumpCount < MaxJumpCount)
	{
		// 2번째 점프(이상)부터 전용 몽타주 재생
		if (CurrentJumpCount >= 1 && DoubleJumpMontage)
		{
			// AbilityTask로 안전하게 재생(능력 종료 시 정지)
			if (UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,DoubleJumpMontage,1,NAME_None,true))
			{
				Task->ReadyForActivation();
			}
		}

		Character->Jump();
		++CurrentJumpCount;
	}
}

void UCharacterJumpAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();
}

void UCharacterJumpAbility::OnLandedCallback()
{
	CurrentJumpCount = 0;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}