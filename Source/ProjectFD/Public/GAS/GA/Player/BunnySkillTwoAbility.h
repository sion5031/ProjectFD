// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnySkillTwoAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnySkillTwoAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()

public:
	UBunnySkillTwoAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> GrantTagGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> CostGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CostAmount = 1.f;

protected:
	// 마나가 부족할 떄 호출될 함수
	UFUNCTION()
	void OnEndAbility();
};
