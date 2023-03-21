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

	UFUNCTION()
	bool IsActorBehind(AActor* actor0, AActor* actor1);


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
	UInputAction* ShootAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* SprintAction;



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
	void EnhancedShot(const FInputActionValue& Value);




	//Actions Methods

	UFUNCTION(BlueprintCallable)
	void MoveBallPoint();

	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable)
	void Pass();

	UFUNCTION(BlueprintCallable)
	void Tackle();

	UFUNCTION()
	void MoveTowardsActor(AActor* _actor);



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
	UAnimMontage* MontagePossession;



	//Player Properties

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	AFootball* KnownBall = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	bool bHasBall = false;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	bool bTeamHasBall = false;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	bool bPlaysAtHome = false;

	UPROPERTY(BlueprintReadWrite, Category = "Player Properties")
	FVector CurrentPosition;
	

	
	//Actions Booleans
	
	
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool needsBall = false;
	
	UPROPERTY(BlueprintReadWrite, Category = "Boolean Override")
	bool isPowering = false;

	UPROPERTY(BlueprintReadWrite, Category = "Action Boolean")
	bool isChasingBall = false;


	
	//Character Properties
	
	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	FFootballer stats;
	
	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	AStadiumCamera* main_cam;

	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	UCharacterAnimationInstance* anim_Class;	
	
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties")
	float stamina = 1.0f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties")
	FVector2D moveAxis;
	


#pragma region Overlap Related

	UPROPERTY(EditAnywhere)
	class USphereComponent* BallDetectionArea;

	UPROPERTY(EditAnywhere)
	class USphereComponent* BallPosessArea;


	//Overlaps 

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
