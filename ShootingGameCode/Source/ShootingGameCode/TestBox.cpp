// Fill out your copyright notice in the Description page of Project Settings.


#include "TestBox.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h" //ĳ���� ����ȯ �ϱ� ����

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
	ACharacter* pChar = Cast<ACharacter>(OtherActor); // ĳ���� ����ȯ�ϱ� ���� - ���� ó��
	if (pChar == nullptr) // ĳ���Ͱ� ������ null -> ��ȯ�ż� ���� �� �Ѵ�.
		return;

	if (IsValid(ActorClass) == false) // ������ ActorClass�� ����� ���� �ÿ� ����
		return;						  

	if (HasAuthority() == false) // ���� �������� �������� �ϱ� ����. HasAuthority = true�� �� ������
		return;

	AActor* pActor = GetWorld()->SpawnActor<AActor>(ActorClass, Arrow->GetComponentTransform()); // Arrow ��ġ���� ActorClass�� ���������� �����Ѵ�.
}

