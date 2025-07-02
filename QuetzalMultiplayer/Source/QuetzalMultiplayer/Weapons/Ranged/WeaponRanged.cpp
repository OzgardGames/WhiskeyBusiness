// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponRanged.h"

AWeaponRanged::AWeaponRanged():
	AWeapon(),
	currentAmmo(1),
	muzzleSocketName(FName("Muzzle_Socket"))
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	isRangedWeapon = true;
}
void AWeaponRanged::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}







