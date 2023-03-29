// Fill out your copyright notice in the Description page of Project Settings.


#include "Football.h"
#include "FootballCharacter.h"
#include "FootballGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsField/PhysicsFieldComponent.h"

// Sets default values
AFootball::AFootball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Com_Collision = CreateDefaultSubobject<USphereComponent>(FName("Collision"));
	Com_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Static Mesh"));
	SetRootComponent(Com_Collision);
	Com_Mesh->SetupAttachment(Com_Collision);
}

// Called when the game starts or when spawned
void AFootball::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFootball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(DaddyPawn)
	{
		//Follow the player point

		if(bIsPosessed)
		{
			FollowDaddy();
		}

		//Checks what team has the ball

		CheckPossession(Cast<AFootballCharacter>(DaddyPawn)->bPlaysAtHome, false);

		
	}
	else
	{
		//Checks what team has the ball
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation()+ GetVelocity(), 10, FColor::Blue);
		CheckPossession(false, true);

	}
	
}

void AFootball::FollowDaddy(){
	// movetolo
	FVector targetLocation = Cast<AFootballCharacter>(DaddyPawn)->GetMesh()->GetSocketLocation("ballSocket");
	targetLocation += Cast<AFootballCharacter>(DaddyPawn)->GetMesh()->GetSocketLocation("ballSocket").ForwardVector * 40;
	targetLocation.Z = 10;
	
	//transform.position = new Vector3(itsTarget.position.x+0.8f, 0 ,itsTarget.position.z);

	SetActorLocation(targetLocation);
	
}

void AFootball::Possess(APawn* _parent) {

	DaddyPawn = _parent;
	bIsPosessed = true;
	Com_Collision->SetSimulatePhysics(false);

	Cast<AFootballCharacter>(DaddyPawn)->bHasBall = true;

	if(Cast<AFootballCharacter>(DaddyPawn)->bPlaysAtHome)
	{

		//Check if player is current

		if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn() != DaddyPawn)
		{
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(DaddyPawn);
		}
	}

}

void AFootball::UnPossess(){
	if(bIsPosessed)
	{

		Cast<AFootballCharacter>(DaddyPawn)->bHasBall = false;
		Cast<AFootballCharacter>(DaddyPawn)->ChargePercentage = 0;
		Cast<AFootballCharacter>(DaddyPawn)->PlayerToPassTo = nullptr;
		LastDaddyPawn = DaddyPawn;
		DaddyPawn = nullptr;
		bIsPosessed = false;
		Com_Collision->SetSimulatePhysics(true);
	}
}

void AFootball::CheckPossession(bool _bHome, bool _bIgnore) {

	if(!_bIgnore)
	{
		if(_bHome)
		{
			Cast<AFootballGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->bTeamHasBallHome = true;
			Cast<AFootballGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->bTeamHasBallAway = false;
		}
		else
		{
			Cast<AFootballGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->bTeamHasBallHome = false;
			Cast<AFootballGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->bTeamHasBallAway = true;
		}
	}
	else
	{
		Cast<AFootballGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->bTeamHasBallHome = false;
		Cast<AFootballGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->bTeamHasBallAway = false;
	}

}

void AFootball::Shoot(bool _chip, FVector _direction, float _force, float _charge){
	UnPossess();

	if(_chip)
	{
		
	}
	_direction.Normalize();
	Com_Collision->SetAllPhysicsLinearVelocity(FVector::Zero(), false);

	FVector ShootVector = _direction * (1500.0 + (_charge * (_force / 20.0 * 2000.0)));
	ShootVector.Z = (400.0 * _charge);

	Com_Collision->AddImpulse(ShootVector, NAME_None, false);
	Com_Collision->AddAngularImpulseInDegrees(ShootVector, NAME_None, true);
}

void AFootball::Pass(AActor* _where, AActor* _from, float _force, float _charge) {
	UnPossess();
	Com_Collision->SetAllPhysicsLinearVelocity(FVector::Zero(), false);

	FVector PassVector;

	if(_where)
	{
		FVector direction = (_where->GetActorLocation() - _from->GetActorLocation()).GetSafeNormal();
		float passingSpeed = 400.f;
		float passingVelocity = passingSpeed * _force * _charge;
		FVector velocity = direction * passingVelocity;
		float errorMargin = FMath::RandRange(0.0f, 20.0f);
		FVector errorDirection = FMath::VRand();
		FVector errorOffset = errorDirection * errorMargin;
		velocity += errorOffset;
		

		Com_Collision->AddImpulse(velocity, NAME_None, true);
	}
	else
	{
		PassVector = _from->GetActorForwardVector() * (400.0 * _force * _charge);
	}


	
}
