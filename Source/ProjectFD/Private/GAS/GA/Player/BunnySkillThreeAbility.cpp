// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/BunnySkillThreeAbility.h"


UBunnySkillThreeAbility::UBunnySkillThreeAbility()
{
}

void UBunnySkillThreeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
