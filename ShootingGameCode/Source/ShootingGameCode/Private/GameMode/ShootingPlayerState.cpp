// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShootingPlayerState.h"
#include "Net/UnrealNetwork.h" // Replicated 처리에서 DOREPLIFETIME 기능을 가지고 있는 라이브러리

AShootingPlayerState::AShootingPlayerState():m_CurHp(100) // 초기값 100
{
}

void AShootingPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShootingPlayerState, m_CurHp); // 이렇게 하면 변수가 바뀌어 모든 클라이언트들에 업데이트가 된다.
												  // DOREPLIFETIME를 사용하려면 위에 인클루드를 해줘야 한다.
}

void AShootingPlayerState::AddDamage(float Damage)
{
	m_CurHp = m_CurHp - Damage;
	OnRep_CurHp();
}

void AShootingPlayerState::OnRep_CurHp() //이벤트가 호출되는 역할
{
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("OnRep_CurHp CurHp=%f"), m_CurHp));

	if (Event_Dele_UpdateHp.IsBound()) // 바인딩이 되면 (이벤트 디스패처)
		Event_Dele_UpdateHp.Broadcast(m_CurHp, 100); // 브로드캐스트 : 바인딩이 되어있는 변수들에게 모든 소식을 업데이트해준다.
}

