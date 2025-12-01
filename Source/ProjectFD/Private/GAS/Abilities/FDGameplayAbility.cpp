// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/FDGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "Components/FDPawnExtensionComponent.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void UFDGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EFDAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UFDGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == EFDAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}


TArray<FHitResult> UFDGameplayAbility::GetHitResultFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweetRadius, float TargetAmount, bool bDrawDebug,
	ETeamAttitude::Type TargetTeam, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResult;
	TSet<AActor*> HitActors;

	// 설정한 값만큼만 감지(0 이하면 무제한)
	const int32 MaxCount = FMath::Max(0, FMath::FloorToInt(TargetAmount));
	int32 CollectedCount = 0;
	bool bReachedLimit = false;
	
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	

	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		if (bReachedLimit)
		{
			break;
		}
		if (!TargetData.IsValid())
		{
			continue;
		}
		
		FVector StartLocation = TargetData->GetOrigin().GetTranslation();
		FVector EndLocation = TargetData->GetEndPoint();
		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorToIgnore;
		if (bIgnoreSelf)
		{
			ActorToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		TArray<FHitResult> Results;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this,StartLocation, EndLocation,
			 SphereSweetRadius, ObjectTypes,false,ActorToIgnore,DrawDebugTrace,Results, false);

		for (const FHitResult& Result : Results)
		{
			if (bReachedLimit)
			{
				break;
			}

			AActor* HitActor = Result.GetActor();
			if (!HitActor || HitActors.Contains(HitActor))
			{
				continue;
			}

			if (OwnerTeamInterface)
			{
				const ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor);
				if (OtherActorTeamAttitude != TargetTeam)
				{
					continue;
				}
			}

			HitActors.Add(HitActor);
			OutResult.Add(Result);

			++CollectedCount;
			if (MaxCount > 0 && CollectedCount >= MaxCount)
			{
				bReachedLimit = true;
				break;
			}
		}
	}

	return OutResult;
}

UPawnCombatComponent* UFDGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UAbilitySystemComponent* UFDGameplayAbility::GetFDAbilitySystemComponentFromActorInfo() const
{
	return Cast<UFDAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

UAnimInstance* UFDGameplayAbility::GetAnimInstance() const
{
	USkeletalMeshComponent* OwningSkeletalMeshComponent = GetOwningComponentFromActorInfo();
	if (OwningSkeletalMeshComponent)
	{
		return OwningSkeletalMeshComponent->GetAnimInstance();
	}
	return nullptr;
}
