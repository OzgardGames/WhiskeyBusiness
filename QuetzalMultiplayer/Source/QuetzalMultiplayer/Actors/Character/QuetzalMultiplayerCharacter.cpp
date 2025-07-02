// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuetzalMultiplayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Components/PickupObjectsComp/PickupObjectInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "RagdollComponent.h"
#include "Blueprint/UserWidget.h"
#include "QuetzalHUD.h"
#include "Player_GeometryCollectionF.h"
#include "Blueprint/UserWidget.h"
#include "DanceMinigameWidget.h"
#include "GameFramework/GameState.h"
#include "WinText.h"
#include "Weapons/Melee/BaseballBat.h"
#include "Weapons/Melee/Mallet.h"
#include "Audio/AudioManager.h"
#include "PostLoginWaitScreen.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AQuetzalMultiplayerCharacter

AQuetzalMultiplayerCharacter::AQuetzalMultiplayerCharacter() : equipedWeapon(nullptr),
lightAttackInputHoldTimer(PLAYER_INPUT_HOLD_DURATION),
specialAttackInputHoldTimer(PLAYER_INPUT_HOLD_DURATION),
lightAttackHoldSwitch(false),
bIsHoldingObject(false),
highLightedObject(),
DEBUG_MSG_TakeDamage(false),
DEBUG_MSG_DamageResponse(false),
holdObjectSocketName("ObjectPickup_BoneSocket"),
damageComp(nullptr),
CharacterStateCPP(nullptr)

{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 51.f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    SetOrientRotationToMovement(true);
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;

    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    setMovementSpeed(PLAYER_WALK_SPEED);

    SetReplicates(true);
    SetReplicateMovement(true);
    bAlwaysRelevant = true;
    /*Circle under player mesh*/
    static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Script/Engine.StaticMesh'/Game/ThirdPerson/StaticMesh/Shape_Plane.Shape_Plane'"));
    StaticMeshUnderCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    StaticMeshUnderCircle->SetupAttachment(RootComponent);
    StaticMeshUnderCircle->SetStaticMesh(DefaultMesh.Object);
    StaticMeshUnderCircle->SetVisibility(true);
    static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Script/Engine.Material'/Game/ThirdPerson/Meterials/M_UnderPlayerCircle.M_UnderPlayerCircle'"));
    if (FoundMaterial.Succeeded())
    {
        UnderCircleMaterial = FoundMaterial.Object;
    }
    UnderCircleDynamicMaterialInst = UMaterialInstanceDynamic::Create(UnderCircleMaterial, StaticMeshUnderCircle);
    StaticMeshUnderCircle->SetMaterial(0, UnderCircleDynamicMaterialInst);
    StaticMeshUnderCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StaticMeshUnderCircle->SetAbsolute(false, false, true);
    /********************/

    /*Interact with objects radius*/
    InteractRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractCollision"));
    InteractRadius->OnComponentBeginOverlap.AddDynamic(this, &AQuetzalMultiplayerCharacter::OnOverlapInteractBegin);
    InteractRadius->OnComponentEndOverlap.AddDynamic(this, &AQuetzalMultiplayerCharacter::OnOverlapInteractEnd);
    InteractRadius->SetGenerateOverlapEvents(true);
    InteractRadius->IgnoreActorWhenMoving(this, true);
    InteractRadius->SetRelativeLocation(FVector(60, 0, -10)); //(X=60.000000,Y=0.000000,Z=-10.000000)
    InteractRadius->SetupAttachment(RootComponent);

    HighlightDynamicMaterialInst = CreateDefaultSubobject<UMaterialInstance>(TEXT("InteractColor"));
    /******************************************/

    /*Throwing collision*/
    throwCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("ThrowCollision"));
    throwCollisionComp->InitSphereRadius(30.0f);
    throwCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AQuetzalMultiplayerCharacter::OnOverlapThrowBegin);
    throwCollisionComp->SetGenerateOverlapEvents(false);
    throwCollisionComp->SetupAttachment(RootComponent);
    /********************/

    /*Damage Comp*/
    damageComp = CreateDefaultSubobject<UDamageSystemComp>(TEXT("Damage System Component"));
    damageComp->OnDeath.AddUniqueDynamic(this, &AQuetzalMultiplayerCharacter::Die);
    damageComp->OnDamageResponse.AddUniqueDynamic(this, &AQuetzalMultiplayerCharacter::DamageResponceHandler);
    damageComp->OnBlock.AddUniqueDynamic(this, &AQuetzalMultiplayerCharacter::DamageBlocked);
    damageComp->OnStaggerDepleted.AddUniqueDynamic(this, &AQuetzalMultiplayerCharacter::StunState);
    // damageComp->OnStaggerDepleted.AddUniqueDynamic(this, &AQuetzalMultiplayerCharacter::DanceStunState);
    damageComp->OnTriggerInvulnerable.AddUniqueDynamic(this, &AQuetzalMultiplayerCharacter::SetIsInvulnerable);
    deathFractureAngularVelocity = FVector(5.f, 5.f, 5.f);
    /********************/
    RagdollComponent = CreateDefaultSubobject<URagdollComponent>(TEXT("Ragdoll Component"));

    GetMesh()->SetAbsolute(false, false, true);
    isInlobbyCPP = true;
}

void AQuetzalMultiplayerCharacter::OnRep_CharacterState()
{
    if (CharacterStateCPP)
    {
        ApplyStateChange(CharacterStateCPP->State);
    }
}

void AQuetzalMultiplayerCharacter::CreateWaitingScreen()
{
    /*if (GetLocalRole() == ROLE_Authority)
    {*/
    //    if (IsValid(loginWidgetTemplate))
    //    {
    //        loginWidget = Cast<UPostLoginWaitScreen>(CreateWidget(GetWorld(), loginWidgetTemplate));
    //    }
    //    if (IsValid(loginWidget))
    //    {
    //        //loginWidget->AddToPlayerScreen();
    //        loginWidget->AddToViewport();
    //        loginWidget->SetVisibility(ESlateVisibility::Visible);
    //    }
    //}

}

void AQuetzalMultiplayerCharacter::SetPlayerStatePointer()
{
    ACharacterState* characterState = Cast<ACharacterState>(GetPlayerState());
    if (characterState)
    {
        characterState->characterPointer = this;
    }
}

void AQuetzalMultiplayerCharacter::checkForWinner()
{
    AGameStateBase* GState = GetWorld()->GetGameState<AGameStateBase>();

    int playersRemaining = GState->PlayerArray.Num();
    ACharacterState* winnerState = nullptr;

    for (APlayerState* PState : GState->PlayerArray)
    {
        ACharacterState* characterState = Cast<ACharacterState>(PState);

        if (characterState)
        {
            if (characterState->StockCPP <= 0)
            {
                playersRemaining--;
            }
            else
            {
                winnerState = characterState;
            }
        }
    }

    if (playersRemaining == 1) // Debug set to 1
    {

        if (winnerState)
        {
            FString winText = FString::Printf(TEXT("Player %d wins! \n Cheers!"), winnerState->PlayerIndex);

            if (WinTextWidget)
            {
                WinTextWidget->winner = FText::FromString(winText);

                WinTextWidget->DisplayWinnerText();
                WinTextWidget->SetVisibility(ESlateVisibility::Visible);



            }
        }
    }
}

void AQuetzalMultiplayerCharacter::Lobby()
{
    AGameStateBase* GState = GetWorld()->GetGameState<AGameStateBase>();

    int playersRemaining = GState->PlayerArray.Num();

    if (isInlobbyCPP)
    {
        FString winText = FString::Printf(TEXT("Waiting for players \n %d/4"), playersRemaining);
    }
}

void AQuetzalMultiplayerCharacter::SetCircleColor(const FVector& Color)
{
    Server_SetCircleColor(Color);
}

void AQuetzalMultiplayerCharacter::SetMaterialS(UMaterialInterface* Material)
{
    Server_SetMaterialS(Material);
}

void AQuetzalMultiplayerCharacter::Server_SetMaterialS_Implementation(UMaterialInterface* Material)
{
    MC_SetMaterialS(Material);
}

void AQuetzalMultiplayerCharacter::MC_SetMaterialS_Implementation(UMaterialInterface* Material)
{
    USkeletalMeshComponent* SkelMesh = GetMesh();
    SkelMesh->SetMaterial(0, Material);
}

void AQuetzalMultiplayerCharacter::Server_SetCircleColor_Implementation(const FVector& Color)
{
    MC_SetCircleColor(Color);
}

void AQuetzalMultiplayerCharacter::MC_SetCircleColor_Implementation(const FVector& Color)
{
    StaticMeshUnderCircle->SetVectorParameterValueOnMaterials("Color", Color);
}

void AQuetzalMultiplayerCharacter::BeginPlay()
{
    // Call the base class
    Super::BeginPlay();

    if (AudioManagerClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        AudioManager = GetWorld()->SpawnActor<AAudioManager>(AudioManagerClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

        if (AudioManager)
        {
            AudioManager->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
        }
    }

    if (CharacterStateCPP == nullptr)
        CharacterStateCPP = GetPlayerState<ACharacterState>();
    if (CharacterStateCPP == nullptr)
        CharacterStateCPP = Cast<ACharacterState>(GetPlayerState());

    // Sets the Character State in the Animation class
    if (myAnimInstance == nullptr)
        myAnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());

    // RagdollComponent = FindComponentByClass<URagdollComponent>();

    ApplyStateChange(ECharacterState::IDLE);
    damageComp->SetMaxHealth(PLAYER_HEALTH);
    damageComp->SetMaxStaggerHealth(PLAYER_STAGGER_HEALTH);
    // Adjust locations on BeginPlay
    GetCapsuleComponent()->InitCapsuleSize(42.f, 51.f);
    GetMesh()->SetRelativeLocation(FVector(0, 0, -50)); //(X=0.000000,Y=0.000000,Z=-50.000000)
    StaticMeshUnderCircle->SetRelativeLocation(FVector(0, 0, -50));
    InteractRadius->SetRelativeLocation(FVector(60, 0, -10)); //(X=60.000000,Y=0.000000,Z=-10.000000)

    if (IsValid(DanceMinigameWidgetClass))
    {
        DanceMinigameWidget = Cast<UDanceMinigameWidget>(CreateWidget(GetWorld(), DanceMinigameWidgetClass));
    }
    if (IsValid(DanceMinigameWidget))
    {
        DanceMinigameWidget->AddToViewport();
        DanceMinigameWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    if (IsValid(WinTextWidgetClass))
    {
        WinTextWidget = Cast<UWinText>(CreateWidget(GetWorld(), WinTextWidgetClass));
    }
    if (IsValid(WinTextWidget))
    {
        WinTextWidget->AddToViewport();
        WinTextWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    //Wait Screen
    FTimerHandle timer;
    GetWorld()->GetTimerManager().SetTimer(timer, this, &AQuetzalMultiplayerCharacter::SetPlayerStatePointer, 0.2f, false);

}

void AQuetzalMultiplayerCharacter::Server_ApplyStateChange_Implementation(ECharacterState newState)
{
    Multicast_ApplyStateChange(newState);
}

// ANY ACTION OR VALUE MADE HERE IS DUPLICATED, MAKE SURE YOU ARE USING UNIQUE VARIABLES OUSIDE HERE
// TODO: FIX NOTIFY REPLICATIONS
void AQuetzalMultiplayerCharacter::Multicast_ApplyStateChange_Implementation(ECharacterState newState)
{
    if (CharacterStateCPP == nullptr)
        return;

    if (CharacterStateCPP->State == newState)
        return;

    // ECharacterState oldState = CharacterState->State;  //incase we need oldState
    FString roleString = (HasAuthority()) ? "SERVER" : "CLIENT";
    UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECharacterState"), true);
    FString StateName = (EnumPtr)
        ? EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(newState)).ToString()
        : FString("Invalid");

    CharacterStateCPP->State = newState;

    switch (CharacterStateCPP->State)
    {
    case ECharacterState::IDLE:
        // SetOrientRotationToMovement(true);
        // RagdollComponent->EnableMovementAtExit();
        // isRagdollStarted = false;
        break;

    case ECharacterState::RUNNING:
        myAnimInstance->bIsMoving = true;
        break;

    case ECharacterState::DASHING:
        DashStart();
        GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &AQuetzalMultiplayerCharacter::DashStop, 0.15f);
        break;

    case ECharacterState::THROW:
        ThrowHeldObject();
        break;

    case ECharacterState::INTERACT:
        PickupObject();
        break;

    case ECharacterState::STUNNED:
        StunState();
        break;

    case ECharacterState::DANCE_STUNNED:
        DanceStunState();
        break;
    case ECharacterState::DANCE_STUNNED_CLEAR:
        DanceClearStunState();
        break;
    case ECharacterState::ATTACKS:

        break;
    case ECharacterState::ATTACKR:
        LightAttackRelease();
        break;
    case ECharacterState::SPECIALATTACKS:
        SpecialAttackPressed();
        break;
    case ECharacterState::SPECIALATTACKR:
        SpecialAttackRelease();
        break;

    case ECharacterState::JUMPING:
        GetCharacterMovement()->bNotifyApex = true;
        CharacterStateCPP->IsOnGround = false;
        break;

    /*case ECharacterState::FALLING:
        CharacterStateCPP->IsOnGround = false;
        break;*/

    case ECharacterState::DEAD:
    {
        AnimStop();
        APlayerState* PState = GetPlayerState<APlayerState>();
        ACharacterState* characterState = Cast<ACharacterState>(PState);
        if (characterState)
        {
            // characterState->HP = 0;
            if (GetHealth() <= 0)
                characterState->StockCPP--;
        }
    }
    Die();
    break;
    case ECharacterState::KNOCKEDBACK:
        if (!bCanRagdoll)
            return;

        BlockRagdollInput();
        if (myAnimInstance != nullptr)
            myAnimInstance->bIsKnockedBack = true;
        SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), InstigatorLocation));
        GetWorld()->GetTimerManager().SetTimer(KnockBackTimerHandle, this, &AQuetzalMultiplayerCharacter::KnockBack, 0.1f);
        break;

    case ECharacterState::RAGDOLLING:

        isRagdollStarted = true;
        if (myAnimInstance != nullptr)
        {
            myAnimInstance->bIsKnockedBack = false;
            myAnimInstance->bIsRagdolling = true;
        }
        DropObject(10.0f);
        RagdollComponent->GoRagdoll();
        break;

    case ECharacterState::RECOVERING:
        if (myAnimInstance != nullptr)
            myAnimInstance->bIsRagdolling = false;
        RagdollComponent->ExitRagdoll();
        ClearStunState();
        break;

    case ECharacterState::RESPAWNING:
        AnimStop();
        break;

    default:
        break;
    }
}

void AQuetzalMultiplayerCharacter::ApplyStateChange(ECharacterState newState)
{
    Server_ApplyStateChange(newState);
}

void AQuetzalMultiplayerCharacter::Die()
{
    checkForWinner();
    isRespawning = true;

    if (!CharacterStateCPP)
    {
        return;
    }

    if (CharacterStateCPP->State != ECharacterState::DEAD) // TODO this needs testing
    {
        ApplyStateChange(ECharacterState::DEAD);
        return;
    }

    int32 laugh = FMath::RandRange(0, 20);
    if (laugh == 18)
    {
        if (AudioManager != nullptr)
        {
            AudioManager->PlayAudioCue(ECueAudio::LAUGH, GetActorLocation());
            AudioManager->SetAudioCueVolume(ECueAudio::LAUGH, 5.0f);
        }
    }

    if (AudioManager != nullptr)
    {
        AudioManager->PlayAudioCue(ECueAudio::DIE, GetActorLocation());
        AudioManager->PlayAudioCue(ECueAudio::DAMAGE, GetActorLocation());
        AudioManager->SetAudioCueVolume(ECueAudio::DIE, 5.0f);
        AudioManager->SetAudioCueVolume(ECueAudio::DAMAGE, 5.0f);
    }

    // Detener movimiento inmediatamente
    GetCharacterMovement()->StopMovementImmediately();
    SetOrientRotationToMovement(false);
    // Desactivar cualquier modo de movimiento (evitar que siga cayendo)
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    if (USkeletalMeshComponent* SkelMesh = GetMesh())
    {
        SkelMesh->SetHiddenInGame(true, false);
        SkelMesh->SetVisibility(false, true);
        SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (UCapsuleComponent* CapComp = GetCapsuleComponent())
    {
        CapComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CapComp->SetEnableGravity(false);
        CapComp->SetVisibility(false);
        CapComp->SetGenerateOverlapEvents(false);
    }

    if (StaticMeshUnderCircle)
    {
        StaticMeshUnderCircle->SetHiddenInGame(true, true);
        StaticMeshUnderCircle->SetVisibility(false, true);
        StaticMeshUnderCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (GetWorld())
    {
        FVector Location = GetActorLocation();
        Location.Z -= 100;
        FRotator Rotation = GetActorRotation();
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        SpawnedActor = GetWorld()->SpawnActor<APlayer_GeometryCollectionF>(
            PlayerFracture.Get(),
            Location,
            Rotation,
            SpawnParams);

        if (SpawnedActor)
        {
            if (UGeometryCollectionComponent* GeometryComp = SpawnedActor->FindComponentByClass<UGeometryCollectionComponent>())
            {
                GeometryComp->SetSimulatePhysics(true);
                GeometryComp->SetEnableGravity(true);
                GeometryComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                GeometryComp->SetNotifyRigidBodyCollision(true);
                GeometryComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
                GeometryComp->SetVisibility(true, true);
            }
        }
        DropWeaponAndObject(500.0f);
        SetActorLocation(FVector(5000, 5000, 5000), false, nullptr, ETeleportType::TeleportPhysics);
        ChangeDeadMaterial();
    }

    FTimerHandle RespawnDelayHandle;

    APlayerState* PState = GetPlayerState<APlayerState>();
    ACharacterState* characterState = Cast<ACharacterState>(PState);

    if (characterState->StockCPP >= 1)
        GetWorld()->GetTimerManager().SetTimer(RespawnDelayHandle, this, &AQuetzalMultiplayerCharacter::HandleRespawnBoxAndMove, 5.0f, false);
    SetHealth(0);
}

void AQuetzalMultiplayerCharacter::ChangeDeadMaterial()
{
    // Server_ChangeDeadMaterial(GeometryComp);
    if (SpawnedActor)
    {
        if (UGeometryCollectionComponent* GeometryComp = SpawnedActor->FindComponentByClass<UGeometryCollectionComponent>())
        {
            GeometryComp->SetMaterial(0, BaseMaterial);
        }
    }
    // MC_ChangeDeadMaterial();
}

void AQuetzalMultiplayerCharacter::Server_ChangeDeadMaterial_Implementation()
{
}

void AQuetzalMultiplayerCharacter::MC_ChangeDeadMaterial_Implementation()
{
    if (SpawnedActor)
    {
        if (UGeometryCollectionComponent* GeometryComp = SpawnedActor->FindComponentByClass<UGeometryCollectionComponent>())
        {
            GeometryComp->SetMaterial(0, BaseMaterial);
        }
    }
}

void AQuetzalMultiplayerCharacter::DropWeaponAndObject(float ObjectThrowForce)
{
    if (equipedWeapon)
    {
        equipedWeapon->DropWeapon();
    }

    DropObject(ObjectThrowForce);
}

void AQuetzalMultiplayerCharacter::DropObject(float ObjectThrowForce)
{
    if (objectBeingHeld)
    {
        // Throw action when object held is in throw notify
        objectBeingHeld->ThrowThisObject(GetActorForwardVector(), ObjectThrowForce);
        // objectBeingHeld->SetOwnerP(nullptr);
        objectBeingHeld = nullptr;
        myAnimInstance->bIsCarrying = false;
        myAnimInstance->bIsThrowingObject = false;
    }
}

void AQuetzalMultiplayerCharacter::GCImpulse()
{
    Server_GCImpulse();
}

void AQuetzalMultiplayerCharacter::Server_GCImpulse_Implementation()
{
    if (SpawnedActor)
    {
        GeometryCollectionComp = SpawnedActor->FindComponentByClass<UGeometryCollectionComponent>();
        if (GeometryCollectionComp)
        {
            // FVector AngularVelocity(5.0f, 5.0f, 5.0f);
            GeometryCollectionComp->ApplyAngularVelocity(0, deathFractureAngularVelocity);
            // GeometryCollectionComp->ApplyAngularVelocity(1, AngularVelocity);
        }
    }
}

void AQuetzalMultiplayerCharacter::HandleRespawnBoxAndMove()
{
    if (HasAuthority()) // Solo el server maneja el respawn inicial
    {
        ApplyStateChange(ECharacterState::RESPAWNING);

        // 1. Buscar todos los PlayerStarts
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

        if (PlayerStarts.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No PlayerStarts found."));
            return;
        }

        // 2. Escoger uno random
        int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
        AActor* ChosenStart = PlayerStarts[RandomIndex];

        if (!ChosenStart)
        {
            UE_LOG(LogTemp, Warning, TEXT("Chosen PlayerStart is null."));
            return;
        }

        // Guardamos la ubicación donde vamos a respawnear
        SpawnLocation = ChosenStart->GetActorLocation();

        // Ajustar ubicación para spawn del box
        FVector BoxSpawnLoc = SpawnLocation;
        BoxSpawnLoc.Z *= 15.0f; // Puedes ajustar este factor si quieres
        BoxSpawnLoc.X -= 100.0f; // Offset en X si necesitas espacio

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(BoxSpawnLoc);
        SpawnTransform.SetRotation(FQuat::Identity);
        SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

        // 3. Spawnear el Box (solo si tenemos clase asignada)
        if (SpawnBoxes)
        {
            ASpawnBox* SpawnedBox = GetWorld()->SpawnActorDeferred<ASpawnBox>(
                SpawnBoxes,
                SpawnTransform,
                this, // Owner
                GetInstigator(),
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

            if (SpawnedBox)
            {

                SpawnedBox->FinishSpawning(SpawnTransform);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("SpawnBox failed to spawn."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnBox class is NULL!"));
        }

        // 4. Segundo Delay de 5.9 segundos para mover al jugador
        //FTimerHandle MoveDelayHandle;
        //GetWorldTimerManager().SetTimer(MoveDelayHandle, [this]()
        //    {
        //        FVector NewLocation = SpawnLocation;
        //        SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

        //        Multicast_Respawn(); // Llama tu función para respawnear visualmente
        //    }, 5.9f, false);

        //PlayerRespawn();
    }
}

void AQuetzalMultiplayerCharacter::PlayerRespawn_Implementation()
{
    FVector NewLocation = SpawnLocation;
    SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

    Multicast_Respawn(); // Llama tu función para respawnear visualmente
}

void AQuetzalMultiplayerCharacter::Multicast_Respawn_Implementation()
{
    Respawn();
}

void AQuetzalMultiplayerCharacter::Respawn()
{
    isRespawning = true;
    APlayerState* PState = GetPlayerState<APlayerState>();
    ACharacterState* characterState = Cast<ACharacterState>(PState);

    if (characterState->StockCPP > 0)
    {

        // Restore Skeletal Mesh visibility and collisions
        if (USkeletalMeshComponent* SkelMesh = GetMesh())
        {
            SkelMesh->SetHiddenInGame(false, true);
            SkelMesh->SetVisibility(true, true);
            SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }

        // Restore Capsule Component collisions

        if (UCapsuleComponent* CapComp = GetCapsuleComponent())
        {
            CapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            CapComp->SetVisibility(true);
            CapComp->SetGenerateOverlapEvents(true);
        }

        if (StaticMeshUnderCircle)
        {
            StaticMeshUnderCircle->SetHiddenInGame(false, true);
            StaticMeshUnderCircle->SetVisibility(true, true);
            StaticMeshUnderCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        // Reset the health and status of the character (you can adjust as needed)
        SetHealth(PLAYER_HEALTH);
        SetStaggerHealth(PLAYER_STAGGER_HEALTH);
        if (equipedWeapon)
        {
            equipedWeapon->DropWeapon(); // Used incase player died where a weapon spawns
        }

        SetIsInvulnerable(false);
        damageComp->SetIsDead(false);
        ResetMovement();
        AnimStop();

        // Search all the PlayerStart type actors in the world
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

        // Desactivar cualquier modo de movimiento (evitar que siga cayendo)
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

        if (m_characterController)
        {
            if (m_characterController->IsLocalController())
            {
                if (m_characterController->HUD)
                {

                    if (characterState)
                    {
                        if (characterState->StockCPP > 0)
                            m_characterController->HUD->respawn();
                    }
                }
            }
        }

        isRespawning = false;
        ApplyStateChange(ECharacterState::IDLE);
    }
}

void AQuetzalMultiplayerCharacter::ResetMovement()
{
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;

    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    SetOrientRotationToMovement(true);
    setMovementSpeed(PLAYER_WALK_SPEED);
}

void AQuetzalMultiplayerCharacter::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    // SetActorRotation(aimRotation);
    Lobby();
    // SetActorRotation(aimRotation);
    if (CharacterStateCPP == nullptr)
        CharacterStateCPP = GetPlayerState<ACharacterState>();
    if (CharacterStateCPP == nullptr)
        CharacterStateCPP = Cast<ACharacterState>(GetPlayerState());
    if (m_characterController == nullptr)
        m_characterController = Cast<ACharacterController>(GetController());

    //CheckPlayerState(deltaTime);
    InvulnerableStateLogic(deltaTime);
    CheckIfLightAttackIsHeld(deltaTime);

    // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("myAnimInstance->bIsHoldMelee: %s"), myAnimInstance->bIsHoldMelee ? TEXT("true") : TEXT("false")));
    // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("myAnimInstance->bIsArmed: %s"), myAnimInstance->bIsArmed ? TEXT("true") : TEXT("false")));

    if (RagdollComponent)
    {
        if (RagdollComponent->bInRagdoll && isRagdollStarted)
        {
            GetWorld()->GetTimerManager().SetTimer(RagdollTimer, this, &AQuetzalMultiplayerCharacter::ExitRagdollAfterTimer, 2.0f); // TODO create const for time
            isRagdollStarted = false;
        }
    }
}

void AQuetzalMultiplayerCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (CharacterStateCPP != nullptr)
    {
        //ApplyStateChange(ECharacterState::IDLE);
        CharacterStateCPP->IsOnGround = true;
    }
}

void AQuetzalMultiplayerCharacter::NotifyJumpApex()
{
    if (CharacterStateCPP != nullptr)
    {
        if (CharacterStateCPP->IsOnGround == false)
        {
            //ApplyStateChange(ECharacterState::FALLING);
        }
    }
}

void AQuetzalMultiplayerCharacter::LightAttackRelease()
{
    if (myAnimInstance == nullptr)
    {
        return;
    }

    lightAttackHoldSwitch = false;

    if (equipedWeapon)
    {
        equipedWeapon->OnAttack.Broadcast(false);
    }
    else
    {
        //// Evitar spamear el push: si ya está activo, se retorna
        if (myAnimInstance->bIsPushing)
        {
            return;
        }

        myAnimInstance->bIsPushing = true;
        // Reinicia la bandera de push mediante un timer
        GetWorld()->GetTimerManager().SetTimer(PushCooldownTimerHandle, this, &AQuetzalMultiplayerCharacter::Server_SetPushState, 0.5f, false);
    }

    lightAttackInputHoldTimer = PLAYER_INPUT_HOLD_DURATION;

    // Inicia el cooldown del ataque
    // GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AQuetzalMultiplayerCharacter::ResetAttackCooldown, PLAYER_ATTACK_COOLDOWN, false);
}

void AQuetzalMultiplayerCharacter::SpecialAttackPressed()
{
}

void AQuetzalMultiplayerCharacter::SpecialAttackRelease()
{
    specialAttackHoldSwitch = false;

    if (equipedWeapon)
    {
        equipedWeapon->OnSpecialAttack.Broadcast(false);
    }
    specialAttackInputHoldTimer = PLAYER_INPUT_HOLD_DURATION;
}

// Force Player to go in STUN state
void AQuetzalMultiplayerCharacter::StunState()
{

    if (CharacterStateCPP)
    {
        if (CharacterStateCPP->State != ECharacterState::STUNNED)
            ApplyStateChange(ECharacterState::STUNNED);
    }

    if (AudioManager != nullptr)
    {
        AudioManager->PlayAudioCue(ECueAudio::STUNNED, GetActorLocation());
        AudioManager->SetAudioCueVolume(ECueAudio::STUNNED, 5.0);
    }

    SetStaggerHealth(PLAYER_STAGGER_HEALTH);
    GetWorld()->GetTimerManager().SetTimer(StunTimer, this, &AQuetzalMultiplayerCharacter::ClearStunState, PLAYER_STUN_DURATION_TIME);
    if (myAnimInstance != nullptr)
        myAnimInstance->bIsStunned = true;
    DropObject(20.0f);

    SetOrientRotationToMovement(false);
}

// Clears Player from STUN state (Can be used to force nack player to move)
void AQuetzalMultiplayerCharacter::ClearStunState()
{
    GetWorld()->GetTimerManager().ClearTimer(StunTimer);
    DanceClearStunState();
    if (myAnimInstance != nullptr)
        myAnimInstance->bIsStunned = false;

    SetOrientRotationToMovement(true);
    SetStaggerHealth(PLAYER_STAGGER_HEALTH);

    if (CharacterStateCPP)
        if (CharacterStateCPP->State == ECharacterState::STUNNED)
            ApplyStateChange(ECharacterState::IDLE);
}

void AQuetzalMultiplayerCharacter::DanceStunState()
{
    if (CharacterStateCPP)
    {
        if (isRespawning)
            return;
        else if (CharacterStateCPP->State != ECharacterState::DANCE_STUNNED && !isRespawning)
            ApplyStateChange(ECharacterState::DANCE_STUNNED);
    }

    if (myAnimInstance != nullptr)
        myAnimInstance->bIsStunned = true;

    if (AudioManager != nullptr)
    {
        AudioManager->PlayAudioCue(ECueAudio::STUNNED, GetActorLocation());
        AudioManager->SetAudioCueVolume(ECueAudio::STUNNED, 5.0);
    }

    DropObject(10.0f);
    SetStaggerHealth(PLAYER_STAGGER_HEALTH);

    if (IsLocallyControlled() && !isRespawning)
    {
        DanceMinigameWidget->SetVisibility(ESlateVisibility::Visible);
        DanceMinigameWidget->GenerateArrows();
    }
}
void AQuetzalMultiplayerCharacter::DanceClearStunState()
{
    if (myAnimInstance != nullptr)
        myAnimInstance->bIsStunned = false;
    if (AudioManager != nullptr)
        AudioManager->PlayAudioCue(ECueAudio::DANCESTUNNED, GetActorLocation());
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    SetOrientRotationToMovement(true);
    SetStaggerHealth(PLAYER_STAGGER_HEALTH);
    if (IsLocallyControlled())
    {
        DanceMinigameWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    if (CharacterStateCPP)
        if (CharacterStateCPP->State == ECharacterState::DANCE_STUNNED)
            ApplyStateChange(ECharacterState::IDLE);
}

// Performs a Dash
void AQuetzalMultiplayerCharacter::DashStart()
{
    if (myAnimInstance == nullptr)
    {
        return;
    }

    myAnimInstance->bIsDashing = true;
    FVector dashDirection = GetActorForwardVector();

    GetCharacterMovement()->GravityScale = 0.0f;
    GetCharacterMovement()->Velocity = dashDirection * 1300.0f;
}

// Resets the gravity on player and stops the dash animation
void AQuetzalMultiplayerCharacter::DashStop()
{
    if (myAnimInstance == nullptr)
    {
        return;
    }

    myAnimInstance->bIsDashing = false;

    GetCharacterMovement()->GravityScale = 1.8f;

    /*if (GetCharacterMovement()->IsFalling())
        ApplyStateChange(ECharacterState::FALLING);
    else*/
        //ApplyStateChange(ECharacterState::IDLE);
}

void AQuetzalMultiplayerCharacter::AnimStop()
{
    if (myAnimInstance)
    {
        myAnimInstance->bIsOnGround = false;
        myAnimInstance->bIsMoving = false;
        myAnimInstance->bIsJumping = false;
        myAnimInstance->bIsJumping = false;
        myAnimInstance->bIsFalling = false;
        myAnimInstance->bIsDashing = false;
        myAnimInstance->bIsPushing = false;
        myAnimInstance->bIsArmed = false;
        myAnimInstance->bIsMelee = false;
        myAnimInstance->bIsRange = false;
        myAnimInstance->bIsStunned = false;
        myAnimInstance->bIsHoldMelee = false;
        myAnimInstance->bIsKnockedBack = false;
        myAnimInstance->bIsAiming = false;
        myAnimInstance->bIsRagdolling = false;
        myAnimInstance->bIsLayingOnBack = false;
        myAnimInstance->bIsGrabing = false;
        myAnimInstance->bIsCarrying = false;
        myAnimInstance->bIsThrowingObject = false;
        myAnimInstance->bIsThrowingWeapon = false;
    }
    if (IsLocallyControlled())
    {
        DanceMinigameWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AQuetzalMultiplayerCharacter::AimRotate(FRotator rot, bool face)
{
    Server_SetRotation(rot, face);
}
void AQuetzalMultiplayerCharacter::Server_SetRotation_Implementation(FRotator rot, bool face)
{
    MC_SetRotation(rot, face);
}
void AQuetzalMultiplayerCharacter::MC_SetRotation_Implementation(FRotator rot, bool face)
{
    aimRotation = rot;
    SetActorRotation(aimRotation);
    SetOrientRotationToMovement(!face);
}

bool AQuetzalMultiplayerCharacter::IsInvulnerable()
{
    return damageComp->IsInvulnerable();
}

void AQuetzalMultiplayerCharacter::SetOrientRotationToMovement(bool active)
{
    GetCharacterMovement()->bOrientRotationToMovement = active;
}

void AQuetzalMultiplayerCharacter::setMovementSpeed(float speed)
{
    Server_setMovementSpeed(speed);
}

void AQuetzalMultiplayerCharacter::Server_setMovementSpeed_Implementation(float speed)
{
    MC_setMovementSpeed(speed);
}

void AQuetzalMultiplayerCharacter::MC_setMovementSpeed_Implementation(float speed)
{
    GetCharacterMovement()->MaxWalkSpeed = speed;
}

void AQuetzalMultiplayerCharacter::OnOverlapInteractBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bIsHoldingObject)
    {
        if ((OtherActor != nullptr) && (OtherActor != this))
        {
            IPickupObjectInterface* pickupObj = Cast<IPickupObjectInterface>(OtherActor);
            if (pickupObj)
            {
                highLightedObject.Add(pickupObj);
                if (highLightedObject.Num() == 1)
                {
                    pickupObj->HighlightThisObject(true);
                }
            }
        }
    }
}

void AQuetzalMultiplayerCharacter::OnOverlapInteractEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!bIsHoldingObject)
    {
        // remove hightlighted object
        if ((OtherActor != nullptr) && (OtherActor != this))
        {
            IPickupObjectInterface* pickupObj = Cast<IPickupObjectInterface>(OtherActor);
            if (pickupObj && highLightedObject.Num() > 0)
            {
                if (highLightedObject.Contains(pickupObj))
                {
                    pickupObj->HighlightThisObject(false);
                    highLightedObject.Remove(pickupObj);
                }
            }
        }
        // highlight next object in the list
        if (highLightedObject.Num() >= 1)
        {
            IPickupObjectInterface* pickupObj = Cast<IPickupObjectInterface>(highLightedObject[0]);
            if (pickupObj)
            {
                pickupObj->HighlightThisObject(true);
            }
        }
    }
}

void AQuetzalMultiplayerCharacter::PickupObject()
{

    Server_ickupObject();
}

void AQuetzalMultiplayerCharacter::Server_ickupObject_Implementation()
{
    MC_PickupObject();
}

void AQuetzalMultiplayerCharacter::MC_PickupObject_Implementation()
{

    if (myAnimInstance == nullptr)
    {
        return;
    }

    if (!bIsHoldingObject)
    {
        myAnimInstance->bIsGrabing = true;
        GetWorld()->GetTimerManager().SetTimer(GrabCheckTimer, this, &AQuetzalMultiplayerCharacter::GrabAnimationStop, 0.4f);
    }

    if (!bIsHoldingObject && highLightedObject.Num() >= 1 && highLightedObject[0]->CanBePickedUp())
    {
        objectBeingHeld = highLightedObject[0];
        if (GetMesh()->DoesSocketExist(holdObjectSocketName))
            objectBeingHeld->PickupThisObject(GetMesh(), holdObjectSocketName);
        bIsHoldingObject = true;
        objectBeingHeld->attachedPlayer = GetController();
        objectBeingHeld->HighlightThisObject(false);
        objectBeingHeld->SetOwnerP(this);
        highLightedObject.Empty();
        AQuetzalMultiplayerCharacter* checkIfCharacter = Cast<AQuetzalMultiplayerCharacter>(objectBeingHeld);
        myAnimInstance->bIsGrabing = false;
        myAnimInstance->bIsCarrying = true;
        if (checkIfCharacter != nullptr)
        {
            auto& TimerManager = GetWorld()->GetTimerManager();
            HoldPlayerTimerDel.BindUFunction(this, FName("DropObject"), 10.f);
            TimerManager.SetTimer(HoldPlayerTimerHandle, HoldPlayerTimerDel, PLAYER_HOLD_OTHER_PLAYER_DURATION, false); // TODO ask how long the player should be held
        }
    }

    ApplyStateChange(ECharacterState::IDLE);//TODO this may replace the state for all player, needs testing
}
void AQuetzalMultiplayerCharacter::GrabAnimationStop()
{
    myAnimInstance->bIsGrabing = false;
}
void AQuetzalMultiplayerCharacter::ThrowHeldObject()
{
    Server_ThrowHeldObject();
}

void AQuetzalMultiplayerCharacter::MC_ThrowHeldObject_Implementation()
{

    if (AudioManager != nullptr)
    {
        AudioManager->PlayAudioCue(ECueAudio::THROW, GetActorLocation());
        AudioManager->SetAudioCueVolume(ECueAudio::THROW, 3.0f);
    }

    if (myAnimInstance == nullptr)
    {
        return;
    }

    if (equipedWeapon)
    {
        equipedWeapon->ThrowWeapon(GetActorForwardVector());
        equipedWeapon = nullptr;

        myAnimInstance->bIsArmed = false;
        myAnimInstance->bIsThrowingWeapon = true;
    }
    else if (objectBeingHeld)
    {

        // Throw action when object held is in throw notify
        objectBeingHeld->ThrowThisObject(GetActorForwardVector(), 500.0f);

        objectBeingHeld = nullptr;

        myAnimInstance->bIsCarrying = false;//TODO maybe call this outside the if statement
        myAnimInstance->bIsThrowingObject = true;
    }
    GetWorld()->GetTimerManager().SetTimer(ThrowCooldownTimerHandle, this, &AQuetzalMultiplayerCharacter::Server_ThrowAnim, 0.6f);
    
    bIsHoldingObject = false;
}

void AQuetzalMultiplayerCharacter::Server_ThrowHeldObject_Implementation()
{

    MC_ThrowHeldObject();
}

void AQuetzalMultiplayerCharacter::HighlightThisObject(bool set)
{
    if (CharacterStateCPP)
    {
        if (CharacterStateCPP->State == ECharacterState::STUNNED)
            GetMesh()->SetRenderCustomDepth(true);
    }
}

void AQuetzalMultiplayerCharacter::PickupThisObject(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
    Server_PickupThisObject(charaterSkelMeshComp, socketName);
}

void AQuetzalMultiplayerCharacter::Server_PickupThisObject_Implementation(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
    MC_PickupThisObject(charaterSkelMeshComp, socketName);
}

void AQuetzalMultiplayerCharacter::MC_PickupThisObject_Implementation(USkeletalMeshComponent* charaterSkelMeshComp, FName socketName)
{
    if (charaterSkelMeshComp == nullptr)
    {
        return;
    }
    if (CanBePickedUp())
    {
        if (USkeletalMeshComponent* SkelMesh = GetMesh())
        {
            SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        if (UCapsuleComponent* CapComp = GetCapsuleComponent())
        {
            CapComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            CapComp->SetEnableGravity(false);
            CapComp->SetGenerateOverlapEvents(false);
        }

        GetWorld()->GetTimerManager().SetTimer(StunTimer, this, &AQuetzalMultiplayerCharacter::ClearStunState, PLAYER_HOLD_OTHER_PLAYER_DURATION);
        GetRootComponent()->AttachToComponent(charaterSkelMeshComp, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), socketName); // NOTE: EAttachmentRule::KeepWorld is needed because snapToTarget moves the UCapsuleComponent
        bIsPickedUp = true;
    }
}

void AQuetzalMultiplayerCharacter::ThrowThisObject(FVector direction, float throwForce)
{
    Server_ThrowThisObject(direction, throwForce);
}
void AQuetzalMultiplayerCharacter::Server_ThrowThisObject_Implementation(FVector direction, float throwForce)
{
    MC_ThrowThisObject(direction, throwForce);
}

void AQuetzalMultiplayerCharacter::MC_ThrowThisObject_Implementation(FVector direction, float throwForce)
{
    GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    ClearStunState();
    LaunchCharacter(direction * throwForce * 5, true, true);
    bIsPickedUp = false;

    // Restore Skeletal Mesh visibility and collisions
    if (USkeletalMeshComponent* SkelMesh = GetMesh())
    {
        SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    // Restore Capsule Component collisions

    if (UCapsuleComponent* CapComp = GetCapsuleComponent())
    {
        CapComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CapComp->SetGenerateOverlapEvents(true);
    }
    // TODO use KNOCKEDBACK when ragdoll replication is working
    // ApplyStateChange(ECharacterState::KNOCKEDBACK);

    throwCollisionComp->SetGenerateOverlapEvents(true);
}

void AQuetzalMultiplayerCharacter::OnOverlapThrowBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (throwCollisionComp == nullptr)
    {
        return;
    }

    throwCollisionComp->SetGenerateOverlapEvents(false);
    if ((OtherActor != nullptr) && (OtherActor != this))
    {
        IDamageInterface* damageReceiver = Cast<IDamageInterface>(OtherActor);
        if (damageReceiver)
        {
            FDamageInfo damageInfo;
            damageInfo.amountOfStaggerDamage = damageReceiver->GetMaxStaggerHealth();
            damageInfo.InstigatedBy = attachedPlayer;
            damageInfo.DamageReceiver = damageReceiver;
            damageInfo.damageType = ECustomDamageType::IMPACT;
            damageInfo.DamageCauser = this;
            damageReceiver->TakeDamage(damageInfo);
        }

        if (AudioManager != nullptr)
        {
            AudioManager->PlayAudioCue(ECueAudio::THROWHIT, GetActorLocation());
            AudioManager->SetAudioCueVolume(ECueAudio::THROWHIT, 5.0f);
        }
    }
}

bool AQuetzalMultiplayerCharacter::CanBePickedUp()
{
    if (!bIsPickedUp && (CharacterStateCPP->State == ECharacterState::STUNNED || CharacterStateCPP->State == ECharacterState::DANCE_STUNNED))
    {
        bCanBePickedUp = true;
    }
    else
    {
        bCanBePickedUp = false;
    }

    return bCanBePickedUp;
}

void AQuetzalMultiplayerCharacter::Server_SetPushState_Implementation()
{
    Multicast_SetPushState();
}

void AQuetzalMultiplayerCharacter::Multicast_SetPushState_Implementation()
{
    if (myAnimInstance == nullptr)
    {
        return;
    }
    //myAnimInstance->bIsPushing = bNewPushState;
    myAnimInstance->bIsPushing = false;
}

void AQuetzalMultiplayerCharacter::SetRangeState(bool bNewRangeState)
{
    if (myAnimInstance == nullptr)
    {
        return;
    }
    //myAnimInstance->bIsRange = bNewRangeState;
}

void AQuetzalMultiplayerCharacter::SetMeleeSate(bool bNewMeleeState)
{
    if (myAnimInstance == nullptr)
    {
        return;
    }
   // myAnimInstance->bIsMelee = bNewMeleeState;
}

void AQuetzalMultiplayerCharacter::KnockBack()
{
    Server_KnockBack(false);
}

void AQuetzalMultiplayerCharacter::Server_KnockBack_Implementation(bool bNewKnockBackState)
{
    Multicast_KnockBack(bNewKnockBackState);
}

void AQuetzalMultiplayerCharacter::Multicast_KnockBack_Implementation(bool bNewKnockBackState)
{
    if (RagdollComponent == nullptr)
    {
        return;
    }
    if (myAnimInstance == nullptr)
    {
        return;
    }
    ApplyStateChange(ECharacterState::RAGDOLLING);//TODO this may replace the state for all player, needs testing
}

void AQuetzalMultiplayerCharacter::Server_ThrowAnim_Implementation()
{
    Multicast_ThrowAnim();
}

void AQuetzalMultiplayerCharacter::Multicast_ThrowAnim_Implementation()
{
    if (myAnimInstance == nullptr)
    {
        return;
    }

    myAnimInstance->bIsThrowingWeapon = false;
    myAnimInstance->bIsThrowingObject = false;

    // ApplyStateChange(ECharacterState::IDLE);
}

//void AQuetzalMultiplayerCharacter::CheckPlayerState(float deltaTime)
//{
//    if (CharacterStateCPP != nullptr)
//    {
//        // check if player is falling
//        if (!GetCharacterMovement()->IsMovingOnGround() && !CharacterStateCPP->IsOnGround)
//        {
//            if (CharacterStateCPP->State != ECharacterState::JUMPING &&
//                CharacterStateCPP->State != ECharacterState::RAGDOLLING &&
//                CharacterStateCPP->State != ECharacterState::DEAD &&
//                CharacterStateCPP->State != ECharacterState::IDLE &&
//                CharacterStateCPP->State != ECharacterState::STUNNED &&
//                CharacterStateCPP->State != ECharacterState::DANCE_STUNNED)
//            {
//                ApplyStateChange(ECharacterState::FALLING);
//            }
//        }
//    }
//}

void AQuetzalMultiplayerCharacter::InvulnerableStateLogic(float deltaTime)
{
    if (IsInvulnerable())
    {
        CharacterStateCPP->InvulnerableTimer -= deltaTime;

        if (CharacterStateCPP->InvulnerableTimer <= 0.0f)
        {
            CharacterStateCPP->InvulnerableTimer = 0.0f;

            GetCapsuleComponent()->SetGenerateOverlapEvents(true);

            GetMesh()->SetVisibility(true);

            SetIsInvulnerable(false);
        }
        else
        {
            flashInvulnerableTimer -= deltaTime;

            if (flashInvulnerableTimer <= 0.0f)
            {
                flashInvulnerableTimer = PLAYER_INVULNERABLE_VISIBILITY_DURATION;

                GetMesh()->ToggleVisibility();
            }
        }
    }
}

void AQuetzalMultiplayerCharacter::CheckIfLightAttackIsHeld(float deltaTime)
{

    if (lightAttackHoldSwitch && !specialAttackHoldSwitch)
    {
        lightAttackInputHoldTimer -= deltaTime;
        if (lightAttackInputHoldTimer <= 0)
        {
            if (equipedWeapon)
            {
                equipedWeapon->OnAttack.Broadcast(true); // TODO maybe move this out of tick
            }
        }
    }
    if (specialAttackHoldSwitch && !lightAttackHoldSwitch)
    {
        specialAttackInputHoldTimer -= deltaTime;
        if (specialAttackInputHoldTimer <= 0)
        {
            if (equipedWeapon)
            {
                equipedWeapon->OnSpecialAttack.Broadcast(true); // TODO maybe move this out of tick
            }
        }
    }
}

// void AQuetzalMultiplayerCharacter::GrabAttemptCheck()
//{
//     if (myAnimInstance == nullptr)
//     {
//         return;
//     }
//
//
//     if (bIsHoldingObject && equipedWeapon == nullptr)
//     {
//
//     }
//     else
//     {
//         myAnimInstance->bIsGrabing = false;
//     }
// }

void AQuetzalMultiplayerCharacter::ExitRagdollAfterTimer()
{
    ApplyStateChange(ECharacterState::RECOVERING);
}

void AQuetzalMultiplayerCharacter::BlockAttackInput()
{
    // Bloquea la entrada de ataque
    bCanAttack = false;
    // Inicia un timer para reestablecer la posibilidad de atacar luego del cooldown
    GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AQuetzalMultiplayerCharacter::ResetAttackInput, PLAYER_ATTACK_COOLDOWN, false);
}

void AQuetzalMultiplayerCharacter::ResetAttackInput()
{
    // Vuelve a habilitar la entrada de ataque
    bCanAttack = true;
}

void AQuetzalMultiplayerCharacter::BlockDashInput()
{
    bCanDash = false;
    GetWorld()->GetTimerManager().SetTimer(DashCooldownTimer, this, &AQuetzalMultiplayerCharacter::ResetDashInput, PLAYER_DASH_COOLDOWN, false);
}

void AQuetzalMultiplayerCharacter::ResetDashInput()
{
    bCanDash = true;
}

void AQuetzalMultiplayerCharacter::BlockGrabInput()
{
    bCanGrab = false;
    GetWorld()->GetTimerManager().SetTimer(GrabCooldownTimerHandle, this, &AQuetzalMultiplayerCharacter::ResetGrabInput, PLAYER_GRAB_COOLDOWN, false);
}

void AQuetzalMultiplayerCharacter::ResetGrabInput()
{
    bCanGrab = true;
}

void AQuetzalMultiplayerCharacter::BlockRagdollInput()
{
    bCanRagdoll = false;
    GetWorld()->GetTimerManager().SetTimer(RagdollCooldownTimerHandle, this, &AQuetzalMultiplayerCharacter::ResetRagdollInput, PLAYER_RAGDOLL_COOLDOWN, false);
}

void AQuetzalMultiplayerCharacter::ResetRagdollInput()
{
    bCanRagdoll = true;
}

void AQuetzalMultiplayerCharacter::SetOwnerP(AActor* Actor)
{
    SetOwner(Actor);
}

void AQuetzalMultiplayerCharacter::SetHealth(int setHealth)
{
    damageComp->SetHealth(setHealth);
}

void AQuetzalMultiplayerCharacter::SetStaggerHealth(int setStagger)
{
    damageComp->SetStaggerHealth(setStagger);
}

bool AQuetzalMultiplayerCharacter::IsBlocking()
{
    return damageComp->IsBlocking();
}

void AQuetzalMultiplayerCharacter::SetMaxHealth(int setMaxHealth)
{
    damageComp->SetMaxHealth(setMaxHealth);
}

void AQuetzalMultiplayerCharacter::SetMaxStaggerHealth(int setMaxStagger)
{
    damageComp->SetMaxStaggerHealth(setMaxStagger);
}


void AQuetzalMultiplayerCharacter::SetIsInvulnerable(bool setInvulnerable)
{
    if (CharacterStateCPP != nullptr && setInvulnerable)
    {
        CharacterStateCPP->InvulnerableTimer = PLAYER_INVULNERABLE_DURATION;
    }
    damageComp->SetIsInvulnerable(setInvulnerable);
}

void AQuetzalMultiplayerCharacter::SetIsBlocking(bool setBlocking)
{
    damageComp->SetIsBlocking(setBlocking);
}

int AQuetzalMultiplayerCharacter::GetHealth()
{
    return damageComp->GetHealth();
}

int AQuetzalMultiplayerCharacter::GetStaggerHealth()
{
    return damageComp->GetStaggerHealth();
}

int AQuetzalMultiplayerCharacter::GetMaxStaggerHealth()
{
    return damageComp->GetMaxStaggerHealth();
}

int AQuetzalMultiplayerCharacter::GetMaxHealth()
{
    return damageComp->GetMaxHealth();
}

int AQuetzalMultiplayerCharacter::Heal(int healAmount)
{
    return damageComp->Heal(healAmount);
}

bool AQuetzalMultiplayerCharacter::TakeDamage(FDamageInfo damageInfo)
{
    if (DEBUG_MSG_TakeDamage)
    {
        FString roleString = (HasAuthority()) ? "SERVER" : "CLIENT";
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
            FString::Printf(TEXT("Character DamageHandle Called on: %d"), damageInfo.amountOfDamage));
        UE_LOG(LogTemp, Warning, TEXT("Character DamageHandle Called on: %s"), *roleString);
    }

    if (AudioManager != nullptr)
    {
        AudioManager->PlayAudioCue(ECueAudio::DAMAGE, GetActorLocation());
        AudioManager->SetAudioCueVolume(ECueAudio::DAMAGE, 5.0f);
    }

    if (CharacterStateCPP != nullptr)
    {
        if (CharacterStateCPP->State == ECharacterState::STUNNED ||
            CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            CharacterStateCPP->State == ECharacterState::RECOVERING)
        {
            damageInfo.amountOfStaggerDamage = 0.f;
        }
    }
    Server_TakeDamage(damageInfo);

    return damageComp->TakeDamage(damageInfo);
}

void AQuetzalMultiplayerCharacter::Server_TakeDamage_Implementation(FDamageInfo damageInfo)
{
    MC_TakeDamage(damageInfo);
}

void AQuetzalMultiplayerCharacter::MC_TakeDamage_Implementation(FDamageInfo damageInfo)
{

    damageComp->TakeDamage(damageInfo);
}

void AQuetzalMultiplayerCharacter::instaKill()
{
    if (equipedWeapon != nullptr)
    {
        equipedWeapon->SetRespawnFlag(true);
        equipedWeapon->ActivateActor(false);
    }
    FDamageInfo damage;
    damage.damageResponse = EDamageResponse::INSTANT_KILL;
    TakeDamage(damage);
}

void AQuetzalMultiplayerCharacter::DamageResponceHandler(EDamageResponse damageResponse, FDamageInfo damageInfo)
{

    if (HasAuthority())
    {
        AWeapon* weapon = Cast<ABaseballBat>(damageInfo.DamageCauser);


        if (weapon != nullptr)
        {
            if (AudioManager != nullptr)
            {
                AudioManager->PlayAudioCue(ECueAudio::BATHIT, GetActorLocation());
                AudioManager->SetAudioCueVolume(ECueAudio::BATHIT, 5.0f);
            }
        }

        weapon = Cast<AMallet>(damageInfo.DamageCauser);
        if (weapon != nullptr)
        {
            if (AudioManager != nullptr)
            {
                AudioManager->PlayAudioCue(ECueAudio::MALLETHIT, GetActorLocation());
                AudioManager->SetAudioCueVolume(ECueAudio::MALLETHIT, 5.0f);
            }
        }

        // TODO play animation bassed on damage response
        switch (damageResponse)
        {
        case EDamageResponse::HIT_REACTION:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "HIT_REACTION");

            break;
        case EDamageResponse::STUN:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "STUN");
            break;
        case EDamageResponse::KNOCK_BACK:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "KNOCK_BACK");
            if (damageInfo.InstigatedBy)
            {
                // myAnimInstance->bIsKnockedBack = true;
                ClearStunState();
                InstigatorLocation = damageInfo.InstigatedBy->GetActorLocation();
                LaunchCharacter(damageInfo.InstigatedBy->GetActorForwardVector() * damageInfo.impactForce, true, false);
                ApplyStateChange(ECharacterState::KNOCKEDBACK);
            }
            break;
        case EDamageResponse::DANCE:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "DANCE");
            break;
        case EDamageResponse::INSTANT_KILL:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "INSTANT_KILL");

            break;
        case EDamageResponse::NONE:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "NONE");
            break;
        case EDamageResponse::_MAX:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "_MAX");
            break;
        default:
            if (DEBUG_MSG_DamageResponse)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "default");
            break;
        }
    }
}

void AQuetzalMultiplayerCharacter::DamageBlocked()
{
    // TODO damage was blocked... play animation or break off shield... etc
}

void AQuetzalMultiplayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, CharacterStateCPP);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, SpawnedActor);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, aimRotation);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, myAnimInstance);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, bIsHoldingObject);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, lightAttackHoldSwitch);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, specialAttackHoldSwitch);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, bCanAttack);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, GeometryCollectionComp);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, equipedWeapon);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, bCanGrab);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, AudioManager);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, AudioManagerClass);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, SpawnLocation);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, BaseMaterial);
    DOREPLIFETIME(AQuetzalMultiplayerCharacter, isInlobbyCPP);
}
