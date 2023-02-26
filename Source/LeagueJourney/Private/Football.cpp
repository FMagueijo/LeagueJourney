// Fill out your copyright notice in the Description page of Project Settings.


#include "Football.h"
#include "FootballCharacter.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"

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

	if (bIsPosessed && DaddyPawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Ball Pos");
		FTransform xx;
		FVector TargetVector = (DaddyPawn->GetActorLocation() + DaddyPawn->GetActorForwardVector());
		TargetVector.Z = 50;
		TargetVector.Normalize();
		xx.SetLocation(TargetVector);

		Com_Collision->AddForce(TargetVector, NAME_None, true);
		//Com_Collision->GetComponentVelocity(, NAME_None, true);


		/*Com_Collision->SetSimulatePhysics(false);
		if(Cast<AFootballCharacter>(WhoHasBall)->stats.Position != "GK")
		{
			(Cast<AFootballCharacter>(WhoHasBall)->isChasingBall) ? Cast<AFootballCharacter>(WhoHasBall)->isChasingBall = false : NULL;
			FVector Where = WhoHasBall->GetActorLocation();
			Where.Z = 11;
			Where += WhoHasBall->GetActorForwardVector() * 50;
			SetActorLocation(Where);
		}
		else
		{
			AttachToActor(WhoHasBall, FAttachmentTransformRules::SnapToTargetIncludingScale, "handy");
		}*/
		
	}
	
}


