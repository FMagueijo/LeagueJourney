// Fill out your copyright notice in the Description page of Project Settings.


#include "FootballGameMode.h"
#include "FootballMatchInstance.h"
#include "FootballMatchOptions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"

AFootballGameMode::AFootballGameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AFootballGameMode::BeginPlay()
{
	Super::BeginPlay();

	bSpecMode = Cast<UFootballMatchInstance>(GetGameInstance())->bSpectate;
	Rate = Cast<UFootballMatchInstance>(GetGameInstance())->MatchRate;
	Difficulty = Cast<UFootballMatchInstance>(GetGameInstance())->Difficulty;
	

	SpawnFootball();
	SpawnCamera();

	TArray<AActor*> outA;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFootballCharacter::StaticClass(), outA);

	for(AActor* ator : outA)
	{
		ator->Destroy();
	}

	if(UFootballMatchInstance * matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
	{
		teamHome = matchInstance->H_Team;
		teamAway = matchInstance->A_Team;

		if (matchInstance->H_SXI.Num() != 11 || matchInstance->A_SXI.Num() != 11)
		{
			SpawnDebugPlayers();
			bSpecMode = false;
		}
		else
		{
			for (FFootballer _jogador : matchInstance->H_SXI)
			{
				SpawnPawn(_jogador, true);
			}
			for (FFootballer _jogador : matchInstance->A_SXI)
			{
				SpawnPawn(_jogador, false);
			}
		}
	}

	for (AActor* _jogador : pawnElevenHome)
	{
		TArray<AActor*> _newPawns = pawnElevenHome;
		_newPawns.Remove(_jogador);
		Cast<AFootballCharacter>(_jogador)->teamColleagues = _newPawns;

	}
	for (AActor* _jogador : pawnElevenAway)
	{
		TArray<AActor*> _newPawns = pawnElevenAway;
		_newPawns.Remove(_jogador);
		Cast<AFootballCharacter>(_jogador)->teamColleagues = _newPawns;
	}

	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	float distance;
	AActor* _closest = UGameplayStatics::FindNearestActor(SpawnedFootball->GetActorLocation(), pawnElevenHome, distance);
	CreateKickOffEvent(Cast<AFootballCharacter>(_closest));
	
}

void AFootballGameMode::CreateKickOffEvent(AFootballCharacter* whoStarts)
{
	if (whoStarts->bKickOff)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Kick OFF");
		SetAllActions(true);
		whoStarts->bThrowIn = false;
		whoStarts->bFreeKick = false;
		whoStarts->bGoalKick = false;
		whoStarts->bKickOff = false;
		whoStarts->bCorner = false;
		bMatchPaused = false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Creted Kick OFF");

		bMatchPaused = true;
		SetAllActions(false);
		whoStarts->SetActorLocation(FVector(0, 0, 95));
		SpawnedFootball->Possess(whoStarts);
		whoStarts->bKickOff = true;
		whoStarts->bCanPass = true;
		whoStarts->bCanCharge = true;
		whoStarts->SetActorRotation(FRotator(0, (whoStarts->bPlaysAtHome) ? 90 : -90, 0));

		if(whoStarts->bPlaysAtHome && !bSpecMode)
		{
			PC->Possess(whoStarts);
		}
	}
}

void AFootballGameMode::CreateGoalKickEvent(AFootballCharacter* _whoGets)
{
	if(_whoGets->bGoalKick)
	{
		SetAllActions(true);
		_whoGets->bThrowIn = false;
		_whoGets->bFreeKick = false;
		_whoGets->bGoalKick = false;
		_whoGets->bKickOff = false;
		_whoGets->bCorner = false;
		bMatchPaused = false;
	}
	else
	{
		SpawnedFootball->Possess(_whoGets);
		ReSpawn();
		SpawnedFootball->Possess(_whoGets);
		bMatchPaused = true;
		SetAllActions(false);

		_whoGets->bKickOff = false;
		_whoGets->bCanPass = true;
		_whoGets->bCanCharge = true;
		_whoGets->bGoalKick = true;
		_whoGets->bFreeKick = false;

		
		if (_whoGets->bPlaysAtHome && !bSpecMode)
		{
			PC->Possess(_whoGets);
		}
	}
}

void AFootballGameMode::CreateThrowInEvent(AFootballCharacter* _whoGets, FVector _where, FRotator _setRot)
{
	if (_whoGets->bThrowIn)
	{
		SetAllActions(true);

		_whoGets->bThrowIn = false;
		_whoGets->bFreeKick = false;
		_whoGets->bGoalKick = false;
		_whoGets->bKickOff = false;
		_whoGets->bCorner = false;

		bMatchPaused = false;
	}
	else
	{
		SetAllActions(false);
		_whoGets->SetActorLocation(_where);
		_whoGets->SetActorRotation(_setRot);
		SpawnedFootball->Possess(_whoGets);


		_whoGets->bThrowIn = true;
		_whoGets->bGoalKick = false;
		_whoGets->bKickOff = false;
		_whoGets->bCorner = false;
		_whoGets->bFreeKick = false;

		_whoGets->bCanPass = true;
		_whoGets->bCanCharge = true;

		bMatchPaused = true;

		if (_whoGets->bPlaysAtHome && !bSpecMode)
		{
			PC->Possess(_whoGets);
		}
	}
}

void AFootballGameMode::CreateFreeKickEvent(AFootballCharacter* _whoGets)
{
	if (_whoGets->bFreeKick)
	{
		SetAllActions(true);

		_whoGets->bThrowIn = false;
		_whoGets->bFreeKick = false;
		_whoGets->bGoalKick = false;
		_whoGets->bKickOff = false;
		_whoGets->bCorner = false;

		bMatchPaused = false;
	}
	else
	{
		SetAllActions(false);
		_whoGets->SetActorRotation(FRotator((_whoGets->bPlaysAtHome) ? 0, 90, 0 : 0, -90, 0));
		SpawnedFootball->Possess(_whoGets);


		_whoGets->bThrowIn = false;
		_whoGets->bFreeKick = true;
		_whoGets->bGoalKick = false;
		_whoGets->bKickOff = false;
		_whoGets->bCorner = false;

		_whoGets->bCanPass = true;
		_whoGets->bCanShoot = true;
		_whoGets->bCanCharge = true;

		bMatchPaused = true;

		TArray<AActor*> all;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFootballCharacter::StaticClass(), all);
		all.Remove(_whoGets);

		for(AActor* actor : all)
		{
			if(AFootballCharacter* fool = Cast<AFootballCharacter>(actor))
			{
				if(FVector::Distance(fool->GetActorLocation(), _whoGets->GetActorLocation()) <= 700)
				{
					FVector dir = fool->GetActorLocation() - _whoGets->GetActorLocation();
					fool->SetActorLocation(fool->GetActorLocation() + dir * 750);
				}
			}
		}

		if (_whoGets->bPlaysAtHome && !bSpecMode)
		{
			PC->Possess(_whoGets);
		}
	}
}


void AFootballGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!bMatchPaused)
	{
		if (SpawnedFootball->bIsPosessed)
		{
			float A_HomeValue;
			if (bSpecMode)
			{
				A_HomeValue = (Cast<AFootballCharacter>(SpawnedFootball->DaddyPawn)->stats.CurrentPosition == "GK") ? 0 : (attackPercentageAway += DeltaSeconds / 2);
			}
			else
			{

				const float scaleMin = (Cast<AFootballCharacter>(PC->GetPawn())->CurrentPosition.Y);
				const float scaleMax = scaleMin + 4000;
				const float x = PC->GetPawn()->GetActorLocation().Y;
				A_HomeValue = FMath::Clamp((x - scaleMin) / (scaleMax - scaleMin) * -2.0, 0, 2);
			}
			

			const float A_AwayValue = (Cast<AFootballCharacter>(SpawnedFootball->DaddyPawn)->stats.CurrentPosition == "GK")? 0 : (attackPercentageAway += DeltaSeconds / 2);
			
			const float D_HomeValue = 2 - (A_AwayValue + 1.0);

			const float D_AwayValue = 2 - (A_HomeValue + 1.0);

			(bTeamHasBallHome) ? HomePoss += DeltaSeconds : AwayPoss += DeltaSeconds;
			TotalPoss = HomePoss + AwayPoss;

			PerHomePoss = (HomePoss / TotalPoss);
			PerAwayPoss = (AwayPoss / TotalPoss);

			attackPercentageHome = (bTeamHasBallHome) ? FMath::Clamp(A_HomeValue, 0.0, 2.0) : FMath::Clamp(D_HomeValue - 0.5, 0.0, 2.0);
			attackPercentageAway = (bTeamHasBallAway) ? FMath::Clamp(A_AwayValue, 0.0, 2.0) : FMath::Clamp(D_AwayValue - 0.5, 0.0, 2.0);
		}
		else
		{
			const float AwayScaleMin = 5000;
			const float AwayScaleMax = -5000;
			const float x = SpawnedFootball->GetActorLocation().Y;

			const float N_AwayValue = FMath::Clamp((x - AwayScaleMax) / (AwayScaleMax - AwayScaleMin) * -2.0, 0, 2);
			const float N_HomeValue = 2 - N_AwayValue;


			//const float N_HomeValue = (attackPercentageHome -= DeltaSeconds * 2);
			//const float N_AwayValue = (attackPercentageAway -= DeltaSeconds * 2);

			attackPercentageHome = FMath::Clamp(N_HomeValue, 0.0f, 2.0);
			attackPercentageAway = FMath::Clamp(N_AwayValue, 0.0f, 2.0);
		}
	}
	
	
	// interesting
	// attackPercentageHome = ;


	
}




void AFootballGameMode::SpawnCamera()
{
	if(cameraClass)
	{
		FTransform T_Camera;

		T_Camera.SetRotation(FQuat(FRotator(0, 180, 0)));
		T_Camera.SetLocation(FVector(0, 0, 0));
		AStadiumCamera* NewCamera = GetWorld()->SpawnActorDeferred<AStadiumCamera>(cameraClass, T_Camera);
		NewCamera->SpawnedFootball = SpawnedFootball;
		NewCamera->FinishSpawning(T_Camera, false);
		SpawnedCamera = NewCamera;
	}
}


void AFootballGameMode::ClockLogic()
{
	
}


#pragma region Odd Methods
//PRIVATE FUNCTIONS

void AFootballGameMode::SpawnFootball()
	{
		if(footballClass)
		{
			FTransform T_Football;

			T_Football.SetRotation(FQuat(FRotator::ZeroRotator));
			T_Football.SetLocation(FVector(0, 0, 50));
			AFootball* NewFootball = GetWorld()->SpawnActorDeferred<AFootball>(footballClass, T_Football);
			NewFootball->FinishSpawning(T_Football, false);
			SpawnedFootball = NewFootball;
		}
	}

void AFootballGameMode::SpawnPawn(FFootballer FootballerStruct, bool isHome)
	{
		if (characterClass)
		{
			if (UFootballMatchInstance* matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
			{
				FVector CurrentPosition = (isHome) ? matchInstance->AllHomePositions.FindRef(FootballerStruct.CurrentPosition) : matchInstance->AllAwayPositions.FindRef(FootballerStruct.CurrentPosition);

				FTransform T_NewPawn;

				T_NewPawn.SetRotation(FQuat(FRotator(0, (isHome) ? 270 : 90, 0)));
				T_NewPawn.SetLocation(CurrentPosition);

				AFootballCharacter* NewPawn = GetWorld()->SpawnActorDeferred<AFootballCharacter>(characterClass, T_NewPawn);
				
				NewPawn->SpawnedCamera = SpawnedCamera;
				NewPawn->CurrentGameMode = this;
				NewPawn->stats = FootballerStruct;
				NewPawn->CurrentPosition = CurrentPosition;
				NewPawn->bPlaysAtHome = isHome;
				NewPawn->KnownBall = SpawnedFootball;

				UMaterialInstanceDynamic* MILight = UMaterialInstanceDynamic::Create(NewPawn->GetMesh()->GetMaterial(1), this); 
				
				MILight->SetTextureParameterValue(FName(TEXT("MainTexture")), (NewPawn->bPlaysAtHome)? teamHome.Texture2DKit : teamAway.Texture2DKit);

				NewPawn->GetMesh()->SetMaterial(1, MILight);



				if (animinstanceDefault && animinstanceGoalkeeper && UKismetStringLibrary::EqualEqual_StriStri("GK", FootballerStruct.CurrentPosition))
				{
					NewPawn->GetMesh()->SetAnimClass(animinstanceGoalkeeper);
					NewPawn->AIControllerClass = GoalkeeperAIClass;
				}

				NewPawn->FinishSpawning(T_NewPawn, false);

				(isHome) ? pawnElevenHome.Add(NewPawn) : pawnElevenAway.Add(NewPawn);
			}
		}
	}

void AFootballGameMode::ReSpawn()
{
	if(pawnElevenHome.Num() > 0 && pawnElevenAway.Num() > 0)
	{
		SpawnedFootball->UnPossess();
		SpawnedFootball->LastDaddyPawn = nullptr;
		SpawnedFootball->Com_Collision->SetAllPhysicsLinearVelocity(FVector(0, 0, 0));
		SpawnedFootball->Com_Collision->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
		SpawnedFootball->SetActorLocation(FVector(0, 0, 25), false, nullptr);

		if(UFootballMatchInstance* matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
		{
			if(pawnElevenHome.Num() > 4 && pawnElevenAway.Num() > 4)
			{
				for (AActor* _pawn : pawnElevenHome)
				{
					Cast<AFootballCharacter>(_pawn)->StopAnimMontage();
					FVector CurrentPosition = matchInstance->AllHomePositions.FindRef(Cast<AFootballCharacter>(_pawn)->stats.CurrentPosition);
					CurrentPosition.Z = 96.638264;
					FTransform T_NewPawn;

					T_NewPawn.SetRotation(FQuat(FRotator(0, 270, 0)));
					T_NewPawn.SetLocation(CurrentPosition);

					_pawn->SetActorTransform(T_NewPawn, false, nullptr);
				}

				for (AActor* _pawn : pawnElevenAway)
				{
					Cast<AFootballCharacter>(_pawn)->StopAnimMontage();
					FVector CurrentPosition = matchInstance->AllAwayPositions.FindRef(Cast<AFootballCharacter>(_pawn)->stats.CurrentPosition);
					CurrentPosition.Z = 96.638264;
					FTransform T_NewPawn;

					T_NewPawn.SetRotation(FQuat(FRotator(0, 90, 0)));
					T_NewPawn.SetLocation(CurrentPosition);

					_pawn->SetActorTransform(T_NewPawn, false, nullptr);
				}

				if(!bSpecMode){

					ChooseStartingPawn();
					
				}
			}

			attackPercentageHome = 0;
			attackPercentageAway = 0;
			
		}

	}
}

void AFootballGameMode::ChooseStartingPawn()
{
	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	PC->UnPossess();
	for (AActor* actor : pawnElevenHome)
	{
		if (Cast<AFootballCharacter>(actor)->stats.CurrentPosition == "ST")
		{
			PC->Possess(Cast<APawn>(actor));
		}
		else
		{
			if (Cast<AFootballCharacter>(actor)->stats.CurrentPosition == "LST" || Cast<AFootballCharacter>(actor)->stats.CurrentPosition == "RST")
			{
				PC->Possess(Cast<APawn>(actor));
			}
		}
	}

	if (PC->GetPawn() == nullptr)
	{
		PC->Possess(Cast<APawn>(pawnElevenHome[0]));
	}

	PC->GetPawn()->Controller = PC;

}

void AFootballGameMode::SetAllActions(bool _action)
{
	for (AActor* _foot : pawnElevenHome)
	{
		if (AFootballCharacter* _footer = Cast<AFootballCharacter>(_foot))
		{
			_footer->bCanSwitch = _action;
			_footer->bCanPass = _action;
			_footer->bCanTackle = _action;
			_footer->bCanMove = _action;
			_footer->bCanShoot = _action;
			_footer->bCanCharge = _action;
		}
	}

	for (AActor* _foot : pawnElevenAway)
	{
		if (AFootballCharacter* _footer = Cast<AFootballCharacter>(_foot))
		{
			_footer->bCanSwitch = _action;
			_footer->bCanPass = _action;
			_footer->bCanTackle = _action;
			_footer->bCanMove = _action;
			_footer->bCanShoot = _action;
			_footer->bCanCharge = _action;
		}
	}
}

//DEBUG ONLY

	void AFootballGameMode::SpawnDebugPlayers()
	{
		if (characterClass)
		{
			if (UFootballMatchInstance* matchInstance = Cast<UFootballMatchInstance>(GetGameInstance()))
			{
				FFootballer _stats;
				_stats.Defending = 10;
				_stats.Passing = 10;
				_stats.Shooting = 10;
				_stats.Pace = 10;
				_stats.Goalkeeping = 10;
				_stats.Stamina = 10;
				_stats.Name = "John Doe";
				_stats.Nation = "Portugal";
				

				TArray<FString> HomeAllPos;
				TArray<FString> AwayAllPos;
				matchInstance->AllHomePositions.GenerateKeyArray(HomeAllPos);
				matchInstance->AllHomePositions.GenerateKeyArray(AwayAllPos);

				if(!HomeAllPos.IsEmpty() && !AwayAllPos.IsEmpty())
				{
					for(int i = 0; i < 11; i++)
					{
						_stats.CurrentPosition = HomeAllPos[i];
						SpawnPawn(_stats, true);
					}

					for (int i = 0; i < 11; i++)
					{
						_stats.CurrentPosition = AwayAllPos[i];
						SpawnPawn(_stats, false);
					}
				}
				
			}
		}
	}
#pragma endregion