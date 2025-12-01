// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/MeleeAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/FDPlayerCharacter.h"


UMeleeAttackAbility::UMeleeAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFDPlayerCharacter* PC = CastChecked<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
	//PC->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAbilityTask_PlayMontageAndWait* PlayPunchTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PunchActionMontage, 1.0f, NAME_None, false);
	PlayPunchTask->OnCompleted.AddDynamic(this, &UMeleeAttackAbility::OnComplateCallback);
	PlayPunchTask->OnInterrupted.AddDynamic(this, &UMeleeAttackAbility::OnInterruptedCallback);
	PlayPunchTask->OnCancelled.AddDynamic(this, &UMeleeAttackAbility::OnInterruptedCallback);
	PlayPunchTask->OnBlendOut.AddDynamic(this, &UMeleeAttackAbility::OnInterruptedCallback);
	PlayPunchTask->ReadyForActivation();

	StartComboTimer();
}

void UMeleeAttackAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	UE_LOG(LogTemp, Warning, TEXT("Punch InputPressed"));

	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboInput = false;
	}
	else
	{
		HasNextComboInput = true;
	}
}

void UMeleeAttackAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UMeleeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AFDPlayerCharacter* ABCharacter = CastChecked<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
	//ABCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	CurrentCombo = 0;
	HasNextComboInput = false;
}

void UMeleeAttackAbility::OnComplateCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMeleeAttackAbility::OnInterruptedCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FName UMeleeAttackAbility::GetNextSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UMeleeAttackAbility::StartComboTimer()
{
	const int32 ComboIndex = FMath::Clamp(CurrentCombo -1, 0, MaxComboCount);
	ensure(EffectiveFrameCount.IsValidIndex(ComboIndex));
	const float ComboEffectiveTime = EffectiveFrameCount[ComboIndex] / FrameRate;
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UMeleeAttackAbility::CheckComboInput, ComboEffectiveTime, false);
	}
}

void UMeleeAttackAbility::CheckComboInput()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboInput)
	{
		MontageJumpToSection(GetNextSection());
		StartComboTimer();
		HasNextComboInput = false;
	}
}
