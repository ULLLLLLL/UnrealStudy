// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode/ShootingGameCodeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h" // Replicated 처리에서 DOREPLIFETIME 기능을 가지고 있는 라이브러리
#include "Weapon.h" // ../../폴더 한 단계 밑으로
#include "GameMode/ShootingPlayerState.h"

//////////////////////////////////////////////////////////////////////////
// AShootingGameCodeCharacter



AShootingGameCodeCharacter::AShootingGameCodeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//변수
	int A(10);
	int B(20);
	int C = A + B;

	//함수
	float H;
	H = F(4); // int D값을 넣었으므로 그 안에는 정수 값만 넣어야 한다.
			  // 4라는 동전을 넣은 F라는 함수의 결과값을 H에 넣었다.
			// 함수는 반드시 ( ) 괄호를 쓴다.

}

// GetLifetimeReplicatedProps : Peplicated 변수의 코드를 연결하는 영역
void AShootingGameCodeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShootingGameCodeCharacter, m_LookAtRotation); // 이렇게 하면 변수가 바뀌어 모든 클라이언트들에 업데이트가 된다.
	DOREPLIFETIME(AShootingGameCodeCharacter, m_pEquipWeapon);
}


void AShootingGameCodeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AShootingGameCodeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Tick이 서버에서 실행되는지 서버에서 실행되는지 보고 서버에서만 실행하도록!
	//HasAuthority()가 true면 서버, false면 클라이언트
	if (HasAuthority() == true)
	{
		m_LookAtRotation = GetControlRotation();
	}
}

float AShootingGameCodeCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green,
		FString::Printf(TEXT("TakeDamage DamageAmonut=%f EventInstigator=&s DamageAmount=%s"),
			DamageAmount,
			*EventInstigator->GetName(),
			*DamageCauser->GetName()));

	AShootingPlayerState* pPS = Cast<AShootingPlayerState>(GetPlayerState());
	if (IsValid(pPS) == false)
		return 0.0f;

	pPS->AddDamage(10.0f);

	return 0.0f;
}

void AShootingGameCodeCharacter::TestWeaponSpawn(TSubclassOf<class AWeapon> WeaponClass) 
{
	AWeapon* pWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, FVector(0, 0, 0), FRotator(0, 0, 0));
	m_pEquipWeapon = pWeapon;

	pWeapon->m_pChar = this;
	pWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("weapon"));

	UpdateBindTestWeapon();
}

void AShootingGameCodeCharacter::UpdateBindTestWeapon()
{
	if (IsValid(GetController()) == false)
	{
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.SetTimer(th_BindTestWeapon, this, &AShootingGameCodeCharacter::UpdateBindTestWeapon, 0.01f, false);
		// 타이머가 돌아가는 함수는 파라미터가 있어서는 안된다. 리턴값도 있어선 안된다. 무조건 void!
		return;
	}
	m_pEquipWeapon->SetOwner(GetController()); // Set Owner은 이렇게 쓴다.
}

AActor* AShootingGameCodeCharacter::GetNearestActor() // 앞이 클래스고 뒤가 함수 이름. 뒤에꺼를 가져다가 자료형인 AActor*에 덧붙여서 헤더에서 선언해준다.
{
	TArray<AActor*>actors;
	GetCapsuleComponent()->GetOverlappingActors(actors, AWeapon::StaticClass());

	double nearestLength = 9999999.0;
	AActor* nearestActor = nullptr;


	for(AActor* target : actors)
	{
	double distance = FVector::Dist(target->GetActorLocation(), GetActorLocation()); // 앞은 총의 위치, 뒤는 캐릭터의 위치
	if (nearestLength < distance)
		continue;
	
	nearestLength = distance;
	nearestActor = target;
	}
	return nearestActor;
}

void AShootingGameCodeCharacter::EventEquip(AActor* pActor)
{
	bUseControllerRotationYaw = true;
	m_pEquipWeapon = pActor;

	IWeaponInterface* pInterface = Cast<IWeaponInterface>(m_pEquipWeapon);

	if (pInterface == nullptr)
		return;

	pInterface->Execute_EventPickup(m_pEquipWeapon, this);
}

void AShootingGameCodeCharacter::ReqDrop_Implementation()
{
	ResDrop();
}

void AShootingGameCodeCharacter::ResDrop_Implementation()
{
	EventUnEquip();
}

void AShootingGameCodeCharacter::EventUnEquip()
{
	bUseControllerRotationYaw = false;
	IWeaponInterface* pInterface = Cast<IWeaponInterface>(m_pEquipWeapon);

	if (pInterface == nullptr)
		return;

	pInterface->Execute_EventDrog(m_pEquipWeapon);

	m_pEquipWeapon = nullptr;
}

float AShootingGameCodeCharacter::F(int D)
{
	return 0.0f;
}


void AShootingGameCodeCharacter::ReqReload_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ReqReload"));
	ResReload();
}

void AShootingGameCodeCharacter::ResReload_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ResReload"));
}


void AShootingGameCodeCharacter::ReqShoot_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ReqShoot"));
	ResShoot();
}

void AShootingGameCodeCharacter::ResShoot_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ResShoot"));

	IWeaponInterface* pInterface = Cast<IWeaponInterface>(m_pEquipWeapon);

	if (pInterface == nullptr)
		return;

	pInterface->Execute_EventTrigger(m_pEquipWeapon);
}

void AShootingGameCodeCharacter::ReqPressF_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ReqPressF"));
	AActor* Z;				// 변수 하나 선언.
							// 헤더에서 선언한 AActor* GetNearestActor();의 출력값을 받아올 것이므로 자료형이 동일하도록
	Z = GetNearestActor();  // 변수 = 함수이름 형식으로 함수를 가져다가 쓸 수 있다.
							// GetNearestActor() 대신 Z를 쓰기 때문에 아래에서도 Z로 치환할 수 있다. ()도 포함하고 있기 때문에 아래에서 쓸 때 ()를 지우고 대신 Z를 쓴다.
	if (IsValid(Z) == false)
	{
		return;
	}
	Z->SetOwner(GetController()); // Set Owner은 이렇게 쓴다. 변수 쓸 때는 괄호 반드시 쓰기
	ResPressF(Z); // Z로 보내는 이유는 자판기에서 음료를 뽑는 것인 GetNearestActor()를 Z로 보내서, Z는 뽑은 음료수가 되는 것이다.
				  // 뽑은 음료만을 가지고 자세히 작업을 하고 싶기 때문에 Z로 치환해서 계속 사용한다.
}

void AShootingGameCodeCharacter::ResPressF_Implementation(AActor* EquipActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ResPressF"));

	if (IsValid(m_pEquipWeapon))
	{
		EventUnEquip();
	}
	
	EventEquip(EquipActor);
}


void AShootingGameCodeCharacter::ReqTestMsg_Implementation() //서버
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ReqTestMsg"));
	ResTestMsg();
}

void AShootingGameCodeCharacter::ResTestMsg_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("ResTestMsg"));
}

void AShootingGameCodeCharacter::ResTestMsgToOwner_Implementation()
{
}


//////////////////////////////////////////////////////////////////////////
// Input

void AShootingGameCodeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//ETriggerEvent::Started = Press
		//ETriggerEvent::Completed = Release
		//ETriggerEvent::Triggered = Press Tick
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShootingGameCodeCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShootingGameCodeCharacter::Look);

		//Test
		EnhancedInputComponent->BindAction(TestAction, ETriggerEvent::Started, this, &AShootingGameCodeCharacter::Test);

		//Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AShootingGameCodeCharacter::Reload);

		//Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AShootingGameCodeCharacter::Shoot);

		//PressF
		EnhancedInputComponent->BindAction(PressFAction, ETriggerEvent::Started, this, &AShootingGameCodeCharacter::PressF);

		//PressF
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &AShootingGameCodeCharacter::Drop);
	}

}

void AShootingGameCodeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AShootingGameCodeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AShootingGameCodeCharacter::Test(const FInputActionValue& Value)
{// -1은 에러타입으로 특정 타입이 없을 시에 사용. 60.0f는 얼마나 오래 메세지를 띄워줄지. 메시지 컬러, 텍스트 내용
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("Test"));
	ReqTestMsg();
}

void AShootingGameCodeCharacter::Reload(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("Reload"));
	ReqReload();
}

void AShootingGameCodeCharacter::Shoot(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("Shoot"));
	ReqShoot();
}

void AShootingGameCodeCharacter::PressF(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("PressF"));
	ReqPressF();
}

void AShootingGameCodeCharacter::Drop(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("RepDorp"));
	ReqDrop();
}
