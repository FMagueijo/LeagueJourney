// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FootballerController.h"
#include "Football.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFootballCharacter::AFootballCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AFootballCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if(APlayerController* _PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC = _PC;
		if (UEnhancedInputLocalPlayerSubsystem* _Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(BaseMappingContext, 0);
		}
	}
}

// Called every frame
void AFootballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if(!isRetarded)
	{
		if(isPowering)
		{
			((Power >= 1) ? Power = 1 : false) ? decideNextMove() : Power += .75 * DeltaTime;
			
		}
	}
	else
	{
		isPowering = false;
		Power = 0;
	}*/
	
	/*if(PC)
	{
		(!isRetarded) ? Move() : NULL;
		(isChasingBall) ? Move(BallActor->GetActorLocation()) : NULL;
	}*/

}

// Called to bind functionality to input
void AFootballCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedMove);
	}
	

}

void AFootballCharacter::EnhancedMove(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	if(CurrentValue.Length() > 0)
	{
		FVector CurrentMove = SpawnedCamera->GetActorForwardVector() * CurrentValue.Y + SpawnedCamera->GetActorRightVector() * CurrentValue.X;
		CurrentMove.Z = 0;
		AddMovementInput(CurrentMove);
	}
}


void AFootballCharacter::Move(FVector Where)
{
	GetCharacterMovement()->MaxWalkSpeed = 200 + (GetInputAxisValue("Sprint") * (stats.Pace / 20 * 400));
	GetCharacterMovement()->RotationRate = FRotator(0, 180 + GetInputAxisValue("Sprint") * 180, 0);
	FVector VectorFinal = Where - GetActorLocation();
	AddMovementInput(VectorFinal);
}


void AFootballCharacter::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<AFootball>(OtherActor))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, "In");
		isChasingBall = true;
	}
}

void AFootballCharacter::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AFootball>(OtherActor))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, "Out");
		isChasingBall = false;
	}
}

void AFootballCharacter::OnSsphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AFootball>(OtherActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, "Out");
		isChasingBall = false;

	}
}
