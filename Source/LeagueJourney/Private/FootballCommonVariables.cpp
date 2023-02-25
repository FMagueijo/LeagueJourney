// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballCommonVariables.h"
#include "FootballCharacter.h"
#include "ImageUtils.h"
#include "Kismet/GameplayStatics.h"

TArray<FVector> AFootballCommonVariables::HomePositions;
TArray<FVector> AFootballCommonVariables::AwayPositions;
// Sets default values
AFootballCommonVariables::AFootballCommonVariables()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	makeArrays();
}

// Called when the game starts or when spawned
void AFootballCommonVariables::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFootballCommonVariables::makeArrays()
{
	HomePositions.Add(FVector(0, 5400, 110));
	HomePositions.Add(FVector(0, 3900, 110));
	HomePositions.Add(FVector(-1200, 3900, 110));
	HomePositions.Add(FVector(1200, 3900, 110));
	HomePositions.Add(FVector(-2900, 3900, 110));
	HomePositions.Add(FVector(2900, 3900, 110));
	HomePositions.Add(FVector(0, 2500, 110));
	HomePositions.Add(FVector(1200, 2500, 110));
	HomePositions.Add(FVector(-1200, 2500, 110));
	HomePositions.Add(FVector(-2900, 2200, 110));
	HomePositions.Add(FVector(2900, 2200, 110));
	HomePositions.Add(FVector(0, 1000, 110));
	HomePositions.Add(FVector(-1200, 1000, 110));
	HomePositions.Add(FVector(1200, 1000, 110));
	HomePositions.Add(FVector(2900, 1000, 110));
	HomePositions.Add(FVector(2900, 1000, 110));

	AwayPositions.Add(FVector(0, -5400, 110));
	AwayPositions.Add(FVector(0, -3900, 110));
	AwayPositions.Add(FVector(1200, -3900, 110));
	AwayPositions.Add(FVector(-1200, -3900, 110));
	AwayPositions.Add(FVector(2900, -3900, 110));
	AwayPositions.Add(FVector(-2900, -3900, 110));
	AwayPositions.Add(FVector(0, -2500, 110));
	AwayPositions.Add(FVector(-1200, -2500, 110));
	AwayPositions.Add(FVector(1200, -2500, 110));
	AwayPositions.Add(FVector(2900, -2200, 110));
	AwayPositions.Add(FVector(-2900, -2200, 110));
	AwayPositions.Add(FVector(0, -1000, 110));
	AwayPositions.Add(FVector(1200, -1000, 110));
	AwayPositions.Add(FVector(-1200, -1000, 110));
	AwayPositions.Add(FVector(2900, -1000, 110));
	AwayPositions.Add(FVector(-2900, -1000, 110));
}

// Called every frame
void AFootballCommonVariables::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UTexture2D* AFootballCommonVariables::FileToTexture2d(FString File)
{
	FString contentPath = FPlatformProcess::UserDir();
	FString FullPath = contentPath + File;
	UTexture2D* texture = nullptr;
	FullPath = FullPath.Replace(TEXT("\\"), TEXT("/"));
	//FPlatformMisc::NormalizePath(FullPath);


	texture = FImageUtils::ImportFileAsTexture2D(FullPath);

	return texture;
}



