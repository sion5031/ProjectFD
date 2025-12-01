// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/Boss/FDBossCharacter.h"

#include "Character/Player/FDPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibrary/FDFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GAS/Monster/EnemyAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsAsset.h"

AFDBossCharacter::AFDBossCharacter()
{
	bReplicates = true;

	HeadShell_L = CreateDefaultSubobject<USkeletalMeshComponent>("HeadShell_L");
	HeadShell_L->SetupAttachment(GetMesh(), "Dm_HeadShell_L");
	HeadShell_L->SetIsReplicated(false);

	HeadShell_R = CreateDefaultSubobject<USkeletalMeshComponent>("HeadShell_R");
	HeadShell_R->SetupAttachment(GetMesh(), "Dm_HeadShell_R");
	HeadShell_R->SetIsReplicated(false);

	RealCore = CreateDefaultSubobject<USkeletalMeshComponent>("RealCore");
	RealCore->SetupAttachment(GetMesh(), "Dm_RealCore_Weakness");
	RealCore->SetIsReplicated(false);

	CoreShell = CreateDefaultSubobject<USkeletalMeshComponent>("CoreShell");
	CoreShell->SetupAttachment(GetMesh(), "Dm_RealCoreShell");
	CoreShell->SetIsReplicated(false);
	
	Clav_L = CreateDefaultSubobject<USkeletalMeshComponent>("Clav_L");
	Clav_L->SetupAttachment(GetMesh(), "Dm_Clav_L");
	Clav_L->SetIsReplicated(true);
	
	Clav_R = CreateDefaultSubobject<USkeletalMeshComponent>("Clav_R");
	Clav_R->SetupAttachment(GetMesh(), "Dm_Clav_R");
	Clav_R->SetIsReplicated(true);

	Knee_L = CreateDefaultSubobject<USkeletalMeshComponent>("Knee_L");
	Knee_L->SetupAttachment(GetMesh(), "Dm_Knee_L");
	Knee_L->SetIsReplicated(true);

	Knee_R = CreateDefaultSubobject<USkeletalMeshComponent>("Knee_R");
	Knee_R->SetupAttachment(GetMesh(), "Dm_Knee_R");
	Knee_R->SetIsReplicated(true);
	
	Plasma = CreateDefaultSubobject<USkeletalMeshComponent>("Plasma");
	Plasma->SetLeaderPoseComponent(GetMesh());
	Plasma->SetupAttachment(GetMesh());
	Plasma->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("Guillotine_Collision");
	BoxCollision->SetupAttachment(GetMesh(), "Dm_Sword_R001");

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFDBossCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AFDBossCharacter, bIsBerserk, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AFDBossCharacter, BossPartsInfo, COND_None, REPNOTIFY_Always);
}

void AFDBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	Parts.AddUnique(HeadShell_L);
	Parts.AddUnique(HeadShell_R);
	Parts.AddUnique(RealCore);
	Parts.AddUnique(CoreShell);
	Parts.AddUnique(Knee_R);
	Parts.AddUnique(Knee_L);
	Parts.AddUnique(Clav_L);
	Parts.AddUnique(Clav_R);
	Parts.AddUnique(Plasma);
	
	BoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionEndOverlap);
	
	if (HasAuthority())
	{
		IncreaseBerserkGage();
	}
	
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetBossSpawn());
	
}

void AFDBossCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetEnemyAttributeSet() && GetEnemyAttributeSet()->GetBerserkGage() >= GetEnemyAttributeSet()->GetMaxBerserkGage() && !bIsBerserk)
	{
		StartBerserk();
	}
}


void AFDBossCharacter::IncreaseBerserkGage()
{
	if (bIsBerserk)
	{
		return;
	}
	
	if (GetEnemyAbilitySystemComponent())
	{
		if (IncreaseBerserkGageGE)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(IncreaseBerserkGageGE, 1.f, ContextHandle);
			if (SpecHandle.IsValid())
			{
				ActiveIncreaseBerserkGage = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, "IncreaseBerserkGage");
			}
		}
	}
}

void AFDBossCharacter::StartBerserk()
{
	if (bIsBerserk || !HasAuthority())
	{
		return;
	}

	// 증가 GE 지우기
	if (ActiveIncreaseBerserkGage.IsValid())
	{
		if (ASC)
		{
			ASC->RemoveActiveGameplayEffect(ActiveIncreaseBerserkGage);
		}
		ActiveIncreaseBerserkGage.Invalidate();
	}
	
	bIsBerserk = true;
	if (GetEnemyAttributeSet())
	{
		GetEnemyAttributeSet()->SetBerserkGage(0.f);
	}
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetBerserkStartTag());

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Start Berserk: %s"), bIsBerserk ? TEXT("true") : TEXT("false")));

	// 무적 판정 넣기
	if (ASC && BerserkGE)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(BerserkGE, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			ActiveBerserkGE = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(BerserkTimerHandle, this, &AFDBossCharacter::EndBerserk, 30.f, false);
	}
}

void AFDBossCharacter::EndBerserk()
{
	if (!HasAuthority()) return;

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald,FString::Printf(TEXT("Ended Berserk: %s"), bIsBerserk ? TEXT("true") : TEXT("false")));

	bIsBerserk = false;
	UFDFunctionLibrary::RemoveGameplayFromActorIfFound(this, UFDGameplayStatic::GetBerserkModeTag());
	UFDFunctionLibrary::AddGameplayTagToActorIfNone(this, UFDGameplayStatic::GetNormalStartTag());

	// 다시 게이지 증가 시작
	IncreaseBerserkGage();
}

void AFDBossCharacter::DestroyPart(const FName& BoneName)
{
	// GetMesh()->SetAllBodiesBelowSimulatePhysics(BoneName, false, true);
	// GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(BoneName, 0.f, true);
	// GetMesh()->SetAllBodiesBelowPhysicsDisabled(BoneName, true, true);
	
	for (USkeletalMeshComponent* Part : Parts)
	{
		if (Part->GetAttachSocketName() == BoneName)
		{
			Part->DestroyComponent();
		}
	}
}

void AFDBossCharacter::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* OwningPawn = GetInstigator<APawn>();

	if (!OwningPawn)
	{
		return;
	}
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (OwningPawn != HitPawn)
		{
			OnCollisionHitTarget.ExecuteIfBound(OtherActor);
			if (UFDFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *OtherActor->GetActorNameOrLabel());
				UGameplayStatics::ApplyDamage(HitPawn, 20.f, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
			}
		}
	}
		
}

void AFDBossCharacter::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* OwningPawn = GetInstigator<APawn>();

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (OwningPawn != HitPawn)
		{
			OnCollisionPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}
