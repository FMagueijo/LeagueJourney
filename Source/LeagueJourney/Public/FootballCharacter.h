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
	virtual void PawnClientRestart() override;
	virtual void Restart() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindClosestPawn(bool isHome);
	
public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	


	//Important Properties

	UPROPERTY(BlueprintReadOnly, Category = "Important Properties")
	APlayerController* PC;

	UPROPERTY(BlueprintReadOnly, Category = "Important Properties")
	UEnhancedInputLocalPlayerSubsystem* SubSystem;

	UPROPERTY(BlueprintReadOnly, Category = "Important Properties")
	AGameModeBase* CurrentGameMode;



	//Important Methods

	UFUNCTION()
	void teamHasBall(bool bHome);
	

	//Camera Properties

	UPROPERTY(BlueprintReadOnly, Category = "Camera Properties")
	AActor* SpawnedCamera;



	//Input Context

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Context")
	UInputMappingContext* BaseMappingContext;



	//Input Actions

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input Actions")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* TackleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* PassAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* CrossAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* ShootAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* SwitchAction;


	//Enhanced Actions

	UFUNCTION()
	void EnhancedCharge(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedMove(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedSprint(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedTackle(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedPass(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedCross(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedSwitch(const FInputActionValue& Value);

	UFUNCTION()
	void EnhancedShot(const FInputActionValue& Value);


	//Actions Methods

	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable)
	void Pass();

	UFUNCTION(BlueprintCallable)
	void Cross();

	UFUNCTION(BlueprintCallable)
	void Tackle();

	UFUNCTION(BlueprintCallable)
	void AddCard();

	UFUNCTION(BlueprintCallable)
	void GetTackled();

	UFUNCTION(BlueprintCallable)
	void GetMaxPace();

	UFUNCTION(BlueprintCallable)
	void DifficultyToStats();


	//Input Properties

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	bool bIsCharging = false;

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	float SprintPercentage = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	float ChargePercentage = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Input Properties")
	AActor* PlayerToPassTo;


	//Montages

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontageTackle;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontageShot;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontagePass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontageCross;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontagePossession;

	UPROPERTY(EditDefaultsOnly, Category = "Animation Properties")
	UAnimMontage* MontageTackled;


	//Event Related

	UPROPERTY(BlueprintReadWrite, Category = "Event Booleans")
	bool bKickOff = false;

	UPROPERTY(BlueprintReadWrite, Category = "Event Booleans")
	bool bThrowIn = false;

	UPROPERTY(BlueprintReadWrite, Category = "Event Booleans")
	bool bFreeKick = false;

	UPROPERTY(BlueprintReadWrite, Category = "Event Booleans")
	bool bCorner = false;

	UPROPERTY(BlueprintReadWrite, Category = "Event Booleans")
	bool bGoalKick = false;

	UFUNCTION(BlueprintCallable, Category = "Event Booleans")
	void CheckEvents();

	//Available Actions

	UPROPERTY(BlueprintReadWrite, Category = "Available Actions")
	bool bCanCharge = true;

	UPROPERTY(BlueprintReadWrite, Category = "Available Actions")
	bool bCanPossess = true;

	UPROPERTY(BlueprintReadWrite, Category = "Available Actions")
	bool bCanSwitch = true;

	UPROPERTY(BlueprintReadWrite, Category = "Available Actions")
	bool bCanShoot = true;

	UPROPERTY(BlueprintReadWrite, Category = "Available Actions")
	bool bCanPass = true;

	UPROPERTY(BlueprintReadWrite, Category = "Available Actions")
	bool bCanTackle = true;

	UPROPERTY(BlueprintReadWrite, Category = "Available Actions")
	bool bCanMove = true;

	//Actions Booleans

	UPROPERTY(BlueprintReadWrite, Category = "Actions Booleans")
	bool bHasBall = false;

	UPROPERTY(BlueprintReadWrite, Category = "Actions Booleans")
	bool bTeamHasBall = false;

	UPROPERTY(BlueprintReadWrite, Category = "Actions Booleans")
	bool bWantsBall = false;

	UPROPERTY(BlueprintReadWrite, Category = "Actions Booleans")
	bool bPlaysAtHome = false;
	

	//Player Properties

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	AFootball* KnownBall = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	FVector CurrentPosition;

	UPROPERTY(BlueprintReadOnly, Category = "Player Properties")
	int cardNumber = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Player Properties")
	float coverageDistance = 1000.0;

	UPROPERTY(BlueprintReadOnly, Category = "Player Properties")
	TArray<AActor*> teamColleagues;

	
	//Character Properties
	
	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	FFootballer stats;
	
	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	AStadiumCamera* main_cam;

	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	UCharacterAnimationInstance* anim_Class;	

	UPROPERTY(BlueprintReadOnly, Category = "Character Properties")
	FVector idealBallCatch;

	UPROPERTY(BlueprintReadOnly, Category = "Character Properties")
	float stamina = 1.0f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties")
	FVector2D moveAxis;

	UPROPERTY(BlueprintReadOnly, Category = "Character Properties")
	FVector moveVector;

	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	float currentOverall;

#pragma region Overlap Related
	
	UPROPERTY(EditAnywhere)
	class USphereComponent* BallPosessArea;


	//Overlaps 
	
	UFUNCTION()
	void OnPossessOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPossessOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


#pragma endregion


};
