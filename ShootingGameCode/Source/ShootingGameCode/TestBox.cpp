// Fill out your copyright notice in the Description page of Project Settings.


#include "TestBox.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h" //캐릭터 형변환 하기 위해

// Sets default values
ATestBox::ATestBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Static"));

	SetRootComponent(Scene);
	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Arrow->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	StaticMesh->SetCollisionProfileName("OverlapAllDynamic");

	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ATestBox::OnBeginOverlap);
}

// Called when the game starts or when spawned
void ATestBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
								int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* pChar = Cast<ACharacter>(OtherActor); // 캐릭터 형변환하기 위해 - 예외 처리
	if (pChar == nullptr) // 캐릭터가 없으면 null -> 반환돼서 실행 안 한다.
		return;

	if (IsValid(ActorClass) == false) // 스폰될 ActorClass에 대상이 있을 시에 실행
		return;						  

	if (HasAuthority() == false) // 공이 서버에만 나오도록 하기 위해. HasAuthority = true일 때 서버만
		return;

	AActor* pActor = GetWorld()->SpawnActor<AActor>(ActorClass, Arrow->GetComponentTransform()); // Arrow 위치에서 ActorClass가 떨어지도록 스폰한다.
}

