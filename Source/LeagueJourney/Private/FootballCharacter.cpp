// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballCharacter.h"
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
	if(DebugStringPosition != "")
	{
		stats.Position = DebugStringPosition;
	}
	
	//Acelaração Maxima
	/*GetCharacterMovement()->MaxAcceleration = stats.Acceleration * 20;
	PC = (Cast<AFootballerController>(GetController()))? Cast<AFootballerController>(GetController()) : nullptr;
	BallActor = UGameplayStatics::GetActorOfClass(GetWorld(), AFootball::StaticClass());*/
	
	
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
	PlayerInputComponent->BindAxis("MoveFW");
	PlayerInputComponent->BindAxis("MoveRG");
	PlayerInputComponent->BindAxis("Sprint");
	PlayerInputComponent->BindAction("Tackle/Shot", IE_Pressed, this, &AFootballCharacter::Tackle);
	if(hasBall)
	{
		
	}
	else
	{
		///
		//PlayerInputComponent->BindAction("Tackle/Shot", IE_Released, this, &AFootballCharacter::decideNextMove);
	}
	

}


void AFootballCharacter::Move()
{
	
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

void AFootballCharacter::decideNextMove()
{

	switch (NextMove)
	{
	case 0:
		break;
	case 1:
		
		break;
	case 2:
		break;
	}
}

void AFootballCharacter::Tackle()
{
	if(!isRetarded)
	{
		isTackling = true;
		isRetarded = true;
	}
	
}



