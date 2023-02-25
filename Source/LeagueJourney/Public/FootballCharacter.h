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

	//Input Actions

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input Actions")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
	UInputMappingContext* BaseMappingContext;

	void EnhancedMove(const FInputActionValue& Value);

	FVector moveAxisX;
	FVector moveAxisY;
	
	AActor* BallActor;

	int NextMove = NULL; // 0= BTPass 1= BTShoot 2= BTCross

	float Power = 0;

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

	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	FVector CurrentPosition;

	UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
	TArray<UAnimInstance*> Animacoes;

	UPROPERTY(EditAnywhere, Category = "Debug Properties")
	FString DebugStringPosition;

	void decideNextMove();
	void Tackle();
	void Shoot();
	void Pass();
	void Receive();
	void Move();
	void Move(FVector Where);


#pragma region Components Related

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;
	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
	class USphereComponent* SmallSphereCollision;
	UFUNCTION()
	void OnSsphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma endregion
};
