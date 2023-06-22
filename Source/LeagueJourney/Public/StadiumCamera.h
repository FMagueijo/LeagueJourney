// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "StadiumCamera.generated.h"

UCLASS()
class LEAGUEJOURNEY_API AStadiumCamera : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AStadiumCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY()
	float CurrentZoom;
	UPROPERTY()
	float CurrentFOV;
	UPROPERTY()
	FVector CurrentOffset;
	UPROPERTY()
	FVector CurrentLocation;
	UPROPERTY()
	FRotator CurrenRotation;

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* Com_SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Com_Camera;

#pragma endregion

#pragma region Properties

	UPROPERTY(BlueprintReadOnly)
	APlayerController* PC;
	FVector CameraCurrentPosition;

	UPROPERTY(BlueprintReadOnly)
	AActor* SpawnedFootball;

	UPROPERTY(EditDefaultsOnly)
	float MinZoom = 1000.0;

	UPROPERTY(EditDefaultsOnly)
	float MaxZoom = 5000.0;

#pragma endregion

#pragma region Functions

	bool isOnScreen(AActor* WhichActor);

#pragma endregion

	
};
