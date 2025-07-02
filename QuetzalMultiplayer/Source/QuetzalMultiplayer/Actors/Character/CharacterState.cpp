// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ACharacterState::ACharacterState():
	State(ECharacterState::IDLE),
	InvulnerableTimer(1.0f),
	IsOnGround(true)
{
	bReplicates = true;
	SetReplicates(true);
}

void ACharacterState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterState, State);
	DOREPLIFETIME(ACharacterState, IsOnGround);
	DOREPLIFETIME(ACharacterState, HP);
	DOREPLIFETIME(ACharacterState, PlayerIndex);
	DOREPLIFETIME(ACharacterState, StockCPP);
}