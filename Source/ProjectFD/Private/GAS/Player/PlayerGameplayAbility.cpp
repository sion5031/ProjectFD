// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Player/PlayerGameplayAbility.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Controller/FDPlayerController.h"


AFDPlayerCharacter* UPlayerGameplayAbility::GetPlayerCharacterFromActorInfo()
{
	if (!CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter = Cast<AFDPlayerCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedPlayerCharacter.IsValid() ? CachedPlayerCharacter.Get() : nullptr;
}

AFDPlayerController* UPlayerGameplayAbility::GetPlayerControllerFromActorInfo()
{
	if (!CachedPlayerController.IsValid())
	{
		CachedPlayerController = Cast<AFDPlayerController>(CurrentActorInfo->PlayerController);
	}

	return CachedPlayerController.IsValid() ? CachedPlayerController.Get() : nullptr;
}

UPlayerCombatComponent* UPlayerGameplayAbility::GetPlayerCombatComponentFromActorInfo()
{
	return GetPlayerCharacterFromActorInfo()->GetCombatComponent();
}
