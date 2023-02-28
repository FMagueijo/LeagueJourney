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
		if (UEnhancedInputLocalPlayerSubsystem* _Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_PC->GetLocalPlayer()))
		{
			_Subsystem->AddMappingContext(BaseMappingContext, 5);
		}
	}
	
}

void AFootballCharacter::FindClosestPawn(bool isHome)
{
	FVector PassVector = GetActorLocation() + GetActorForwardVector() * (5000 * ChargePercentage);

	DrawDebugSphere(GetWorld(), PassVector, 100, 16, FColor::Red);
	DrawDebugLine(GetWorld(), GetActorLocation(), PassVector, FColor::Red, false, -1, 0, 10);
	
	float x = 0;

	TArray<AActor*> AllTeamPlayer = (isHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway;
	AllTeamPlayer.Remove(this);
	AActor* ClosestPawn = UGameplayStatics::FindNearestActor(PassVector, AllTeamPlayer, x);

	PlayerToPassTo = ClosestPawn;
	
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::SanitizeFloat(stats.Passing / 20.0 * 2000.0) + " - > " + ((PlayerToPassTo) ? PlayerToPassTo->GetName() : "None Found"));

}

void AFootballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bHasBall && KnownBall != nullptr)
	{
		ChaseBall(KnownBall);
	}

	if(bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		FindClosestPawn(bPlaysAtHome);
		ChargePercentage = FMath::Clamp(ChargePercentage += 1 * DeltaTime, 0.f, 1.f);
	}

}

void AFootballCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedMove);
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedSprint);

		
		
		//Charging Inputs
		enhancedInputComponent->BindAction(TackleAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedCharge);
		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Started, this, &AFootballCharacter::EnhancedCharge);
		//Charging Input Complete
		enhancedInputComponent->BindAction(TackleAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedShot);
		enhancedInputComponent->BindAction(PassAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedPass);
	}
	

}


void AFootballCharacter::EnhancedTackle(const FInputActionValue& Value)
{

	
}

void AFootballCharacter::EnhancedPass(const FInputActionValue& Value)
{
	if (bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		
		if (ChargePercentage >= 1 || !Value.Get<bool>())
		{
			bIsCharging = false;
			PlayAnimMontage(MontagePass, 1, EName::None);
			ChargePercentage = 0;
		}
	}
}

void AFootballCharacter::EnhancedShot(const FInputActionValue& Value)
{
	if (bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		if (ChargePercentage >= 1 || !Value.Get<bool>())
		{
			bIsCharging = false;
			PlayAnimMontage(MontageShot, 1, EName::None);
		}
	}
}

void AFootballCharacter::EnhancedSprint(const FInputActionValue& Value)
{
	SprintPercentage = Value.Get<float>();
}


void AFootballCharacter::EnhancedCharge(const FInputActionValue& Value)
{
	if (!bIsCharging && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		PlayerToPassTo = nullptr;
		bIsCharging = Value.Get<bool>();
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Charging");
	}
}

void AFootballCharacter::EnhancedMove(const FInputActionValue& Value)
{
	FVector2D CurrentValue = Value.Get<FVector2D>();
	CurrentValue.Normalize();
	FVector FinalValue = (SpawnedCamera != nullptr) ? Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetForwardVector() * CurrentValue.Y + Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetRightVector() * CurrentValue.X : CurrentPosition;
	FinalValue.Z = 0;

	GetCharacterMovement()->MaxWalkSpeed = (FinalValue.Length() > .8) ? (300 + stats.Pace / 20 * 100) + (SprintPercentage * (stats.Pace / 20 * 350)) : 300;
	GetCharacterMovement()->RotationRate = (FinalValue.Length() > .8) ? FRotator(0, 180 + SprintPercentage * 180, 0) : FRotator::ZeroRotator;


	FinalValue.Normalize();
	AddMovementInput(FinalValue);
}





void AFootballCharacter::Shoot()
{
	if (KnownBall && bHasBall)
	{
		if(ChargePercentage == 0)
		{
			ChargePercentage = FMath::FRandRange(0.1, 1.0);
		}
		Cast<AFootball>(KnownBall)->DaddyPawn = nullptr;
		Cast<AFootball>(KnownBall)->bIsPosessed = false;
		Cast<AFootball>(KnownBall)->Com_Collision->SetSimulatePhysics(true);
		FVector ShootVector = GetActorForwardVector() * (700.0 + ( ChargePercentage * (stats.Shooting / 20.0 * 1800.0)));
		ShootVector.Z = (350 * ChargePercentage);
		Cast<AFootball>(KnownBall)->Com_Collision->AddImpulse(ShootVector, EName::None, false);
		bHasBall = false;
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
			if(PlayerToPassTo == nullptr)
			{
				TArray<AActor*> AllTeamPlayer = (bPlaysAtHome) ? Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenHome : Cast<AFootballGameMode>(CurrentGameMode)->pawnElevenAway;
				AllTeamPlayer.Remove(this);
				float x;
				PlayerToPassTo = UGameplayStatics::FindNearestActor(GetActorLocation(), AllTeamPlayer, x);
				
			}
		}
		Cast<AFootball>(KnownBall)->DaddyPawn = nullptr;
		Cast<AFootball>(KnownBall)->bIsPosessed = false;
		Cast<AFootball>(KnownBall)->Com_Collision->SetSimulatePhysics(true);
		
		FVector PassVector = PlayerToPassTo->GetActorLocation() - KnownBall->GetActorLocation();

		if(PlayerToPassTo != nullptr)
		{
			PassVector = PlayerToPassTo->GetActorLocation() - KnownBall->GetActorLocation();
			FRotator lookAtPass = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerToPassTo->GetActorLocation());
			SetActorRotation(FRotator(0, lookAtPass.Yaw, 0));
		}
		else
		{
			PassVector = GetActorForwardVector();
		}
		PassVector.Normalize();
		PassVector *= (700.0 + (ChargePercentage * (stats.Passing / 20.0 * 1800.0)));
		PassVector.Z = (250 * ChargePercentage);
		GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, PassVector.ToString());
		Cast<AFootball>(KnownBall)->Com_Collision->AddImpulse(PassVector, EName::None, false);
		bHasBall = false;
	}
	ChargePercentage = 0;
}

void AFootballCharacter::ChaseBall(AActor* ball)
{
	FVector MoveToVector =  ball->GetActorLocation() - GetActorLocation();

	GetCharacterMovement()->MaxWalkSpeed = (MoveToVector.Length() > .8) ? (200 + stats.Pace / 20 * 100) + (SprintPercentage * (stats.Pace / 20 * 200)) : 200;
	GetCharacterMovement()->RotationRate = (MoveToVector.Length() > .8) ? FRotator(0, 180 + SprintPercentage * 180, 0) : FRotator::ZeroRotator;

	AddMovementInput(MoveToVector);
}

void AFootballCharacter::OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<AFootball>(OtherActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Les go");
		KnownBall = Cast<AFootball>(OtherActor);
	}

}

void AFootballCharacter::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AFootball>(OtherActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Les a not a go");
		KnownBall = nullptr;
	}
}

void AFootballCharacter::OnPosessOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFootball* ball = Cast<AFootball>(OtherActor))
	{
		if(!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
		{
			ball->DaddyPawn = this;
			ball->bIsPosessed = true;
			ball->Com_Collision->SetSimulatePhysics(false);
			bHasBall = true;
			
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
			ball->DaddyPawn = nullptr;
			ball->bIsPosessed = false;
			ball->Com_Collision->SetSimulatePhysics(true);
			bHasBall = false;
		}
	}
}


