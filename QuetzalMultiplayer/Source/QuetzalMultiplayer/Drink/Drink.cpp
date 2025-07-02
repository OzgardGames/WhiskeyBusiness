// Fill out your copyright notice in the Description page of Project Settings.


#include "Drink/Drink.h"

// Sets default values
ADrink::ADrink()
{
    PrimaryActorTick.bCanEverTick = true;
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->InitSphereRadius(50.0f); // 设置碰撞半径为50
    CollisionSphere->SetCollisionProfileName(TEXT("Trigger")); // 设置碰撞配置为触发器
    RootComponent = CollisionSphere; // 将碰撞球体设为根组件

    // 创建静态网格组件
    DrinkMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DrinkMesh"));
    DrinkMesh->SetupAttachment(RootComponent); // 将网格组件附加到根组件
    DrinkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 禁用网格的碰撞

    // 默认属性
    DrinkName = TEXT("Default Drink");
    DrinkType = EDrinkType::None;
    EffectDuration = 0.0f;

    SetReplicates(true);
    SetReplicateMovement(true);
    bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ADrink::BeginPlay()
{
	Super::BeginPlay();
    
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADrink::OnOverlapBegin);

    

    SetRespawnFlag(true);
    SetRespawnTime(10);//TODO set timers in begin play of other drinks 

    ActivateActor(false);
}

// Called every frame
void ADrink::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    FloatDrink(DeltaTime);
}

void ADrink::ApplyEffect(AQuetzalMultiplayerCharacter* Character)
{
    SetRespawnFlag(true);
    ActivateActor(false);
}

void ADrink::ConsumeDrink()
{
}

void ADrink::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AQuetzalMultiplayerCharacter* Character = Cast<AQuetzalMultiplayerCharacter>(OtherActor);
    if (Character)
    {
        // 应用饮料效果
        ApplyEffect(Character);

        // 消耗饮料
        ConsumeDrink(); 
    }
}

void ADrink::FloatDrink(float DeltaTime)
{
    FRotator spinRotation = FRotator(0.0f, 100.0f, 0.0f);
    UpDownLocation = GetActorLocation();
    drinkFloatingTimer += DeltaTime;
    UpDownLocation.Z = GetActorLocation().Z + 0.2f * FMath::Sin(2 * PI * drinkFloatingTimer / 2);
    

    SetActorLocation(UpDownLocation);
    SetActorRotation(spinRotation * drinkFloatingTimer);
}

void ADrink::SetRespawnTime(float time)
{
    Server_SetRespawnTime(time);
}

void ADrink::Server_SetRespawnTime_Implementation(float time)
{
    MC_SetRespawnTime(time);
}

void ADrink::MC_SetRespawnTime_Implementation(float time)
{
    respawnTime = time;
}

void ADrink::SetRespawnFlag(bool respawnFlag)
{
    Server_SetRespawnFlag(respawnFlag);
}

void ADrink::Server_SetRespawnFlag_Implementation(bool respawn)
{
    MC_SetRespawnFlag(respawn);
}

void ADrink::MC_SetRespawnFlag_Implementation(bool respawn)
{
    bRespawnFlag = respawn;
}

void ADrink::RespawnActor(UStructSpawnLocationAndRotation spawnLocationAndRotation)
{
    Server_RespawnActor(spawnLocationAndRotation.spawnLocation, spawnLocationAndRotation.spawnRotation);
}

void ADrink::Server_RespawnActor_Implementation(FVector spawnLocation, FRotator spawnRotation)
{
    MC_RespawnActor(spawnLocation, spawnRotation);
}

void ADrink::MC_RespawnActor_Implementation(FVector spawnLocation, FRotator spawnRotation)
{
    ActivateActor(true);
    SetActorLocationAndRotation(spawnLocation, spawnRotation);
}
float ADrink::GetRespawnTime()
{
    return respawnTime;
}
bool ADrink::GetRespawnFlag()
{
    return bRespawnFlag;
}
void ADrink::ActivateActor(bool active)
{
    Server_ActivateActor(active);
}

void ADrink::Server_ActivateActor_Implementation(bool active)
{
    MC_ActivateActor(active);
}

void ADrink::MC_ActivateActor_Implementation(bool active)
{
    // Hides visible components
    SetActorHiddenInGame(!active);

    // Disables collision components
    SetActorEnableCollision(active); //TODO this may turn on throw collision need testing

    // Stops the Actor from ticking
    SetActorTickEnabled(active);
}