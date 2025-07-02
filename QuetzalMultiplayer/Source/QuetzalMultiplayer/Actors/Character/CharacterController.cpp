// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Character/CharacterController.h"
#include "QuetzalMultiplayerCharacter.h"
#include "QuetzalHUD.h"
#include "GameFramework/GameState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "CharacterAnimInstance.h"
#include <GameFramework/InputSettings.h>
#include <GameFramework/InputDeviceSubsystem.h>
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DanceMinigameWidget.h"
#include "QuetzalHUD.h"
#include "Audio/AudioManager.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "QuetzalMultiplayerGameMode.h"
#include "PostLoginWaitScreen.h"

ACharacterController::ACharacterController() :
    inputSubsystem(nullptr),
    inputMappingContext(nullptr),
    moveAction(nullptr),
    jumpAction(nullptr),
    lookAction(nullptr),
    lightAttackAction(nullptr),
    interactorThrow(nullptr),
    StunAction(nullptr),
    DashAction(nullptr),
    character(nullptr)
{
    arrowsinput.Add(EDanceArrowDirection::A_LEFT, false);
    arrowsinput.Add(EDanceArrowDirection::A_RIGHT, false);
    arrowsinput.Add(EDanceArrowDirection::A_UP, false);
    arrowsinput.Add(EDanceArrowDirection::A_DOWN, false);
    arrowsinput.Add(EDanceArrowDirection::A_DEFAULT, false);
}

void ACharacterController::BeginPlay()
{
    Super::BeginPlay();
    FTimerHandle timer;
    GetWorld()->GetTimerManager().SetTimer(timer, this, &ACharacterController::CreateHUD, 3.0f, false);
}

void ACharacterController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    character = Cast<AQuetzalMultiplayerCharacter>(aPawn);
}

void ACharacterController::OnUnPossess()
{
    Super::OnUnPossess();

    character = nullptr;
}
// setup Input
void ACharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {

        // Jumping
        EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Started, this, &ACharacterController::OnJumpPressed);
        EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &ACharacterController::OnJumpRelease);

        // Moving
        EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ACharacterController::OnMovePressed);
        EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Completed, this, &ACharacterController::OnMoveReleased);

        // Looking
        EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &ACharacterController::Look);

        // attack
        EnhancedInputComponent->BindAction(lightAttackAction, ETriggerEvent::Completed, this, &ACharacterController::LightAttackRelease);
        EnhancedInputComponent->BindAction(lightAttackAction, ETriggerEvent::Triggered, this, &ACharacterController::LightAttackPressed);

        // Special attack
        EnhancedInputComponent->BindAction(specialAttackAction, ETriggerEvent::Completed, this, &ACharacterController::SpecialAttackRelease);
        EnhancedInputComponent->BindAction(specialAttackAction, ETriggerEvent::Triggered, this, &ACharacterController::SpecialAttackPressed);

        // Throw Held Object
        EnhancedInputComponent->BindAction(interactorThrow, ETriggerEvent::Started, this, &ACharacterController::InteractorThrow);
        // Drop Weapon
        EnhancedInputComponent->BindAction(dropWeaponAction, ETriggerEvent::Started, this, &ACharacterController::DropWeapon);

        //Stun Character
        EnhancedInputComponent->BindAction(StunAction, ETriggerEvent::Triggered, this, &ACharacterController::StunTriggered);

        EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ACharacterController::DashTriggered);

        EnhancedInputComponent->BindAction(ToggleHUDAction, ETriggerEvent::Triggered, this, &ACharacterController::ToggleHUD);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void ACharacterController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if (inputSubsystem == nullptr)
    {
        inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
        if (inputSubsystem != nullptr)
            inputSubsystem->AddMappingContext(inputMappingContext, 0);
    }

    if (character == nullptr && Player != nullptr)
    {
        if (GetWorld() != nullptr)
        {
            ACharacter* playerChar = Player->GetPlayerController(GetWorld())->GetCharacter();
            if (playerChar)
            {
                character = Cast<AQuetzalMultiplayerCharacter>(playerChar);
            }
        }
    }

    APlayerState* PpState = GetPlayerState<APlayerState>();
    ACharacterState* characterStatee = Cast<ACharacterState>(PpState);
    if (characterStatee)
        UpdateHP(characterStatee);
    
    //Setting HP for HUD
    if (GetWorld() != nullptr)
    {
        AQuetzalMultiplayerGameMode* gameMode = Cast<AQuetzalMultiplayerGameMode>(GetWorld()->GetAuthGameMode());

        //Player State HP gets set to player health each frame
        if (gameMode)
        {



            for (auto play : gameMode->characterPointersCPP)
            {
                APlayerState* PState = GetPlayerState<APlayerState>();
                ACharacterState* characterState = Cast<ACharacterState>(PState);

                if (characterState && character)
                {
                    FVector Color;
                    switch (characterState->PlayerIndex)
                    {
                    case 1: //Red
                        Color = FVector(255, 10, 10);
                        character->SetMaterialS(BaseMaterial);
                        character->BaseMaterial = BaseMaterial;
                        character->SpawnedActor = BaseSpawnedActor;
                        break;
                    case 2: //Blue
                        Color = FVector(10, 10, 255);
                        character->SetMaterialS(GhostMaterial);
                        character->BaseMaterial = GhostMaterial;
                        character->SpawnedActor = GhostSpawnedActor;

                        break;
                    case 3: //Green
                        Color = FVector(10, 255, 10);
                        character->SetMaterialS(RainBowMaterial);
                        character->BaseMaterial = RainBowMaterial;
                        character->SpawnedActor = RainBowSpawnedActor;

                        break;
                    case 4: //Yellow
                        Color = FVector(255, 255, 10);
                        character->SetMaterialS(GoldMaterial);
                        character->BaseMaterial = GoldMaterial;
                        character->SpawnedActor = GoldSpawnedActor;

                        break;
                    default:
                        Color = FVector(FMath::RandRange(0, 255), FMath::RandRange(0, 255), FMath::RandRange(0, 255));
                        break;
                    }

                    character->SetCircleColor(Color);
                    

                }
            }
        }

    }
    if (HUD)
    {
        HUD->addEnemyHUD();
        HUD->Tick(deltaTime);
    }

    if (IsInputGamepad())//GamePad
    {
        APlayerController::bShowMouseCursor = 0;
    }
    else //Mouse
    {
        APlayerController::bShowMouseCursor = 1;
    }
}


void ACharacterController::UpdateHP(ACharacterState* characterState)
{
    Server_UpdateHP(characterState);
}

void ACharacterController::Server_UpdateHP_Implementation(ACharacterState* characterState)
{

    if (character != nullptr && characterState != nullptr)
    {
        if (characterState->HP != character->GetHealth())
        {
            if (characterState->HP == 0)
                int foo = 0;


            characterState->HP = character->GetHealth();
            if (HUD)
            {
                HUD->setPlayerHUDHealth();
            }
        }
    }


    Multicast_UpdateHP(characterState);
}

void ACharacterController::Multicast_UpdateHP_Implementation(ACharacterState* characterState)
{

    if (character != nullptr && characterState != nullptr)
    {
        if (characterState->HP != character->GetHealth())
        {
            if (characterState->HP == 0)
                int foo = 0;

            characterState->HP = character->GetHealth();
            if (HUD)
            {
                HUD->setPlayerHUDHealth();
            }
        }
    }

}


void ACharacterController::OnMovePressed(const FInputActionValue& Value)
{

    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->isInlobbyCPP == true)
        {
            return;
        }

        // input is a Vector2D
        FVector2D movementVector = Value.Get<FVector2D>();

        if (character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED)
        {
            OnDanceGamePressed(movementVector);
        }
        else
        {
            //Make sure the direction is always forward/backward and never altered
            character->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), movementVector.Y);

            //Make sure the direction is always right/left and never altered
            character->AddMovementInput(FVector(0.0f, 1.0f, 0.0f), movementVector.X);
        }
    }

}

void ACharacterController::OnMoveReleased(const struct FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING || character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        if (character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED)
        {
            OnDanceGameRelease();
        }
        else
        {
            character->ApplyStateChange(ECharacterState::IDLE);
        }
    }
}

void ACharacterController::CreateHUD()
{
    if (HUDTemplate && IsLocalPlayerController())
    {
        //if (GetLocalRole() == ROLE_AutonomousProxy)
        {
            HUD = NewObject<AQuetzalHUD>(this, HUDTemplate);
            HUD->CreateHUD(this);
        }

        APlayerState* PState = GetLocalPlayer()->GetPlayerController(GetWorld())->GetPlayerState<APlayerState>();
        ACharacterState* characterState = Cast<ACharacterState>(PState);

        if (characterState && HUD)
        {
            m_playerState = characterState;
        }
    }

    //ClientSetHUD(HUDTemplate);
}

float ACharacterController::GetMoveValue()
{

    if (inputSubsystem != nullptr)
    {
        FInputActionValue InputActionValue = inputSubsystem->GetPlayerInput()->GetActionValue(moveAction);
        return InputActionValue.Get<float>();
    }
    return 0.0f;
}



bool ACharacterController::IsInputGamepad()
{
    //TODO needs testing with a Gamepad
    UInputDeviceSubsystem* InputDeviceSubsystem = GetGameInstance()->GetEngine()->GetEngineSubsystem<UInputDeviceSubsystem>();

    if (InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(GetPlatformUserId()).PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad) {
        return true;
    }
    return false;
}

void ACharacterController::Look(const FInputActionValue& Value)
{

    //TODO rotate circle with arrow under character


}

void ACharacterController::OnJumpPressed(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        character->ApplyStateChange(ECharacterState::JUMPING);
        character->Jump();
        if (character->AudioManager != nullptr)
        character->AudioManager->PlayAudioCue(ECueAudio::JUMP, character->GetActorLocation());
    }
}

void ACharacterController::OnJumpRelease(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        //character->ApplyStateChange(ECharacterState::FALLING);
        character->StopJumping();
    }
}

void ACharacterController::DashTriggered(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->isInlobbyCPP == true)
        {
            return;
        }


        if (character->bIsHoldingObject && character->equipedWeapon == nullptr)
        {
            return;
        }

        if (character->bCanDash)
        {
            character->BlockDashInput();
            character->ApplyStateChange(ECharacterState::DASHING);
            if (character->AudioManager != nullptr)
            {
                character->AudioManager->PlayAudioCue(ECueAudio::DASH, character->GetActorLocation());
                character->AudioManager->SetAudioCueVolume(ECueAudio::DASH, 3.0f);
            }


        }
    }
}

void ACharacterController::StunTriggered(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->isInlobbyCPP == true)
        {
            return;
        }

        character->ApplyStateChange(ECharacterState::STUNNED);
        if (character->AudioManager != nullptr)
            character->AudioManager->PlayAudioCue(ECueAudio::STUNNED, character->GetActorLocation());
    }
}

void ACharacterController::DanceStunTriggered(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->isInlobbyCPP == true)
        {
            return;
        }

        character->ApplyStateChange(ECharacterState::DANCE_STUNNED);
        if (character->AudioManager != nullptr)
            character->AudioManager->PlayAudioCue(ECueAudio::DANCESTUNNED, character->GetActorLocation());
    }
}

void ACharacterController::LightAttackPressed(const FInputActionValue& Value)
{
    // Comprueba estados que impiden el ataque
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        MakeCharacterFaceMouseCursor(true);
        if (character->specialAttackHoldSwitch == false)
        {
            



            if (character->equipedWeapon || character->bCanAttack)
            {

                character->lightAttackHoldSwitch = true;
                character->ApplyStateChange(ECharacterState::ATTACKS);

            }
            // Solo bloqueamos el input de ataque si el personaje NO tiene un arma equipada
            if (!character->equipedWeapon && character->bCanAttack)
            {
                character->BlockAttackInput();
            }
        }
    }
}

void ACharacterController::GrabSound()
{
    if (holdHitAudio == false)
    {
        character->AudioManager->PlayAudioCue(ECueAudio::PLAYERGRAB, character->GetActorLocation());
        character->AudioManager->SetAudioCueVolume(ECueAudio::PLAYERGRAB, 1.0f);
        holdHitAudio = true;
    }
}

void ACharacterController::LightAttackRelease(const FInputActionValue& Value)
{
    // Comprueba estados que impiden finalizar el ataque
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->IsInvulnerable() ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        MakeCharacterFaceMouseCursor(false);
        // Si el personaje no tiene arma y bCanAttack es false, se evita procesar el release.
       /* if (!character->equipedWeapon && !character->bCanAttack)
        {
            return;
        }*/
        if (character->specialAttackHoldSwitch == false)
        {
            if (character->AudioManager != nullptr)
            {
                character->AudioManager->StopAudioCue(ECueAudio::PLAYERGRAB);
                character->AudioManager->PlayAudioCue(ECueAudio::PLAYERATTACK, character->GetActorLocation());
                character->AudioManager->SetAudioCueVolume(ECueAudio::PLAYERATTACK, 1.0f);
                holdHitAudio = false;
            }
            character->ApplyStateChange(ECharacterState::ATTACKR);
            //// Solo bloqueamos el input de ataque si el personaje NO tiene un arma equipada
            //if (!character->equipedWeapon)
            //{
            //    character->BlockAttackInput();
            //}
        }
    }
}
/** Called for Attack input */
void ACharacterController::SpecialAttackPressed(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        //MakeCharacterFaceMouseCursor(true); // for the spin attack on the mallet to work, need to make the weapons decide when you face the mouse
        if (character->lightAttackHoldSwitch == false)
        {
            

            if (character->bCanAttack)
            {
                if (character->equipedWeapon)
                {
                    if (character->AudioManager != nullptr)
                    {
                        GetWorld()->GetTimerManager().SetTimer(HoldTimerHandle, this, &ACharacterController::GrabSound, 0.5f);
                    }
                }
                character->specialAttackHoldSwitch = true;
                character->BlockAttackInput();
                character->ApplyStateChange(ECharacterState::SPECIALATTACKS);

            }
        }
    }

}

/** Called for Attack input */
void ACharacterController::SpecialAttackRelease(const FInputActionValue& Value)
{

    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->IsInvulnerable() ||
            character->CharacterStateCPP->State == ECharacterState::KNOCKEDBACK ||
            character->CharacterStateCPP->State == ECharacterState::RAGDOLLING ||
            character->CharacterStateCPP->State == ECharacterState::RECOVERING ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        MakeCharacterFaceMouseCursor(false);
        if (character->lightAttackHoldSwitch == false)
        {
            if (character->equipedWeapon)
            {
                if (character->AudioManager != nullptr)
                {
                    holdHitAudio = false;
                    character->AudioManager->StopAudioCue(ECueAudio::PLAYERGRAB);
                    character->AudioManager->PlayAudioCue(ECueAudio::PLAYERATTACK, character->GetActorLocation());
                    character->AudioManager->SetAudioCueVolume(ECueAudio::PLAYERATTACK, 1.0f);
                }
            }

            character->ApplyStateChange(ECharacterState::SPECIALATTACKR);

        }
    }
}

void ACharacterController::InteractorThrow(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->isInlobbyCPP == true)
        {
            return;
        }

        if (character->bIsHoldingObject)
        {
            if (character->AudioManager != nullptr)
            {
                character->AudioManager->PlayAudioCue(ECueAudio::PLAYERATTACK, character->GetActorLocation());
                character->AudioManager->SetAudioCueVolume(ECueAudio::PLAYERATTACK, 0.8f);
            }
            character->ApplyStateChange(ECharacterState::THROW);
        }
        else if (character->bCanGrab)
        {
            if (character->AudioManager != nullptr)
            {
                character->AudioManager->PlayAudioCue(ECueAudio::PLAYERGRAB, character->GetActorLocation());
                character->AudioManager->SetAudioCueVolume(ECueAudio::PLAYERGRAB, 1.0f);
            }
            character->BlockGrabInput();
            character->ApplyStateChange(ECharacterState::INTERACT);
        }
    }
}

void ACharacterController::ToggleHUD(const FInputActionValue& Value)
{
    if (character != nullptr)
    {
        if (HUD)
        {
            HUD->toggleHUD();
        }
    }
}

void ACharacterController::DropWeapon(const FInputActionValue& Value)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->isInlobbyCPP == true)
        {
            return;
        }

        if (character->equipedWeapon != nullptr)
        {
            character->equipedWeapon->DropWeapon();
        }
    }
}

void ACharacterController::OnDanceGameRelease()
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if (character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED)
        {
            for (auto& checkTrue : arrowsinput)
            {
                if (checkTrue.Value)
                {
                    checkTrue.Value = !checkTrue.Value;
                }
            }
        }
    }
}

void ACharacterController::OnDanceGamePressed(FVector2D movementVector)
{
    if (character != nullptr)
    {
        UDanceMinigameWidget* DanceMinigameWidget = character->DanceMinigameWidget;
        EDanceArrowDirection directionInput = EDanceArrowDirection::A_DEFAULT;
        if (DanceMinigameWidget)
        {
            switch ((int)movementVector.X)
            {
            case 1://d
                if (arrowsinput[EDanceArrowDirection::A_RIGHT] != true)
                    directionInput = EDanceArrowDirection::A_RIGHT;
                break;
            case -1://a
                if (arrowsinput[EDanceArrowDirection::A_LEFT] != true)
                    directionInput = EDanceArrowDirection::A_LEFT;
                break;
            default:
                break;
            }
            switch ((int)movementVector.Y)
            {
            case 1://w
                if (arrowsinput[EDanceArrowDirection::A_UP] != true)
                    directionInput = EDanceArrowDirection::A_UP;
                break;
            case -1://s
                if (arrowsinput[EDanceArrowDirection::A_DOWN] != true)
                    directionInput = EDanceArrowDirection::A_DOWN;
                break;
            default:
                break;
            }
            arrowsinput[directionInput] = true;
        }

        if (directionInput != EDanceArrowDirection::A_DEFAULT)
            DanceMinigameWidget->CheckPlayerInput(directionInput);

        if (DanceMinigameWidget->GetHasWon())
        {
            character->ApplyStateChange(ECharacterState::DANCE_STUNNED_CLEAR);
        }
    }
}

void ACharacterController::MakeCharacterFaceMouseCursor(bool faceCursor)
{
    if (character != nullptr && character->CharacterStateCPP != nullptr)
    {
        if(character->CharacterStateCPP->State == ECharacterState::RESPAWNING ||
            character->CharacterStateCPP->State == ECharacterState::STUNNED ||
            character->CharacterStateCPP->State == ECharacterState::DANCE_STUNNED ||
            character->isInlobbyCPP == true)
        {
            return;
        }
        if (IsLocalPlayerController())
        {
            FVector characterLoc = character->GetActorLocation();
            FHitResult hitResult;
            GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, hitResult);
            FVector hitLoc = hitResult.Location;
            FRotator newRot = character->GetActorRotation();
            float newYaw = (hitLoc - characterLoc).Rotation().Yaw;
            newRot.Yaw = newYaw;
            character->AimRotate(newRot, faceCursor);
        }

    }
}

void ACharacterController::onRespawn()
{
    if (HUD)
    {

        HUD->respawn(); //TO REMOVE * UNUSED
    }
}
