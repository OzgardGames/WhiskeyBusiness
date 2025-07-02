#include "QuetzalMultiplayerGameMode.h"
#include "Actors/Character/QuetzalMultiplayerCharacter.h"
#include "CharacterState.h" 
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"


AQuetzalMultiplayerGameMode::AQuetzalMultiplayerGameMode()
{
    // Set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }

    static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/ThirdPerson/Blueprints/CharacterController/BP_CharacterController"));
    if (PlayerControllerBPClass.Class != NULL)
    {
        PlayerControllerClass = PlayerControllerBPClass.Class;
    }
}

void AQuetzalMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);


    UE_LOG(LogTemp, Warning, TEXT("New player has logged in. Assigning PlayerIndexes..."));


    AGameStateBase* GState = GetGameState<AGameStateBase>();
    if (!GState)
    {
        UE_LOG(LogTemp, Error, TEXT("GameState is NULL in PostLogin!"));
        return;
    }
    int Looper = GState->PlayerArray.Num();

    for (int32 i = 0; i < Looper; ++i)
    {
        ACharacterState* CharacterState = Cast<ACharacterState>(GState->PlayerArray[i]);
        if (CharacterState)
        {
            CharacterState->PlayerIndex = i + 1;
            UE_LOG(LogTemp, Warning, TEXT("Assigned PlayerIndex %d to %s"), CharacterState->PlayerIndex, *CharacterState->GetName());
        }
    }

    numPlayersCPP = GetNumPlayers();

}
void AQuetzalMultiplayerGameMode::PreLogin(
    const FString& Options,
    const FString& Address,
    const FUniqueNetIdRepl& UniqueId,
    FString& ErrorMessage
)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

    // Block if game has already started
    if (bGameStarted)
    {
        ErrorMessage = TEXT("Game has already started.");
        return;
    }

    // Block if max players is reached
    if (GetNumPlayers() >= MaxPlayers)
    {
        ErrorMessage = TEXT("Server is full.");
        return;
    }
}

void AQuetzalMultiplayerGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    APlayerController* PlayerController = Cast<APlayerController>(Exiting);
    if (PlayerController)
    {
        //characterPointersCPP.Remove(PlayerController);
        UE_LOG(LogTemp, Warning, TEXT("Player disconnected. Remaining players: %d"), characterPointersCPP.Num());

        // OnPlayerListUpdated(characterPointersCPP.Num());
    }
}

// Called every frame
void AQuetzalMultiplayerGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //TODO switch to delegates vs using tick, broadcast when object is ready to be respawned
    for (AActor* actor : actorsToSpawn)
    {
        if (actor != nullptr)
        {
            ISpawnActorInterface* spawnActorInterface = Cast<ISpawnActorInterface>(actor);
            if (spawnActorInterface != nullptr)
            {
                if (spawnActorInterface->GetRespawnFlag())
                {
                    SpawnActorHandler(actor);
                }
            }
        }
    }
}

void AQuetzalMultiplayerGameMode::SpawnActorHandler(AActor* actor)
{
    if (GetWorld() != nullptr && actor != nullptr)
    {
        ISpawnActorInterface* spawnActorInterface = Cast<ISpawnActorInterface>(actor);
        if (spawnActorInterface != nullptr)
        {
            spawnActorInterface->SetRespawnFlag(false);
            float spawnTime = spawnActorInterface->GetRespawnTime();
            FTimerHandle spawnTimer;
            FTimerDelegate spawnTimerDelegate;
            auto& TimerManager = GetWorld()->GetTimerManager();
            spawnTimerDelegate.BindUFunction(this, FName("SpawnActor"), actor);
            TimerManager.SetTimer(spawnTimer, spawnTimerDelegate, spawnTime, false);
        }
    }
}

void AQuetzalMultiplayerGameMode::SpawnActor(AActor* actorToSpawn)
{
    if (actorToSpawn != nullptr)
    {
        ISpawnActorInterface* spawnActorInterface = Cast<ISpawnActorInterface>(actorToSpawn);
        if (spawnActorInterface != nullptr)
        {
            spawnActorInterface->RespawnActor(spawnActorInterface->originalSpawnLocationAndRotation);
        }
    }
}

void AQuetzalMultiplayerGameMode::BeginPlay()
{
    Super::BeginPlay();

    //Get All Actors
    TArray<AActor*> ActorsToFind;
    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorsToFind);
    }
    for (AActor* actor : ActorsToFind)
    {
        if (actor != nullptr)
        {
            //Find all actors with ISpawnActorInterface and add them to the list
            bool bIsImplemented = actor->Implements<USpawnActorInterface>();
            if (bIsImplemented)
            {
                ISpawnActorInterface* spawnActorInterface = Cast<ISpawnActorInterface>(actor);
                actorsToSpawn.AddUnique(actor);
            }
        }
    }

    //Get and set all actors with ISpawnActorInterface spawn location and rotation
    for (AActor* actor : actorsToSpawn)
    {
        ISpawnActorInterface* spawnActorInterface = Cast<ISpawnActorInterface>(actor);
        if (spawnActorInterface != nullptr)
        {
            spawnActorInterface->originalSpawnLocationAndRotation.spawnLocation = actor->GetActorLocation();
            spawnActorInterface->originalSpawnLocationAndRotation.spawnRotation = actor->GetActorRotation();
        }
    }

}