// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballGameMode.h"
#include "FootballMatchInstance.h"
#include "FootballMatchOptions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"

AFootballGameMode::AFootballGameMode()
{
	
}

void AFootballGameMode::BeginPlay()
{
	Super::BeginPlay();


	SpawnFootball();
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
	PC->Possess(pawnElevenHome[pawnElevenHome.Num() - 1]);

	SpawnCamera();
}

void AFootballGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}




void AFootballGameMode::SpawnCamera()
{
	if(cameraClass)
	{
		FTransform local_Transform;
		local_Transform.SetRotation(FQuat(FRotator(0, 180, 0)));
		MainCamera = GetWorld()->SpawnActorDeferred<AStadiumCamera>(cameraClass, local_Transform);
		if (MainCamera)
		{
			UGameplayStatics::FinishSpawningActor(MainCamera, local_Transform);

		}
	}
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
			AActor* NewFootball = GetWorld()->SpawnActorDeferred<AFootball>(footballClass, T_Football);
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

				if (animinstanceDefault && animinstanceGoalkeeper)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FootballerStruct.CurrentPosition);
					( !UKismetStringLibrary::EqualEqual_StriStri("GK", FootballerStruct.CurrentPosition) ) ? NewPawn->GetMesh()->SetAnimClass(animinstanceDefault) : NewPawn->GetMesh()->SetAnimClass(animinstanceGoalkeeper);
				}

				NewPawn->FinishSpawning(T_NewPawn, false);

				(isHome) ? pawnElevenHome.Add(NewPawn) : pawnElevenAway.Add(NewPawn);
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