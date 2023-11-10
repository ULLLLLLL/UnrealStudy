// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h" // Replicated 처리에서 DOREPLIFETIME 기능을 가지고 있는 라이브러리
#include "GameFramework/SpringArmComponent.h"
#include "GameMode/ShootingHUD.h"

// Sets default values
AWeapon::AWeapon():m_Ammo(30) // 초기값 설정
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	WeaponMesh->SetCollisionProfileName("OverlapAllDynamic");

	bReplicates = true;
	SetReplicateMovement(true);

	SetRootComponent(WeaponMesh);
		
}
 // GetLifetimeReplicatedProps : Replicated 변수의 코드를 연결하는 영역
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, m_pChar); // 이렇게 하면 변수가 바뀌어 모든 클라이언트들에 업데이트가 된다.
	DOREPLIFETIME(AWeapon, m_Ammo); // (클래스명, 변수)인데 클래스명을 동일하게 맞춰줘야 한다.
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
		return; // 캐릭터가 index0번이 아니면(내가 아니면) 하지마

	FVector vStart;
	FVector vEnd;
	CalcShootStartEndPos(vStart, vEnd);
	//여기 아래부터는 vStart, vEnd 값이 계산돼서 들어가 있다.
	
	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("(Client)ReqApplyDamage"));
	ReqApplyDamage(vStart, vEnd);
}

void AWeapon::EventPickup_Implementation(ACharacter* AChar) // 맨뒤 괄호가 입력값
{
	m_pChar = AChar; // m_pChar이라는 변수 값에 캐릭터형 변수를 넣어줄 것이다
					 // 참조 : 라인을 끄는 것 2가지 방법 ( . )과 ( -> )
					 // m_pChar 정의에 * 포인터가 있으면 ->화살표를 쓴다.
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

	FCollisionQueryParams collisionQuery; // 컬리전 체크 추가 옵션
	collisionQuery.AddIgnoredActor(m_pChar); // 히트 체크 시 무시할 액터 추가

	FHitResult result;
	bool itHit = GetWorld()->LineTraceSingleByObjectType(result, vStart, vEnd, collisionObjs, collisionQuery);
	DrawDebugLine(GetWorld(), vStart, vEnd, FColor::Red, false, 5.0f); // 계속 보여주는 거는 false로 간다
	if (itHit == false) // 라인트레이더스(총)가 캐릭터랑 히트면=총을 맞은 것. 히트가 아니면 리턴
		return;

	ACharacter* pHitChar = Cast<ACharacter>(result.GetActor());
	if (pHitChar == nullptr) // pHitChar가 nullptr이라는 건 result.GetActor()가 캐릭터가 아니다.
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

	APlayerController* pPlayer = GetWorld()->GetFirstPlayerController(); // 플레이어 0번(Player index 0번)
	FVector vCameraLoc = pPlayer->PlayerCameraManager->GetCameraLocation(); // 카메라 위치
	FVector vCameraForward = pPlayer->PlayerCameraManager->GetActorForwardVector(); // 액터 위치(방향)

	vStart = (vCameraForward * (pArm->TargetArmLength + 100)) + vCameraLoc;
	vEnd = (vCameraForward * 10000) + vCameraLoc;
}

void AWeapon::UseAmmo()
{
	m_Ammo = m_Ammo - 1; // Ammo -= -1;
	m_Ammo = FMath::Clamp(m_Ammo, 0, 30); // min이 0이고 max가 30
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

	pHud->OnUpdateMyAmmo(Ammo); // HUD에서 생성한 이벤트를 여기에서 이렇게 호출해준다.
}
