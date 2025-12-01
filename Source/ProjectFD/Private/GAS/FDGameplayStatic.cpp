// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FDGameplayStatic.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/GE/Tagging/AddingTagGameplayEffect.h"
#include "GAS/Player/PlayerAttributeSet.h"


FGameplayTag UFDGameplayStatic::GetMeleeAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.MeleeAttack"));
}

FGameplayTag UFDGameplayStatic::GetRangeAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.RangeAttack"));
}

FGameplayTag UFDGameplayStatic::GetEquipWeaponAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.Equip"));
}

FGameplayTag UFDGameplayStatic::GetUnequipWeaponAbilityTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.Unequip"));
}

FGameplayTag UFDGameplayStatic::GetOwnerCombatTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Combat"));
}

FGameplayTag UFDGameplayStatic::GetOwnerNormalTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Normal"));
}

FGameplayTag UFDGameplayStatic::GetDeadStatusTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Dead"));
}

FGameplayTag UFDGameplayStatic::GetOwnerAimingTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Player.Status.Aiming"));
}

FGameplayTag UFDGameplayStatic::GetHitWeakPointTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.WeakPoint"));
}

FGameplayTag UFDGameplayStatic::GetBunnySkillOneTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.BunnySkill.One"));
}

FGameplayTag UFDGameplayStatic::GetBunnySkillTwoTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Ability.BunnySkill.Two"));
}

float UFDGameplayStatic::GetWeakDamage(AActor* Actor)
{
	if (!Actor)
	{
		return 0.0f;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	
	if (!ASC)
	{
		return 0.0f;
	}
	
	const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
	
	if (!PlayerAttrSet)
	{
		return 0.0f;
	}
	
	return PlayerAttrSet->GetWeakDamage();
}

bool UFDGameplayStatic::GrantGameplayTagByEffect(UAbilitySystemComponent* ASC, FGameplayTag Tag)
{
	if (ASC->GetAvatarActor()->HasAuthority())
	{
		TSubclassOf<UAddingTagGameplayEffect> EffectClass = UAddingTagGameplayEffect::StaticClass();
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->DynamicGrantedTags.AddTag(Tag);

			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			return true;
		}		
	}

	return false;
}

bool UFDGameplayStatic::ActorHasGameplayTag(AActor* Actor, FGameplayTag Tag)
{
	UAbilitySystemComponent* ASC = Actor->FindComponentByClass<UAbilitySystemComponent>();
	if (ASC)
	{
		return ASC->HasMatchingGameplayTag(Tag);
	}
	return false;
}

FGameplayTag UFDGameplayStatic::GetBerserkModeTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.State.Berserk"));
}

FGameplayTag UFDGameplayStatic::GetBerserkStartTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Start.Berserk"));
}

FGameplayTag UFDGameplayStatic::GetNormalModeTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.State.Normal"));
}

FGameplayTag UFDGameplayStatic::GetNormalStartTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Start.Normal"));
}

FGameplayTag UFDGameplayStatic::GetBossSpawn()
{
	return FGameplayTag::RequestGameplayTag(FName("Enemy.Boss.Spawn"));
}
