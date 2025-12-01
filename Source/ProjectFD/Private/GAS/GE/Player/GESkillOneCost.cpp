// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/Player/GESkillOneCost.h"

#include "GAS/Player/PlayerAttributeSet.h"


UGESkillOneCost::UGESkillOneCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo ManaModifier;
	ManaModifier.Attribute = UPlayerAttributeSet::GetManaAttribute();
	ManaModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Skill.Cost"));
	ManaModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SBC);

	Modifiers.Add(ManaModifier);
}
