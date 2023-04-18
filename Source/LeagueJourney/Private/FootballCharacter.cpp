// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FootballGameMode.h"
#include "FootballerController.h"
#include "Football.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFootballCharacter::AFootballCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BallPosessArea = CreateDefaultSubobject<USphereComponent>(FName("BallPosessArea"));
}


// Called when the game starts or when spawned
void AFootballCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BallPosessArea->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);


	BallPosessArea->OnComponentBeginOverlap.AddDynamic(this, &AFootballCharacter::OnPossessOverlapBegin);

	if (APlayerController* _PC = Cast<AFootballerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PC = _PC;
		if (UEnhancedInputLocalPlayerSubsystem* _Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(_PC->GetLocalPlayer()))
		{
			_Subsystem->AddMappingContext(BaseMappingContext, 5);
		}
	}


	//Set default values
	
	if(stats.CurrentPosition == "GK")
	{
		coverageDistance = 200 + stats.Goalkeeping * 300;
	}
	else
	{
		coverageDistance = 200 + stats.Defending * 4000;
	}

	TArray<AActor*> _allColleagues;
	
	if (stats.CurrentPosition == "GK" && bPlaysAtHome)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, "pass -> No player  " + FString::FromInt(_allColleagues.Num()));
	}
	if(CurrentGameMode)
	{
		if (!Cast<AFootballGameMode>(CurrentGameMode)->bSpecMode && !bPlaysAtHome)
		{
			DifficultyToStats();
		}
	}
	
}


void AFootballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (KnownBall)
	{
		//Ball Detection Check

		if(bKickOff && bHasBall)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "TenhoBola");
		}

		if(bWantsBall)
		{
			bWantsBall = (FVector::Distance(KnownBall->GetActorLocation(), GetActorLocation()) <= (500 + coverageDistance * ((!bTeamHasBall) ? 1 : 0)/2));
		}else
		{
			bWantsBall = (FVector::Distance(KnownBall->GetActorLocation(), GetActorLocation()) <= (500 + coverageDistance * ((!bTeamHasBall) ? 1 : 0)));
		}
	}

	//Find Closest Team Pawn + Add To Charge Percentage

	if (bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		FindClosestPawn(bPlaysAtHome);
		ChargePercentage = FMath::Clamp(ChargePercentage += 1 * DeltaTime, 0.f, 1.f);
		
	}

	//Decrement Stamina depending on player stats

	if (GetVelocity().Length() > 0)
	{
		stamina = FMath::Clamp(stamina -= (.005f + (1.0f / stats.Stamina) * .005f) * DeltaTime, 0.0f, 1.0f);
		
	}


	DrawDebugString(GetWorld(), GetActorLocation(), "Wants Ball : " + FString::SanitizeFloat(bWantsBall), 0,
	                FColor::Red, .01, false, 1);
	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 30,
	                "Team Ball : " + FString::SanitizeFloat(bTeamHasBall), 0, FColor::Yellow, .01, false, 1);
	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 60,
	                "Has Ball : " + FString::SanitizeFloat(bHasBall), 0, FColor::Green, .01, false, 1);


	//Team has ball?
	teamHasBall(bPlaysAtHome);
}


//Input Handler

void AFootballCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Always available Actions

		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &AFootballCharacter::EnhancedMove);

		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this,
		                                   &AFootballCharacter::EnhancedSprint);


		//One Shot Input

		enhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Started, this,
		                                   &AFootballCharacter::EnhancedSwitch);

		enhancedInputComponent->BindAction(TackleAction, ETriggerEvent::Started, this,
		                                   &AFootballCharacter::EnhancedTackle);

		//Charging Inputs

		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this,
		                                   &AFootballCharacter::EnhancedCharge);

		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Started, this,
		                                   &AFootballCharacter::EnhancedCharge);


		//Complete Charge

		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this,
		                                   &AFootballCharacter::EnhancedShot);

		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Triggered, this,
		                                   &AFootballCharacter::EnhancedPass);
	}
}


//Enhanced Actions

void AFootballCharacter::EnhancedSprint(const FInputActionValue& Value)
{
	SprintPercentage = Value.Get<float>();
}

void AFootballCharacter::EnhancedCharge(const FInputActionValue& Value)
{
	if (!bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && bCanCharge)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Charign");
		PlayerToPassTo = nullptr;
		bIsCharging = Value.Get<bool>();
	}
}

void AFootballCharacter::EnhancedMove(const FInputActionValue& Value)
{
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && bCanMove)
	{
		FVector2D CurrentValue = Value.Get<FVector2D>();
		moveAxis = CurrentValue;
		CurrentValue.Normalize();
		FVector FinalValue = (SpawnedCamera != nullptr)
			                     ? Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetForwardVector() * CurrentValue.Y
			                     + Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetRightVector() * CurrentValue.X
			                     : CurrentPosition;
		FinalValue.Z = 0;

		GetCharacterMovement()->MaxWalkSpeed = (FinalValue.Length() > .8)
			                                       ? 300.0 + stamina * (SprintPercentage * (stats.Pace / 20.0 * 300))
			                                       : 300;
		GetCharacterMovement()->RotationRate = (FinalValue.Length() > .8)
			                                       ? FRotator(0, 180 + SprintPercentage * 180, 0)
			                                       : FRotator::ZeroRotator;


		FinalValue.Normalize();
		AddMovementInput(FinalValue);
	}
	else
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			FVector2D CurrentValue = Value.Get<FVector2D>();
			moveAxis = CurrentValue;
			CurrentValue.Normalize();
			FVector FinalValue = (SpawnedCamera != nullptr)
				                     ? Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetForwardVector() *
				                     CurrentValue.Y + Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetRightVector()
				                     * CurrentValue.X
				                     : CurrentPosition;
			FinalValue.Z = GetActorLocation().Z;

			GetCharacterMovement()->MaxWalkSpeed = (FinalValue.Length() > .8)
				                                       ? 300.0 + stamina * (SprintPercentage * (stats.Pace / 20.0 *
					                                       200))
				                                       : 300;
			GetCharacterMovement()->RotationRate = (FinalValue.Length() > .8)
				                                       ? FRotator(0, 180 + SprintPercentage * 180, 0)
				                                       : FRotator::ZeroRotator;

			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FinalValue));
		}
	}
}

void AFootballCharacter::EnhancedTackle(const FInputActionValue& Value)
{
	if (!bHasBall && !bTeamHasBall && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && bCanTackle)
	{
		PlayAnimMontage(MontageTackle, 1, EName::None);
		if (ChargePercentage >= 1 || !Value.Get<bool>())
		{
			bIsCharging = false;
			PlayAnimMontage(MontageShot, 1, EName::None);
		}
	}
}

void AFootballCharacter::EnhancedPass(const FInputActionValue& Value)
{
	if (bIsCharging && bHasBall && bTeamHasBall && bCanPass && !Cast<AFootballGameMode>(CurrentGameMode)->bCountDown)
	{
		if (ChargePercentage >= 1 || !Value.Get<bool>())
		{
			bIsCharging = false;
			PlayAnimMontage(MontagePass, 1, EName::None);
		}
	}
	else
	{
		bIsCharging = false;
	}
}

void AFootballCharacter::EnhancedSwitch(const FInputActionValue& Value)
{
	if (!bHasBall && bCanSwitch)
	{
		TArray<AActor*> AllTeamPlayer = teamColleagues;
		AllTeamPlayer.Remove(this);
		float x;
		AActor* _newChar = UGameplayStatics::FindNearestActor(KnownBall->GetActorLocation(), AllTeamPlayer, x);
		FVector _oldVel = _newChar->GetVelocity();
		
		PC->Possess(Cast<APawn>(_newChar));
		Cast<AFootballCharacter>(_newChar)->AddMovementInput(_oldVel, 3, true);
		PlayerToPassTo = nullptr;
		bIsCharging = false;
		ChargePercentage = 0;
	}
}

void AFootballCharacter::EnhancedShot(const FInputActionValue& Value)
{
	if (bIsCharging && bHasBall && bTeamHasBall && bCanShoot)
	{
		if (ChargePercentage >= 1 || !Value.Get<bool>())
		{
			bIsCharging = false;
			PlayAnimMontage(MontageShot, 1, EName::None);
		}
	}
	else
	{
		bIsCharging = false;
	}
}


//Actions

void AFootballCharacter::Shoot()
{
	if (bHasBall && bCanShoot)
	{

		TArray<AActor*> _targets;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), (bPlaysAtHome) ? "awayTarget" : "homeTarget", _targets);
		const AActor* _randTarget = _targets[FMath::RandRange(0, _targets.Num() - 1)];
		float _distance = FVector::Distance(GetActorLocation(), _randTarget->GetActorLocation());


		if (ChargePercentage == 0)
		{
			FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), _randTarget->GetActorLocation());
			_newRot.Pitch = this->GetActorRotation().Pitch;
			SetActorRotation(_newRot);
			ChargePercentage = FMath::FRandRange(0.1, 1.0);

			Cast<AFootball>(KnownBall)->Shoot(_distance, _randTarget->GetActorLocation() - GetActorLocation(),stats.Shooting, ChargePercentage);
		}
		else
		{

			FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation()+GetActorForwardVector());
			_newRot.Pitch = this->GetActorRotation().Pitch;
			SetActorRotation(_newRot);
			Cast<AFootball>(KnownBall)->Shoot(_distance, GetActorForwardVector(), stats.Shooting, ChargePercentage);
		}
	}
}

void AFootballCharacter::Pass()
{
	if (bHasBall && bCanPass && !Cast<AFootballGameMode>(CurrentGameMode)->bCountDown)
	{
		if(PlayerToPassTo == nullptr)
		{
			if (PC->GetPawn() == this)
			{
				float x;
				const FVector TraceStart = GetActorLocation() + GetActorForwardVector() * (200 + ChargePercentage * (stats.
					Passing / 20.0 * 4000));
				PlayerToPassTo = UGameplayStatics::FindNearestActor(TraceStart, teamColleagues, x);
			}
			else
			{
				ChargePercentage = FMath::FRandRange(0.1, 1.0);
				TArray<AActor*> AllTeamPlayer;

				for (AActor* _plaer : teamColleagues)
				{
					if (bPlaysAtHome)
					{
						if (_plaer->GetActorLocation().Y < GetActorLocation().Y)
						{
							AllTeamPlayer.Add(_plaer);
							DrawDebugSphere(GetWorld(), _plaer->GetActorLocation(), 100, 16, FColor::Magenta, false, 15, 0, 50);
						}
					}
					else
					{
						if (_plaer->GetActorLocation().Y > GetActorLocation().Y)
						{
							AllTeamPlayer.Add(_plaer);
							DrawDebugSphere(GetWorld(), _plaer->GetActorLocation(), 100, 16, FColor::Magenta, false, 15, 0, 50);
						}
					}

				}

				float x;
				if (AllTeamPlayer.Num() > 0)
				{
					if (FMath::RandRange(0.0, 1.0) > .5)
					{
						PlayerToPassTo = AllTeamPlayer[FMath::RandRange(0, AllTeamPlayer.Num() - 1)];
					}
					else
					{
						PlayerToPassTo = UGameplayStatics::FindNearestActor(GetActorLocation(), AllTeamPlayer, x);
					}
				}
				else
				{
					if (teamColleagues.Num() > 0)
					{
						if (FMath::RandRange(0.0, 1.0) > .5)
						{
							PlayerToPassTo = teamColleagues[FMath::RandRange(0, teamColleagues.Num() - 1)];
						}
						else
						{
							PlayerToPassTo = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, x);
						}
					}
				}


			}
		}

		if(PlayerToPassTo)
		{
			(bKickOff) ? Cast<AFootballGameMode>(CurrentGameMode)->CreateKickOffEvent(this) : NULL;
			if(stats.CurrentPosition == "GK")
			{

				(bGoalKick) ? Cast<AFootballGameMode>(CurrentGameMode)->CreateGoalKickEvent(this) : NULL;
			}

			FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerToPassTo->GetActorLocation());
			_newRot.Pitch = this->GetActorRotation().Pitch;
			SetActorRotation(_newRot);
			Cast<AFootball>(KnownBall)->Pass(PlayerToPassTo, this, stats.Passing, ChargePercentage);
		}
	}
}

void AFootballCharacter::Tackle()
{
	if (!bHasBall && bCanTackle)
	{
		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
		}; // Query only pawns
		const TArray<AActor*> ActorsToIgnore = {this}; // Ignore the current player's pawn
		FHitResult OutHit;
		UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetMesh()->GetSocketLocation("ballSocket"),
		                                                  GetMesh()->GetSocketLocation("ballSocket"),
		                                                  150.0 + stats.Defending / 20.0 * 300.0, ObjectTypes, false,
		                                                  ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHit, true);

		if (AFootballCharacter* _char = Cast<AFootballCharacter>(OutHit.GetActor()))
		{
			const float _fallChance = FMath::FRandRange(0.0, 1.0);
			const float _foulChance = FMath::FRandRange(0.0, 1.0);
			const float _possessChance = FMath::FRandRange(0.0, 1.0);

			//Tackle
			if(_char->bPlaysAtHome != bPlaysAtHome && _fallChance <= .5 + stats.Defending * 0.025)
			{
				//Card
				if(_foulChance <= .8 - stats.Defending * 0.025 || KnownBall->DaddyPawn != _char)
				{
					AddCard();
					//create event
				}
				else
				{
					//Possess
					if (_possessChance <= .1 + stats.Defending * 0.025 && KnownBall->DaddyPawn == _char)
					{
						KnownBall->Possess(this);
					}
				}

				_char->GetTackled();
			}
		}
	}

	PlayerToPassTo = nullptr;
}

void AFootballCharacter::AddCard()
{
	cardNumber++;

	if (cardNumber == 2)
	{
		float x = 0;
		AActor* closeA = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, x);

		for(AActor* _colleague : teamColleagues)
		{
			Cast<AFootballCharacter>(_colleague)->teamColleagues.Remove(this);
		}

		(bPlaysAtHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome.Remove(this) : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway.Remove(this);

		(bPlaysAtHome) ? PC->Possess(Cast<APawn>(closeA)) : NULL;

		//create event

		Destroy();
	}
}

void AFootballCharacter::GetTackled()
{
	if (bHasBall)
	{
		KnownBall->UnPossess();
	}
	bIsCharging = false;
	PlayAnimMontage(MontageTackled, .6, EName::None);

}

void AFootballCharacter::GetMaxPace()
{
	
	GetCharacterMovement()->MaxWalkSpeed = 300.0 + stamina * (((bTeamHasBall) ? 1 : 1.5) * (stats.Pace / 20.0 * 300));
	GetCharacterMovement()->RotationRate = FRotator(0, 180 + 1 * 180, 0);

}

void AFootballCharacter::DifficultyToStats()
{
	float _diff = Cast<AFootballGameMode>(CurrentGameMode)->Difficulty;

	(_diff == 0) ? _diff = 0.5 : NULL;

	stats.Defending *= _diff;
	stats.Goalkeeping *= _diff;
	stats.Pace *= _diff;
	stats.Passing *= _diff;
	stats.Shooting *= _diff;
	stats.Stamina *= _diff;
}

void AFootballCharacter::FindClosestPawn(bool isHome)
{
	if (PC->GetPawn() == this)
	{
		const FVector TraceStart = GetActorLocation() + GetActorForwardVector() * (200 + ChargePercentage * (stats.
			Passing / 20.0 * 4000)); // Start the trace at a distance of 100 units from the player
		const FVector BoxExtent = FVector(200 + ChargePercentage * (stats.Passing / 20.0 * 4000),
			600 - (ChargePercentage * (20.0 / stats.Passing * 25.0)), 200);
		// Set the dimensions of the box

		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
		}; // Query only pawns
		const TArray<AActor*> ActorsToIgnore = { this }; // Ignore the current player's pawn

		TArray<FHitResult> OutHits;

		TArray<AActor*> _allPlayers;
		UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), TraceStart, TraceStart, BoxExtent, GetActorRotation(),
			ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame,
			OutHits, true);


		for (FHitResult _hit : OutHits)
		{
			if (Cast<AFootballCharacter>(_hit.GetActor())->bPlaysAtHome == bPlaysAtHome)
			{
				_allPlayers.Add(_hit.GetActor());
			}
		}

		if (_allPlayers.Num() > 0)
		{
			float x = 0;
			PlayerToPassTo = UGameplayStatics::FindNearestActor(TraceStart * 2, _allPlayers, x);
			DrawDebugSphere(GetWorld(), PlayerToPassTo->GetActorLocation(), 500, 16, FColor::Green);
		}

	}
	else
	{
		TArray<AActor*> _attackingColleagues;
		TArray<AActor*> _defendingColleagues;

		for (AActor* _footballer : teamColleagues)
		{
			if (_footballer->GetActorLocation().Y >= GetActorLocation().Y)
			{
				_attackingColleagues.Add(_footballer);
			}
			else
			{
				_defendingColleagues.Add(_footballer);
			}
		}

		AActor* _currentNearest;
		float distance = 0;
		float _perAttack = FMath::RandRange(0.0, 1.0);
		float _perShort = FMath::RandRange(0.0, 1.0);

		if (_perAttack >= .4)
		{
			if (_attackingColleagues.Num() > 0)
			{
				_currentNearest = (_perShort >= .5)
					? UGameplayStatics::FindNearestActor(
						GetActorLocation(), _attackingColleagues, distance)
					: _attackingColleagues[FMath::RandRange(0, _attackingColleagues.Num() - 1)];
			}
			else
			{
				_currentNearest = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, distance);
			}
		}
		else
		{
			if (_defendingColleagues.Num() > 0)
			{
				_currentNearest = (_perShort >= .5)
					? UGameplayStatics::FindNearestActor(
						GetActorLocation(), _defendingColleagues, distance)
					: _defendingColleagues[FMath::RandRange(0, _defendingColleagues.Num() - 1)];
			}
			else
			{
				_currentNearest = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, distance);
			}
		}

		if (_currentNearest == nullptr)
		{
			_currentNearest = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, distance);
		}

		PlayerToPassTo = _currentNearest;
	}
}

void AFootballCharacter::teamHasBall(bool bHome)
{
	if (AFootballGameMode* _gameMode = Cast<AFootballGameMode>(CurrentGameMode))
	{
		if (bHome)
		{
			bTeamHasBall = _gameMode->bTeamHasBallHome;
		}
		else
		{
			bTeamHasBall = _gameMode->bTeamHasBallAway;
		}
	}
}


#pragma region Overlaps

//Detections Overlap Methods

//Possession Overlap Methods

void AFootballCharacter::OnPossessOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	if (AFootball* ball = Cast<AFootball>(OtherActor))
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			if (!Cast<AFootball>(OtherActor)->bIsPosessed)
			{
				Cast<AFootball>(OtherActor)->Possess(this);
			}
		}
	}
}

void AFootballCharacter::OnPossessOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AFootball* ball = Cast<AFootball>(OtherActor))
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			Cast<AFootball>(OtherActor)->UnPossess();
		}
	}
}

#pragma endregion
