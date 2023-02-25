// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballerController.h"

#include "ImageUtils.h"
#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameManager.h"
#include "Football.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AFootballerController::AFootballerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bAutoManageActiveCameraTarget = false;

	
}

void AFootballerController::BeginPlay()
{
	Super::BeginPlay();
	/*FTransform local_FTTransform;
	local_FTTransform.SetLocation(FVector::Zero());

	Bola = UGameplayStatics::GetActorOfClass(GetWorld(), AFootball::StaticClass());
	

	FFootballer local_stats;
	local_stats.Acceleration = 20;
	local_stats.Pace = 20;
	for(int i = 0; i < 11; i++)
	{
		SpawnPawnFootballer(i, local_stats);
	}

	BrodCamera->PlayerPrincipal = TeamPlayers[1];
	ControlledPlayer = Cast<AFootballCharacter>(TeamPlayers[1]);
	this->Possess(TeamPlayers[1]);

	if(AGameManager::countriesGetter.Num() == 1)
	{
		AGameManager::fLoadJson();
	}

	CreateMatch();*/
}

void AFootballerController::CreateMatch()
{
	if (ClassMatchSettings)
	{
		FTransform local_Tranform;
		local_Tranform.SetIdentityZeroScale();
		matchSettings = GetWorld()->SpawnActorDeferred<AMatchSettings>(ClassMatchSettings, local_Tranform);
		if (matchSettings)
		{
			UGameplayStatics::FinishSpawningActor(matchSettings, local_Tranform);
			matchSettings->MatchStatus(true);
			matchSettings->CreateMatchUserInterface();
		}
	}
}

void AFootballerController::SpawnPawnFootballer(int PositionIndex, FFootballer stats)
{

	/*FTransform local_FTTransform;
	local_FTTransform.SetLocation(AFootballCommonVariables::HomePositions[PositionIndex]);

	if (ACharacter* xCHar = AFootballCommonVariables::ReturnMadePlayer(stats, FootballCharacterClass, false, GetWorld()))
	{
		TeamPlayers.Add(xCHar);
		Cast<AFootballCharacter>(xCHar)->CurrentPosition = AFootballCommonVariables::HomePositions[PositionIndex];
		UGameplayStatics::FinishSpawningActor(xCHar, local_FTTransform);
		SetViewTarget(BrodCamera);
	}*/

}


void AFootballerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/*DrawDebugSphere(GetWorld(), ControlledPlayer->GetActorLocation() + SpecialAxis*10, 25, 16, FColor::Magenta);
	(!ControlledPlayer->isRetarded) ? Move() : NULL;
	(ControlledPlayer->isChasingBall && !ControlledPlayer->isRetarded) ? Move(Cast<AFootballCharacter>(GetPawn())->BallActor->GetActorLocation()) : NULL;

	if (ControlledPlayer->isChasingBall && !Cast<AFootball>(Bola)->isTaken && UKismetMathLibrary::Vector_Distance(ControlledPlayer->GetActorLocation(), Bola->GetActorLocation()) < 100)
	{
		ControlledPlayer->hasBall = true;
		Cast<AFootball>(Bola)->isTaken = true;
		Cast<AFootball>(Bola)->WhoHasBall = this->GetPawn();
		ControlledPlayer->isChasingBall = false;
		ControlledPlayer->isRetarded  = false;
	}
	


	if(isPowering)
	{
		power += .8 * DeltaSeconds;
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, FString::SanitizeFloat(power));
		if (power >= 1)
		{
			DecideNextMove();
		}
	}
	*/
}


void AFootballerController::Move()
{

	MoveAxis = BrodCamera->Com_Camera->GetForwardVector() * GetInputAxisValue("MoveFW") + BrodCamera->Com_Camera->GetRightVector() * GetInputAxisValue("MoveRG");
	MoveAxis.Z = 0;
	ControlledPlayer->GetCharacterMovement()->MaxWalkSpeed = (MoveAxis.Length() > .4) ? 200 + (GetInputAxisValue("Sprint") * (Cast<AFootballCharacter>(GetPawn())->stats.Pace / 20 * 400)) : 200;
	ControlledPlayer->GetCharacterMovement()->RotationRate = (MoveAxis.Length() > .4) ? FRotator(0, 180 + GetInputAxisValue("Sprint") * 180, 0) : FRotator::ZeroRotator;
	MoveAxis.Normalize();
	ControlledPlayer->AddMovementInput(MoveAxis);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, MoveAxis.GetSafeNormal().ToString());
}

void AFootballerController::Move(FVector Where)
{
	
	ControlledPlayer->GetCharacterMovement()->MaxWalkSpeed = 200 + (GetInputAxisValue("Sprint") * (ControlledPlayer->stats.Pace / 20 * 400));
	ControlledPlayer->GetCharacterMovement()->RotationRate = FRotator(0, 180 + GetInputAxisValue("Sprint") * 180, 0);
	FVector VectorFinal = Where - ControlledPlayer->GetActorLocation();
	ControlledPlayer->AddMovementInput(VectorFinal);
}

bool AFootballerController::isOnScreen(AActor* WhichActor)
{
	FVector2D ScreenLocation;
	ProjectWorldLocationToScreen(WhichActor->GetActorLocation(), ScreenLocation);
	
	int32 ScreenWidth = 0;
	int32 ScreenHeight = 0;
	GetViewportSize(ScreenWidth, ScreenHeight);

	int32 ScreenX = (int32)ScreenLocation.X;
	int32 ScreenY = (int32)ScreenLocation.Y;

	return ScreenX >= 0 && ScreenY >= 0 && ScreenX < ScreenWidth&& ScreenY < ScreenHeight;
}

void AFootballerController::DecideNextMove()
{
	if(ControlledPlayer->hasBall)
	{
		ControlledPlayer->isRetarded = true;
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Shoot");
		isPowering = false;
		if (nextMove == "shoot")
		{
			ControlledPlayer->PlayAnimMontage(Anim_Shoot, 1);
		}
		else if(nextMove == "tackle")
		{

		}
	}
	else
	{

		isPowering = false;
		nextMove = "null";
		power = 0;
	}

}

void AFootballerController::ShootAnim()
{

	isPowering = true;
	nextMove = "shoot";
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "ShootAnim");
}

void AFootballerController::Shoot()
{


	Cast<AFootball>(Bola)->isTaken = false;
	Cast<AFootball>(Bola)->Com_Collision->SetSimulatePhysics(true);
	Cast<AFootball>(Bola)->Com_Collision->SetPhysicsLinearVelocity(FVector::Zero());
	Cast<AFootball>(Bola)->Com_Collision->SetPhysicsAngularVelocityInDegrees(FVector::Zero());
	Cast<AFootball>(Bola)->Com_Collision->AddImpulse(ControlledPlayer->GetActorForwardVector() * (1200 + power * 1000) + ControlledPlayer->GetActorUpVector() * (power * 400), NAME_None, false);
	Cast<AFootball>(Bola)->Com_Collision->AddImpulse(ControlledPlayer->GetActorRightVector() * (GetInputAxisValue("SpecialAxisFW") * 1000), NAME_None, true);
	ControlledPlayer->StopAnimMontage();

	isPowering = false;
	nextMove = "null";
	power = 0;
	ControlledPlayer->hasBall = false;
	ControlledPlayer->isRetarded = false;
}

void AFootballerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if(InputComponent)
	{
		InputComponent->BindAxis("MoveFW");
		InputComponent->BindAxis("MoveRG");
		InputComponent->BindAxis("SpecialAxisRG");
		InputComponent->BindAxis("SpecialAxisFW");
		InputComponent->BindAxis("Sprint");

	}
}
