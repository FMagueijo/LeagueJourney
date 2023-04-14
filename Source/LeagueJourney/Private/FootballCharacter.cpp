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
	BallDetectionArea = CreateDefaultSubobject<USphereComponent>(FName("BallDetectionArea"));
	BallPosessArea = CreateDefaultSubobject<USphereComponent>(FName("BallPosessArea"));
}



// Called when the game starts or when spawned
void AFootballCharacter::BeginPlay()
{
	Super::BeginPlay();

	BallDetectionArea->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	BallPosessArea->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);


	BallPosessArea->OnComponentBeginOverlap.AddDynamic(this, &AFootballCharacter::OnPosessOverlapBegin);

	if(APlayerController* _PC = Cast<AFootballerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PC = _PC;
		if (UEnhancedInputLocalPlayerSubsystem* _Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_PC->GetLocalPlayer()))
		{
			_Subsystem->AddMappingContext(BaseMappingContext, 5);
		}
	}


	//Set default values

	coverageDistance = stats.Defending / 20.0 * 1500;

	TArray<AActor*> _allColleagues;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFootballCharacter::StaticClass(), _allColleagues);
	for(AActor* _footballer : _allColleagues)
	{
		if(Cast<AFootballCharacter>(_footballer)->bPlaysAtHome == bPlaysAtHome)
		{
			teamColleagues.Add(Cast<AFootballCharacter>(_footballer));
		}
	}
	teamColleagues.Remove(this);

}



void AFootballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(KnownBall)
	{
		//Ball Detection Check

		bWantsBall = (FVector::Distance(KnownBall->GetActorLocation(), GetActorLocation()) <= 1000 + coverageDistance * ((bTeamHasBall) ? 1 : 0));
	}
	
	//Find Closest Team Pawn + Add To Charge Percentage

	if(bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		FindClosestPawn(bPlaysAtHome);
		ChargePercentage = FMath::Clamp(ChargePercentage += 1 * DeltaTime, 0.f, 1.f);
	}

	//Decrement Stamina depending on player stats

	if(GetVelocity().Length() > 0){
		stamina = FMath::Clamp(stamina -= (.005f + (1.0f / stats.Stamina) * .005f) * DeltaTime, 0.0f, 1.0f);
		DrawDebugString(GetWorld(), GetActorLocation(), "Current Stamina = " + FString::SanitizeFloat(stamina), 0, FColor::Yellow, .01, false, 2);

	}

	

	DrawDebugString(GetWorld(), GetActorLocation(), "Wants Ball : " + FString::SanitizeFloat(bWantsBall), 0, FColor::Red, .01, false, 1);
	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector*30, "Team Ball : " + FString::SanitizeFloat(bTeamHasBall), 0, FColor::Yellow, .01, false, 1);
	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector*60, "Has Ball : " + FString::SanitizeFloat(bHasBall), 0, FColor::Green, .01, false, 1);
	

	//Team has ball?
	teamHasBall(bPlaysAtHome);
}


//Input Handler

void AFootballCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{

		//Always available Actions

		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedMove);

		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedSprint);


		//One Shot Input

		enhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedSwitch);

		enhancedInputComponent->BindAction(TackleAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedTackle);

		//Charging Inputs

		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedCharge);

		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedCharge);


		//Complete Charge

		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedShot);

		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedPass);


		
	}

}


//Enhanced Actions

void AFootballCharacter::EnhancedSprint(const FInputActionValue& Value)
{
	SprintPercentage = Value.Get<float>();
}

void AFootballCharacter::EnhancedCharge(const FInputActionValue& Value)
{
	if (!bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Charign");
		PlayerToPassTo = nullptr;
		bIsCharging = Value.Get<bool>();
	}
}

void AFootballCharacter::EnhancedMove(const FInputActionValue& Value)
{
	if(!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		FVector2D CurrentValue = Value.Get<FVector2D>();
		moveAxis = CurrentValue;
		CurrentValue.Normalize();
		FVector FinalValue = (SpawnedCamera != nullptr) ? Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetForwardVector() * CurrentValue.Y + Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetRightVector() * CurrentValue.X : CurrentPosition;
		FinalValue.Z = 0;

		GetCharacterMovement()->MaxWalkSpeed = (FinalValue.Length() > .8) ? 300.0 + stamina * (SprintPercentage * (stats.Pace / 20.0 * 250.0)) : 300;
		GetCharacterMovement()->RotationRate = (FinalValue.Length() > .8) ? FRotator(0, 180 + SprintPercentage * 180, 0) : FRotator::ZeroRotator;


		FinalValue.Normalize();
		AddMovementInput(FinalValue);
	}
}

void AFootballCharacter::EnhancedTackle(const FInputActionValue& Value)
{
	if (!bHasBall && !bTeamHasBall && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
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
	if (bIsCharging && bHasBall && bTeamHasBall)
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
	if (!bHasBall)
	{
		TArray<AActor*> AllTeamPlayer = (bPlaysAtHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway;
		AllTeamPlayer.Remove(this);
		float x;
		AActor* _newChar = UGameplayStatics::FindNearestActor(KnownBall->GetActorLocation(), AllTeamPlayer, x);
		PC->Possess(Cast<APawn>(_newChar));
		PlayerToPassTo = nullptr;
		bIsCharging = false;
		ChargePercentage = 0;
	}
}

void AFootballCharacter::EnhancedShot(const FInputActionValue& Value)
{
	if (bIsCharging && bHasBall && bTeamHasBall)
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
	if (bHasBall)
	{
		if(ChargePercentage == 0)
		{
			ChargePercentage = FMath::FRandRange(0.1, 1.0);
			TArray<AActor*> _targets;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), "homeTarget", _targets);
			Cast<AFootball>(KnownBall)->Shoot(false, _targets[FMath::RandRange(0, _targets.Num()-1)]->GetActorLocation()-GetActorLocation(), stats.Shooting, ChargePercentage);
		}
		else
		{
			Cast<AFootball>(KnownBall)->Shoot(false, GetActorForwardVector(), stats.Shooting, ChargePercentage);
		}
	}

}

void AFootballCharacter::Pass()
{
	if (bHasBall)
	{
		if (ChargePercentage == 0 || PlayerToPassTo == nullptr)
		{
			ChargePercentage = FMath::FRandRange(0.1, 1.0);
			TArray<AActor*> AllTeamPlayer = (bPlaysAtHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway;
			AllTeamPlayer.Remove(this);
			float x;
			
			if (FMath::RandRange(0.0, 1.0) > 50)
			{

				PlayerToPassTo = AllTeamPlayer[FMath::RandRange(0, AllTeamPlayer.Num() - 1)];
			}
			else
			{
				PlayerToPassTo = UGameplayStatics::FindNearestActor(GetActorLocation(), AllTeamPlayer, x);
			}
		}

		Cast<AFootball>(KnownBall)->Pass(PlayerToPassTo, this, stats.Passing, ChargePercentage);

	}
	
}

void AFootballCharacter::Tackle()
{
	if(!bHasBall)
	{
		
		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) }; // Query only pawns
		const TArray<AActor*> ActorsToIgnore = { this }; // Ignore the current player's pawn
		FHitResult OutHit;
		UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), GetMesh()->GetSocketLocation("ballSocket"), GetMesh()->GetSocketLocation("ballSocket"), 150.0 + stats.Defending / 20.0 * 300.0, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHit, true);

		if(AFootballCharacter* _char = Cast<AFootballCharacter>(OutHit.GetActor()))
		{
			//Doesnt play in the same team
			if(_char->bPlaysAtHome != bPlaysAtHome)
			{
				//base value of 20% successful tackle
				if(FMath::FRandRange(0.0, 1.0) >= .2 + .035 * stats.Defending)
				{

					//base value of 20% successful non foul or 50% if no ball
					if (FMath::FRandRange(0.0, 1.0) >= .2 + .035 * stats.Defending || (!_char->bHasBall && FMath::FRandRange(0.0, 1.0) >= .5))
					{
						AddCard();

						//TODO: add free-kick
					}
					else
					{
						//base value of 50% successful ball possession
						if (FMath::FRandRange(0.0, 1.0) >= .5)
						{
							KnownBall->Possess(this);
						}
					}

					_char->GetTackled();
				}
				if (FMath::RandRange(0.0, 1.0) > _char->stats.Defending * 0.01)
				{
					//This will find if he has ball and play his fall animation

					

					//Add card

					if (FMath::RandRange(0.0, 1.0) > stats.Defending * 0.045 || !_char->bHasBall)
					{
					}
					else
					{
						
					}
				}
			}
			
			
		}
	}

	PlayerToPassTo = nullptr;
}

void AFootballCharacter::MoveBallPoint(){
	
}

void AFootballCharacter::MoveTowardsActor(AActor* _actor)
{
	FVector MoveToVector =  _actor->GetActorLocation() - GetActorLocation();
	GetCharacterMovement()->MaxWalkSpeed = (MoveToVector.Length() > .8) ? (200 + stats.Pace / 20 * 100) + (SprintPercentage * (stats.Pace / 20 * 200)) : 200;
	GetCharacterMovement()->RotationRate = (MoveToVector.Length() > .8) ? FRotator(0, 180 + SprintPercentage * 180, 0) : FRotator::ZeroRotator;

	AddMovementInput(MoveToVector);
}

void AFootballCharacter::AddCard()
{
	cardNumber++;
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Yellow");

	if(cardNumber == 2)
	{
		float x = 0;
		AActor* closeA = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, x);

		(bPlaysAtHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome.Remove(this) : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway.Remove(this);
		(bPlaysAtHome) ? PC->Possess(Cast<APawn>(closeA)) : NULL;

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

	(FMath::FRandRange(0.0, 1.0) >= .3) ? PlayAnimMontage(MontageTackled, .75, EName::None) : NULL;
}


#pragma region Global Methods

void AFootballCharacter::FindClosestPawn(bool isHome)
{
	
	if (PC->GetPawn() == this)
	{
		const FVector TraceStart = GetActorLocation() + GetActorForwardVector() * (200 + ChargePercentage * (stats.Passing / 20.0 * 4000)); // Start the trace at a distance of 100 units from the player
		const FVector BoxExtent = FVector(200 + ChargePercentage * (stats.Passing / 20.0 * 4000), 600 - (ChargePercentage * (20.0 / stats.Passing * 25.0)), 200); // Set the dimensions of the box

		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) }; // Query only pawns
		const TArray<AActor*> ActorsToIgnore = { this }; // Ignore the current player's pawn

		TArray<FHitResult> OutHits;

		TArray<AActor*> _allPlayers;
		UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), TraceStart, TraceStart, BoxExtent, GetActorRotation(), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHits, true);
		

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

		if(_perAttack >= .4)
		{
			if (_attackingColleagues.Num() > 0)
			{
				_currentNearest = (_perShort >= .5) ? UGameplayStatics::FindNearestActor(GetActorLocation(), _attackingColleagues, distance) : _attackingColleagues[FMath::RandRange(0, _attackingColleagues.Num()-1)];
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
				_currentNearest = (_perShort >= .5) ? UGameplayStatics::FindNearestActor(GetActorLocation(), _defendingColleagues, distance) : _defendingColleagues[FMath::RandRange(0, _defendingColleagues.Num() - 1)];
			}
			else
			{
				_currentNearest = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, distance);
			}
		}

		if(_currentNearest == nullptr)
		{
			_currentNearest = UGameplayStatics::FindNearestActor(GetActorLocation(), teamColleagues, distance);
		}

		PlayerToPassTo =  _currentNearest;
	}

	
	
	
}

bool AFootballCharacter::IsActorBehind(AActor* actor0, AActor* actor1)
{
	FVector Actor1Location = actor0->GetActorLocation();
	FVector Actor1Forward = actor0->GetActorForwardVector();

	FVector Actor2Location = actor1->GetActorLocation();
	FVector Actor2Forward = actor1->GetActorForwardVector();

	FVector Actor2ToActor1 = Actor1Location - Actor2Location;

	Actor1Forward.Normalize();
	Actor2Forward.Normalize();

	float DotProduct = FVector::DotProduct(Actor2ToActor1, Actor2Forward);
	if (DotProduct < 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}


}

void AFootballCharacter::teamHasBall(bool bHome)
{
	if(AFootballGameMode* _gameMode = Cast<AFootballGameMode>(CurrentGameMode)){
		if(bHome)
		{
			bTeamHasBall = _gameMode->bTeamHasBallHome;
		}
		else
		{
			bTeamHasBall = _gameMode->bTeamHasBallAway;
		}
	}
}

#pragma endregion

#pragma region Overlaps

//Detections Overlap Methods

void AFootballCharacter::OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == KnownBall)
	{
		bWantsBall = true;
	}

}

void AFootballCharacter::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == KnownBall)
	{
		bWantsBall = false;
	}
}

//Possession Overlap Methods

void AFootballCharacter::OnPosessOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFootball* ball = Cast<AFootball>(OtherActor))
	{
		if(!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			if(!Cast<AFootball>(OtherActor)->bIsPosessed)
			{
				Cast<AFootball>(OtherActor)->Possess(this);
			}
			
		}
	}
}

void AFootballCharacter::OnPosessOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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