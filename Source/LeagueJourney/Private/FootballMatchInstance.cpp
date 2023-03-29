 // Fill out your copyright notice in the Description page of Project Settings.


#include "FootballMatchInstance.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ImageUtils.h"
#include "JsonObjectConverter.h"



UFootballMatchInstance::UFootballMatchInstance()
{
	AllHomePositions.Add("GK", FVector(0, 5400.0, 10));

	AllHomePositions.Add("LB", FVector(-2600, 3900, 10));
	AllHomePositions.Add("LCB", FVector(-1400, 3900, 10));
	AllHomePositions.Add("CB", FVector(0, 3900, 10));
	AllHomePositions.Add("RCB", FVector(1400, 3900, 10));
	AllHomePositions.Add("RB", FVector(2600, 3900, 10));

	AllHomePositions.Add("LM", FVector(-2600, 2500, 10));
	AllHomePositions.Add("LCM", FVector(-1400, 2500, 10));
	AllHomePositions.Add("CM", FVector(0, 2500, 10));
	AllHomePositions.Add("RCM", FVector(1400, 2500, 10));
	AllHomePositions.Add("RM", FVector(2600, 2500, 10));

	AllHomePositions.Add("LW", FVector(-2600, 700, 10));
	AllHomePositions.Add("LST", FVector(-1400, 700, 10));
	AllHomePositions.Add("ST", FVector(0, 700, 10));
	AllHomePositions.Add("RST", FVector(1400, 700, 10));
	AllHomePositions.Add("RW", FVector(2600, 700, 10));



	AllAwayPositions.Add("GK", FVector(0, -5360.0, 10));

	AllAwayPositions.Add("LB", FVector(2600, -3900, 10));
	AllAwayPositions.Add("LCB", FVector(1400, -3900, 10));
	AllAwayPositions.Add("CB", FVector(0, -3900, 10));
	AllAwayPositions.Add("RCB", FVector(-1400, -3900, 10));
	AllAwayPositions.Add("RB", FVector(-2600, -3900, 10));

	AllAwayPositions.Add("LM", FVector(2600, -2500, 10));
	AllAwayPositions.Add("LCM", FVector(1400, -2500, 10));
	AllAwayPositions.Add("CM", FVector(0, -2500, 10));
	AllAwayPositions.Add("RCM", FVector(-1400, -2500, 10));
	AllAwayPositions.Add("RM", FVector(-2600, -2500, 10));

	AllAwayPositions.Add("LW", FVector(2600, -700, 10));
	AllAwayPositions.Add("LST", FVector(1400, -700, 10));
	AllAwayPositions.Add("ST", FVector(0, -700, 10));
	AllAwayPositions.Add("RST", FVector(-1400, -700, 10));
	AllAwayPositions.Add("RW", FVector(-2600, -700, 10));
}

 void UFootballMatchInstance::Init()
 {
	 Super::Init();
	 //FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UFootballMatchInstance::BeginLoadingScreen);
	 //FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UFootballMatchInstance::EndLoadingScreen);
 }

 void UFootballMatchInstance::BeginLoadingScreen(const FString& MapName)
 {
	 
 }

 void UFootballMatchInstance::EndLoadingScreen(UWorld* InLoadedWorld)
 {
	 
 }




 void UFootballMatchInstance::FinishLoadLevel(const FString& _map)
 {
	 GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Red, "Done");

	 UGameplayStatics::OpenLevel(this, FName(*_map));
 }
 void UFootballMatchInstance::LoadFirstDB()
{
	FString jsonFileName = "THIRDST\\Database\\DB.json";
	jsonFileName = jsonFileName.Replace(TEXT("\\"), TEXT("/"));
	FString FullPath = FPlatformProcess::UserDir() + jsonFileName;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Start");
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FullPath))
	{

		FString JsonStr;
		FFileHelper::LoadFileToString(JsonStr, *FullPath);

		if (FJsonObjectConverter::JsonArrayStringToUStruct(JsonStr, &FootballDatabase))
		{
			for (int c = 0; c < FootballDatabase.Num(); c++) {
				for (int d = 0; d < FootballDatabase[c].Divisions.Num(); d++) {
					for (int t = 0; t < FootballDatabase[c].Divisions[d].Teams.Num(); t++) {
						FTeam* x;
						x = &FootballDatabase[c].Divisions[d].Teams[t];
						FString ImgFullPath = FPlatformProcess::UserDir() + x->Crest;
						ImgFullPath = ImgFullPath.Replace(TEXT("\\"), TEXT("/"));
						
						x->Texture2DCrest = FImageUtils::ImportFileAsTexture2D(ImgFullPath);

					}
				}
			}
		}
	}
}

UTexture2D* UFootballMatchInstance::URLToTexture(FString File)
{
	FString contentPath = FPlatformProcess::UserDir();
	FString FullPath = contentPath + File;
	UTexture2D* texture = nullptr;
	FullPath = FullPath.Replace(TEXT("\\"), TEXT("/"));
	//FPlatformMisc::NormalizePath(FullPath);


	texture = FImageUtils::ImportFileAsTexture2D(FullPath);

	return texture;
}

