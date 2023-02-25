// Copyright Epic Games, Inc. All Rights Reserved.

#include "LeagueJourneyGameMode.h"
#include "LeagueJourneyCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALeagueJourneyGameMode::ALeagueJourneyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
