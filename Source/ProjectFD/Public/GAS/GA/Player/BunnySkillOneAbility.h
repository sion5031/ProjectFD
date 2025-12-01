// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "BunnySkillOneAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UBunnySkillOneAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBunnySkillOneAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UAnimMontage* SkillMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CostGEClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<UGameplayEffect> CooldownGEClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TargetSweepSphereRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CostAmount = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillDamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TargetAmount = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SkillCooldown = 5.f;


	UFUNCTION()
	void StartTargetSweep(FGameplayEventData EventData);
};
