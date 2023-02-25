// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MatchSettings.h"
#include "FootballerController.h"
#include "FootballCharacter.h"
#include "FootballMatchInstance.h"
#include "FootballGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LEAGUEJOURNEY_API AFootballGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	AFootballGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//Booleans
	bool bAddedTime = false;

	//Integers
	int CurrentHalf = 1;
	int NumberHalfes = 2;

	//Functions
	void SpawnPawn(FFootballer FootballerStruct, bool isHome);

	void SpawnCamera();

	void SpawnFootball();

public:


	//Football Properties
	UPROPERTY(BlueprintReadOnly, Category = "Camera Properties")
	AStadiumCamera* SpawnedCamera;



	//Football Properties
	UPROPERTY(BlueprintReadOnly, Category = "Football Properties")
	AFootball* SpawnedFootball;



	//Player Properties
	UPROPERTY(BlueprintReadOnly, Category = "Player Properties")
	APlayerController* PC;



	//Teams Properties

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	bool bTeamHasBallHome = false;

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	bool bTeamHasBallAway = false;

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	float attackPercentageHome = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	float attackPercentageAway = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	FTeam teamHome;

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	FTeam teamAway;

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	TArray<APawn*> pawnElevenHome;

	UPROPERTY(BlueprintReadOnly, Category = "Teams Properties")
	TArray<APawn*> pawnElevenAway;



	//Match Properties

	UPROPERTY(BlueprintReadOnly, Category = "Match Properties")
	bool bMatchPaused = false;

	UPROPERTY(BlueprintReadOnly, Category = "Match Properties")
	int Minutes = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Match Properties")
	int AddedTime = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Match Properties")
	float Seconds = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Match Properties")
	AStadiumCamera* MainCamera;



	//Animation

	UPROPERTY(EditAnywhere, Category="Animation Instances")
	UClass* animinstanceGoalkeeper;

	UPROPERTY(EditAnywhere, Category = "Animation Instances")
	UClass* animinstanceDefault;



	//Subclasses

	UPROPERTY(EditAnywhere, Category = "Classes das Variaveis")
	TSubclassOf<AActor> footballClass;

	UPROPERTY(EditAnywhere, Category = "Classes das Variaveis")
	TSubclassOf<AActor> cameraClass;

	UPROPERTY(EditAnywhere, Category = "Classes das Variaveis")
	TSubclassOf<ACharacter> characterClass;



	//DEBUG Variables

	UPROPERTY(EditAnywhere, Category = "Boolean Overrides")
	bool bSpawnMatchUI = false;

	UPROPERTY(EditAnywhere, Category = "Boolean Overrides")
	bool bSpawnTutorialUI = false;

	UFUNCTION()
	void SpawnDebugPlayers();

};
