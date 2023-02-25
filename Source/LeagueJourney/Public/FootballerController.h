// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StadiumCamera.h"
#include "MatchSettings.h"
#include "FootballCharacter.h"
#include "FootballMatchInstance.h"
#include "Football.h"
#include "FootballerController.generated.h"

/**
 * 
 */


UCLASS()
class LEAGUEJOURNEY_API AFootballerController : public APlayerController
{
	GENERATED_BODY()
public:
	AFootballerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	
	
	UPROPERTY()
	AActor* Bola;

	bool isHome = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isPowering = false;
	
};
