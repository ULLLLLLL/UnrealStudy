// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShootingHUD.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/ShootingPlayerState.h"

void AShootingHUD::BeginPlay()
{
	Super::BeginPlay();

	//if (IsValid(HudWidgetClass) == false)
	//	return;
	check(HudWidgetClass); // check : check ()���� ��ü�� ���������̸� �ڵ尡 �״´�. (������ ���� ��ġ�� ��Ȯ�ϰ� �� �� �ִ�!)

	HudWidget = CreateWidget<UUserWidget>(GetWorld(), HudWidgetClass);
	HudWidget->AddToViewport();

	BindMyPlayerState(); // �� ��������� ���⼭ ȣ��!
}

void AShootingHUD::BindMyPlayerState()
{
	APlayerController* pPC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if (IsValid(pPC))
	{
		AShootingPlayerState* pPS = Cast<AShootingPlayerState>(pPC->PlayerState);
		if (IsValid(pPS))
		{
			pPS->Event_Dele_UpdateHp.AddDynamic(this, &AShootingHUD::OnUpdateMyHp); // ����ó�� ���ε��ϴ� ��, AddDynamic : ���ε�(�ν�Ÿ�׷� �ȷο��ؼ� �������� ������ ������ �޾ƿ�)
			OnUpdateMyHp(pPS->m_CurHp, 100);
			return; // ������ �ɷ����µ� Ÿ�̸� ���� �ȵǴ� �ݵ�� ����!
		}
	}
	FTimerManager& timerManager = GetWorld()->GetTimerManager(); // ������ ������ �ȵǸ� ����� �Ѿ�ͼ� Ÿ�̸� ������.(���� ��!)
	timerManager.SetTimer(th_BindMyPlayerState, this, &AShootingHUD::BindMyPlayerState, 0.01f, false);
}

void AShootingHUD::OnUpdateMyHp_Implementation(float CurHp, float MaxHp)
{
}

void AShootingHUD::OnUpdateMyAmmo_Implementation(int Ammo)
{
}
