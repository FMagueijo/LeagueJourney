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

	if (isTaken && WhoHasBall)
	{
		Com_Collision->SetSimulatePhysics(false);
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
		}
	}
	else
	{
		Com_Collision->SetSimulatePhysics(true);
		FHitResult Hit = PerformRaycast();
		if (Hit.GetActor() && GetVelocity().Length() > 1100)
		{
			DrawDebugSphere(GetWorld(), Hit.Location, 100, 16, FColor::Red);
			// Check if the hit actor has the target tag
			if (Hit.GetComponent()->ComponentHasTag("BallDetector") || Hit.GetActor()->ActorHasTag("BallDetector"))
			{
				if (Hit.Location.X >= 290)
				{
					Com_Collision->AddForce(FVector::BackwardVector * (GetVelocity().Length()), NAME_None, true);
				}
				else if (Hit.Location.X <= -290)
				{
					Com_Collision->AddForce(FVector::ForwardVector * (GetVelocity().Length()), NAME_None, true);
				}
			}
		}
		
	}
}

FHitResult AFootball::PerformRaycast()
{
	// Calculate the end location for the raycast using the view rotation and the maximum distance
	FVector EndLocation = GetActorLocation() + (GetVelocity().GetSafeNormal() * 10000);

	// Set up the trace parameters
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;


	TraceParams.AddIgnoredActor((WhoHasBall) ? WhoHasBall : nullptr);
	TraceParams.AddIgnoredActor(this);

	// Perform the raycast
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), EndLocation, ECC_WorldStatic, TraceParams);

	DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Red);
	
	return Hit;
}

