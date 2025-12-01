// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_NormalLaser.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UANS_NormalLaser : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	FName MuzzleSocketName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float FollowSpeed = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float ExtraLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	float TraceInterval = 0.33f;

protected:
	FVector PrevEnd;

	float TimerAccumulator = 0.f;
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
