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

	UnPossess();

	DaddyPawn = _parent;
	bIsPosessed = true;
	Com_Collision->SetSimulatePhysics(false);

	Cast<AFootballCharacter>(DaddyPawn)->bHasBall = true;

	if(Cast<AFootballCharacter>(DaddyPawn)->bPlaysAtHome)
	{

		//Check if player is current

		if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn() != DaddyPawn && !Cast<AFootballGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->bSpecMode)
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

void AFootball::Shoot(float _distance, FVector _direction, float _force, float _charge){
	UnPossess();
	
	_direction.Normalize();
	Com_Collision->SetAllPhysicsLinearVelocity(FVector::Zero(), false);


	float DistanceFactor = _distance / (1000.0 + (90.0 * _force));
	float speed = (3000.0 + (_charge * (_force / 20.0 * 3000.0)));
	FVector ShootVector = _direction * speed;


	ShootVector.Z = FMath::Clamp(_charge * 700.0 + FMath::FRandRange(0.0, 1000.0 * DistanceFactor), 0, 1700);
	ShootVector.X += FMath::Clamp(FMath::FRandRange(DistanceFactor* -1000, DistanceFactor * 1000), -1300, 1300);
	
	Com_Collision->AddImpulse(ShootVector, NAME_None, true);

}

void AFootball::Pass(AActor* _where, AActor* _from, float _force, float _charge) {
	UnPossess();
	Com_Collision->SetAllPhysicsLinearVelocity(FVector::Zero(), false);

	FVector Predicted_Position;

	if(_where)
	{

		float Distance = FVector::Distance(_where->GetActorLocation(), GetActorLocation());

		if(Distance <= 500)
		{
			_charge = FMath::Clamp(_charge, 0, 0.3);
		}
		float ballSpeed = 2000.0 + FMath::Clamp(_charge, 0, (Distance <= 500) ? .5 : 1) * (_force / 20 * 3500.0);

		float Time = Distance / ballSpeed;
		Predicted_Position = _where->GetActorLocation() + (_where->GetVelocity() * Time);

		DrawDebugSphere(GetWorld(), Predicted_Position, 50, 16, FColor::Red,false, 15, 0, 20);
		DrawDebugSphere(GetWorld(), _where->GetActorLocation(), 50, 16, FColor::Green,false, 15, 0, 20);
		DrawDebugDirectionalArrow(GetWorld(), _where->GetActorLocation(), Predicted_Position, 50, FColor::Blue, false, 15, 0, 20);

		FVector direction = (Predicted_Position - GetActorLocation()).GetSafeNormal();
		direction *= ballSpeed;
		direction.Z = 500.0 * _charge;
		Com_Collision->AddImpulse(direction, NAME_None, true);
	}
	else
	{
		
	}


	
}
