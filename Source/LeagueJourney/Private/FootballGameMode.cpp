// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballGameMode.h"
#include "FootballMatchInstance.h"
#include "FootballMatchOptions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"

AFootballGameMode::AFootballGameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AFootballGameMode::BeginPlay()
{
	Super::BeginPlay();


	SpawnFootball();
	SpawnCamera();

	if(UFootballMatchInstance * matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
	{
		teamHome = matchInstance->H_Team;
		teamAway = matchInstance->A_Team;

		if (matchInstance->H_SXI.Num() != 11 || matchInstance->A_SXI.Num() != 11)
		{
			SpawnDebugPlayers();
		}
		else
		{
			for (FFootballer _jogador : matchInstance->H_SXI)
			{
				SpawnPawn(_jogador, true);
			}
			for (FFootballer _jogador : matchInstance->A_SXI)
			{
				SpawnPawn(_jogador, false);
			}
		}
	}
	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	float distance;
	AActor* _closest = UGameplayStatics::FindNearestActor(SpawnedFootball->GetActorLocation(), pawnElevenHome, distance);
	CreateKickOffEvent(Cast<AFootballCharacter>(_closest));
	
}


void AFootballGameMode::CreateKickOffEvent(AFootballCharacter* whoStarts)
{
	if (whoStarts->bKickOff)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Kick OFF");
		SetAllActions(true);
		whoStarts->bKickOff = false;
		bMatchPaused = false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Creted Kick OFF");

		bMatchPaused = true;
		SetAllActions(false);
		whoStarts->SetActorLocation(FVector(0, 0, 95));
		SpawnedFootball->Possess(whoStarts);
		whoStarts->bKickOff = true;
		whoStarts->bCanPass = true;
		whoStarts->bCanCharge = true;
		if(whoStarts->bPlaysAtHome)
		{
			PC->Possess(whoStarts);
		}
	}
}



void AFootballGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!bMatchPaused)
	{
		if (SpawnedFootball->bIsPosessed)
		{

			float A_HomeValue = ((2 * PC->GetPawn()->GetActorLocation().Y) / (Cast<AFootballCharacter>(PC->GetPawn())->CurrentPosition.Y - 4000))+.5;


			float A_AwayValue = (attackPercentageAway += DeltaSeconds / 2);

			float D_HomeValue = 2 - A_AwayValue;

			float D_AwayValue = 2 - A_HomeValue;

			(bTeamHasBallHome) ? HomePoss += DeltaSeconds : AwayPoss += DeltaSeconds;
			TotalPoss = HomePoss + AwayPoss;

			PerHomePoss = HomePoss / TotalPoss;
			PerAwayPoss = AwayPoss / TotalPoss;

			attackPercentageHome = (bTeamHasBallHome) ? FMath::Clamp(A_HomeValue, 0.0, 2.0) : FMath::Clamp(D_HomeValue, 0.0, 2.0);
			attackPercentageAway = (bTeamHasBallAway) ? FMath::Clamp(A_AwayValue, 0.0, 2.0) : FMath::Clamp(D_AwayValue, 0.0, 2.0);
		}
		else
		{

			const float N_HomeValue = (attackPercentageHome -= DeltaSeconds / 2);
			const float N_AwayValue = (attackPercentageAway -= DeltaSeconds / 2);

			attackPercentageHome = FMath::Clamp(N_HomeValue, 0.0f, 2.0f);
			attackPercentageAway = FMath::Clamp(N_AwayValue, 0.0f, 2.0f);
		}
	}
	
	
	// interesting
	// attackPercentageHome = ;


	
}




void AFootballGameMode::SpawnCamera()
{
	if(cameraClass)
	{
		FTransform T_Camera;

		T_Camera.SetRotation(FQuat(FRotator(0, 180, 0)));
		T_Camera.SetLocation(FVector(0, 0, 0));
		AStadiumCamera* NewCamera = GetWorld()->SpawnActorDeferred<AStadiumCamera>(cameraClass, T_Camera);
		NewCamera->SpawnedFootball = SpawnedFootball;
		NewCamera->FinishSpawning(T_Camera, false);
		SpawnedCamera = NewCamera;
	}
}


void AFootballGameMode::ClockLogic()
{
	
}


#pragma region Odd Methods
//PRIVATE FUNCTIONS

void AFootballGameMode::SpawnFootball()
	{
		if(footballClass)
		{
			FTransform T_Football;

			T_Football.SetRotation(FQuat(FRotator::ZeroRotator));
			T_Football.SetLocation(FVector(0, 0, 50));
			AFootball* NewFootball = GetWorld()->SpawnActorDeferred<AFootball>(footballClass, T_Football);
			NewFootball->FinishSpawning(T_Football, false);
			SpawnedFootball = NewFootball;
		}
	}

void AFootballGameMode::SpawnPawn(FFootballer FootballerStruct, bool isHome)
	{
		if (characterClass)
		{
			if (UFootballMatchInstance* matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
			{
				FVector CurrentPosition = (isHome) ? matchInstance->AllHomePositions.FindRef(FootballerStruct.CurrentPosition) : matchInstance->AllAwayPositions.FindRef(FootballerStruct.CurrentPosition);

				FTransform T_NewPawn;

				T_NewPawn.SetRotation(FQuat(FRotator(0, (isHome) ? 270 : 90, 0)));
				T_NewPawn.SetLocation(CurrentPosition);

				AFootballCharacter* NewPawn = GetWorld()->SpawnActorDeferred<AFootballCharacter>(characterClass, T_NewPawn);
				
				NewPawn->SpawnedCamera = SpawnedCamera;
				NewPawn->CurrentGameMode = this;
				NewPawn->stats = FootballerStruct;
				NewPawn->CurrentPosition = CurrentPosition;
				NewPawn->bPlaysAtHome = isHome;
				NewPawn->KnownBall = SpawnedFootball;

				if (animinstanceDefault && animinstanceGoalkeeper && UKismetStringLibrary::EqualEqual_StriStri("GK", FootballerStruct.CurrentPosition))
				{
					NewPawn->GetMesh()->SetAnimClass(animinstanceGoalkeeper);
					NewPawn->AIControllerClass = GoalkeeperAIClass;
				}

				NewPawn->FinishSpawning(T_NewPawn, false);

				(isHome) ? pawnElevenHome.Add(NewPawn) : pawnElevenAway.Add(NewPawn);
			}
		}
	}

void AFootballGameMode::ReSpawn()
{
	if(pawnElevenHome.Num() > 0 && pawnElevenAway.Num() > 0)
	{
		SpawnedFootball->UnPossess();
		SpawnedFootball->LastDaddyPawn = nullptr;
		SpawnedFootball->Com_Collision->SetAllPhysicsLinearVelocity(FVector(0, 0, 0));
		SpawnedFootball->Com_Collision->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
		SpawnedFootball->SetActorLocation(FVector(0, 0, 25), false, nullptr);

		if(UFootballMatchInstance* matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
		{
			if(pawnElevenHome.Num() > 4 && pawnElevenAway.Num() > 4)
			{
				for (AActor* _pawn : pawnElevenHome)
				{
					Cast<AFootballCharacter>(_pawn)->StopAnimMontage();
					FVector CurrentPosition = matchInstance->AllHomePositions.FindRef(Cast<AFootballCharacter>(_pawn)->stats.CurrentPosition);
					CurrentPosition.Z = 96.638264;
					FTransform T_NewPawn;

					T_NewPawn.SetRotation(FQuat(FRotator(0, 270, 0)));
					T_NewPawn.SetLocation(CurrentPosition);

					_pawn->SetActorTransform(T_NewPawn, false, nullptr);
				}

				for (AActor* _pawn : pawnElevenAway)
				{
					Cast<AFootballCharacter>(_pawn)->StopAnimMontage();
					FVector CurrentPosition = matchInstance->AllAwayPositions.FindRef(Cast<AFootballCharacter>(_pawn)->stats.CurrentPosition);
					CurrentPosition.Z = 96.638264;
					FTransform T_NewPawn;

					T_NewPawn.SetRotation(FQuat(FRotator(0, 90, 0)));
					T_NewPawn.SetLocation(CurrentPosition);

					_pawn->SetActorTransform(T_NewPawn, false, nullptr);
				}


				ChooseStartingPawn();
			}

			attackPercentageHome = 0;
			attackPercentageAway = 0;
			
		}

	}
}

void AFootballGameMode::ChooseStartingPawn()
{
	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	PC->UnPossess();
	for (AActor* actor : pawnElevenHome)
	{
		if (Cast<AFootballCharacter>(actor)->stats.CurrentPosition == "ST")
		{
			PC->Possess(Cast<APawn>(actor));
		}
		else
		{
			if (Cast<AFootballCharacter>(actor)->stats.CurrentPosition == "LST" || Cast<AFootballCharacter>(actor)->stats.CurrentPosition == "RST")
			{
				PC->Possess(Cast<APawn>(actor));
			}
		}
	}

	if (PC->GetPawn() == nullptr)
	{
		PC->Possess(Cast<APawn>(pawnElevenHome[0]));
	}

	PC->GetPawn()->Controller = PC;

}

void AFootballGameMode::SetAllActions(bool _action)
{
	for (AActor* _foot : pawnElevenHome)
	{
		if (AFootballCharacter* _footer = Cast<AFootballCharacter>(_foot))
		{
			_footer->bCanSwitch = _action;
			_footer->bCanPass = _action;
			_footer->bCanTackle = _action;
			_footer->bCanMove = _action;
			_footer->bCanShoot = _action;
			_footer->bCanCharge = _action;
		}
	}

	for (AActor* _foot : pawnElevenAway)
	{
		if (AFootballCharacter* _footer = Cast<AFootballCharacter>(_foot))
		{
			_footer->bCanSwitch = _action;
			_footer->bCanPass = _action;
			_footer->bCanTackle = _action;
			_footer->bCanMove = _action;
			_footer->bCanShoot = _action;
			_footer->bCanCharge = _action;
		}
	}
}

//DEBUG ONLY

	void AFootballGameMode::SpawnDebugPlayers()
	{
		if (characterClass)
		{
			if (UFootballMatchInstance* matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
			{
				FFootballer _stats;
				TArray<FVector> allPos;
				matchInstance->AllHomePositions.GenerateValueArray(allPos);
				if (!allPos.IsEmpty())
				{
					for (FVector _V : allPos)
					{

						FTransform transPositionHome;
						transPositionHome.SetRotation(FQuat(FRotator(0, 270, 0)));
						transPositionHome.SetLocation(_V);
						AFootballCharacter* theNewchar = GetWorld()->SpawnActorDeferred<AFootballCharacter>(characterClass, transPositionHome);
						theNewchar->CurrentPosition = _V;
						theNewchar->stats = DefaultStats;
						GEngine->AddOnScreenDebugMessage(-1, 150, FColor::Purple, SpawnedCamera->GetName());
						theNewchar->SpawnedCamera = SpawnedCamera;
						(animinstanceDefault) ? theNewchar->GetMesh()->SetAnimClass(animinstanceDefault) : nullptr;
						theNewchar->FinishSpawning(transPositionHome, false);
						pawnElevenHome.Add(theNewchar);
					}
				}
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::FromInt(allPos.Num()));

			}
		}
	}
#pragma endregion