// Source/ProjectFD/Private/GAS/GA/Player/GA_RangeWeaponAttack.cpp

// c++
#include "GAS/GA/Player/RangeAttackAbility.h"

#include "GAS/FDGameplayStatic.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/FDAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Player/FDPlayerCharacter.h"
#include "Equipment/EquipmentActor.h"
#include "GameFramework/DamageType.h"
#include "GAS/Player/PlayerAttributeSet.h"

URangeAttackAbility::URangeAttackAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    bRetriggerInstancedAbility = true;

    if (!DamageType)
    {
        DamageType = UDamageType::StaticClass();
    }
}

bool URangeAttackAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
    const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        UE_LOG(LogTemp, Warning, TEXT("CanActivateAbility: Super::CanActivateAbility() failed"));
        return false;
    }

    const UFDAbilitySystemComponent* AbilitySystemComponent = Cast<UFDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
    const UPlayerAttributeSet* PlayerAttrSet = AbilitySystemComponent ? AbilitySystemComponent->GetSet<UPlayerAttributeSet>() : nullptr;
    if (!PlayerAttrSet || PlayerAttrSet->GetAmmo() <= 0)
    {
        return false;
    }

    return true;
}


void URangeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    ASC = Cast<UFDAbilitySystemComponent>(ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);

    ACharacter* Character = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // �착 무기 찾기(�시: 부찡터�서 �색)
    AEquipmentActor* Equipment = nullptr;
    {
        TArray<AActor*> Attached;
        Character->GetAttachedActors(Attached);
        for (AActor* A : Attached)
        {
            if ((Equipment = Cast<AEquipmentActor>(A)) != nullptr)
            {
                break;
            }
        }
    }

    // FireMontage �생 줬발�이�StartMontage�건너�고 바로 FireMontage �작
    bool bSkipStart = false;
    if (USkeletalMeshComponent* Mesh = Character->GetMesh())
    {
        if (UAnimInstance* Anim = Mesh->GetAnimInstance())
        {
            bSkipStart = FireMontage && Anim->Montage_IsPlaying(FireMontage);
        }
    }

    if (bSkipStart)
    {
        OnStartMontageCompleted();
    }
    else
    {

        if (UAbilityTask_PlayMontageAndWait* StartTask =
                UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, StartMontage))
        {
            StartTask->OnCompleted.AddDynamic(this, &URangeAttackAbility::OnStartMontageCompleted);
            StartTask->OnCancelled.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
            StartTask->OnInterrupted.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
            StartTask->ReadyForActivation();
        }
        else
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        }
    }
}


void URangeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    Cast<AFDPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get())->FireWeapon(false);
}

void URangeAttackAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);
    Cast<AFDPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get())->FireWeapon(false);
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URangeAttackAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void URangeAttackAbility::OnEndAbility()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

#pragma region -- TraceAndDamage--
bool URangeAttackAbility::DoLineTrace(const FGameplayAbilityActorInfo* ActorInfo, FHitResult& OutHit,
                                        FVector& OutStart, FVector& OutEnd) const
{
    OutHit = FHitResult{};
    OutStart = FVector::ZeroVector;
    OutEnd = FVector::ZeroVector;

    if (!ActorInfo) return false;

    const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character) return false;

    UWorld* World = ActorInfo->AvatarActor.IsValid() ? ActorInfo->AvatarActor->GetWorld() : nullptr;
    if (!World) return false;

    USkeletalMeshComponent* WeaponMesh = nullptr;

    {
        TArray<USkeletalMeshComponent*> SkelMeshes;
        Character->GetComponents(SkelMeshes);
        for (USkeletalMeshComponent* Comp : SkelMeshes)
        {
            if (!Comp || Comp == Character->GetMesh()) continue;
            const bool bHasSocket = Comp->DoesSocketExist(MuzzleSocketName);
            const bool bHasBone   = Comp->GetBoneIndex(MuzzleSocketName) != INDEX_NONE;
            if (bHasSocket || bHasBone)
            {
                WeaponMesh = Comp;
                break;
            }
        }
    }

    // if (!WeaponMesh)
    // {
    //     TArray<AActor*> AttachedActors;
    //     Character->GetAttachedActors(AttachedActors);
    //     for (AActor* Attached : AttachedActors)
    //     {
    //         if (!Attached) continue;
    //
    //         TArray<USkeletalMeshComponent*> AttachedSkelMeshes;
    //         Attached->GetComponents(AttachedSkelMeshes);
    //         for (USkeletalMeshComponent* Comp : AttachedSkelMeshes)
    //         {
    //             if (!Comp) continue;
    //             const bool bHasSocket = Comp->DoesSocketExist(MuzzleSocketName);
    //             const bool bHasBone   = Comp->GetBoneIndex(MuzzleSocketName) != INDEX_NONE;
    //             if (bHasSocket || bHasBone)
    //             {
    //                 WeaponMesh = Comp;
    //                 break;
    //             }
    //         }
    //         if (WeaponMesh) break;
    //     }
    //}

    auto TryGetWorldLocation = [](const USkeletalMeshComponent* Skel, const FName& Name, FVector& OutLoc) -> bool
    {
        if (!Skel) return false;
    
        const bool bHasSocket = Skel->DoesSocketExist(Name);
        const int32 BoneIdx = Skel->GetBoneIndex(Name);
    
        if (bHasSocket)
        {
            OutLoc = Skel->GetSocketLocation(Name);
            return true;
        }
    
        if (BoneIdx != INDEX_NONE)
        {
            FTransform BoneTM = Skel->GetBoneTransform(BoneIdx);
            BoneTM = BoneTM * Skel->GetComponentToWorld();
            OutLoc = BoneTM.GetLocation();
            return true;
        }
        return false;
    };

    FVector Start = Character->GetActorLocation();
    if (!(WeaponMesh && TryGetWorldLocation(WeaponMesh, MuzzleSocketName, Start)))
    {
         if (!TryGetWorldLocation(Character->GetMesh(), MuzzleSocketName, Start))
         {
            Start = Character->GetActorLocation();
         }
    }

    FVector CamLoc;
    FRotator CamRot = Character->GetActorRotation();
    if (AController* PC = Character->GetController())
    {
        PC->GetPlayerViewPoint(CamLoc, CamRot);
    }
    CamLoc = Start;
    
    const FVector Dir = CamRot.Vector();
    const FVector End = Start + Dir * TraceDistance;
    
    FCollisionQueryParams Params(SCENE_QUERY_STAT(GA_RangeWeaponAttackTrace), false);
    Params.AddIgnoredActor(Character);
    
    const bool bHit = World->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params);
    
    OutStart = Start;
    OutEnd = End;
    return bHit;
}

void URangeAttackAbility::ApplyDamageFromHit(const FHitResult& Hit, const ACharacter* SourceCharacter) const
{
    if (!Hit.GetActor() || !SourceCharacter) return;

    AActor* HitActor = Hit.GetActor();

    const uint8 SourceTeam = GetTeamIdFromActorSafe(SourceCharacter);
    const uint8 TargetTeam = GetTeamIdFromActorSafe(HitActor);
    if (SourceTeam != 255 && TargetTeam != 255 && SourceTeam == TargetTeam)
    {
        return;
    }
    const UPlayerAttributeSet* PlayerAttrSet = ASC ? ASC->GetSet<UPlayerAttributeSet>() : nullptr;
    float WeaponCriticalDamage = PlayerAttrSet ? PlayerAttrSet->GetWeaponCriticalDamage() : 1.f;
    float WeaponCriticalChance = PlayerAttrSet ? PlayerAttrSet->GetWeaponCriticalChance() : 0.f;
    float DamageAmount = PlayerAttrSet ? PlayerAttrSet->GetWeaponDamage() : 0.f;

    WeaponCriticalChance = FMath::Clamp(WeaponCriticalChance, 0.f, 1.f);
    bool bCriticalHit = FMath::FRand() <= WeaponCriticalChance;
    
    if (bCriticalHit)
    {
        DamageAmount *= WeaponCriticalDamage;
    }
    

    // if (Hit.Component.IsValid())
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("Hit Component Name: %s"), *Hit.Component->GetName());
    // }
    
    // if (Hit.Component.IsValid() && Hit.Component->ComponentHasTag(UFDGameplayStatic::GetHitWeakPointTag().GetTagName()))
    // {
    //     DamageAmount *= WeakPointMultiplier;
    // }

    // TArray<FName> WeakPointBones = {FName("Dm_Clav_L"), FName("Dm_Clav_R")};
    //
    // if (Hit.BoneName != NAME_None)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("Hit Bone Name: %s"), *Hit.BoneName.ToString());
    //     for (FName BoneName : WeakPointBones)
    //     {
    //         if (Hit.BoneName == BoneName)
    //         {
    //             DamageAmount *= WeakPointMultiplier;
    //             break;
    //         }
    //     }
    // }

    const FVector ShotDir = (Hit.TraceEnd - Hit.TraceStart).IsNearlyZero()
        ? SourceCharacter->GetActorForwardVector()
        : (Hit.TraceEnd - Hit.TraceStart).GetSafeNormal();

    UGameplayStatics::ApplyPointDamage(
        HitActor,
        DamageAmount,
        ShotDir,
        Hit,
        SourceCharacter->GetController(),
        const_cast<ACharacter*>(SourceCharacter),
        DamageType);
}

void URangeAttackAbility::Server_ApplyHit_Implementation(const FHitResult& Hit)
{
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
    if (!ActorInfo || !ActorInfo->IsNetAuthority()) return;

    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character || !Hit.GetActor()) return;

    FHitResult FinalHit = Hit;

    if (bServerReTrace)
    {
        FHitResult ServerHit;
        FVector Start, End;
        const bool bServerGotHit = DoLineTrace(ActorInfo, ServerHit, Start, End);
        if (!bServerGotHit || ServerHit.GetActor() != Hit.GetActor())
        {
            return;
        }
        FinalHit = ServerHit;
    }

    ApplyDamageFromHit(FinalHit, Character);
}


uint8 URangeAttackAbility::GetTeamIdFromActorSafe(const AActor* InActor)
{
    if (!InActor)
    {
        return uint8(255);
    }

    if (const IGenericTeamAgentInterface* TeamIF_Actor = Cast<IGenericTeamAgentInterface>(InActor))
    {
        return TeamIF_Actor->GetGenericTeamId().GetId();
    }

    const APawn* P = Cast<APawn>(InActor);
    if (!P) return uint8(255);

    const AController* C = P->GetController();
    if (C)
    {
        if (const IGenericTeamAgentInterface* TeamIF_Ctrl = Cast<IGenericTeamAgentInterface>(C))
        {
            return TeamIF_Ctrl->GetGenericTeamId().GetId();
        }

        if (const APlayerState* PS_Ctrl = C->PlayerState)
        {
            if (const IGenericTeamAgentInterface* TeamIF_PS = Cast<IGenericTeamAgentInterface>(PS_Ctrl))
            {
                return TeamIF_PS->GetGenericTeamId().GetId();
            }
        }
    }

    if (const APlayerState* PS_Pawn = P->GetPlayerState())
    {
        if (const IGenericTeamAgentInterface* TeamIF_PS2 = Cast<IGenericTeamAgentInterface>(PS_Pawn))
        {
            return TeamIF_PS2->GetGenericTeamId().GetId();
        }
    }

    return uint8(255);
}

void URangeAttackAbility::PerformLineTraceAndDamage(const FGameplayAbilityActorInfo* ActorInfo, bool bApplyDamage)
{
    if (!ActorInfo) return;
    if (!ActorInfo->IsNetAuthority()) return; // �버�서맘행

    const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Character) return;

    FHitResult Hit;
    FVector Start, End;
    const bool bHit = DoLineTrace(ActorInfo, Hit, Start, End);

    if (UWorld* World = Character->GetWorld())
    {
        if (bDrawDebug)
        {
            const FColor LineColor = bHit ? FColor::Red : FColor::Green;
            DrawDebugLine(World, Start, End, LineColor, false, 1.0f, 0, 1.5f);
            if (bHit)
            {
                DrawDebugPoint(World, Hit.ImpactPoint, 12.f, FColor::Yellow, false, 1.0f);
            }
        }
    }

    if (!bHit) return;

    if (bApplyDamage)
    {
        ApplyDamageFromHit(Hit, Character);
    }
}

#pragma endregion

void URangeAttackAbility::OnStartMontageCompleted()
{
    if (UAbilityTask_PlayMontageAndWait* FireTask =
            UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FireMontage))
    {
        FireTask->OnCompleted.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->OnBlendOut.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->OnCancelled.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->OnInterrupted.AddDynamic(this, &URangeAttackAbility::OnEndAbility);
        FireTask->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName("Event.Weapon.Fire")), nullptr, false, true);
        WaitGameplayEventTask->EventReceived.AddDynamic(this, &URangeAttackAbility::OnLineTraceAndDamage);
        WaitGameplayEventTask->ReadyForActivation();
        Cast<AFDPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get())->FireWeapon(true);
        // FireMontage가 �작�면발사
    }
    else
    {
        OnEndAbility();
    }
}

void URangeAttackAbility::OnLineTraceAndDamage(FGameplayEventData Payload)
{
    PerformLineTraceAndDamage(GetCurrentActorInfo(), true);
    // GE �펙 �성 밁용
    FGameplayEffectContextHandle GEContextHandle = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle GrantTagSpecHandle = ASC->MakeOutgoingSpec(AmmoGEClass, 1.f, GEContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*GrantTagSpecHandle.Data.Get());

    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
    AFDPlayerCharacter* PC = CastChecked<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
    ASC = Cast<UFDAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
    const UPlayerAttributeSet* PlayerAttrSet = ASC->GetSet<UPlayerAttributeSet>();
    if (PlayerAttrSet->GetAmmo() <= 0 )
    {
        PC->FireWeapon(false);
        OnEndAbility();
    }
}
