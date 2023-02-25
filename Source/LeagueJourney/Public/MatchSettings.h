// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FootballMatchInstance.h"
#include "MatchUserWidget.h"
#include "GameFramework/Actor.h"
#include "MatchSettings.generated.h"

UCLASS()
class LEAGUEJOURNEY_API AMatchSettings : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatchSettings();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FText ClockLogic();
	bool isBeingPlayed = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TMap<FString, int> Scorers;

	TArray<FFootballer> HomeLineUp;
	TArray<FFootballer> AwayLineUp;
	TArray<FTeam> Teams;

	FVector2D MatchScore;
	FVector2D MatchTime;
	FText MatchTimeTxt;

	int addedTime = 0;
	int endTime = 0;
	float MatchSpeed = 25.f;

	UPROPERTY(EditAnywhere)
	class TSubclassOf<UMatchUserWidget> ClassMatchUserWidget;
	UMatchUserWidget* matchUserWidget;


#pragma region Functions
	void AddToScore(FVector2D added);
	void SetScore(FVector2D setS);
	void MatchStatus(bool isAllowedToPlay);
	void CreateMatchUserInterface();
#pragma endregion

};
