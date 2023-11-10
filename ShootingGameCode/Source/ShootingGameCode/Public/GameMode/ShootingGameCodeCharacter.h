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
	class UInputAction* LookAction; //두 줄이 한 세트

	/** Test Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TestAction; // 키 입력

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
	// Server : 서버에서 실행
	// Reliable : 신뢰성
	UFUNCTION(Server, Reliable)
	void ReqTestMsg();

	// Server : 모두에서 실행
	UFUNCTION(NetMultiCast, Reliable)
	void ResTestMsg();

	// Client : 소유중인 클라이언트에서 실행
	UFUNCTION(Client, Reliable)
	void ResTestMsgToOwner();

public:
	UFUNCTION(BlueprintCallable)
	void TestWeaponSpawn(TSubclassOf<class AWeapon> WeaponClass); // 부모 클래스가 Weapon이어야 한다.
																  // (C++폴더에서 weapon 파생 클래스 생성하거나, 블루프린트 생성에 들어가서 weapon을 검색해서 생성
																  // 혹은 클래스 세팅에 들어가서 디테일에 부모클래스를 바꿔주면 된다.
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

	AActor* GetNearestActor(); //AActor*이 변수형이다. 변수의 이름 앞이 전부 변수형

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
	float F(int D); // void 안에 있는 것이 출력. float의 위치 : F라는 값을 어떤 형태로(float) 내보낼 것인지 그 자료형을 쓰는 위치이다.
					// void는 출력값이 없으니 내보낼 값도 없다.
					// F는  함수의 이름이 들어갈 위치
					// 함수 옆 ( ) 안 값은 파라메타이다. 입력값. -> 동전을 넣고 얼마짜리 라면을 뽑을 것인지. 그 동전의 위치이다.
};

