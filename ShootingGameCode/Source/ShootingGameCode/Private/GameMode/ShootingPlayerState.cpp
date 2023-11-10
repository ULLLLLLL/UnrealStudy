// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShootingPlayerState.h"
#include "Net/UnrealNetwork.h" // Replicated ó������ DOREPLIFETIME ����� ������ �ִ� ���̺귯��

AShootingPlayerState::AShootingPlayerState():m_CurHp(100) // �ʱⰪ 100
{
}

void AShootingPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShootingPlayerState, m_CurHp); // �̷��� �ϸ� ������ �ٲ�� ��� Ŭ���̾�Ʈ�鿡 ������Ʈ�� �ȴ�.
												  // DOREPLIFETIME�� ����Ϸ��� ���� ��Ŭ��带 ����� �Ѵ�.
}

void AShootingPlayerState::AddDamage(float Damage)
{
	m_CurHp = m_CurHp - Damage;
	OnRep_CurHp();
}

void AShootingPlayerState::OnRep_CurHp() //�̺�Ʈ�� ȣ��Ǵ� ����
{
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("OnRep_CurHp CurHp=%f"), m_CurHp));

	if (Event_Dele_UpdateHp.IsBound()) // ���ε��� �Ǹ� (�̺�Ʈ ����ó)
		Event_Dele_UpdateHp.Broadcast(m_CurHp, 100); // ��ε�ĳ��Ʈ : ���ε��� �Ǿ��ִ� �����鿡�� ��� �ҽ��� ������Ʈ���ش�.
}

