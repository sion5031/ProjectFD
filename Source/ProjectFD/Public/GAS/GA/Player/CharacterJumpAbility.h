// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "CharacterJumpAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UCharacterJumpAbility : public UFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCharacterJumpAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	UFUNCTION()
	void OnLandedCallback();

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	int32 MaxJumpCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	int32 CurrentJumpCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* DoubleJumpMontage;
};
