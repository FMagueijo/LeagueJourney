// Fill out your copyright notice in the Description page of Project Settings.


#include "StadiumCamera.h"

#include "GameFramework/Character.h"
#include "FootballerController.h"
#include "FootballGameMode.h"
#include "Camera/CameraModifier_CameraShake.h"
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

	
	if(const AFootball* _football = Cast<AFootball>(SpawnedFootball))
	{
		//First Location Test
		FVector MidPoint = (_football->bIsPosessed) ? _football->DaddyPawn->GetActorLocation() + _football->DaddyPawn->GetVelocity() * 1.3 + _football->DaddyPawn->GetActorForwardVector() * 500 : _football->GetActorLocation() + _football->GetVelocity();
		CurrentLocation = FMath::Lerp(GetActorLocation(), MidPoint, DeltaTime / 2);
		MidPoint.Z = 0;
		
		//First Rotation Test
		CurrenRotation = FRotator(0, 180, 0);
		
		//First Zoom Test
		CurrentZoom = MaxZoom;

		//First Offset Test
		CurrentOffset = FVector(0, 0, 1500);

		//First FOV Test
		CurrentFOV = 40;
		

		if (GetActorLocation().Y < -2500 || GetActorLocation().Y > 2500)
		{
			//Second Zoom Test
			CurrentZoom = MinZoom;

			//Second FOV Test
			CurrentFOV = 70;
			
		}

		if (AFootballCharacter* _char = Cast<AFootballCharacter>(_football->DaddyPawn))
		{
			if (!(_char->bFreeKick || _char->bGoalKick))
			{
				CurrentLocation.Y = FMath::Clamp(CurrentLocation.Y, -2500, 2500);
			}
		}

		if (AFootballCharacter* _char = Cast<AFootballCharacter>(_football->DaddyPawn))
		{
			
			//Third Zoom Test
			CurrentZoom += (MaxZoom - MinZoom) * _char->ChargePercentage;
			
			

			if (_char->bFreeKick || _char->bGoalKick)
			{

				//Second Rotation Test
				CurrenRotation = FRotator(0, (_char->bPlaysAtHome) ? -90 : 90, 0);

				//Third Zoom Test
				CurrentZoom = 1000;

				//Third Offset Test
				CurrentOffset = FVector(0, 0, 250);

				//Third FOV Test
				CurrentFOV = 90;
			}
		}
		
		SetActorLocation(CurrentLocation);
		SetActorRotation(CurrenRotation);
		Com_SpringArm->TargetArmLength = FMath::Lerp(Com_SpringArm->TargetArmLength, CurrentZoom, DeltaTime * 2);
		Com_SpringArm->TargetOffset = CurrentOffset;
		Com_Camera->FieldOfView = FMath::Lerp(Com_Camera->FieldOfView, CurrentFOV, DeltaTime * 2);

		//Camera Rotate Towards Current Location
		Com_Camera->SetWorldRotation(FMath::Lerp(Com_Camera->GetComponentRotation(), UKismetMathLibrary::FindLookAtRotation(Com_Camera->GetComponentLocation(), MidPoint), DeltaTime * 5));

	}

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

	return ScreenX >= .8 && ScreenY >= .8 && ScreenX < ScreenWidth * .8 && ScreenY < ScreenHeight * .8;
}
