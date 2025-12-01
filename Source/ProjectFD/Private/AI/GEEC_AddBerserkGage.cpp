// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GEEC_AddBerserkGage.h"

#include "AbilitySystemComponent.h"
#include "GAS/Monster/EnemyAttributeSet.h"


UGEEC_AddBerserkGage::UGEEC_AddBerserkGage()
{
	
}

void UGEEC_AddBerserkGage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                  FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UEnemyAttributeSet::GetBerserkGageAttribute(), // 수정할 Attribute
			EGameplayModOp::Additive,                      // 더하기 방식
			1.0f                                           // 값
		)
	);
}
