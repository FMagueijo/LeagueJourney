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

	float minmaxYposition = 3000;

#pragma endregion

#pragma region Functions

	bool isOnScreen(AActor* WhichActor);

#pragma endregion

	
};
