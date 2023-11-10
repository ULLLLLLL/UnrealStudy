// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class SHOOTINGGAMECODE_API AWeapon : public AActor, public IWeaponInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EventTrigger();

	virtual void EventTrigger_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EventShoot();

	virtual void EventShoot_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EventPickup(ACharacter* AChar);

	virtual void EventPickup_Implementation(ACharacter* AChar) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EventDrog();

	virtual void EventDrog_Implementation() override;

public:
	// Server : 서버에서 실행
	// Reliable : 신뢰성
	UFUNCTION(Server, Reliable)
	void ReqApplyDamage(FVector vStart, FVector vEnd);

	UFUNCTION()
	void OnRep_Ammo();

	UFUNCTION()
	void UseAmmo();

	void UpdateHUD_MyAmmo(int Ammo);

public:
	void CalcShootStartEndPos(FVector& vStart, FVector& vEnd); // 암 길이 구하기

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ShootMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* FireEffect; // 정보는 블루프린트에서 넣어줄 것이다.

	UPROPERTY(Replicated, BlueprintReadWrite)
	ACharacter* m_pChar;  // *가 있으면 ->로 참조하는 것. 없으면 .으로 참조한다.
						  // 참조한다는 것은 핀을 끌어서 라인을 만든다는 것이다.

	UPROPERTY(ReplicatedUsing = OnRep_Ammo)
	int m_Ammo;
};
