// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"


AMatchSettings::AMatchSettings()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AMatchSettings::BeginPlay()
{
	Super::BeginPlay();
	
}

FText AMatchSettings::ClockLogic()
{
	FString local_Timer;

	//Dar tempo extra
	if(MatchTime.X == 44 || MatchTime.X == 89 || endTime != 0)
	{
		if(addedTime == 0)
		{
			addedTime = FMath::RandRange(1, 6);
			endTime += MatchTime.X + 1 + addedTime;
		}

		if (MatchTime.X < endTime && endTime != 0)
		{
			local_Timer = UKismetTextLibrary::Conv_IntToText(MatchTime.X, false, true, 2, 3).ToString() + ":"
				+ UKismetTextLibrary::Conv_IntToText(MatchTime.Y, false, true, 2, 3).ToString() + " + " + FString::FromInt(addedTime);
		}
		else
		{
			MatchTime.X -= addedTime;
			MatchTime.Y = 0;
			endTime = 0;
			addedTime = 0;
			isBeingPlayed = false;

			local_Timer = UKismetTextLibrary::Conv_IntToText(MatchTime.X, false, true, 2, 3).ToString() + ":"
				+ UKismetTextLibrary::Conv_IntToText(MatchTime.Y, false, true, 2, 3).ToString();
		}
	}
	else
	{
		local_Timer = UKismetTextLibrary::Conv_IntToText(MatchTime.X, false, true, 2, 3).ToString() + ":"
			+ UKismetTextLibrary::Conv_IntToText(MatchTime.Y, false, true, 2, 3).ToString();
	}

	return FText::FromString(local_Timer);
}

// Called every frame
void AMatchSettings::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(isBeingPlayed && matchUserWidget)
	{
		
		MatchTime.Y += MatchSpeed * DeltaTime;
		
		if (MatchTime.Y >= 60)
		{
			MatchTime.Y = 0;
			MatchTime.X += 1;
		}
		MatchTimeTxt = ClockLogic();
		matchUserWidget->TBTimer->SetText(MatchTimeTxt);
		matchUserWidget->TBScoreHome->SetText(FText::FromString(FString::FromInt(MatchScore.X)));
		matchUserWidget->TBScoreAway->SetText(FText::FromString(FString::FromInt(MatchScore.Y)));
	}
}


void AMatchSettings::AddToScore(FVector2D added)
{
	MatchScore += added;
}

void AMatchSettings::SetScore(FVector2D setS)
{
	MatchScore = setS;
}

void AMatchSettings::MatchStatus(bool isAllowedToPlay)
{
	isBeingPlayed = isAllowedToPlay;
}

void AMatchSettings::CreateMatchUserInterface()
{
	if (ClassMatchUserWidget)
	{
		matchUserWidget = CreateWidget<UMatchUserWidget>(GetWorld(), ClassMatchUserWidget);
		matchUserWidget->AddToViewport();
	}
}

