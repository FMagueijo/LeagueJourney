// Fill out your copyright notice in the Description page of Project Settings.


#include "StadiumCamera.h"

#include "GameFramework/Character.h"
#include "FootballerController.h"
#include "FootballGameMode.h"
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
	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

// Called every frame
void AStadiumCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//CameraCurrentPosition = (PC->GetPawn()->GetActorLocation() + SpawnedFootball->GetActorLocation()) / 2;
	CameraCurrentPosition = PC->GetPawn()->GetActorLocation();
	CameraCurrentPosition.Z = 0;
	
	FRotator CameraFinalRotation = UKismetMathLibrary::FindLookAtRotation(Com_Camera->GetComponentLocation(), CameraCurrentPosition);
	Com_Camera->SetWorldRotation(CameraFinalRotation);

	CameraCurrentPosition.Y = FMath::Clamp(CameraCurrentPosition.Y, minmaxYposition * -1, minmaxYposition);

	this->SetActorLocation(CameraCurrentPosition);

	/*if (isOnScreen((PC->GetPawn()) ? PC->GetPawn() : nullptr) && isOnScreen(SpawnedFootball))
	{
		(Com_SpringArm->TargetArmLength > 1300) ? Com_SpringArm->TargetArmLength -= 10 * DeltaTime : NULL;
	}
	else
	{

		Com_SpringArm->TargetArmLength += 1000 * DeltaTime;
	}*/

	Com_SpringArm->TargetArmLength = FMath::Clamp(Com_SpringArm->TargetArmLength, 1300, 15000);
	
}

bool AStadiumCamera::isOnScreen(AActor* WhichActor)
{
	FVector2D ScreenLocation;
	PC->ProjectWorldLocationToScreen(WhichActor->GetActorLocation(), ScreenLocation);

	int32 ScreenWidth = 0;
	int32 ScreenHeight = 0;
	PC->GetViewportSize(ScreenWidth, ScreenHeight);

	int32 ScreenX = (int32)ScreenLocation.X;
	int32 ScreenY = (int32)ScreenLocation.Y;

	return ScreenX >= 1 && ScreenY >= 1 && ScreenX < ScreenWidth&& ScreenY < ScreenHeight;
}
