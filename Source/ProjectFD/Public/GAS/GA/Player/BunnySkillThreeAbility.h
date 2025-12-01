// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnySkillThreeAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnySkillThreeAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBunnySkillThreeAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	
	
};
