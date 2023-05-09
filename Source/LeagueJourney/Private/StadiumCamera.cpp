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

	//CameraCurrentPosition = (PC->GetPawn()->GetActorLocation() + SpawnedFootball->GetActorLocation()) / 2;
	/*CameraCurrentPosition = (PC->GetPawn() != nullptr) ? PC->GetPawn()->GetActorLocation() : FVector(0, 0, 0);
	CameraCurrentPosition.Z = 0;
	
	FRotator CameraFinalRotation = UKismetMathLibrary::FindLookAtRotation(Com_Camera->GetComponentLocation(), CameraCurrentPosition);
	Com_Camera->SetWorldRotation(CameraFinalRotation);

	CameraCurrentPosition.Y = FMath::Clamp(CameraCurrentPosition.Y, minmaxYposition * -1, minmaxYposition);

	this->SetActorLocation(CameraCurrentPosition);*/

	if(const AFootball* _football = Cast<AFootball>(SpawnedFootball))
	{
		FVector MidPoint = (_football->bIsPosessed) ? _football->DaddyPawn->GetActorLocation() + _football->DaddyPawn->GetActorForwardVector() * 500 : _football->GetActorLocation() + _football->GetVelocity();
		
		MidPoint = FMath::Lerp(GetActorLocation(), MidPoint, DeltaTime / 2);

		Com_Camera->SetWorldRotation(FMath::Lerp(Com_Camera->GetComponentRotation(), UKismetMathLibrary::FindLookAtRotation(Com_Camera->GetComponentLocation(), MidPoint), DeltaTime));
		
		SetActorLocation(MidPoint);

		if(const AFootballCharacter* _char = Cast<AFootballCharacter>(_football->DaddyPawn))
		{
			const float ArmLength = MaxZoom - ((MaxZoom-MinZoom) * _char->ChargePercentage);
			Com_SpringArm->TargetArmLength = FMath::Lerp(Com_SpringArm->TargetArmLength, ArmLength, DeltaTime/2);
		}
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
