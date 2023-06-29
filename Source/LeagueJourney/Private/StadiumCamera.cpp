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
		FVector MidPoint =( _football->GetActorLocation() + ((_football->bIsPosessed) ? _football->DaddyPawn->GetActorLocation() : FVector::Zero()) ) / ( (_football->bIsPosessed) ? 2 : 1);
		//CurrentLocation = FMath::Lerp(GetActorLocation(), MidPoint, DeltaTime / 2);
		MidPoint.Z = 0;
		
		//First Rotation Test
		CurrenRotation = FRotator(0, 180, 0);
		
		//First Zoom Test
		CurrentZoom = MaxZoom;

		//First Offset Test
		CurrentOffset = FVector(0, 0, 1500);

		//First FOV Test
		CurrentFOV = 75;
		
		float rate = FMath::Abs(MidPoint.Y) / 5500.0;

		CurrentLocation = FVector(0, 0, 0);
		CurrenRotation = FRotator(0, -180, 0);
		CurrentZoom = 3500;
		CurrentFOV = 70 - 60 * rate;
		CurrentOffset = FVector(0, 0, 1500);


		if (AFootballCharacter* _char = Cast<AFootballCharacter>(_football->DaddyPawn))
		{
			if ((_char->bFreeKick || _char->bGoalKick))
			{
				CurrentFOV = 70;
				CurrentZoom = 750;
				CurrentLocation = MidPoint;
				CurrenRotation = _char->GetActorRotation();
				if(_char->moveVector.Length() >= .1)
				{
					CurrenRotation = UKismetMathLibrary::FindLookAtRotation(_char->GetActorLocation(), _char->GetActorLocation() + _char->moveVector);
				}
				
				CurrentOffset = FVector(0, 0, (_char->bFreeKick)? 200 : 350);
			}
		}
		
		SetActorLocation(FMath::Lerp(GetActorLocation(), CurrentLocation, DeltaTime));
		SetActorRotation(FMath::Lerp(GetActorRotation(), CurrenRotation, DeltaTime));
		Com_SpringArm->TargetArmLength = FMath::Lerp(Com_SpringArm->TargetArmLength, CurrentZoom, DeltaTime * 2);
		Com_SpringArm->TargetOffset = CurrentOffset;
		Com_Camera->FieldOfView = FMath::Lerp(Com_Camera->FieldOfView, CurrentFOV, DeltaTime);

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
