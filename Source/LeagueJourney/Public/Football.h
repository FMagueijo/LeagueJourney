// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Football.generated.h"

UCLASS()
class LEAGUEJOURNEY_API AFootball : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFootball();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Ball Properties

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsPosessed = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	APawn* DaddyPawn = nullptr;
	

	UPROPERTY(BlueprintReadWrite)
	class USphereComponent* Com_Collision;

	UPROPERTY(BlueprintReadWrite)
	class UStaticMeshComponent* Com_Mesh;
	
	FHitResult PerformRaycast();

	UFUNCTION()
	void UnPossess();
	void CheckPossession(bool _bHome, bool _bIgnore);

	UFUNCTION()
	void Shoot(FVector _direction, float _force, float _charge);
	void Pass(AActor* _where, AActor* _from, float _force, float _charge);

	UFUNCTION()
	void FollowDaddy();

	UFUNCTION()
	void Possess(APawn* _parent);
};
