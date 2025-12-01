// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AnimNoitfyState/ANS_NormalLaser.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "KismetTraceUtils.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


void UANS_NormalLaser::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;
	PrevEnd = MeshComp->GetSocketLocation(MuzzleSocketName);
	
}

void UANS_NormalLaser::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	AAIController* AIC = Cast<AAIController>(OwnerActor->GetInstigatorController());
	if (!AIC) return;
	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor) return;

	TimerAccumulator += FrameDeltaTime;
	
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector CurrentEnd = FMath::VInterpTo(PrevEnd, TargetLocation, FrameDeltaTime, FollowSpeed);
	
	// FVector Dir = (TargetLocation - MuzzleLocation).GetSafeNormal();
	FVector Dir = (CurrentEnd - MuzzleLocation).GetSafeNormal();
	float Distance = (TargetLocation - MuzzleLocation).Size();
	float LaserLength = Distance + ExtraLength;
	FVector End = MuzzleLocation + Dir * LaserLength;

	PrevEnd = CurrentEnd;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());

	if (TimerAccumulator >= TraceInterval)
	{
		TimerAccumulator = 0.f;

		bool bHit = MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult,MuzzleLocation, End, ECC_Visibility, Params);

		if (bHit)
		{
			if (Cast<AFDPlayerCharacter>(HitResult.GetActor()))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, *HitResult.GetActor()->GetActorNameOrLabel());
				// 데미지 적용
				UGameplayStatics::ApplyDamage(TargetActor, 5.f, OwnerActor->GetInstigatorController(), OwnerActor, nullptr);
				
			}
		}
	
		DrawDebugLine(MeshComp->GetWorld(), MuzzleLocation, End, FColor::Red, false, 0.1f);
	}
	
}