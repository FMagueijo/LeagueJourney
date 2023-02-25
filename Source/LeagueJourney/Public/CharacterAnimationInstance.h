// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimationInstance.generated.h"

/**
 * 
 */
UCLASS()
class LEAGUEJOURNEY_API UCharacterAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animation Properties")
	float anim_Speed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animation Properties")
	bool isJogging = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animation Properties")
	bool isShooting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animation Properties")
	bool isTackling = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animation Properties")
	float pawn_speed = 0;
};
