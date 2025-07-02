// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObjects/Piano.h"
#include "GameFramework/GameState.h"
#include <Kismet/GameplayStatics.h>
APiano::APiano() :
	damageComp(nullptr)
{
	/*Damage Comp*/
	damageComp = CreateDefaultSubobject<UDamageSystemComp>(TEXT("Damage System Component"));
	damageComp->OnDeath.AddUniqueDynamic(this, &APiano::OnBreakPiano);

	SetMaxHealth(PIANO_MAX_HEALTH);
}

void APiano::OnBreakPiano()
{
    if (BreakSoundCue)
    {
        UGameplayStatics::PlaySoundAtLocation(this, BreakSoundCue, GetActorLocation());
    }
    TArray<AActor*> ActorsToFind;
    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuetzalMultiplayerCharacter::StaticClass(), ActorsToFind);
    }
    for (AActor* actor : ActorsToFind)
    {
        AQuetzalMultiplayerCharacter* character = Cast<AQuetzalMultiplayerCharacter>(actor);
      
        if (character != nullptr && instigator != nullptr)
        {
            if (character != instigator)
            {
                character->ApplyStateChange(ECharacterState::DANCE_STUNNED);
            }
        }
    }

	auto& TimerManager = GetWorld()->GetTimerManager();
	
    TimerManager.SetTimer(pianoCooldown, this, &APiano::resetPiano, 10.f);
   
    SetPianoColor(BrokenMaterial, BrokenMaterial, BrokenMaterial);
}


int APiano::GetHealth()
{
	return damageComp->GetHealth();
}

int APiano::GetMaxHealth()
{
	return damageComp->GetMaxHealth();
}

void APiano::SetMaxHealth(int setMaxHealth)
{
	damageComp->SetMaxHealth(setMaxHealth);
}

void APiano::SetHealth(int setHealth)
{
	damageComp->SetHealth(setHealth);
}

bool APiano::TakeDamage(FDamageInfo damageInfo)
{
    if(damageInfo.InstigatedBy!= nullptr)
	    instigator = Cast<AQuetzalMultiplayerCharacter>(damageInfo.InstigatedBy);
	return damageComp->TakeDamage(damageInfo);
}

void APiano::SetIsDead(bool isDead)
{
    damageComp->SetIsDead(isDead);
}

void APiano::SetPianoColor(UMaterialInterface* mat0, UMaterialInterface* mat1, UMaterialInterface* mat2)
{
   
   GetStaticMeshComponent()->SetMaterial(0,mat0);
   GetStaticMeshComponent()->SetMaterial(1, mat1);
   GetStaticMeshComponent()->SetMaterial(2, mat2);
}

void APiano::resetPiano()
{
    if (BreakSoundCue)
    {
        UGameplayStatics::PlaySoundAtLocation(this, RestoreSoundCue, GetActorLocation());
    }
    SetHealth(PIANO_MAX_HEALTH);
    SetIsDead(false);
    SetPianoColor(originalMaterial0, originalMaterial1, originalMaterial2);
}

