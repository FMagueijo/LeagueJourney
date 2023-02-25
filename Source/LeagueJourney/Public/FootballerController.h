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
	

	TArray<FVector> positionDefend;
	TArray<FVector> positionAttack;

public:
	virtual void Tick(float DeltaSeconds) override;
	

	UPROPERTY(EditAnywhere, Category="Classes")
	TSubclassOf<AStadiumCamera> CameraClass;

	UPROPERTY(EditAnywhere, Category = "Classes")
	TSubclassOf<AFootballCharacter> FootballCharacterClass;

	UPROPERTY()
	AStadiumCamera* BrodCamera;
	UPROPERTY()
	AActor* Bola;


	void CreateMatch();

	UPROPERTY(EditAnywhere)
	class TSubclassOf<AMatchSettings> ClassMatchSettings;
	AMatchSettings* matchSettings;

	bool isSolo = true;
	bool isHome = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isPowering = false;

	FTeam Team;

	TArray<ACharacter*> TeamPlayers;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFootballCharacter* ControlledPlayer;

	void SpawnPawnFootballer(int PositionIndex, FFootballer stats);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector MoveAxis;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpecialAxis;

	void Move();
	UFUNCTION(BlueprintCallable)
	void Move(FVector Where);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float power = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString nextMove = "null";

	UFUNCTION(BlueprintCallable)
	bool isOnScreen(AActor* WhichActor);

	UFUNCTION(BlueprintCallable)
	void DecideNextMove();
	UFUNCTION(BlueprintCallable)
	void ShootAnim();

	UFUNCTION(BlueprintCallable)
	void Shoot();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* Anim_Shoot;
};
