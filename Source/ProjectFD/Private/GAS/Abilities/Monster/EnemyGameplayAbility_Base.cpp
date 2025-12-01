// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Monster/EnemyGameplayAbility_Base.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/FDAbilitySystemComponent.h"


UEnemyGameplayAbility_Base::UEnemyGameplayAbility_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UEnemyGameplayAbility_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	if (PlayToMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlayToMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UEnemyGameplayAbility_Base::K2_EndAbility);
		PlayMontage->OnCancelled.AddDynamic(this, &UEnemyGameplayAbility_Base::K2_EndAbility);
		PlayMontage->OnCompleted.AddDynamic(this, &UEnemyGameplayAbility_Base::K2_EndAbility);
		PlayMontage->OnInterrupted.AddDynamic(this, &UEnemyGameplayAbility_Base::K2_EndAbility);
		PlayMontage->ReadyForActivation();
	}

	if (CooldownGameplayEffect)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
				CooldownGameplayEffect, GetAbilityLevel());

			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			}
		}
	}
}
