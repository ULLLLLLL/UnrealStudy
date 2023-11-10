// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShootingPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDele_UpdateHp, float, CurHp, float, MaxHp);
/**
 * 
 */
UCLASS()
class SHOOTINGGAMECODE_API AShootingPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AShootingPlayerState();
	
public:
	UFUNCTION(BlueprintCallable)
	void AddDamage(float Damage);

	UFUNCTION() // ¹Ýµå½Ã UÆã¼Ç
	void OnRep_CurHp();

public:
	UPROPERTY(ReplicatedUsing=OnRep_CurHp)
	float m_CurHp;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FDele_UpdateHp Event_Dele_UpdateHp;
};
