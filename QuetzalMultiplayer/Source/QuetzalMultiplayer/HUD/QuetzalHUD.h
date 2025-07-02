// Fill out your copyright notice in the Description page of Project Settings.

/****************************************************************************
*	Name: QuetzalHUD
*	Version: 0.2
*	Created: 2025/02/02
*	Description:
*	- child class of AHUD
*	- HUD class for the game, will be toggleable controls all the HUD elements
*	Change Log:
*  -2025/02/04
*		-Swapped focus from using UMG to Slate (TODO: Remove UMG and clean up)
*
*       Next steps
*       1. Decide heart size and distance between other hearts
*       2. Deicde enemy hearts size (set up with checking to see how many enemies
*       3. Set up function to swap heart with half heart sprite
*
* Version: 0.3
*  -2025/02/17
*       -Hearts and score displays fine, score markers need to be updated
*       -HUD Toggle is implemented as well, but isnt tied to any button
*
*       Next Steps
*       1. Move HUD elements to fit Player Icon
*       2. Bind button to toggle HUD
*       3. Make heart damage
*
* Version: 0.4
* -2025/02/28
*       -Fixing movement code so all HUD objects move off screen cleanly
*       -Made it so that after 5 score elements the score will be counted with a number instead of a tally
*       -Various bug fixes
*
* -2025/03/03
*       -Implemented code that makes the HUD go up and down with time to show display the feature
*       -Cleaning up code and commenting it to prepare it for Alpha 1 submission
* 
* Version: 0.5
* -2025/03/20
*       -HUD now gets created per player to be replicated. 
*
* Version: 0.6
* -2025/04/01
*       -HUD fully works with respawn and is fully replicated for each player. 
* -2025/04/05
*       -Bug fixes and added healing functionality
*
****************************************************************************/
//Added enum 03 16
//Merged Dev into my branch, removed the input I added 03 17

/**
 * @file QuetzalHUD.h
 * @brief HUD class.
 * @version 0.3
 * @date 2025-02-4
 *
 *	20250204-FB-v0.2: added Hearts and half heart placement
 *	20250210-FB-v0.2: added Score, and toggle movement
 *	20250217-FB-v0.3: added Aspect ratio into scale code
 *	20250223-FB-v0.4: added Documentation and formatted code
 */

#pragma once



 //TO BE DEPRICATED
UENUM(BlueprintType)
enum class EHUDObjectType : uint8
{
    PlayerHearts        UMETA(DisplayName = "Player Hearts"),
    EnemyHearts         UMETA(DisplayName = "Enemy Hearts"),
    PlayerScore         UMETA(DisplayName = "Player Score"),
    EnemyScore          UMETA(DisplayName = "Enemy Score"),
    PlayerScoreText     UMETA(DisplayName = "Player Score Text"),
    EnemyScoreText      UMETA(DisplayName = "Enemy Score Text"),
};

struct HUDElements
{
    TSharedPtr<class SCompoundWidget> m_HUDElement;
    FVector2D minMaxPos;
};

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "QuetzalHUD.generated.h"

class AQuetzalMultiplayerCharacter;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FViewport::ViewportResizedEvent);
/**
 * @class AQuetzalHUD
 * @brief HUD class, displays all relevent UI elements in game
 */

UCLASS()
class QUETZALMULTIPLAYER_API AQuetzalHUD : public AHUD
{
    GENERATED_BODY()

public:

    AQuetzalHUD();
    //HUD Element Templates

    /** Heart Template. */
    UPROPERTY(EditAnywhere, Category = "UI Assets")
    class UTexture2D* m_heartTemplate;

    /** Half Heart Template. */
    UPROPERTY(EditAnywhere, Category = "UI Assets")
    class UTexture2D* m_halfHeartTemplate;

    /** Quarter Heart Template. */
    UPROPERTY(EditAnywhere, Category = "UI Assets")
    class UTexture2D* m_quarterHeartTemplate;

    /** Three Quarter Heart Template. */
    UPROPERTY(EditAnywhere, Category = "UI Assets")
    class UTexture2D* m_threeQuarterHeartTemplate;

    /** Score Template. */
    UPROPERTY(EditAnywhere, Category = "UI Assets")
    class UTexture2D* m_stockTemplate;

    /** Font Template */
    UPROPERTY(EditAnywhere, Category = "UI Assets")
    class UObject* m_font;
    //TODO Make custom Font or find one more suited for our game

     /**
    * @brief Gets the games Resolution if it gets changed
    * @param ViewPort A pointer to the game's viewport
    * @param val Depricated from the Unreal's resolution function
    */

    void getGameResolution(FViewport* ViewPort, uint32 val);

    /**
   * @brief Toggle's the HUD to remove all optional HUD elements
   */
    UFUNCTION(BlueprintCallable, Category = "ToggleHUD")
    void toggleHUD();

    void moveHUD();

    void addEnemyHUD();

    void respawn();

    /**
   * @brief Sets the active player's health
   * @param character A pointer to the character's health being changed
   */
    void setPlayerHUDHealth();

    /**
   * @brief Sets all enemies's health
   * @param character A pointer to the character's health being changed
   */
    void setEnemyHUDHealth(); //, int enemyNumber

    //UFUNCTION(NetMulticast, Reliable)
    //void Multicast_setEnemyHUDHealth();

    void addPlayerScore();

    void addEnemyScore();

    void CreateHUD(class ACharacterController* controller);

    virtual void Tick(float DeltaTime) override;
protected:

    virtual void BeginPlay() override;

private:

    /** Test variables, to be depricated */

    int m_enemyPoints; //Remove once characterScore updates

    float m_moveTimer = 3.0f;
    //Is HUD on screen
    bool m_onScreen = true;
    //Array of drawn players
    bool m_playersAreDrawn[8] = { false };
    //Array of previous player healths (stores old player health to see if its changed and updates)
    int playerHealthArray[8] = {0};
    //Array of previous player stock (stores old player stock to see if its changed and updates)
    int playerStockArray[8] = {3};
    //Array of if Stock was initizalized
    bool m_stockDrawn[8];
    //Map of enemy numbers with player ids
    TMap<int, int>enemyNumber; //<Player index, enemyNumber>
    //Stock removal
    bool m_stockRemoved = false;
    //The number of drawn enemies
    int numEnemiesDrawn;

    /** Is the HUD currently in motion */
    bool m_inMotion = false;
    /** Number of other players in the game */
    int m_numPlayers;
    /** The player score (might be useless if score is stored in player)*/
    int m_playerScore;
    /** The speed that the UI will move off the screen when toggled */
    float m_HUDMoveSpeed = 0;

    /** Player Pointers */
    ACharacterController* m_playerController;

    /** HUD Objects */
#define HealthWidgetVector std::vector<TSharedPtr<class SHealthWidget>>
#define ScoreWidgetVector std::vector<TSharedPtr<class SScoreWidget>>
#define ParentWidgetVector std::vector<TSharedPtr<class SCompoundWidget>>

    /** Player's vector of Health Widget objects */
    HealthWidgetVector m_selectedPlayersHearts;
    /** Player's vector of Score Widget objects */
    ScoreWidgetVector m_selectedPlayersScore;

    /** Player's Score Text Widget */
    TSharedPtr<class STextWidget> m_selectedPlayerScoreText;
    TMap<FString, TSharedPtr<class STextWidget>> m_enemiesScoreText;

    /** Game Time Widget */
    TSharedPtr<class STextWidget> m_timerText;

    /** Map containing a pointer to a vector of all enemy hearts*/
    TMap<FString, HealthWidgetVector> m_enemyHearts;
    TMap<FString, ScoreWidgetVector> m_enemyScore;

    /** All Hud Elements */
    std::vector<TSharedPtr<class SCompoundWidget>> m_AllHeartElements;
    std::vector<TSharedPtr<class SCompoundWidget>> m_AllScoreElements;
    std::vector<TSharedPtr<class SCompoundWidget>> m_AllHUDElements;

    std::vector<HUDElements> m_HUDElements;

    /** Player's vector of Score Widget objects */
    FIntPoint m_screenResolution;
};
