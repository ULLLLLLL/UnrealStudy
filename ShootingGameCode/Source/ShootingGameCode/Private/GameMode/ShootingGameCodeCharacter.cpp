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
#include "Net/UnrealNetwork.h" // Replicated ó������ DOREPLIFETIME ����� ������ �ִ� ���̺귯��
#include "Weapon.h" // ../../���� �� �ܰ� ������
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

	//����
	int A(10);
	int B(20);
	int C = A + B;

	//�Լ�
	float H;
	H = F(4); // int D���� �־����Ƿ� �� �ȿ��� ���� ���� �־�� �Ѵ�.
			  // 4��� ������ ���� F��� �Լ��� ������� H�� �־���.
			// �Լ��� �ݵ�� ( ) ��ȣ�� ����.

}

// GetLifetimeReplicatedProps : Peplicated ������ �ڵ带 �����ϴ� ����
void AShootingGameCodeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShootingGameCodeCharacter, m_LookAtRotation); // �̷��� �ϸ� ������ �ٲ�� ��� Ŭ���̾�Ʈ�鿡 ������Ʈ�� �ȴ�.
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

	//Tick�� �������� ����Ǵ��� �������� ����Ǵ��� ���� ���������� �����ϵ���!
	//HasAuthority()�� true�� ����, false�� Ŭ���̾�Ʈ
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
		// Ÿ�̸Ӱ� ���ư��� �Լ��� �Ķ���Ͱ� �־�� �ȵȴ�. ���ϰ��� �־ �ȵȴ�. ������ void!
		return;
	}
	m_pEquipWeapon->SetOwner(GetController()); // Set Owner�� �̷��� ����.
}

AActor* AShootingGameCodeCharacter::GetNearestActor() // ���� Ŭ������ �ڰ� �Լ� �̸�. �ڿ����� �����ٰ� �ڷ����� AActor*�� ���ٿ��� ������� �������ش�.
{
	TArray<AActor*>actors;
	GetCapsuleComponent()->GetOverlappingActors(actors, AWeapon::StaticClass());

	double nearestLength = 9999999.0;
	AActor* nearestActor = nullptr;


	for(AActor* target : actors)
	{
	double distance = FVector::Dist(target->GetActorLocation(), GetActorLocation()); // ���� ���� ��ġ, �ڴ� ĳ������ ��ġ
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
	AActor* Z;				// ���� �ϳ� ����.
							// ������� ������ AActor* GetNearestActor();�� ��°��� �޾ƿ� ���̹Ƿ� �ڷ����� �����ϵ���
	Z = GetNearestActor();  // ���� = �Լ��̸� �������� �Լ��� �����ٰ� �� �� �ִ�.
							// GetNearestActor() ��� Z�� ���� ������ �Ʒ������� Z�� ġȯ�� �� �ִ�. ()�� �����ϰ� �ֱ� ������ �Ʒ����� �� �� ()�� ����� ��� Z�� ����.
	if (IsValid(Z) == false)
	{
		return;
	}
	Z->SetOwner(GetController()); // Set Owner�� �̷��� ����. ���� �� ���� ��ȣ �ݵ�� ����
	ResPressF(Z); // Z�� ������ ������ ���Ǳ⿡�� ���Ḧ �̴� ���� GetNearestActor()�� Z�� ������, Z�� ���� ������� �Ǵ� ���̴�.
				  // ���� ���Ḹ�� ������ �ڼ��� �۾��� �ϰ� �ͱ� ������ Z�� ġȯ�ؼ� ��� ����Ѵ�.
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


void AShootingGameCodeCharacter::ReqTestMsg_Implementation() //����
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
{// -1�� ����Ÿ������ Ư�� Ÿ���� ���� �ÿ� ���. 60.0f�� �󸶳� ���� �޼����� �������. �޽��� �÷�, �ؽ�Ʈ ����
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
