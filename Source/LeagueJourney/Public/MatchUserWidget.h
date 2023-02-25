// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanel.h"
#include "MatchUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class LEAGUEJOURNEY_API UMatchUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TBScoreHome;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TBScoreAway;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TBTeamHome;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TBTeamAway;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TBHTScoreHome;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TBHTScoreAway;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TBTimer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USizeBox* ScoreHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* HTHUD;
};
