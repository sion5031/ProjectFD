// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Player/PlayerGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "MeleeAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UMeleeAttackAbility : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMeleeAttackAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	
	UPROPERTY(EditDefaultsOnly, Category="Punch", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PunchActionMontage;

	// 콤보 입력 허용 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Punch")
	float ComboWindowTime = 0.35f;

protected:
	UFUNCTION()
	void OnComplateCallback();

	UFUNCTION()
	void OnInterruptedCallback();

	FName GetNextSection();
	void StartComboTimer();
	void CheckComboInput();
	

	uint8 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool HasNextComboInput = false;

	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix = FString::Printf(TEXT("C"));

	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount = 4;

	// 콤보 애니메이션 재생 속도 (프레임 레이트)
	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate = 30.f;

	// 각 콤보 단계별 유효 프레임 수 (0-기반 인덱스)
	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount = {15.f, 10.f, 10.f, 10.f};
	
};
