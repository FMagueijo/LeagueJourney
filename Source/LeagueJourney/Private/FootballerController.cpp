// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballerController.h"

#include "ImageUtils.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameManager.h"
#include "Football.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"

AFootballerController::AFootballerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bAutoManageActiveCameraTarget = false;

	
}

void AFootballerController::BeginPlay()
{
	Super::BeginPlay();
	/*FTransform local_FTTransform;
	local_FTTransform.SetLocation(FVector::Zero());

	Bola = UGameplayStatics::GetActorOfClass(GetWorld(), AFootball::StaticClass());
	

	FFootballer local_stats;
	local_stats.Acceleration = 20;
	local_stats.Pace = 20;
	for(int i = 0; i < 11; i++)
	{
		SpawnPawnFootballer(i, local_stats);
	}

	BrodCamera->PlayerPrincipal = TeamPlayers[1];
	ControlledPlayer = Cast<AFootballCharacter>(TeamPlayers[1]);
	this->Possess(TeamPlayers[1]);

	if(AGameManager::countriesGetter.Num() == 1)
	{
		AGameManager::fLoadJson();
	}

	CreateMatch();*/
}


void AFootballerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/*DrawDebugSphere(GetWorld(), ControlledPlayer->GetActorLocation() + SpecialAxis*10, 25, 16, FColor::Magenta);
	(!ControlledPlayer->isRetarded) ? Move() : NULL;
	(ControlledPlayer->isChasingBall && !ControlledPlayer->isRetarded) ? Move(Cast<AFootballCharacter>(GetPawn())->BallActor->GetActorLocation()) : NULL;

	if (ControlledPlayer->isChasingBall && !Cast<AFootball>(Bola)->isTaken && UKismetMathLibrary::Vector_Distance(ControlledPlayer->GetActorLocation(), Bola->GetActorLocation()) < 100)
	{
		ControlledPlayer->hasBall = true;
		Cast<AFootball>(Bola)->isTaken = true;
		Cast<AFootball>(Bola)->WhoHasBall = this->GetPawn();
		ControlledPlayer->isChasingBall = false;
		ControlledPlayer->isRetarded  = false;
	}
	


	if(isPowering)
	{
		power += .8 * DeltaSeconds;
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, FString::SanitizeFloat(power));
		if (power >= 1)
		{
			DecideNextMove();
		}
	}
	*/
}

void AFootballerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	/*if (InputComponent)
	{
		InputComponent->BindAxis("MoveFW");
		InputComponent->BindAxis("MoveRG");
		InputComponent->BindAxis("SpecialAxisRG");
		InputComponent->BindAxis("SpecialAxisFW");
		InputComponent->BindAxis("Sprint");

	}*/
}
