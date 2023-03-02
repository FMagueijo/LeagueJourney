// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAnimationInstance.h"
#include "GameManager.h"
#include "Animation/AnimInstance.h"
#include "StadiumCamera.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "FootballCharacter.generated.h"

class AFootball;
class UEnhancedInputLocalPlayerSubsystem;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class LEAGUEJOURNEY_API AFootballCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFootballCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindClosestPawn(bool isHome);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//

	APlayerController* PC;
	UEnhancedInputLocalPlayerSubsystem* SubSystem;



	//Camera Properties

	AActor* SpawnedCamera;



	//Input

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* BaseMappingContext;



	//Input Actions

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input Actions")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* TackleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* PassAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* SprintAction;



	//Input Functions

	UFUNCTION()
	void EnhancedCharge(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedMove(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedTackle(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedPass(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedShot(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedSprint(const FInputActionValue& Value);



	//Input Properties

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	bool bIsCharging = false;

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	float SprintPercentage = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	float ChargePercentage = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	AActor* PlayerToPassTo;

	AGameModeBase* CurrentGameMode;

	//Montages

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontageTackle;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontageShot;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontagePass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontagePossession;



	//Player Properties

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	AFootball* KnownBall = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	bool bHasBall = false;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	bool bPlaysAtHome = false;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	FVector CurrentPosition;


	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable)
	void Pass();

	UFUNCTION()
	void ChaseBall(AActor* ball);

	bool IsActorBehind(AActor* actor0, AActor* actor1);
	
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool hasBall = false;
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool needsBall = false;
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isPowering = false;

	//Action Booleans
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isRetarded = false;
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isTackling = false;
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isShooting = false;
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isPassing = false;
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isBeingFouled = false;
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isChasingBall = false;

	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	FFootballer stats;

	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	AStadiumCamera* main_cam;
	

	UPROPERTY(BlueprintReadWrite, Category = "Character Animation Properties")
	UCharacterAnimationInstance* anim_Class;



	UPROPERTY(EditAnywhere, Category = "Debug Properties")
	FString DebugStringPosition;
	

#pragma region Components Related

	UPROPERTY(EditAnywhere)
	class USphereComponent* BallDetectionArea;

	UPROPERTY(EditAnywhere)
	class USphereComponent* BallPosessArea;

	UFUNCTION()
	void OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnPosessOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPosessOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


#pragma endregion
};
