// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/FDGameplayStatic.h"
#include "FDAnimInstance.generated.h"

class UFDAbilitySystemComponent;
//enum ECombatState : uint8;
class UCharacterMovementComponent;
/**
 * 
 */

UCLASS()
class PROJECTFD_API UFDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
	ACharacter* OwnerCharacter;
	UPROPERTY()
	UCharacterMovementComponent* OwnerMovementComponent;
	float Speed;
	bool bIsJumping;
	bool bIsSprint;
	bool bIsCombatTag;
	//bool bHasCombatTagCheck;
	//ECombatState CombatState;
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	// void InitializeAbilitySystemComponent();

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsJumping() const { return bIsJumping; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	FORCEINLINE bool IsSprint() const { return bIsSprint; }
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	bool bCombatTag() const { return bIsCombatTag; }
	// UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	// bool bAmingTag() const { return ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerAimingTag()); }

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	float LocomotionDirection;

public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck) const;
/*---------------------------------------------------------*/
	// 태그를 평가해 적합한 무기 레이어로 전환
	UFUNCTION(BlueprintCallable, Category="AnimLayers")
	void RefreshWeaponAnimLayer();

	// 현재 연결된 무기 레이어 클래스
	UFUNCTION(BlueprintPure, Category="AnimLayers")
	TSubclassOf<UAnimInstance> GetCurrentWeaponAnimLayer() const { return CurrentWeaponAnimLayerClass; }

protected:
	// 전환 대상 선택 규칙
	virtual TSubclassOf<UAnimInstance> SelectDesiredWeaponAnimLayerClass() const;

	// 실제 링크 처리
	void ApplyWeaponLayerClass(TSubclassOf<UAnimInstance> DesiredClass);

	// 레이어 인터페이스를 구현한 기본/ 무기 전용 애님 레이어 클래스들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AnimLayers")
	TSubclassOf<UAnimInstance> DefaultWeaponLayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AnimLayers")
	TSubclassOf<UAnimInstance> MGWeaponLayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AnimLayers")
	TSubclassOf<UAnimInstance> LNCWeaponLayerClass;

	// 태그 설정
	// 태그 설정(디자이너가 변경 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AnimLayers|Tags")
	FGameplayTag MGTag = FGameplayTag::RequestGameplayTag(FName("Player.Status.Weapon.MG"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AnimLayers|Tags")
	FGameplayTag LNCTag = FGameplayTag::RequestGameplayTag(FName("Player.Status.Weapon.LNC"));

	// 현재 링크된 레이어(중복 링크 방지)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="AnimLayers")
	TSubclassOf<UAnimInstance> CurrentWeaponAnimLayerClass;
};
