// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShootingHUD.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/ShootingPlayerState.h"

void AShootingHUD::BeginPlay()
{
	Super::BeginPlay();

	//if (IsValid(HudWidgetClass) == false)
	//	return;
	check(HudWidgetClass); // check : check ()안의 객체가 비정상적이면 코드가 죽는다. (에러가 나는 위치를 명확하게 알 수 있다!)

	HudWidget = CreateWidget<UUserWidget>(GetWorld(), HudWidgetClass);
	HudWidget->AddToViewport();

	BindMyPlayerState(); // 다 만들었으니 여기서 호출!
}

void AShootingHUD::BindMyPlayerState()
{
	APlayerController* pPC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if (IsValid(pPC))
	{
		AShootingPlayerState* pPS = Cast<AShootingPlayerState>(pPC->PlayerState);
		if (IsValid(pPS))
		{
			pPS->Event_Dele_UpdateHp.AddDynamic(this, &AShootingHUD::OnUpdateMyHp); // 디스패처에 바인딩하는 것, AddDynamic : 바인딩(인스타그램 팔로우해서 포스팅할 때마다 정보를 받아옴)
			OnUpdateMyHp(pPS->m_CurHp, 100);
			return; // 위에서 걸러졌는데 타이머 돌면 안되니 반드시 리턴!
		}
	}
	FTimerManager& timerManager = GetWorld()->GetTimerManager(); // 위에서 성립이 안되면 여기로 넘어와서 타이머 돌린다.(위로 가!)
	timerManager.SetTimer(th_BindMyPlayerState, this, &AShootingHUD::BindMyPlayerState, 0.01f, false);
}

void AShootingHUD::OnUpdateMyHp_Implementation(float CurHp, float MaxHp)
{
}

void AShootingHUD::OnUpdateMyAmmo_Implementation(int Ammo)
{
}
