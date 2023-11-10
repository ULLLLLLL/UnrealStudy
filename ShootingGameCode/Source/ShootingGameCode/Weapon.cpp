// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h" // Replicated ó������ DOREPLIFETIME ����� ������ �ִ� ���̺귯��
#include "GameFramework/SpringArmComponent.h"
#include "GameMode/ShootingHUD.h"

// Sets default values
AWeapon::AWeapon():m_Ammo(30) // �ʱⰪ ����
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	WeaponMesh->SetCollisionProfileName("OverlapAllDynamic");

	bReplicates = true;
	SetReplicateMovement(true);

	SetRootComponent(WeaponMesh);
		
}
 // GetLifetimeReplicatedProps : Replicated ������ �ڵ带 �����ϴ� ����
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, m_pChar); // �̷��� �ϸ� ������ �ٲ�� ��� Ŭ���̾�Ʈ�鿡 ������Ʈ�� �ȴ�.
	DOREPLIFETIME(AWeapon, m_Ammo); // (Ŭ������, ����)�ε� Ŭ�������� �����ϰ� ������� �Ѵ�.
}


// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::EventTrigger_Implementation()
{
	if (IsValid(ShootMontage) == false)
		return;

	m_pChar->PlayAnimMontage(ShootMontage);
}

void AWeapon::EventShoot_Implementation()
{
	if (FireEffect == nullptr)
		return;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect,
		WeaponMesh->GetSocketLocation("muzzle"),
		WeaponMesh->GetSocketRotation("muzzle"),
		FVector(0.2f, 0.2f, 0.2f));

	ACharacter* pPlayer0 = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (pPlayer0 != m_pChar)
		return; // ĳ���Ͱ� index0���� �ƴϸ�(���� �ƴϸ�) ������

	FVector vStart;
	FVector vEnd;
	CalcShootStartEndPos(vStart, vEnd);
	//���� �Ʒ����ʹ� vStart, vEnd ���� ���ż� �� �ִ�.
	
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("(Client)ReqApplyDamage"));
	ReqApplyDamage(vStart, vEnd);
}

void AWeapon::EventPickup_Implementation(ACharacter* AChar) // �ǵ� ��ȣ�� �Է°�
{
	m_pChar = AChar; // m_pChar�̶�� ���� ���� ĳ������ ������ �־��� ���̴�
					 // ���� : ������ ���� �� 2���� ��� ( . )�� ( -> )
					 // m_pChar ���ǿ� * �����Ͱ� ������ ->ȭ��ǥ�� ����.
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToComponent(m_pChar->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("weapon"));
	UpdateHUD_MyAmmo(m_Ammo);

}

void AWeapon::EventDrog_Implementation()
{
	UpdateHUD_MyAmmo(0);
	m_pChar = nullptr;
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetSimulatePhysics(true);
	SetOwner(nullptr);
}

void AWeapon::ReqApplyDamage_Implementation(FVector vStart, FVector vEnd)
{
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("(Server)ReqApplyDamage"));

	UseAmmo();

	FCollisionObjectQueryParams collisionObjs;
	collisionObjs.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	collisionObjs.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	collisionObjs.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	collisionObjs.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
	collisionObjs.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
	collisionObjs.AddObjectTypesToQuery(ECollisionChannel::ECC_Destructible);

	FCollisionQueryParams collisionQuery; // �ø��� üũ �߰� �ɼ�
	collisionQuery.AddIgnoredActor(m_pChar); // ��Ʈ üũ �� ������ ���� �߰�

	FHitResult result;
	bool itHit = GetWorld()->LineTraceSingleByObjectType(result, vStart, vEnd, collisionObjs, collisionQuery);
	DrawDebugLine(GetWorld(), vStart, vEnd, FColor::Red, false, 5.0f); // ��� �����ִ� �Ŵ� false�� ����
	if (itHit == false) // ����Ʈ���̴���(��)�� ĳ���Ͷ� ��Ʈ��=���� ���� ��. ��Ʈ�� �ƴϸ� ����
		return;

	ACharacter* pHitChar = Cast<ACharacter>(result.GetActor());
	if (pHitChar == nullptr) // pHitChar�� nullptr�̶�� �� result.GetActor()�� ĳ���Ͱ� �ƴϴ�.
		return;
	
	UGameplayStatics::ApplyDamage(pHitChar, 10.0f, m_pChar->GetController(),this, UDamageType::StaticClass());
}

void AWeapon::OnRep_Ammo() 
{
	UpdateHUD_MyAmmo(m_Ammo);
}

void AWeapon::CalcShootStartEndPos(FVector& vStart, FVector& vEnd)
{
	if (IsValid(m_pChar) == false)
		return;

	USpringArmComponent* pArm = Cast<USpringArmComponent>(m_pChar->GetComponentByClass(USpringArmComponent::StaticClass()));
	if (IsValid(pArm) == false)
		return;

	APlayerController* pPlayer = GetWorld()->GetFirstPlayerController(); // �÷��̾� 0��(Player index 0��)
	FVector vCameraLoc = pPlayer->PlayerCameraManager->GetCameraLocation(); // ī�޶� ��ġ
	FVector vCameraForward = pPlayer->PlayerCameraManager->GetActorForwardVector(); // ���� ��ġ(����)

	vStart = (vCameraForward * (pArm->TargetArmLength + 100)) + vCameraLoc;
	vEnd = (vCameraForward * 10000) + vCameraLoc;
}

void AWeapon::UseAmmo()
{
	m_Ammo = m_Ammo - 1; // Ammo -= -1;
	m_Ammo = FMath::Clamp(m_Ammo, 0, 30); // min�� 0�̰� max�� 30
	OnRep_Ammo();
}

void AWeapon::UpdateHUD_MyAmmo(int Ammo)
{
	if (IsValid(m_pChar) == false)
		return;

	APlayerController* pPlayer0 = GetWorld()->GetFirstPlayerController();
	if (IsValid(pPlayer0) == false)
		return;

	AController* pOwner = m_pChar->GetController();
	if (pPlayer0 != pOwner)
		return;

	AShootingHUD* pHud = Cast<AShootingHUD>(pPlayer0->GetHUD());
	if (IsValid(pHud) == false)
		return;

	pHud->OnUpdateMyAmmo(Ammo); // HUD���� ������ �̺�Ʈ�� ���⿡�� �̷��� ȣ�����ش�.
}
