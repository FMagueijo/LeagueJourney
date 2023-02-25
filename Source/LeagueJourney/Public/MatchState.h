// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "MatchState.generated.h"

/**
 * 
 */
UCLASS()
class LEAGUEJOURNEY_API AMatchState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	AMatchState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	FText ClockLogic();
	bool isBeingPlayed = false;

public:


	//Home Properties
	float Home_AttackPercentage = 0;



	int CurrentHalf = 1;
	int NumberHalfes = 2;
	int Minutes = 0;
	float Seconds = 0;
	int AddedTime = 0;
	bool bOnAddedTime = false;
	bool bIsPlaying = true;

	//(Nome, Minuto)
	TMap<FString, int> Scorers;

	//0-10 -> StartXI 11-17 -> Bench
	TArray<FFootballer> HomeLineUp;
	TArray<FFootballer> AwayLineUp;

	//0->Home 1->Away
	TArray<FTeam> Teams;

	FVector2D MatchScore = FVector2D(0,0);
	FVector2D MatchTime = FVector2D(0, 0);
	FText MatchTimeTxt;

	int addedTime = 0;
	int endTime = 0;
	UPROPERTY(EditAnywhere)
	float MatchSpeed = 25.f;

	UPROPERTY(EditAnywhere)
	class TSubclassOf<UMatchUserWidget> ClassMatchUserWidget;

	UMatchUserWidget* matchUserWidget;

	
#pragma region Functions
	UFUNCTION(BlueprintCallable)
	void AddToScore(FVector2D added);
	UFUNCTION(BlueprintCallable)
	void SetScore(FVector2D setS);
	UFUNCTION(BlueprintCallable)
	void MatchStatus(bool isAllowedToPlay);
	UFUNCTION(BlueprintCallable)
	void CreateMatchUserInterface();
	UFUNCTION(BlueprintCallable)
	bool ChangePauseState(bool ConditionPause);
#pragma endregion

};
