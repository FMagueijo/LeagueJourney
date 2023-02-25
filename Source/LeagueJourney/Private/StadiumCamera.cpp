// Fill out your copyright notice in the Description page of Project Settings.


#include "StadiumCamera.h"

#include "GameFramework/Character.h"
#include "FootballerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AStadiumCamera::AStadiumCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Com_Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Com_SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("Spring Arm"));
	SetRootComponent(Com_SpringArm);
	Com_Camera->SetupAttachment(Com_SpringArm);

}

// Called when the game starts or when spawned
void AStadiumCamera::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AStadiumCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ObjectsToFollow.Num() > 0)
	{
		FVector CameraFinalPosition;
		for(AActor* ator : ObjectsToFollow)
		{
			CameraFinalPosition += ator->GetActorLocation();
		}

		CameraFinalPosition /= ObjectsToFollow.Num();
		CameraFinalPosition.Z = 0;

		FRotator CameraFinalRotation = UKismetMathLibrary::FindLookAtRotation(Com_Camera->GetComponentLocation(), CameraFinalPosition);
		Com_Camera->SetWorldRotation(CameraFinalRotation);
		
		CameraFinalPosition.Y = FMath::Clamp(CameraFinalPosition.Y, minmaxYposition*-1, minmaxYposition);
		
		this->SetActorLocation(CameraFinalPosition);
		for(AActor* ator : ObjectsToFollow)
		{
			if(isOnScreen(ator))
			{
				(Com_SpringArm->TargetArmLength > 1300) ? Com_SpringArm->TargetArmLength -= 1000 * DeltaTime : NULL;
			}
			else
			{
				Com_SpringArm->TargetArmLength += 100 * DeltaTime;
			}
		}

		Com_SpringArm->TargetArmLength = FMath::Clamp(Com_SpringArm->TargetArmLength, 1300, 15000);
	}
	
}

bool AStadiumCamera::isOnScreen(AActor* WhichActor)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector2D ScreenLocation;
	PC->ProjectWorldLocationToScreen(WhichActor->GetActorLocation(), ScreenLocation);

	int32 ScreenWidth = 0;
	int32 ScreenHeight = 0;
	PC->GetViewportSize(ScreenWidth, ScreenHeight);

	int32 ScreenX = (int32)ScreenLocation.X;
	int32 ScreenY = (int32)ScreenLocation.Y;

	return ScreenX >= 0 && ScreenY >= 0 && ScreenX < ScreenWidth&& ScreenY < ScreenHeight;
}
