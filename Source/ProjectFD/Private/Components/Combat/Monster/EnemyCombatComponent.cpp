// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/Monster/EnemyCombatComponent.h"

#include "Character/Monster/FDMonsterCharacter.h"
#include "Character/Monster/Boss/FDBossCharacter.h"
#include "Components/BoxComponent.h"
#include "GAS/Monster/EnemyAttributeSet.h"


AFDMonsterCharacter* UEnemyCombatComponent::GetEnemyCharacter()
{
	return Cast<AFDMonsterCharacter>(GetOwner());
}

void UEnemyCombatComponent::ToggleCollision(bool bShouldEnable)
{
	if (GetEnemyCharacter())
	{
		AFDBossCharacter* BossCharacter = Cast<AFDBossCharacter>(GetEnemyCharacter());
		
		if (bShouldEnable)
		{
			BossCharacter->GetGuillotineCollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			BossCharacter->GetGuillotineCollision()->SetHiddenInGame(false);
		}
		else
		{
			BossCharacter->GetGuillotineCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BossCharacter->GetGuillotineCollision()->SetHiddenInGame(true);

			OverlappedActor.Empty();
		}
	}
}

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, GetOwningPawn()->GetActorNameOrLabel() + TEXT(" Hit ") + HitActor->GetActorNameOrLabel());
}

void UEnemyCombatComponent::OnCollisionPulledFromTargetActor(AActor* InteractedActor)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, GetOwningPawn()->GetActorNameOrLabel() + TEXT(" Collision pulled Target ") + InteractedActor->GetActorNameOrLabel());
}

void UEnemyCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (AFDBossCharacter* Boss = Cast<AFDBossCharacter>(GetOwner()))
	{
		Boss->OnCollisionHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
		Boss->OnCollisionPulledFromTarget.BindUObject(this, &ThisClass::OnCollisionPulledFromTargetActor);
	}
}
