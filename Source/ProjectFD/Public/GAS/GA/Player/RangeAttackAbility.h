// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "RangeAttackAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PROJECTFD_API URangeAttackAbility : public UFDGameplayAbility
{
	GENERATED_BODY()

public:
	URangeAttackAbility();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	UFUNCTION()
	void OnEndAbility();
protected:
	//-- 실험 --
	// 라인트레이스 수행 및 데미지 적용
	//void PerformLineTraceAndDamage(const FGameplayAbilityActorInfo* ActorInfo);
	// 클라: 트레이스만 + 서버 RPC, 서버: 트레이스 + 데미지 적용
	void PerformLineTraceAndDamage(const FGameplayAbilityActorInfo* ActorInfo, bool bApplyDamage);
	// 트레이스만 수행(스타트/엔드도 반환하여 디버그 표시)
	bool DoLineTrace(const FGameplayAbilityActorInfo* ActorInfo, FHitResult& OutHit, FVector& OutStart, FVector& OutEnd) const;

	// 서버 측 데미지 적용(팀 판정 포함)
	void ApplyDamageFromHit(const FHitResult& Hit, const ACharacter* SourceCharacter) const;

	UFUNCTION(Server, Reliable)
	void Server_ApplyHit(const FHitResult& Hit);

	// 서버 재검증을 위해 재트레이스할지 여부(치트 방지)
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	bool bServerReTrace = true;

	// -- 실험 끝 --

	static uint8 GetTeamIdFromActorSafe(const AActor* InActor);
	
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack", meta=(ClampMin="100.0", UIMin="100.0"))
	float TraceDistance = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	FName MuzzleSocketName = "FX_Muzzle";
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	TObjectPtr<UAnimMontage> FireMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	TObjectPtr<UAnimMontage> StartMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	bool bDrawDebug = true;
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	TSubclassOf<UDamageType> DamageType;
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	float DefaultDamage = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category = "RangeAttack")
	FGameplayAttribute DamageAttribute;
	UPROPERTY(EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> AmmoGEClass;
	UPROPERTY(Transient)
	bool bIsInputHeld = false;
	UPROPERTY()
	UFDAbilitySystemComponent* ASC;
	UFUNCTION()
	void OnStartMontageCompleted();
	UFUNCTION()
	void OnLineTraceAndDamage(FGameplayEventData Payload);
	
};