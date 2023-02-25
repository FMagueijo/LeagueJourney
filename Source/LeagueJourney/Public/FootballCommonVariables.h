// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FootballCommonVariables.generated.h"




UCLASS()
class LEAGUEJOURNEY_API AFootballCommonVariables : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFootballCommonVariables();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void makeArrays();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static UTexture2D* FileToTexture2d(FString File);
	
	static TArray<FVector> HomePositions;

	static TArray<FVector> AwayPositions;
};
