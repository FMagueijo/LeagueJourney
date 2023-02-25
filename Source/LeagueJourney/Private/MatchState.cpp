// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"

AMatchState::AMatchState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMatchState::BeginPlay()
{
	Super::BeginPlay();



	//Clock and UI
	CreateMatchUserInterface();
	MatchStatus(true);
	MatchTime = FVector2D::Zero();
	SetScore(FVector2D(0, 0));
}

void AMatchState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);



	//Clock
	if (bIsPlaying && matchUserWidget)
	{
		Seconds += MatchSpeed * DeltaSeconds;
		if(Seconds >= 60)
		{
			Seconds = 0;
			Minutes++;
		}

		MatchTimeTxt = ClockLogic();
		matchUserWidget->TBTimer->SetText(MatchTimeTxt);
		matchUserWidget->TBScoreHome->SetText(FText::FromString(FString::FromInt(MatchScore.X)));
		matchUserWidget->TBScoreAway->SetText(FText::FromString(FString::FromInt(MatchScore.Y)));
	}
}

FText AMatchState::ClockLogic()
{
	if(Minutes == 44 || Minutes == 89 && !bOnAddedTime)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "afefeaf0");
		AddedTime = FMath::RandRange(1, 7);
		bOnAddedTime = true;
	}
	int _condition = (CurrentHalf == 1)? 45 : 90;
	if(Minutes >= _condition + AddedTime && bOnAddedTime)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "afefeaf1");
		bOnAddedTime = false;
		bIsPlaying = false;
		AddedTime = 0;
		switch (CurrentHalf)
		{
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
	}

	FString local_Timer;
	if(bIsPlaying)
	{
		if (bOnAddedTime)
		{
			local_Timer = UKismetTextLibrary::Conv_IntToText(Minutes, false, true, 2, 3).ToString() + ":"
				+ UKismetTextLibrary::Conv_IntToText(Seconds, false, true, 2, 3).ToString()+"+"+FString::FromInt(AddedTime);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "afefeaf");
		}
		else
		{
			local_Timer = UKismetTextLibrary::Conv_IntToText(Minutes, false, true, 2, 3).ToString() + ":"
				+ UKismetTextLibrary::Conv_IntToText(Seconds, false, true, 2, 3).ToString();
		}
	}
	else
	{
		(CurrentHalf == 1)? local_Timer = "HalfTime" : local_Timer = "FullTime";

	}

	return FText::FromString(local_Timer);
}

bool AMatchState::ChangePauseState(bool ConditionPause)
{
	if(ConditionPause == NULL)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), !UGameplayStatics::IsGamePaused(GetWorld()));
		return UGameplayStatics::IsGamePaused(GetWorld());
	}

	UGameplayStatics::SetGamePaused(GetWorld(), ConditionPause);
	return UGameplayStatics::IsGamePaused(GetWorld());
}

void AMatchState::AddToScore(FVector2D added)
{
	MatchScore += added;
}

void AMatchState::SetScore(FVector2D setS)
{
	MatchScore = setS;
}

void AMatchState::MatchStatus(bool isAllowedToPlay)
{
	isBeingPlayed = isAllowedToPlay;
}

void AMatchState::CreateMatchUserInterface()
{
	if (ClassMatchUserWidget)
	{
		matchUserWidget = CreateWidget<UMatchUserWidget>(GetWorld(), ClassMatchUserWidget);
		matchUserWidget->AddToViewport();
	}
}
