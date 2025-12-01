// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/FDGameplayAbility.h"
#include "PlayerGameplayAbility.generated.h"

class UPlayerAbilitySystemComponent;
class UPlayerCombatComponent;
class AFDPlayerController;
class AFDPlayerCharacter;
/**
 * 
 */
UCLASS()
class PROJECTFD_API UPlayerGameplayAbility : public UFDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "GA")
	AFDPlayerCharacter* GetPlayerCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "GA")
	AFDPlayerController* GetPlayerControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "GA")
	UPlayerCombatComponent* GetPlayerCombatComponentFromActorInfo();

private:
	TWeakObjectPtr<AFDPlayerCharacter> CachedPlayerCharacter;
	TWeakObjectPtr<AFDPlayerController> CachedPlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "GA" )
	bool bShouldDrawDebug = true;

protected:
	UFUNCTION()
	FORCEINLINE bool ShouldDebugDraw() const { return bShouldDrawDebug; }
	
	
};
