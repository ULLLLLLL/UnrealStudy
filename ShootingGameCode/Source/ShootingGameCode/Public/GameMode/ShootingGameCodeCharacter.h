// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemInteraction.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ShootingGameCodeCharacter.generated.h"


UCLASS(config=Game)
class AShootingGameCodeCharacter : public ACharacter, public IItemInteraction
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction; //�� ���� �� ��Ʈ

	/** Test Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TestAction; // Ű �Է�

	/** Test Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	/** est Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	/** Test Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressFAction;

	/** Test Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DropAction;

public:
	//void TestWeaponSpawn();

public:
	AShootingGameCodeCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for looking input */
	void Test(const FInputActionValue& Value);

	/** Called for looking input */
	void Reload(const FInputActionValue& Value);

	/** Called for looking input */
	void Shoot(const FInputActionValue& Value);

	/** Called for looking input */
	void PressF(const FInputActionValue& Value);

	/** Called for looking input */
	void Drop(const FInputActionValue& Value);

protected:
	void ServerReloadResTestMsg_Implementation();
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	
	virtual void Tick(float DeltaTime) override;

	/**
 * Apply damage to this actor.
 * @see https://www.unrealengine.com/blog/damage-in-ue4
 * @param DamageAmount		How much damage to apply
 * @param DamageEvent		Data package that fully describes the damage received.
 * @param EventInstigator	The Controller responsible for the damage.
 * @param DamageCauser		The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
 * @return					The amount of damage actually applied.
 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	// https://docs.unrealengine.com/4.27/ko/InteractiveExperiences/Networking/Actors/RPCs/
	// Server : �������� ����
	// Reliable : �ŷڼ�
	UFUNCTION(Server, Reliable)
	void ReqTestMsg();

	// Server : ��ο��� ����
	UFUNCTION(NetMultiCast, Reliable)
	void ResTestMsg();

	// Client : �������� Ŭ���̾�Ʈ���� ����
	UFUNCTION(Client, Reliable)
	void ResTestMsgToOwner();

public:
	UFUNCTION(BlueprintCallable)
	void TestWeaponSpawn(TSubclassOf<class AWeapon> WeaponClass); // �θ� Ŭ������ Weapon�̾�� �Ѵ�.
																  // (C++�������� weapon �Ļ� Ŭ���� �����ϰų�, �������Ʈ ������ ���� weapon�� �˻��ؼ� ����
																  // Ȥ�� Ŭ���� ���ÿ� ���� �����Ͽ� �θ�Ŭ������ �ٲ��ָ� �ȴ�.
public:
	UFUNCTION(Server, Reliable)
	void ReqReload();


	UFUNCTION(NetMultiCast, Reliable)
	void ResReload();

public:
	UFUNCTION(Server, Reliable)
	void ReqShoot();


	UFUNCTION(NetMultiCast, Reliable)
	void ResShoot();

	void UpdateBindTestWeapon();

public:
	UFUNCTION(Server, Reliable)
	void ReqPressF();

	UFUNCTION(NetMultiCast, Reliable)
	void ResPressF(AActor* EquipActor);

	AActor* GetNearestActor(); //AActor*�� �������̴�. ������ �̸� ���� ���� ������

	void EventEquip(AActor* pActor);

public:
	UFUNCTION(Server, Reliable)
	void ReqDrop();

	UFUNCTION(NetMultiCast, Reliable)
	void ResDrop();

	void EventUnEquip();
		
public:
	UPROPERTY(Replicated)
	FRotator m_LookAtRotation;

	UFUNCTION(BlueprintPure)
	FRotator GetLookAtRotation() { return m_LookAtRotation; };

	UPROPERTY(Replicated, BlueprintReadWrite)
	AActor* m_pEquipWeapon;

	FTimerHandle th_BindTestWeapon;

public:
	float F(int D); // void �ȿ� �ִ� ���� ���. float�� ��ġ : F��� ���� � ���·�(float) ������ ������ �� �ڷ����� ���� ��ġ�̴�.
					// void�� ��°��� ������ ������ ���� ����.
					// F��  �Լ��� �̸��� �� ��ġ
					// �Լ� �� ( ) �� ���� �Ķ��Ÿ�̴�. �Է°�. -> ������ �ְ� ��¥�� ����� ���� ������. �� ������ ��ġ�̴�.
};

