
#include "Audio/AudioManager.h"
#include "Kismet/GameplayStatics.h"

AAudioManager::AAudioManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AAudioManager::BeginPlay()
{
    Super::BeginPlay();
}

void AAudioManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAudioManager::PlayAudioCue(ECueAudio AudioType, FVector Location)
{
    if (SoundCueMap.Contains(AudioType) && SoundCueMap[AudioType])
    {
        if (ActiveAudioComponents.Contains(AudioType) && ActiveAudioComponents[AudioType])
        {
            ActiveAudioComponents[AudioType]->Stop();
        }

        UAudioComponent *AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundCueMap[AudioType], Location);

        if (AudioComponent)
        {
            ActiveAudioComponents.Add(AudioType, AudioComponent);
        }
    }
}

void AAudioManager::StopAudioCue(ECueAudio AudioType)
{
    if (ActiveAudioComponents.Contains(AudioType))
    {
        if (UAudioComponent *AudioComp = ActiveAudioComponents[AudioType])
        {
            if (AudioComp->IsPlaying())
            {
                AudioComp->Stop();
            }
        }
        ActiveAudioComponents.Remove(AudioType);
    }
}

void AAudioManager::SetAudioCueVolume(ECueAudio AudioType, float volume)
{
    if (ActiveAudioComponents.Contains(AudioType))
    {
        if (UAudioComponent* AudioComp = ActiveAudioComponents[AudioType])
        {
            if (AudioComp->IsPlaying())
            {
                AudioComp->SetVolumeMultiplier(volume);
            }
        }
    }
}
