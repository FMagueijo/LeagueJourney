// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FootballGameMode.h"
#include "FootballerController.h"
#include "Football.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	BallDetectionArea->OnComponentBeginOverlap.AddDynamic(this, &AFootballCharacter::OnDetectionOverlapBegin);
	BallDetectionArea->OnComponentEndOverlap.AddDynamic(this, &AFootballCharacter::OnDetectionOverlapEnd);
	BallPosessArea->OnComponentBeginOverlap.AddDynamic(this, &AFootballCharacter::OnPosessOverlapBegin);
	if(APlayerController* _PC = Cast<AFootballerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PC = _PC;
		if (UEnhancedInputLocalPlayerSubsystem* _Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_PC->GetLocalPlayer()))
		{
			_Subsystem->AddMappingContext(BaseMappingContext, 5);
		}
	}
	
}



void AFootballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Ball Dettection Condition

	if(AFootballGameMode* AGM = Cast<AFootballGameMode>(CurrentGameMode))
	{
		if(AGM->SpawnedFootball)
		{
			if(!AGM->SpawnedFootball->bIsPosessed)
			{
				BallDetectionArea->SetSphereRadius(BallDetectionArea->GetUnscaledSphereRadius() + 500 * DeltaTime);
			}
			else
			{
				BallDetectionArea->SetSphereRadius(FMath::Lerp(BallDetectionArea->GetUnscaledSphereRadius() + 200 * DeltaTime, 200, 0.5));
			}
		}
	}
	
	//Find Closest Team Pawn

	if(PC->GetPawn() == this)
	{
		FindClosestPawn(bPlaysAtHome);	
	}
	
	//Move towards ball when close enough

	if (!bHasBall && KnownBall != nullptr && PC->GetPawn() == this && moveAxis.Length() == 0)
	{
		MoveTowardsActor(KnownBall);
	}
	
	//Find Closest Team Pawn + Add To Charge Percentage

	if(bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		FindClosestPawn(bPlaysAtHome);
		ChargePercentage = FMath::Clamp(ChargePercentage += 1 * DeltaTime, 0.f, 1.f);
	}

	//Decrement Stamina depending on player stats

	if(moveAxis.Length() > 0){
		stamina = FMath::Clamp(stamina -= (.005f + (1.0f / stats.Stamina) * .015f) * DeltaTime, 0.0f, 1.0f);
		DrawDebugString(GetWorld(), GetActorLocation(), "Current Stamina = " + FString::SanitizeFloat(stamina), 0, FColor::Yellow, .01, false, 2);

	}

	

	DrawDebugString(GetWorld(), GetActorLocation(), "Has Ball : " + FString::SanitizeFloat(bHasBall), 0, FColor::Red, .01, false, 1);
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

		GetCharacterMovement()->MaxWalkSpeed = (FinalValue.Length() > .8) ? (300.0 + stats.Pace / 20.0 * 100.0) + (SprintPercentage * (stats.Pace / 20.0 * 350.0)) : 300;
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
			ChargePercentage = 0;
		}
	}
	else
	{
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
			ChargePercentage = 0;
		}
	}
	else
	{
		bIsCharging = false;
		ChargePercentage = 0;
	}
}



//Actions

void AFootballCharacter::Shoot()
{
	if (KnownBall && bHasBall)
	{
		if(ChargePercentage == 0)
		{
			ChargePercentage = FMath::FRandRange(0.1, 1.0);
		}

		Cast<AFootball>(KnownBall)->Shoot(GetActorForwardVector(), stats.Shooting, ChargePercentage);
	}
	ChargePercentage = 0;
}

void AFootballCharacter::Pass()
{
	if (KnownBall && bHasBall)
	{
		if (ChargePercentage == 0)
		{
			ChargePercentage = FMath::FRandRange(0.1, 1.0);
			FindClosestPawn(bPlaysAtHome);
			if (PlayerToPassTo == nullptr)
			{
				TArray<AActor*> AllTeamPlayer = (bPlaysAtHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway;
				AllTeamPlayer.Remove(this);
				float x;
				PlayerToPassTo = UGameplayStatics::FindNearestActor(GetActorLocation(), AllTeamPlayer, x);

			}
		}

		Cast<AFootball>(KnownBall)->Pass(PlayerToPassTo, this, stats.Passing, ChargePercentage);

	}
	ChargePercentage = 0;
	
}

void AFootballCharacter::Tackle()
{
	if(bHasBall && KnownBall)
	{
		Cast<AFootball>(KnownBall)->Shoot(GetActorForwardVector(), 1, 1);
	}
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



#pragma region Global Methods

void AFootballCharacter::FindClosestPawn(bool isHome)
{
	FVector PassVector = GetActorLocation() + GetActorForwardVector() * (5000 * ChargePercentage);

	DrawDebugSphere(GetWorld(), PassVector, 100, 16, FColor::Red);
	DrawDebugSphere(GetWorld(), PassVector, 300 + ChargePercentage * (stats.Passing/20.0 * 4000), 16, FColor::Blue);
	//DrawDebugLine(GetWorld(), GetActorLocation(), PassVector, FColor::Red, false, -1, 0, 10);
	
	float x = 0;
	if(CurrentGameMode)
	{
		TArray<AActor*> AllTeamPlayer = (isHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway;
		TArray<AActor*> RealAllTeamPlayer;
		AllTeamPlayer.Remove(this);

		for (AActor* clActor : AllTeamPlayer)
		{

			FVector DirectionToPlayer = clActor->GetActorLocation() - PassVector;
			if (DirectionToPlayer.GetSafeNormal().CosineAngle2D(GetActorForwardVector()) > .9 - (ChargePercentage * .9))
			{
				RealAllTeamPlayer.Add(clActor);
			}

		}

		AActor* closeA = UGameplayStatics::FindNearestActor(PassVector, RealAllTeamPlayer, x);
		for (AActor* clActor : RealAllTeamPlayer)
		{

		}

		PlayerToPassTo = closeA;
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
	if(Cast<AFootball>(OtherActor))
	{
		
		KnownBall = Cast<AFootball>(OtherActor);
	}

}

void AFootballCharacter::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AFootball>(OtherActor))
	{
		
		KnownBall = nullptr;
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

			if(!bPlaysAtHome)
			{
				float x = 0;
				AActor* ClosestPawn = UGameplayStatics::FindNearestActor(GetActorLocation(), Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome, x);
				if(PC->GetPawn() != ClosestPawn)
				{
					PC->Possess(Cast<APawn>(ClosestPawn));
				}
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
			
			if (!bPlaysAtHome)
			{
				float x = 0;
				AActor* ClosestPawn = UGameplayStatics::FindNearestActor(GetActorLocation(), Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome, x);
				if (PC->GetPawn() != ClosestPawn)
				{
					PC->Possess(Cast<APawn>(ClosestPawn));
				}
			}
		}
	}
}

#pragma endregion