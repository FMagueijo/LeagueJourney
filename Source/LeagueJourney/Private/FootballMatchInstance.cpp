 // Fill out your copyright notice in the Description page of Project Settings.


#include "FootballMatchInstance.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ImageUtils.h"
#include "JsonObjectConverter.h"



UFootballMatchInstance::UFootballMatchInstance()
{
	AllHomePositions.Add("GK", FVector(0, 5400.0, 10));

	AllHomePositions.Add("LB", FVector(-2300, 4300, 10));
	AllHomePositions.Add("LCB", FVector(-750, 4300, 10));
	AllHomePositions.Add("CB", FVector(0, 4300, 10));
	AllHomePositions.Add("RCB", FVector(750, 4300, 10));
	AllHomePositions.Add("RB", FVector(2300, 4300, 10));

	AllHomePositions.Add("LM", FVector(-2600, 2000, 10));
	AllHomePositions.Add("LCM", FVector(-1400, 2000, 10));
	AllHomePositions.Add("CM", FVector(0, 2000, 10));
	AllHomePositions.Add("RCM", FVector(1400, 2000, 10));
	AllHomePositions.Add("RM", FVector(2600, 2000, 10));

	AllHomePositions.Add("LW", FVector(-2600, 700, 10));
	AllHomePositions.Add("LST", FVector(-1400, 700, 10));
	AllHomePositions.Add("ST", FVector(0, 1000, 10));
	AllHomePositions.Add("RST", FVector(1400, 700, 10));
	AllHomePositions.Add("RW", FVector(2600, 700, 10));



	AllAwayPositions.Add("GK", FVector(0, -5360.0, 10));

	AllAwayPositions.Add("LB", FVector(2300, -4300, 10));
	AllAwayPositions.Add("LCB", FVector(750, -4300, 10));
	AllAwayPositions.Add("CB", FVector(0, -4300, 10));
	AllAwayPositions.Add("RCB", FVector(-750, -4300, 10));
	AllAwayPositions.Add("RB", FVector(-2300, -4300, 10));

	AllAwayPositions.Add("LM", FVector(2600, -2000, 10));
	AllAwayPositions.Add("LCM", FVector(1400, -2000, 10));
	AllAwayPositions.Add("CM", FVector(0, -2000, 10));
	AllAwayPositions.Add("RCM", FVector(-1400, -2000, 10));
	AllAwayPositions.Add("RM", FVector(-2600, -2000, 10));

	AllAwayPositions.Add("LW", FVector(2600, -700, 10));
	AllAwayPositions.Add("LST", FVector(1400, -700, 10));
	AllAwayPositions.Add("ST", FVector(0, -1000, 10));
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


 void FLeague::generateMatchdays()
 {
	 int numTeams = table.Num();
	 const int numRounds = numTeams - 1;
	 const int matchesPerRound = numTeams / 2;
	 for(int jorn = 0; jorn < 2; ++jorn)
	 {
		 for (int round = 0; round < numRounds; ++round)
		 {
			 FLeagueMatchday matchday;

			 for (int match = 0; match < matchesPerRound; ++match)
			 {
				 int homeTeamIndex = (round + match) % (numTeams - 1);
				 int awayTeamIndex = (numTeams - 1 - match + round) % (numTeams - 1);

				 if (match == 0)
				 {
					 // Fix the first team in the middle if the number of teams is odd
					 awayTeamIndex = numTeams - 1;
				 }

				 FLeagueMatch leagueMatch;

				 leagueMatch.Home = table[homeTeamIndex];
				 leagueMatch.Away = table[awayTeamIndex];

				 matchday.matches.Add(leagueMatch);
			 }

			 matchdays.Add(matchday);

		 }
	 }

 }

 void FLeague::AdvanceMatchDay()
 {
	 // Check if there are more matchdays left
	 if (currentMatchday >= matchdays.Num())
	 {
		 // No more matchdays to advance
		 return;
	 }

	 // Get the current matchday
	 FLeagueMatchday& _currentMatchday = matchdays[currentMatchday];

	 // Iterate through each match in the current matchday
	 for (FLeagueMatch& match : _currentMatchday.matches)
	 {
		if(!match.Finished)
		{
			// Update the scores (assuming some scoring logic here)
			int roundsMatch = FMath::RandRange(1, 15);
			for(int i = 0; i < roundsMatch; i++)
			{
				if(FMath::RandRange(0.0,1.0) > .8)
				{
					(FMath::RandRange(0.0, 1.0) > .5) ? match.Score.X++ : match.Score.Y++;
				}
			}
		}

		 // Find the home and away teams in the league table
		 FLeagueTeam* homeTeam = table.FindByPredicate([&](const FLeagueTeam& Team) { return Team.id == match.Home.id; });
		 FLeagueTeam* awayTeam = table.FindByPredicate([&](const FLeagueTeam& Team) { return Team.id == match.Away.id; });

		 if (homeTeam && awayTeam)
		 {
			 // Update points and goal difference for each team
			if(match.Score.X != match.Score.Y)
			{
				if(match.Score.X > match.Score.Y)
				{
					homeTeam->points += 3;
					homeTeam->WDL.X += 1;
					awayTeam->WDL.Z += 1;
				}
				else
				{
					awayTeam->points += 3;
					awayTeam->WDL.X += 1;
					homeTeam->WDL.Z += 1;
					
				}
			}
			else
			{
				homeTeam->points += 1;
				awayTeam->points += 1;

				awayTeam->WDL.Y += 1;
				homeTeam->WDL.Y += 1;
			}
			 
			 homeTeam->ga += match.Score.X;
			 homeTeam->ga -= match.Score.Y;
			 awayTeam->ga += match.Score.Y;
			 awayTeam->ga -= match.Score.X;
		 }

		 match.Finished = true;
	 }
	 

	 // Advance to the next matchday
	 currentMatchday++;
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


						FString kit_ImgFullPath = FPlatformProcess::UserDir() + x->Kit;
						kit_ImgFullPath = kit_ImgFullPath.Replace(TEXT("\\"), TEXT("/"));

						x->Texture2DCrest = FImageUtils::ImportFileAsTexture2D(ImgFullPath);
						x->Texture2DKit = FImageUtils::ImportFileAsTexture2D(kit_ImgFullPath);
						
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

 void UFootballMatchInstance::generateMatchdays(int numTeams, int numMax, int numMin)
 {

	numTeams = FMath::Clamp(numTeams, numMin, numMax);
	if(numTeams % 2 != 0)
	{
		numTeams = FMath::Clamp(numTeams - 1, numMin, numMax);
	}

	CurrentLeague.table.Empty();
	CurrentLeague.matchdays.Empty();

	CurrentLeague.currentMatchday = 0;
	TArray<FCountry> TempCounts = FootballDatabase;

	for(int i = 0; i < numTeams; i++)
	{
		

		FCountry& CurrentCountry = TempCounts[FMath::RandRange(0, TempCounts.Num() - 1)];
		FDivision& CurrentDivison = CurrentCountry.Divisions[FMath::RandRange(0, CurrentCountry.Divisions.Num() - 1)];

		int randomIndexTeam = FMath::RandRange(0, CurrentDivison.Teams.Num() - 1);

		FLeagueTeam CurrentTeam;
		if(CurrentDivison.Teams.IsValidIndex(randomIndexTeam))
		{
			CurrentTeam.Team = CurrentDivison.Teams[randomIndexTeam];

			CurrentTeam.ga = 0;
			CurrentTeam.id = i;
			CurrentTeam.points = 0;
			CurrentLeague.table.Add(CurrentTeam);

			CurrentDivison.Teams.RemoveAt(randomIndexTeam);
		}
	}

	if(CurrentLeague.table.Num() % 2 != 0)
	{
		CurrentLeague.table.RemoveAt(0);
	}

	CurrentLeague.PlayerTeam = CurrentLeague.table[FMath::RandRange(0, CurrentLeague.table.Num() - 1)];
	CurrentLeague.PlayerTeam.playerOwned = true;
	CurrentLeague.generateMatchdays();
 }

 void UFootballMatchInstance::AdvanceMatchDay()
 {
	 CurrentLeague.AdvanceMatchDay();

	 CurrentLeague.table.Sort([](const FLeagueTeam& TeamA, const FLeagueTeam& TeamB) {
		 if (TeamA.points == TeamB.points)
		 {
			 return TeamA.ga > TeamB.ga; // Sort by goal difference (ga) in descending order
		 }

		 return TeamA.points > TeamB.points; // Sort by points in descending order
		 });

 }

