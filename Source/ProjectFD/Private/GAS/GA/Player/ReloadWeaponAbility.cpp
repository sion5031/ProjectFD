// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/ReloadWeaponAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Equipment/EquipmentActor.h"
#include "GameFramework/Character.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/Player/PlayerAttributeSet.h"


UReloadWeaponAbility::UReloadWeaponAbility()
{
}

void UReloadWeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	ASC = Cast<UFDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	if (PlayerAttrSet->GetGeneralAmmo() <= 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("소지하고 있는 탄약이 없습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	// 장착 무기 찾기(예시: 부착 액터에서 탐색)
	AEquipmentActor* Equipment = nullptr;
	{
		TArray<AActor*> Attached;
		Character->GetAttachedActors(Attached);
		for (AActor* A : Attached)
		{
			if ((Equipment = Cast<AEquipmentActor>(A)) != nullptr)
			{
				break;
			}
		}
	}

	UAnimMontage* ReloadMontage = Equipment ? Equipment->GetReloadMontage() : nullptr;
	if (!ReloadMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReloadMontage가 유효하지 않습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ReloadMontage);
		PlaySkillMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlaySkillMontageTask->ReadyForActivation();
	}

	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.Weapon.Reload")), nullptr, false, true);
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageEnded);
		WaitEventTask->ReadyForActivation();
	}

	// float CurrentAmmo = PlayerAttrSet->GetAmmo();
	// float MaxAmmo = PlayerAttrSet->GetMaxAmmo();
	// float GeneralAmmo = PlayerAttrSet->GetGeneralAmmo();
	//
	// FGameplayAttribute GeneralAmmoAttribute = UPlayerAttributeSet::GetGeneralAmmoAttribute();
	// FGameplayAttribute CurrentAmmoAttribute = UPlayerAttributeSet::GetAmmoAttribute();
	// if ( GeneralAmmo < CurrentAmmo && (GeneralAmmo + CurrentAmmo) > MaxAmmo )
	// {
	// 	ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Additive, MaxAmmo - CurrentAmmo);
	// 	ASC->ApplyModToAttributeUnsafe(GeneralAmmoAttribute, EGameplayModOp::Override, GeneralAmmo - (MaxAmmo - CurrentAmmo));
	// }
	// else if ( (GeneralAmmo + CurrentAmmo) <= MaxAmmo )
	// {
	// 	ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Additive, GeneralAmmo);
	// 	ASC->ApplyModToAttributeUnsafe(GeneralAmmoAttribute, EGameplayModOp::Additive, -GeneralAmmo);
	// }
	// else
	// {
	// 	ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Override, MaxAmmo);
	// 	ASC->ApplyModToAttributeUnsafe(GeneralAmmoAttribute, EGameplayModOp::Additive, CurrentAmmo - MaxAmmo);
	// }
}

void UReloadWeaponAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UReloadWeaponAbility::OnMontageEnded(FGameplayEventData Payload)
{
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	float CurrentAmmo = PlayerAttrSet->GetAmmo();
	float MaxAmmo = PlayerAttrSet->GetMaxAmmo();
	float GeneralAmmo = PlayerAttrSet->GetGeneralAmmo();
	
	FGameplayAttribute GeneralAmmoAttribute = UPlayerAttributeSet::GetGeneralAmmoAttribute();
	FGameplayAttribute CurrentAmmoAttribute = UPlayerAttributeSet::GetAmmoAttribute();
	if ( GeneralAmmo < CurrentAmmo && (GeneralAmmo + CurrentAmmo) > MaxAmmo )
	{
		ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Additive, MaxAmmo - CurrentAmmo);
		ASC->ApplyModToAttributeUnsafe(GeneralAmmoAttribute, EGameplayModOp::Override, GeneralAmmo - (MaxAmmo - CurrentAmmo));
	}
	else if ( (GeneralAmmo + CurrentAmmo) <= MaxAmmo )
	{
		ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Additive, GeneralAmmo);
		ASC->ApplyModToAttributeUnsafe(GeneralAmmoAttribute, EGameplayModOp::Additive, -GeneralAmmo);
	}
	else
	{
		ASC->ApplyModToAttributeUnsafe(CurrentAmmoAttribute, EGameplayModOp::Override, MaxAmmo);
		ASC->ApplyModToAttributeUnsafe(GeneralAmmoAttribute, EGameplayModOp::Additive, CurrentAmmo - MaxAmmo);
	}

	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
