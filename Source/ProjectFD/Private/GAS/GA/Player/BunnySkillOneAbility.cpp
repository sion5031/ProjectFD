// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/Player/BunnySkillOneAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/FDGameplayStatic.h"
#include "GAS/Player/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


UBunnySkillOneAbility::UBunnySkillOneAbility()
{
	ActivationRequiredTags.AddTag(UFDGameplayStatic::GetOwnerCombatTag());
}

void UBunnySkillOneAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
	const FGameplayEffectSpecHandle CostSpecHandle = ASC->MakeOutgoingSpec(CostGEClass, 1.f, GEContextHandle);
	const FGameplayEffectSpecHandle CooldownSpecHandle = ASC->MakeOutgoingSpec(CooldownGEClass, 1.f, GEContextHandle);
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	const float BunnySkillOneCooldown = SkillCooldown * PlayerAttrSet->GetSkillCooldown();

	CooldownSpecHandle.Data->SetDuration(BunnySkillOneCooldown, true);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		ASC->ApplyGameplayEffectSpecToSelf(*CooldownSpecHandle.Data.Get());
	}
	
	// SetByCaller를 이용해 CostAmount 조정
	if (CostSpecHandle.IsValid())
	{
		const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Skill.Cost"));
		const float BunnySkillOneCost = CostAmount * PlayerAttrSet->GetSkillCost();

		if (PlayerAttrSet->GetMana() < BunnySkillOneCost )
		{
			UE_LOG(LogTemp, Warning, TEXT("마나가 부족합니다."));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		
		CostSpecHandle.Data->SetSetByCallerMagnitude(CostTag, -BunnySkillOneCost);
		// 적용은 Authority 또는 PredictionKey 보유 시 한 번만
		if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) && CostSpecHandle.Data.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*CostSpecHandle.Data.Get());
		}
	}
	
	
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,true, false);
		return;
	}
	

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlaySkillMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SkillMontage);
		PlaySkillMontageTask->OnBlendOut.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->OnCompleted.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->OnInterrupted.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->OnCancelled.AddDynamic(this, &UBunnySkillOneAbility::K2_EndAbility);
		PlaySkillMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitSkillEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Ability.BunnySkill.One")), nullptr, false, true);
		WaitSkillEventTask->EventReceived.AddDynamic(this, &UBunnySkillOneAbility::StartTargetSweep);
		WaitSkillEventTask->ReadyForActivation();
	}	
}

void UBunnySkillOneAbility::StartTargetSweep(FGameplayEventData EventData)
{
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	UAbilitySystemComponent* ASC = GetFDAbilitySystemComponentFromActorInfo();
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	const float BunnySkillOneRadius = TargetSweepSphereRadius * PlayerAttrSet->GetSkillRadius();
	
	TArray<FHitResult> TargetHitResults = GetHitResultFromSweepLocationTargetData(EventData.TargetData, BunnySkillOneRadius, TargetAmount, ShouldDebugDraw(), ETeamAttitude::Hostile, false);
	if (K2_HasAuthority())
	{
		for (FHitResult HitResult : TargetHitResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());

			FVector StartLocation = GetOwningActorFromActorInfo()->GetActorLocation();
			FVector EndLocation = HitResult.ImpactPoint;
			
			const float BunnySkillOneDamage = SkillDamageMultiplier * PlayerAttrSet->GetSkillDamage();
			UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), BunnySkillOneDamage, (EndLocation - StartLocation).GetSafeNormal(), HitResult, OwnerActor->GetInstigatorController(), OwnerActor, UDamageType::StaticClass());
		}
	}
}
