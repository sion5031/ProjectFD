// C++
#include "GAS/GA/Player/CharacterRollAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Player/FDPlayerCharacter.h"

void UCharacterRollAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFDPlayerCharacter* Character = Cast<AFDPlayerCharacter>(ActorInfo->AvatarActor.Get());
	bool IsFalling = false;
	if(Character)
	{
		IsFalling = Character->GetMovementComponent()->IsFalling();
	}
		
	//능력이 적용이 문제가 되면 (자원부족...cost) or Jumping 상태시 탈출
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo) || IsFalling)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateAbility 탈출"));
		EndAbility(Handle, ActorInfo, ActivationInfo,true, false);
		return;
	}
	
	if (RollMontage && ActorInfo->AvatarActor.IsValid())
	{
		// 어빌리티 태스크로 몽타주 실행 (자동 동기화)
		UAbilityTask_PlayMontageAndWait* MontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, NAME_None, RollMontage, 1.0f
			);
		MontageTask->ReadyForActivation();
		
		if (Character && Character->HasAuthority())
		{
			TotalRollDistance = 600.0f;
			RollElapsedTime = 0.0f;
			RollDuration = RollMontage->GetPlayLength()*0.6f; // 몽타주 길이의 60% 동안 롤 이동
			RollCharacter = Character;

			Character->GetWorldTimerManager().SetTimer(
				RollTimerHandle, this, &UCharacterRollAbility::TickRollMove, 0.01f, true
			);
		}
	}
}

void UCharacterRollAbility::TickRollMove()
{
	if (!RollCharacter)
		return;

	RollElapsedTime += 0.01f;
	float Alpha = FMath::Clamp(RollElapsedTime / RollDuration, 0.0f, 1.0f);

	// 처음엔 빠르고 점점 느려지는 감속 이징 적용
	float EasedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 1.2f); // 감속 정도는 2.0f로 조절
	float CurrentDistance = TotalRollDistance * EasedAlpha;
	float PrevDistance = TotalRollDistance * FMath::InterpEaseOut(0.0f, 1.0f, Alpha - 0.01f / RollDuration, 1.2f);
	float MoveDistance = CurrentDistance - PrevDistance;

	FVector Forward = RollCharacter->GetActorForwardVector();
	RollCharacter->AddActorWorldOffset(Forward * MoveDistance, true);

	if (Alpha >= 1.0f)
	{
		RollCharacter->GetWorldTimerManager().ClearTimer(RollTimerHandle);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}