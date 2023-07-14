// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FootballCommonVariables.h"
#include "FootballMatchOptions.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "FootballMatchInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FFootballer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	FString Nation;
	UPROPERTY(BlueprintReadWrite)
	FString Position;
	UPROPERTY(BlueprintReadWrite)
	int Pace;
	UPROPERTY(BlueprintReadWrite)
	int Shooting;
	UPROPERTY(BlueprintReadWrite)
	int Passing;
	UPROPERTY(BlueprintReadWrite)
	int Defending;
	UPROPERTY(BlueprintReadWrite)
	int Stamina;
	UPROPERTY(BlueprintReadWrite)
	int Goalkeeping;
	UPROPERTY(BlueprintReadWrite)
	FString CurrentPosition;

};

USTRUCT(BlueprintType)
struct FTeam
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString Abbreviation;

	UPROPERTY(BlueprintReadWrite)
	FString Kit;

	UPROPERTY(BlueprintReadWrite)
	FString Crest;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Texture2DCrest;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Texture2DKit;

	UPROPERTY(BlueprintReadWrite)
	TArray<FFootballer> Footballers;

};

USTRUCT(BlueprintType)
struct FDivision
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	TArray<FTeam> Teams;

};

USTRUCT(BlueprintType)
struct FCountry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	TArray<FDivision> Divisions;
};



//League shit

USTRUCT(BlueprintType)
struct FLeagueTeam
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	int id;

	UPROPERTY(BlueprintReadWrite)
	FTeam Team;

	UPROPERTY(BlueprintReadWrite)
	int points;

	UPROPERTY(BlueprintReadWrite)
	FVector WDL;


	UPROPERTY(BlueprintReadWrite)
	int ga;

	bool playerOwned = false;
};

USTRUCT(BlueprintType)
struct FLeagueMatch
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(BlueprintReadWrite)
	FLeagueTeam Home;

	UPROPERTY(BlueprintReadWrite)
	bool Finished = false;
	
	UPROPERTY(BlueprintReadWrite)
	FLeagueTeam Away;

	UPROPERTY(BlueprintReadWrite)
	FVector2D Score;
	
};

USTRUCT(BlueprintType)
struct FLeagueMatchday
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FLeagueMatch> matches;
};

USTRUCT(BlueprintType)
struct FLeague
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FLeagueTeam> table;

	UPROPERTY(BlueprintReadWrite)
	TArray<FLeagueMatchday> matchdays;

	UPROPERTY(BlueprintReadWrite)
	FLeagueTeam PlayerTeam;

	UPROPERTY(BlueprintReadWrite)
	int currentMatchday;
	
	void generateMatchdays();
	
	void AdvanceMatchDay();
	
};


UCLASS()
class LEAGUEJOURNEY_API UFootballMatchInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	void FinishLoadLevel(const FString& _map);
	bool startupdone = false;
public:
	UFootballMatchInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

	/*
	 * Basic Match Transport Settings
	 */

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FVector> AllHomePositions;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FVector> AllAwayPositions;
	
	
	UPROPERTY(BlueprintReadOnly)
	float LoadPer = 0;

	UPROPERTY(BlueprintReadWrite)
	int Difficulty = 1;

	UPROPERTY(BlueprintReadWrite)
	int MatchRate = 15;
	//3 / 45

	UPROPERTY(BlueprintReadWrite)
	bool bSpectate = false;

	UPROPERTY(BlueprintReadWrite)
	bool bLeaguePlay = false;

	UPROPERTY(BlueprintReadWrite)
	FString LevelToLoad = "StadiumMCB";
	
	UPROPERTY(BlueprintReadWrite)
	FTeam H_Team;

	UPROPERTY(BlueprintReadWrite)
	FTeam A_Team;

	UPROPERTY(BlueprintReadWrite)
	TArray<FFootballer> H_SXI;

	UPROPERTY(BlueprintReadWrite)
	TArray<FFootballer> A_SXI;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCountry> FootballDatabase;
	
	UFUNCTION(BlueprintCallable)
	void LoadFirstDB();

	UTexture2D* URLToTexture(FString File);



	UPROPERTY(BlueprintReadWrite)
	FLeague CurrentLeague;

	UFUNCTION(BlueprintCallable)
	void generateMatchdays(int numTeams, int numMax, int numMin);

	UFUNCTION(BlueprintCallable)
	void AdvanceMatchDay();
};
