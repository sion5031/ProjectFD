// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FDAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Player/FDPlayerCharacter.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "FunctionLibrary/FDFunctionLibrary.h"


void UFDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	// 애니메이션에서 사용할 캐릭터, 캐릭터 무브먼트 캐시
	OwnerCharacter = Cast<AFDCharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
		if (!ASC)
		{
			// UE_LOG(LogTemp, Error, TEXT("AnimInstance: ASC is nullptr!"));
		}
	}
	RefreshWeaponAnimLayer();
}

void UFDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		Speed = OwnerCharacter->GetVelocity().Length();

		AFDPlayerCharacter* PlayerCharacter = Cast<AFDPlayerCharacter>(OwnerCharacter);
		if (PlayerCharacter)
		{
			bIsSprint = PlayerCharacter->GetSprinting();
			//CombatState = PlayerCharacter->GetCombating();
			bIsJumping = PlayerCharacter->GetJumping();
		}
		if (!ASC)
		{
			// UE_LOG(LogTemp, Error, TEXT("AnimInstance: ASC is nullptr!"));
			ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
		}
		if (ASC)
		{
			bIsCombatTag = ASC->HasMatchingGameplayTag(UFDGameplayStatic::GetOwnerCombatTag());
		}
	}
	if (OwnerMovementComponent)
	{
		bIsJumping = OwnerMovementComponent->IsFalling();
	}
// ----------------
	const TSubclassOf<UAnimInstance> Desired = SelectDesiredWeaponAnimLayerClass();
	if (Desired != CurrentWeaponAnimLayerClass)
	{
		ApplyWeaponLayerClass(Desired);
	}
	//----------------
}

void UFDAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// ---
	if (!OwnerCharacter || !OwnerMovementComponent)
	{
		return;
	}
	
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorRotation());
}

// bool UFDAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
// {
// 	if (APawn* OwningPawn = TryGetPawnOwner())
// 	{
// 		return UFDFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
// 	}
//
// 	return false;
// }

bool UFDAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	AActor* Owner = GetOwningActor();
	if (!IsValid(Owner) || !TagToCheck.IsValid())
	{
		return false;
	}
	return UFDGameplayStatic::ActorHasGameplayTag(Owner, TagToCheck);
}

void UFDAnimInstance::RefreshWeaponAnimLayer()
{
	ApplyWeaponLayerClass(SelectDesiredWeaponAnimLayerClass());
}

TSubclassOf<UAnimInstance> UFDAnimInstance::SelectDesiredWeaponAnimLayerClass() const
{
	if (MGWeaponLayerClass && DoesOwnerHaveTag(MGTag))
	{
		return MGWeaponLayerClass;
	}
	if (LNCWeaponLayerClass && DoesOwnerHaveTag(LNCTag))
	{
		return LNCWeaponLayerClass;
	}
	return DefaultWeaponLayerClass;
}

void UFDAnimInstance::ApplyWeaponLayerClass(TSubclassOf<UAnimInstance> DesiredClass)
{
	USkeletalMeshComponent* SkelComp = GetSkelMeshComponent();
	if (!IsValid(SkelComp))
	{
		return;
	}

	// 원하는 클래스가 없으면 언링크, 있으면 링크
	if (!DesiredClass)
	{
		if (CurrentWeaponAnimLayerClass)
		{
			SkelComp->UnlinkAnimClassLayers(CurrentWeaponAnimLayerClass);
		}
		CurrentWeaponAnimLayerClass = nullptr;
		return;
	}
	
	// 다른 클래스로 전환 시 기존 레이어 언링크
	if (CurrentWeaponAnimLayerClass && CurrentWeaponAnimLayerClass != DesiredClass)
	{
		SkelComp->UnlinkAnimClassLayers(CurrentWeaponAnimLayerClass);
	}

	SkelComp->LinkAnimClassLayers(DesiredClass);
	CurrentWeaponAnimLayerClass = DesiredClass;
}


// void UFDAnimInstance::InitializeAbilitySystemComponent()
// {
// 	OwnerCharacter = Cast<AFDCharacter>(TryGetPawnOwner());
// 	if (OwnerCharacter)
// 	{
// 		ASC = Cast<UFDAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter));
// 	}
// }
