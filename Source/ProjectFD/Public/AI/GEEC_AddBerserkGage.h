// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_AddBerserkGage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFD_API UGEEC_AddBerserkGage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UGEEC_AddBerserkGage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
