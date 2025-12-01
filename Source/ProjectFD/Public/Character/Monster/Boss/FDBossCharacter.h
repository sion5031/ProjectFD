// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Monster/FDMonsterCharacter.h"
#include "FDBossCharacter.generated.h"

class AFDBossParts;
class UBoxComponent;

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*)

USTRUCT(BlueprintType)
struct FBossPart
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	float PartHealth = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	FName BoneName;
};

/**
 * 
 */
UCLASS()
class PROJECTFD_API AFDBossCharacter : public AFDMonsterCharacter
{
	GENERATED_BODY()
	
public:
	AFDBossCharacter();

	// Delegate
	FOnTargetInteractedDelegate OnCollisionHitTarget;
	FOnTargetInteractedDelegate OnCollisionPulledFromTarget;

	// Getter
	UBoxComponent* GetGuillotineCollision() const { return BoxCollision; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetBerserkMode() { return bIsBerserk; }

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// LifeCycle
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// Collision handling
	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// BerserkMode
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> IncreaseBerserkGageGE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> BerserkGE;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveIncreaseBerserkGage;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveBerserkGE;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	bool bIsBerserk = false;

	UFUNCTION()
	void IncreaseBerserkGage();

	UFUNCTION()
	void StartBerserk();

	UFUNCTION()
	void EndBerserk();
	
	FTimerHandle BerserkTimerHandle;
	
public:
	// Boss Parts
	UFUNCTION()
	void DestroyPart(const FName& BoneName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletalMeshComponent*> Parts;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FBossPart> BossPartsInfo;

protected:
	// Boss Parts Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* HeadShell_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* HeadShell_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Clav_L;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Clav_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* RealCore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* CoreShell;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Knee_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Knee_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Parts")
	USkeletalMeshComponent* Plasma;

	// Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	UBoxComponent* BoxCollision;
	
};
