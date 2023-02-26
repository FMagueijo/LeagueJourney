// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FootballGameMode.h"
#include "FootballerController.h"
#include "Football.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFootballCharacter::AFootballCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AFootballCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if(APlayerController* _PC = Cast<AFootballerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (UEnhancedInputLocalPlayerSubsystem* _Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_PC->GetLocalPlayer()))
		{
			_Subsystem->AddMappingContext(BaseMappingContext, 5);
		}
	}
	
}

void AFootballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFootballCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedMove);
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFootballCharacter::EnhancedSprint);
		
	}
	

}

void AFootballCharacter::EnhancedMove(const FInputActionValue& Value)
{
	FVector2D CurrentValue = Value.Get<FVector2D>();
	CurrentValue.Normalize();
	FVector FinalValue = (SpawnedCamera != nullptr)?  Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetForwardVector() * CurrentValue.Y + Cast<AStadiumCamera>(SpawnedCamera)->Com_Camera->GetRightVector() * CurrentValue.X : CurrentPosition;
	FinalValue.Z = 0;
	
	GetCharacterMovement()->MaxWalkSpeed = (FinalValue.Length() > .7) ? (200 + stats.Pace/20 * 200) + (SprintPercentage * (stats.Pace / 20 * 400)) : 200;
	GetCharacterMovement()->RotationRate = (FinalValue.Length() > .7) ? FRotator(0, 180  + SprintPercentage * 180, 0) : FRotator::ZeroRotator;
	

	FinalValue.Normalize();
	AddMovementInput(FinalValue);
}

void AFootballCharacter::EnhancedSprint(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::SanitizeFloat(CurrentValue));
	SprintPercentage = CurrentValue;
}


