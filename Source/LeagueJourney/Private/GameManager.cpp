// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

#include "ImageUtils.h"
#include "JsonObjectConverter.h"

TArray<FCountry> AGameManager::countriesGetter;
// Sets default values
AGameManager::AGameManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameManager::fLoadJson()
{
	FString jsonFileName = "IKomplicate\\Database\\GameDB0.ikdb";
	jsonFileName = jsonFileName.Replace(TEXT("\\"), TEXT("/"));
	FString FullPath = FPlatformProcess::UserDir() + jsonFileName;
	/*if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FullPath))
	{

		FString JsonStr;
		FFileHelper::LoadFileToString(JsonStr, *FullPath);

		if (FJsonObjectConverter::JsonArrayStringToUStruct(JsonStr, &countriesGetter))
		{
			for (int i = 0; i < countriesGetter.Num(); i++)
			{
				countriesGetter[i].TFlag = FileToTexture2d(countriesGetter[i].Flag);
			}
		}
	}*/
	
}

UTexture2D* AGameManager::FileToTexture2d(FString File)
{
	FString contentPath = FPlatformProcess::UserDir();
	FString FullPath = contentPath + File;
	UTexture2D* texture = nullptr;
	FullPath = FullPath.Replace(TEXT("\\"), TEXT("/"));
	//FPlatformMisc::NormalizePath(FullPath);


	texture = FImageUtils::ImportFileAsTexture2D(FullPath);

	return texture;
}
