// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

/**
 * 
 */
 UINTERFACE(Blueprintable)
class SHOOTINGGAMECODE_API UWeaponInterface : public UInterface //����� U weapon�������̽�
{
	GENERATED_BODY()
	
};


 class SHOOTINGGAMECODE_API IWeaponInterface //����� I weapon�������̽�
 {
	 GENERATED_BODY()


 public:
	 UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	 void EventTrigger();

	 UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	 void EventShoot();

	 UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	 void EventPickup(ACharacter* AChar);

	 UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	 void EventDrog();
 };