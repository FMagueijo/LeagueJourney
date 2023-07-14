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
#include "Engine/InputDelegateBinding.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFootballCharacter::AFootballCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BallPosessArea = CreateDefaultSubobject<USphereComponent>(FName("BallPosessArea"));
}

void AFootballCharacter::PawnClientRestart()
{
	if (PC && PC->IsLocalController())
	{
		// Handle camera possession
		if (PC->bAutoManageActiveCameraTarget)
		{
			PC->AutoManageActiveCameraTarget(this);
		}

		// Set up player input component, if there isn't one already.
		if (InputComponent == nullptr)
		{
			InputComponent = CreatePlayerInputComponent();
			if (InputComponent)
			{
				SetupPlayerInputComponent(InputComponent);
				InputComponent->RegisterComponent();
				if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
				{
					InputComponent->bBlockInput = bBlockInput;
					UInputDelegateBinding::BindInputDelegatesWithSubojects(this, InputComponent);
				}
			}
		}
	}
}

void AFootballCharacter::Restart()
{
	UPawnMovementComponent* MovementComponent = GetMovementComponent();
	if (MovementComponent)
	{
		//MovementComponent->StopMovementImmediately();
	}
	//ConsumeMovementInputVector();
	//RecalculateBaseEyeHeight();
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
		coverageDistance = 1000 + stats.Goalkeeping / 20 * 1500;
	}
	else
	{
		coverageDistance = 1000 + stats.Defending / 20 * 3000;
	}

	
	if (stats.CurrentPosition == "GK" && bPlaysAtHome)
	{
		const TArray<AActor*> _allColleagues;
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
	
	if(IsValid(CurrentGameMode))
	{
		bIsCharging = (!Cast<AFootballGameMode>(CurrentGameMode)->bCountDown && bCanCharge && bIsCharging);
	}
	if (IsValid(KnownBall))
	{
		if (bHasBall)
		{
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, "bCharge ->" + FString::FromInt(bIsCharging));
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, "Percentage->" + FString::SanitizeFloat(ChargePercentage));
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, "Player to pass to->" + ((PlayerToPassTo) ? PlayerToPassTo->GetName() : "None"));
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, "Move vector->" + moveVector.ToString());
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, "Move Axis->" + moveAxis.ToString());
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, "Sprint Percentage->" + FString::SanitizeFloat(SprintPercentage));
			GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, "Distance to ball->" + FString::SanitizeFloat(FVector2D::Distance(FVector2D(KnownBall->GetActorLocation()), FVector2D(GetActorLocation()))));
		}
		//Ball Detection Check

		bWantsBall = (FVector2D::Distance(FVector2D(KnownBall->GetActorLocation()), FVector2D(GetActorLocation())) <= (500 + coverageDistance * ((!bTeamHasBall) ? 1 : 0)));
		

		if(PC->GetPawn() == this && !bHasBall && FVector2D::Distance(FVector2D(KnownBall->GetActorLocation()), FVector2D(GetActorLocation())) <= 300)
		{
			AddMovementInput(KnownBall->GetActorLocation() + KnownBall->GetVelocity().GetSafeNormal() - GetActorLocation(), 1, true);
		}
	}

	//Find Closest Team Pawn + Add To Charge Percentage
	if(!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		if (bIsCharging)
		{
			FindClosestPawn(bPlaysAtHome);
			ChargePercentage = FMath::Clamp(ChargePercentage += 1.2 * DeltaTime, 0.f, 1.f);
		}
	}
	


	//Decrement Stamina depending on player stats

	if (GetVelocity().Length() > 0)
	{
		stamina = FMath::Clamp(stamina -= (.0005f + (.01f / stats.Stamina)) * DeltaTime, 0.0f, 1.0f);
		
	}


	DrawDebugString(GetWorld(), GetActorLocation(), "Wants Ball : " + FString::SanitizeFloat(bWantsBall), 0,
	                FColor::Red, .01, false, 1);
	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 30,
	                "Team Ball : " + FString::SanitizeFloat(bTeamHasBall), 0, FColor::Yellow, .01, false, 1);
	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 60,
	                "Has Ball : " + FString::SanitizeFloat(bHasBall), 0, FColor::Green, .01, false, 1);

	GetMaxPace();
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

		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedMove);
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedMove);
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Ongoing, this, &AFootballCharacter::EnhancedMove);
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AFootballCharacter::EnhancedMove);

		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedSprint);


		//One Shot Input

		enhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedSwitch);


		//Charging Inputs

		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedCharge);

		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedCharge);

		enhancedInputComponent->BindAction(CrossAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedCharge);

		enhancedInputComponent->BindAction(TackleAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedTackle);

		//Complete Charge

		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedShot);

		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedPass);

		enhancedInputComponent->BindAction(CrossAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedCross);
		
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
	moveAxis = Value.Get<FVector2D>();
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && bCanMove)
	{
		if(SprintPercentage > .7)
		{
			AddMovementInput(GetActorForwardVector() + moveVector * FMath::Clamp((1.0 - SprintPercentage), 0.05, 1.0));
		}
		else
		{
			AddMovementInput(moveVector);
		}
	}
}

void AFootballCharacter::EnhancedTackle(const FInputActionValue& Value)
{
	if (bCanTackle && (!bTeamHasBall) && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		PlayAnimMontage(MontageTackle, 1, EName::None);
		ChargePercentage = 0.0;
		bIsCharging = false;
	}

}

void AFootballCharacter::EnhancedPass(const FInputActionValue& Value)
{
	if (bIsCharging && bHasBall && bTeamHasBall)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "About to pass");
		if ((ChargePercentage >= 1 || !Value.Get<bool>()) && bHasBall && bCanPass && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && !Cast<AFootballGameMode>(CurrentGameMode)->bCountDown)
		{
			bIsCharging = false;
			PlayAnimMontage(MontagePass, 1, EName::None);
		}
	}
	else
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			ChargePercentage = 0.0;
			bIsCharging = false;
		}
	}
}

void AFootballCharacter::EnhancedCross(const FInputActionValue& Value)
{
	if (bIsCharging && bHasBall && bTeamHasBall)
	{
		if ((ChargePercentage >= 1 || !Value.Get<bool>()) && bCanPass && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && !Cast<AFootballGameMode>(CurrentGameMode)->bCountDown)
		{
			bIsCharging = false;
			FPredictProjectilePathParams _params;
			
			PlayAnimMontage(MontageCross, 1, EName::None);
		}
	}
	else
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			ChargePercentage = 0.0;
			bIsCharging = false;
		}
	}
}

void AFootballCharacter::EnhancedShot(const FInputActionValue& Value)
{
	if (bIsCharging && !bKickOff && !bGoalKick && bHasBall && bTeamHasBall)
	{
		if ((ChargePercentage >= 1 || !Value.Get<bool>()) && bHasBall && bTeamHasBall && bCanShoot && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			bIsCharging = false;
			PlayAnimMontage(MontageShot, 1, EName::None);
		}
	}
	else
	{
		if(!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			ChargePercentage = 0.0;
			bIsCharging = false;
		}
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



//Actions

void AFootballCharacter::Shoot()
{
	if (bHasBall && bCanShoot)
	{

		TArray<AActor*> _targets;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), (bPlaysAtHome) ? "awayTarget" : "homeTarget", _targets);
		const AActor* _randTarget = _targets[FMath::RandRange(0, _targets.Num() - 1)];
		float _distance = FVector::Distance(GetActorLocation(), _randTarget->GetActorLocation());

		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, FString::SanitizeFloat(ChargePercentage));

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

			FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation()+moveVector.GetSafeNormal());
			_newRot.Pitch = this->GetActorRotation().Pitch;
			SetActorRotation(_newRot);
			DrawDebugSphere(GetWorld(), GetActorLocation() + moveVector, 50, 16, FColor::Green, false, 15, 0, 20);

			Cast<AFootball>(KnownBall)->Shoot(_distance, GetActorForwardVector(), stats.Shooting, ChargePercentage);
		}

		if(AFootballGameMode* gamemode = Cast<AFootballGameMode>(CurrentGameMode))
		{
			(bPlaysAtHome) ? gamemode->HShots += 1 : gamemode->AShots += 1;
		}

		CheckEvents();
	}
}

void AFootballCharacter::Pass()
{
	if (bHasBall && bCanPass && !Cast<AFootballGameMode>(CurrentGameMode)->bCountDown)
	{
		if(PC->GetPawn() != this)
		{
			if (PlayerToPassTo == nullptr && teamColleagues.Num() > 0)
			{
				ChargePercentage = FMath::FRandRange(0.1, 1.0);
				TArray<AActor*> AllTeamPlayer;

				for (AActor* _plaer : teamColleagues)
				{
					if ((bPlaysAtHome) ? _plaer->GetActorLocation().Y < GetActorLocation().Y : _plaer->GetActorLocation().Y > GetActorLocation().Y)
					{
						AllTeamPlayer.Add(_plaer);
					}
				}

				if (AllTeamPlayer.Num() > 0)
				{
					for (AActor* _plaer : AllTeamPlayer)
					{
						if (Cast<AFootballCharacter>(_plaer)->currentOverall >= currentOverall)
						{
							PlayerToPassTo = _plaer;
							break;
						}
					}
				}
			}

			if (PlayerToPassTo == nullptr)
			{
				if (FMath::RandRange(0.0, 1.0) > .5)
				{
					PlayerToPassTo = teamColleagues[FMath::RandRange(0, teamColleagues.Num() - 1)];
				}
				else
				{
					float x;
					PlayerToPassTo = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, x);
				}
			}
		}
		else
		{
			if(PlayerToPassTo == nullptr)
			{
				FVector _where = GetActorLocation() + moveVector.GetSafeNormal() * (200 + ChargePercentage * (stats.Passing / 20.0 * 4000));
				float x;
				PlayerToPassTo = UGameplayStatics::FindNearestActor(_where, teamColleagues, x);
			}

			
		}

		if(PlayerToPassTo)
		{
			(PC->GetPawn() == this) ? PC->Possess(Cast<APawn>(PlayerToPassTo)) : NULL;
			FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerToPassTo->GetActorLocation());
			_newRot.Pitch = this->GetActorRotation().Pitch;
			SetActorRotation(_newRot);

			Cast<AFootball>(KnownBall)->Pass(PlayerToPassTo, this, stats.Passing, ChargePercentage);


			if (AFootballGameMode* gamemode = Cast<AFootballGameMode>(CurrentGameMode))
			{
				(bPlaysAtHome) ? gamemode->HPass += 1 : gamemode->APass += 1;
			}

			CheckEvents();
		}
		
	}
}

void AFootballCharacter::Cross()
{
	if (bHasBall && bCanPass && !Cast<AFootballGameMode>(CurrentGameMode)->bCountDown)
	{
		if (PC->GetPawn() != this)
		{
			if (PlayerToPassTo == nullptr && teamColleagues.Num() > 0)
			{
				ChargePercentage = FMath::FRandRange(0.1, 1.0);
				TArray<AActor*> AllTeamPlayer;

				for (AActor* _plaer : teamColleagues)
				{
					if ((bPlaysAtHome) ? _plaer->GetActorLocation().Y < GetActorLocation().Y : _plaer->GetActorLocation().Y > GetActorLocation().Y)
					{
						AllTeamPlayer.Add(_plaer);
					}
				}

				if (AllTeamPlayer.Num() > 0)
				{
					for (AActor* _plaer : AllTeamPlayer)
					{
						if (Cast<AFootballCharacter>(_plaer)->currentOverall >= currentOverall)
						{
							PlayerToPassTo = _plaer;
							break;
						}
					}
				}
			}

			if (PlayerToPassTo == nullptr)
			{
				if (FMath::RandRange(0.0, 1.0) > .5)
				{
					PlayerToPassTo = teamColleagues[FMath::RandRange(0, teamColleagues.Num() - 1)];
				}
				else
				{
					float x;
					PlayerToPassTo = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, x);
				}
			}
		}
		else
		{
			if (PlayerToPassTo == nullptr)
			{
				FVector _where = GetActorLocation() + moveVector.GetSafeNormal() * (200 + ChargePercentage * (stats.Passing / 20.0 * 4000));
				float x;
				PlayerToPassTo = UGameplayStatics::FindNearestActor(_where, teamColleagues, x);
			}
		}

		if (PlayerToPassTo)
		{
			(PC->GetPawn() == this) ? PC->Possess(Cast<APawn>(PlayerToPassTo)) : NULL;
			FRotator _newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerToPassTo->GetActorLocation());
			_newRot.Pitch = this->GetActorRotation().Pitch;
			SetActorRotation(_newRot);

			Cast<AFootball>(KnownBall)->Cross(PlayerToPassTo, this, stats.Passing, ChargePercentage);
			CheckEvents();
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
		                                                  50.0 + stats.Defending / 20.0 * 150.0, ObjectTypes, false,
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
					if(Cast<AFootballGameMode>(CurrentGameMode))
					{
						Cast<AFootballGameMode>(CurrentGameMode)->CreateFreeKickEvent(_char);
					}
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

			if (AFootballGameMode* gamemode = Cast<AFootballGameMode>(CurrentGameMode))
			{
				(bPlaysAtHome) ? gamemode->HTackles += 1 : gamemode->ATackles += 1;
			}
		}
	}

	PlayerToPassTo = nullptr;
}

void AFootballCharacter::AddCard()
{
	cardNumber++;
	if(AFootballGameMode* gamemode = Cast<AFootballGameMode>(CurrentGameMode))
	{
		(bPlaysAtHome) ? gamemode->HYellow += 1 : gamemode->AYellow += 1;
	}
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

		if (AFootballGameMode* gamemode = Cast<AFootballGameMode>(CurrentGameMode))
		{
			(bPlaysAtHome) ? gamemode->HRed += 1 : gamemode->ARed += 1;
		}

		//create event

		Destroy();
	}
}

void AFootballCharacter::GetTackled()
{
	if (bHasBall && !bFreeKick)
	{
		KnownBall->UnPossess();
	}
	bIsCharging = false;
	PlayAnimMontage(MontageTackled, .6, EName::None);

}

void AFootballCharacter::GetMaxPace()
{
	if(PC->GetPawn() == this)
	{
		FVector2D CurrentValue = moveAxis;
		CurrentValue.Normalize();
		FVector FinalValue = (SpawnedCamera != nullptr)
			? Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetForwardVector() * CurrentValue.Y
			+ Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetRightVector() * CurrentValue.X
			: CurrentPosition;
		FinalValue.Z = 0;
		moveVector = FinalValue;

		GetCharacterMovement()->MaxWalkSpeed = 300.0 + stamina * (SprintPercentage * (stats.Pace / 20.0 * 450));
		GetCharacterMovement()->RotationRate = FRotator(0, 360 - SprintPercentage * 230, 0);

	}
	else
	{

		GetCharacterMovement()->MaxWalkSpeed = 300.0 + stamina * (((bTeamHasBall) ? 1.0 : 1.5) * (stats.Pace / 20.0 * 450));
		GetCharacterMovement()->RotationRate = FRotator(0, 180 + 1 * 180, 0);
	}
	




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

void AFootballCharacter::CheckEvents()
{

	//Events 
	(bKickOff) ? Cast<AFootballGameMode>(CurrentGameMode)->CreateKickOffEvent(this) : NULL;
	(bGoalKick) ? Cast<AFootballGameMode>(CurrentGameMode)->CreateGoalKickEvent(this) : NULL;
	(bThrowIn) ? Cast<AFootballGameMode>(CurrentGameMode)->CreateThrowInEvent(this) : NULL;
	(bFreeKick) ? Cast<AFootballGameMode>(CurrentGameMode)->CreateFreeKickEvent(this) : NULL;
}

void AFootballCharacter::FindClosestPawn(bool isHome)
{
	const FVector TraceStart = GetActorLocation() + ((moveAxis.Length() > 0) ? moveVector.GetSafeNormal() : GetActorForwardVector()) * (200 + ChargePercentage * (stats.Passing / 20.0 * 12000));; // Start the trace at a distance of 100 units from the player
	const FVector BoxExtent = FVector(200 + ChargePercentage * (stats.Passing / 20.0 * 12000), 600 + (ChargePercentage * (20.0 / stats.Passing * 500)), 200);;
	

	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	}; // Query only pawns
	const TArray<AActor*> ActorsToIgnore = { this }; // Ignore the current player's pawn

	TArray<FHitResult> OutHits;

	TArray<AActor*> _allPlayers;
	UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), TraceStart, TraceStart, BoxExtent, UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), (moveAxis.Length() > 0)? GetActorLocation() + moveVector : GetActorLocation() + GetActorForwardVector()),
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
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && bCanPossess)
		{
			Cast<AFootball>(OtherActor)->UnPossess();
		}
	}
}

#pragma endregion
