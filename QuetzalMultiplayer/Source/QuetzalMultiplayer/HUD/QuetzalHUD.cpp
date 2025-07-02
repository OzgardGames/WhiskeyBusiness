// Fill out your copyright notice in the Description page of Project Settings.

#include "QuetzalHUD.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Fonts/CompositeFont.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Net/UnrealNetwork.h"
#include "QuetzalMultiplayerCharacter.h"
#include "Slate/SHealthWidget.h"
#include "Slate/SScoreWidget.h"
#include "Slate/STextWidget.h"
#include "SlateOptMacros.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/SWeakWidget.h"
#include "Widgets/SViewport.h"

const int MAX_HEARTS = 3;
const float DISTANCE_BETWEEN_HEARTS = 100.0f;
const FVector2D INITIAL_POS(100.0f, 10.0f);
const FScale2D INITIAL_SCALE(0.1, 0.083);

const float DISTANCE_BETWEEN_ENEMY_HEARTS = 50.0f;
const FVector2D INITIAL_ENEMY_HEART_POS(700.0f, 10.0f);
const FScale2D INITIAL_ENEMY_HEART_SCALE(0.05f, 0.041f);

const int MAX_SCORE = 4;
const float DISTANCE_BETWEEN_SCORE = 50.0f;
const FVector2D INITIAL_SCORE_POS(150.0f, 100.0f);
const FVector2D INITIAL_SCORE_TEXT_POS(200.0f, 120.0f);
const FScale2D INITIAL_SCORE_SCALE(0.04f, 0.06f);

const float DISTANCE_BETWEEN_ENEMY_SCORE = 25.0f;
const FVector2D INITIAL_ENEMY_SCORE_POS(720.0f, 60.0f);
const FVector2D INITIAL_ENEMY_SCORE_TEXT_POS(750.0f, 70.0f);
const FScale2D INITIAL_ENEMY_SCORE_SCALE(0.02f, 0.03f);

AQuetzalHUD::AQuetzalHUD()
    :m_heartTemplate(nullptr),
    m_threeQuarterHeartTemplate(nullptr),
    m_halfHeartTemplate(nullptr),
    m_quarterHeartTemplate(nullptr),
    m_stockTemplate(nullptr)
{
}

void AQuetzalHUD::BeginPlay()
{
    Super::BeginPlay();
}



void AQuetzalHUD::CreateHUD(ACharacterController* controller)
{
    //Get the controller instead
    m_playerController = controller;

    //Get the number of players
    m_numPlayers = GetWorld()->GetNumControllers();

    //Initializing this function to trigger when screen size changes
    FViewport::ViewportResizedEvent.AddUObject(this, &AQuetzalHUD::getGameResolution);
    UGameViewportClient* viewPort = GetWorld()->GetGameViewport();

    //Getting the aspect ratio for HUD element creation
    FVector2D screenSize;
    viewPort->GetViewportSize(screenSize);
    float aspectRatio = float(screenSize.X) / float(screenSize.Y);

    //Initializing Player's Hearts
    TSharedPtr<class SHealthWidget> HealthWidget;
    std::vector<TSharedPtr<class SCompoundWidget>> heartVector;
    FVector2D HeartPos;

    for (int i = 0; i < MAX_HEARTS; i++)
    {
        HealthWidget = SNew(SHealthWidget).HeartImage(m_heartTemplate);                                                                  //Setting the heart image from blueprint
        HealthWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));                                                                    //Setting the heart size 
        HeartPos = FVector2D(INITIAL_POS.X + DISTANCE_BETWEEN_HEARTS * i, INITIAL_POS.Y);
        FSlateRenderTransform transHearts(INITIAL_SCALE, HeartPos);                                                                      //Setting the transform pos of the heart
        HealthWidget->SetRenderTransform(transHearts);                                                                                   //Setting the heart pos
        m_selectedPlayersHearts.push_back(HealthWidget);                                                                                 //Pushing the hearts into a vector of all the player's hearts
        m_AllHeartElements.push_back(HealthWidget);                                                                                      //Pushing the hearts into a vector of all hearts

        HUDElements element;
        element.m_HUDElement = HealthWidget;
        element.minMaxPos = FVector2D(HeartPos.Y, HeartPos.Y - 300);
        m_HUDElements.push_back(element);
    }

    TSharedPtr<class SScoreWidget> ScoreWidget;

    //Stock Icons
    for (int i = 0; i < 3; i++)
    {
        ScoreWidget = SNew(SScoreWidget).ScoreImage(m_stockTemplate);
        ScoreWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
        FSlateRenderTransform trans4(INITIAL_SCORE_SCALE, FVector2D(INITIAL_SCORE_POS.X + DISTANCE_BETWEEN_SCORE * i, INITIAL_SCORE_POS.Y));
        ScoreWidget->SetRenderTransform(trans4);
        m_selectedPlayersScore.push_back(ScoreWidget);
        m_AllScoreElements.push_back(ScoreWidget);

        HUDElements element;
        element.m_HUDElement = ScoreWidget;
        element.minMaxPos = FVector2D(INITIAL_SCORE_POS.Y, INITIAL_SCORE_POS.Y - 300);
        m_HUDElements.push_back(element);
    }

    for (auto widget : m_selectedPlayersHearts)
    {
        viewPort->AddViewportWidgetContent(widget.ToSharedRef());
    }
    for (auto widget : m_AllScoreElements)
    {
        viewPort->AddViewportWidgetContent(widget.ToSharedRef());
    }

    APlayerState* PState = m_playerController->GetPlayerState<APlayerState>(); //m_playerHearts <- debug
    ACharacterState* characterState = Cast<ACharacterState>(PState);

    playerHealthArray[characterState->PlayerIndex] = characterState->HP;
    //For toggle if the move speed is negative the HUD will move off screen and vice versa
    m_HUDMoveSpeed = -250;

    //This sets the players score equal to the num score objects made (if the player starts with a score)
    m_playerScore = m_selectedPlayersScore.size();
    m_enemyPoints = 0;
}

void AQuetzalHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AGameStateBase* GState = GetWorld()->GetGameState<AGameStateBase>();
    m_numPlayers = GState->PlayerArray.Num();

    APlayerState* PpState = m_playerController->GetPlayerState<APlayerState>();
    ACharacterState* characterStatee = Cast<ACharacterState>(PpState);

    if (characterStatee->PlayerIndex == 0)
    {
        int foo = 0;
    }

    //Add enemies and respawn their hearts if they die
    addEnemyHUD();
    //Update Player Health
    setPlayerHUDHealth();
    //Update Enemy Health
    setEnemyHUDHealth();

    //This if statement only runs the movement code if it is set in motion
    if (m_inMotion)
        moveHUD();
}

void AQuetzalHUD::getGameResolution(FViewport* ViewPort, uint32 val)
{
    //This function updates all the HUD objects with the aspect ratio so the HUD doesnt stretch
    m_screenResolution = ViewPort->GetSizeXY();
    float aspectRatio = float(m_screenResolution.X) / float(m_screenResolution.Y);

    for (int i = 0; i < m_AllHeartElements.size(); i++)
    {
        m_AllHeartElements.at(i)->SetContentScale(FVector2D(1 / aspectRatio, 1));
    }

    for (int i = 0; i < m_AllScoreElements.size(); i++)
    {
        m_AllScoreElements.at(i)->SetContentScale(FVector2D(1 / aspectRatio, 1));
    }

    //Text aspect ratio (used for score)
    if (m_selectedPlayerScoreText)
    {
        FVector2D pos = m_selectedPlayerScoreText.ToSharedRef()->GetRenderTransform()->GetTranslation();

        FSlateRenderTransform trans5(FScale2f(1 / aspectRatio, 0.5), FVector2D(pos));
        m_selectedPlayerScoreText->SetRenderTransform(trans5);
    }
}

void AQuetzalHUD::toggleHUD()
{
    //This sets the movement of the HUD objects so they move on or off screen as needed
    if (m_onScreen)
        m_HUDMoveSpeed = -250.0f;
    else
        m_HUDMoveSpeed = 250.0f;

    m_onScreen = !m_onScreen;
    //This line is needed to set the movement in motion in the Tick() function
    m_inMotion = true;
}

void AQuetzalHUD::moveHUD()
{
    //Moves HUD elements on or off screen
    float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

    for (int i = 0; i < m_HUDElements.size(); i++)
    {
        auto originalPos = m_HUDElements.at(i).m_HUDElement->GetRenderTransform();

        float xPos = float(originalPos->GetTranslation().X);
        float yPos = float(originalPos->GetTranslation().Y);

        yPos += m_HUDMoveSpeed * DeltaTime;

        if (yPos < m_HUDElements.at(i).minMaxPos.Y && !m_onScreen)
        {
            yPos = m_HUDElements.at(i).minMaxPos.Y; //Y for max
            m_inMotion = false;
        }
        else if (yPos > m_HUDElements.at(i).minMaxPos.X && m_onScreen)
        {
            yPos = m_HUDElements.at(i).minMaxPos.X; //X for min
        }

        originalPos->SetTranslation(FVector2D(xPos, yPos));

        m_HUDElements.at(i).m_HUDElement->SetRenderTransform(originalPos);
    }
}

void AQuetzalHUD::addEnemyHUD()
{
    //Get the aspect ratio
    UGameViewportClient* viewPort = GetWorld()->GetGameViewport();
    FVector2D screenSize;
    viewPort->GetViewportSize(screenSize);
    float aspectRatio = float(screenSize.X) / float(screenSize.Y);

    AGameStateBase* GState = GetWorld()->GetGameState<AGameStateBase>();
    for (APlayerState* PState : GState->PlayerArray)
    {
        ACharacterState* characterState = Cast<ACharacterState>(PState);
        if (m_playerController)
        {
            int playerNum = characterState->PlayerIndex;

            //If the array is on the owning player or if the player is already drawn continue
            if (characterState == m_playerController->GetPlayerState<ACharacterState>() || m_playersAreDrawn[playerNum])
                continue;
            else
                m_playersAreDrawn[playerNum] = true; //Else mark the enemy as drawn


            //If enemy has not been added before
            if (!enemyNumber.Contains(playerNum))
            {
                enemyNumber.Add(playerNum, numEnemiesDrawn);
                numEnemiesDrawn++;
            }

            TSharedPtr<class SHealthWidget> EnemyHealthWidget;

            FString EnemyNumber = FString::FromInt(characterState->PlayerIndex);
            HealthWidgetVector EnemyHearts;
            ScoreWidgetVector EnemyScores;

            //If hearts are still on screen, remove them
            if (m_enemyHearts.Contains(EnemyNumber))
            {
                if (!m_enemyHearts[EnemyNumber].empty())
                {
                    int size = m_enemyHearts[EnemyNumber].size();
                    for (int j = 0; j < size; j++)
                    {
                        viewPort->RemoveViewportWidgetContent(m_enemyHearts[EnemyNumber].back().ToSharedRef());
                        m_enemyHearts[EnemyNumber].pop_back();
                    }
                }
            }
            if (characterState->StockCPP > 0)
            {
                //Reset health to max
                playerHealthArray[playerNum] = 12;
                //characterState->HP = 12;

                //Initialize the players hearts with position based on their enemy number
                for (int i = 0; i < MAX_HEARTS; i++)
                {
                    EnemyHealthWidget = SNew(SHealthWidget).HeartImage(m_heartTemplate);
                    EnemyHealthWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
                    FSlateRenderTransform transEnemyHearts(INITIAL_ENEMY_HEART_SCALE,
                        FVector2D(INITIAL_ENEMY_HEART_POS.X + DISTANCE_BETWEEN_ENEMY_HEARTS * i + enemyNumber[playerNum] * 500, INITIAL_ENEMY_HEART_POS.Y)); //TODO is m_onScreen = false
                    EnemyHealthWidget->SetRenderTransform(transEnemyHearts);
                    EnemyHearts.push_back(EnemyHealthWidget);

                    HUDElements element;
                    element.m_HUDElement = EnemyHealthWidget;
                    element.minMaxPos = FVector2D(INITIAL_ENEMY_HEART_POS.Y, INITIAL_ENEMY_HEART_POS.Y - 300);
                    m_HUDElements.push_back(element);

                    viewPort->AddViewportWidgetContent(EnemyHealthWidget.ToSharedRef());
                }
            }

            if (!m_stockDrawn[playerNum]) // Add the enemy stock if they've never been drawn before
            {
                TSharedPtr<class SScoreWidget> ScoreWidget;

                for (int i = 0; i < 3; i++)
                {
                    //Enemy Stock Position
                    ScoreWidget = SNew(SScoreWidget).ScoreImage(m_stockTemplate);
                    ScoreWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
                    FSlateRenderTransform trans6(INITIAL_ENEMY_SCORE_SCALE, FVector2D(INITIAL_ENEMY_SCORE_POS.X + DISTANCE_BETWEEN_ENEMY_SCORE * i + enemyNumber[playerNum] * 500, INITIAL_ENEMY_SCORE_POS.Y));
                    ScoreWidget->SetRenderTransform(trans6);
                    EnemyScores.push_back(ScoreWidget);

                    HUDElements element;
                    element.m_HUDElement = ScoreWidget;
                    element.minMaxPos = FVector2D(INITIAL_ENEMY_SCORE_POS.Y, INITIAL_ENEMY_SCORE_POS.Y - 300);
                    m_HUDElements.push_back(element);


                    viewPort->AddViewportWidgetContent(ScoreWidget.ToSharedRef());
                }
                m_enemyScore.Add(EnemyNumber, EnemyScores);
                m_stockDrawn[playerNum] = true;
                playerStockArray[playerNum] = 3;
            }
            else if (m_stockDrawn[playerNum])
            {
                //Remove the Stock at the end
                FString enemyIndex = FString::FromInt(characterState->PlayerIndex);
                if (m_enemyScore.Contains(enemyIndex))
                {
                    if (playerStockArray[playerNum] != characterState->StockCPP)
                    {
                        if (!m_enemyScore[enemyIndex].empty())
                        {
                            playerStockArray[playerNum] = characterState->StockCPP;
                            viewPort->RemoveViewportWidgetContent(m_enemyScore[enemyIndex].back().ToSharedRef());
                            m_enemyScore[enemyIndex].pop_back();
                        }
                    }
                }
            }
            m_enemyHearts.Add(EnemyNumber, EnemyHearts);
        }
    }
}


void AQuetzalHUD::respawn()
{
    //Getting the aspect ratio for HUD element creation
    UGameViewportClient* viewPort = GetWorld()->GetGameViewport();
    FVector2D screenSize;
    viewPort->GetViewportSize(screenSize);
    float aspectRatio = float(screenSize.X) / float(screenSize.Y);

    //Initializing Player's Hearts
    TSharedPtr<class SHealthWidget> HealthWidget;
    FVector2D HeartPos;

    //If hearts were not removed for some reason, remove all of them
    if (!m_selectedPlayersHearts.empty())
    {
        int size = m_selectedPlayersHearts.size();
        for (int j = 0; j < size; j++)
        {
            if (!m_selectedPlayersHearts.empty())
            {
                for (int k = 0; k < m_HUDElements.size(); k++)
                {
                    if (m_HUDElements.at(k).m_HUDElement == m_selectedPlayersHearts.back())
                    {
                        m_HUDElements.erase(m_HUDElements.begin() + k);
                    }
                }
                viewPort->RemoveViewportWidgetContent(m_selectedPlayersHearts.back().ToSharedRef());
                m_selectedPlayersHearts.pop_back();
            }
        }
    }

    //Add the starting hearts
    for (int i = 0; i < MAX_HEARTS; i++)
    {
        HealthWidget = SNew(SHealthWidget).HeartImage(m_heartTemplate);
        HealthWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
        HeartPos = FVector2D(INITIAL_POS.X + DISTANCE_BETWEEN_HEARTS * i, INITIAL_POS.Y);
        FSlateRenderTransform transHearts(INITIAL_SCALE, HeartPos);
        HealthWidget->SetRenderTransform(transHearts);
        m_selectedPlayersHearts.push_back(HealthWidget);

        HUDElements element;
        element.m_HUDElement = HealthWidget;
        element.minMaxPos = FVector2D(HeartPos.Y, HeartPos.Y - 200);
        m_HUDElements.push_back(element);

        viewPort->AddViewportWidgetContent(HealthWidget.ToSharedRef());
    }

    //Reset stock removal
    m_stockRemoved = false;

    APlayerState* PState = m_playerController->GetPlayerState<APlayerState>();
    ACharacterState* characterState = Cast<ACharacterState>(PState);
    int playerNum = characterState->PlayerIndex;

    //Reset health array to max
    playerStockArray[playerNum] = characterState->StockCPP;
    //set the playerHealthArray to players HP
    playerHealthArray[characterState->PlayerIndex] = m_playerController->m_playerState->HP;

    //Debug code for removing stock

    //if (!m_selectedPlayersScore.empty())
    //{
    //    if (playerStockArray[playerNum] != characterState->Stock)
    //    {
    //        viewPort->RemoveViewportWidgetContent(m_selectedPlayersScore.back().ToSharedRef());
    //        m_selectedPlayersScore.pop_back();

    //    }
    //}
}

void AQuetzalHUD::setPlayerHUDHealth()
{
    if (m_selectedPlayersHearts.empty()) //If the hearts are evmpty dont run this code
    {
        return;
    }

    if (m_playerController)
    {
        //Get the screen size and aspectRatio
        UGameViewportClient* viewPort = GetWorld()->GetGameViewport();
        FVector2D screenSize;
        viewPort->GetViewportSize(screenSize);
        float aspectRatio = float(screenSize.X) / float(screenSize.Y);
        TSharedPtr<SHealthWidget> HealthWidget;
        FSlateRenderTransform transform;

        APlayerState* PState = m_playerController->GetPlayerState<APlayerState>();
        ACharacterState* characterState = Cast<ACharacterState>(PState);
        int playerNum = characterState->PlayerIndex;

        //Only change HUD if HP != old playerHealthArray HP
        if (characterState->HP != playerHealthArray[characterState->PlayerIndex])
        {
            //set the playerHealthArray to players HP
            playerHealthArray[characterState->PlayerIndex] = m_playerController->m_playerState->HP;
            if (characterState && m_playerController->m_playerState)
            {
                if (m_playerController->m_playerState->HP < 12)
                {
                    int health = m_playerController->m_playerState->HP; //m_playerHearts <- debug

                    int removalNum = 1;

                    health %= 4;

                    if (m_selectedPlayersHearts.size() == 3 && characterState->HP <= 4)
                    {
                        removalNum = 3;
                    }
                    else if (m_selectedPlayersHearts.size() == 2 && characterState->HP <= 4 || m_selectedPlayersHearts.size() == 3 && characterState->HP <= 8)
                    {
                        removalNum = 2;
                    }

                    if (m_playerController->m_playerState->HP <= 0 && !m_stockRemoved)
                    {
                        
                            viewPort->RemoveViewportWidgetContent(m_selectedPlayersScore.back().ToSharedRef());
                            m_selectedPlayersScore.pop_back();
                            m_stockRemoved = true;

                        
                    }

                    for (int i = 0; i < removalNum; i++)
                    {
                        for (int k = 0; k < m_HUDElements.size(); k++) //Remove the HUD element from the list of all elements
                        {
                            if (m_HUDElements.at(k).m_HUDElement == m_selectedPlayersHearts.back())
                            {
                                m_HUDElements.erase(m_HUDElements.begin() + k);
                            }
                        }
                        if (health != 12 && !m_selectedPlayersHearts.empty()) //Remove back of hearts
                        {
                            viewPort->RemoveViewportWidgetContent(m_selectedPlayersHearts.back().ToSharedRef());
                            m_selectedPlayersHearts.pop_back();
                        }
                    }

                    if (health == 3)
                    {
                        HealthWidget = SNew(SHealthWidget).HeartImage(m_threeQuarterHeartTemplate);
                    }
                    else if (health == 2)
                    {
                        HealthWidget = SNew(SHealthWidget).HeartImage(m_halfHeartTemplate);
                    }
                    else if (health == 1)
                    {
                        HealthWidget = SNew(SHealthWidget).HeartImage(m_quarterHeartTemplate);
                    }
                    else if (health == 0 && m_playerController->m_playerState->HP > 0)
                    {
                        HealthWidget = SNew(SHealthWidget).HeartImage(m_heartTemplate);
                    }

                    if (HealthWidget) // If a widget was made, add it
                    {
                        HealthWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
                        transform = FSlateRenderTransform(INITIAL_SCALE, FVector2D(INITIAL_POS.X + DISTANCE_BETWEEN_HEARTS * m_selectedPlayersHearts.size(), INITIAL_POS.Y));
                        //TODO if m_onScreen = false
                        HealthWidget->SetRenderTransform(transform);

                        HUDElements element;
                        element.m_HUDElement = HealthWidget;
                        FVector2D HeartPos = FVector2D(INITIAL_POS.X + DISTANCE_BETWEEN_HEARTS * m_selectedPlayersHearts.size(), INITIAL_POS.Y);
                        element.minMaxPos = FVector2D(HeartPos.Y, HeartPos.Y - 200);
                        m_HUDElements.push_back(element);

                        m_selectedPlayersHearts.push_back(HealthWidget);
                        viewPort->AddViewportWidgetContent(HealthWidget.ToSharedRef());
                    }
                }
                else if (m_playerController->m_playerState->HP >= 12)
                {
                    if (!m_selectedPlayersHearts.empty()) //max hp
                    {
                        int size = m_selectedPlayersHearts.size();
                        for (int j = 0; j < size; j++)
                        {
                            if (!m_selectedPlayersHearts.empty())
                            {
                                for (int k = 0; k < m_HUDElements.size(); k++)
                                {
                                    if (m_HUDElements.at(k).m_HUDElement == m_selectedPlayersHearts.back())
                                    {
                                        m_HUDElements.erase(m_HUDElements.begin() + k);
                                    }
                                }
                                viewPort->RemoveViewportWidgetContent(m_selectedPlayersHearts.back().ToSharedRef());
                                m_selectedPlayersHearts.pop_back();
                            }
                        }

                        //Add the healed hearts
                        for (int i = 0; i < MAX_HEARTS; i++)
                        {
                            FVector2D HeartPos;

                            HealthWidget = SNew(SHealthWidget).HeartImage(m_heartTemplate);
                            HealthWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
                            transform = FSlateRenderTransform(INITIAL_SCALE, FVector2D(INITIAL_POS.X + DISTANCE_BETWEEN_HEARTS * m_selectedPlayersHearts.size(), INITIAL_POS.Y));

                            HealthWidget->SetRenderTransform(transform);
                            m_selectedPlayersHearts.push_back(HealthWidget);

                            HUDElements element;
                            element.m_HUDElement = HealthWidget;
                            element.minMaxPos = FVector2D(HeartPos.Y, HeartPos.Y - 200);
                            m_HUDElements.push_back(element);

                            viewPort->AddViewportWidgetContent(HealthWidget.ToSharedRef());
                        }
                    }
                }
            }
        }
    }
}

void AQuetzalHUD::setEnemyHUDHealth()
{
    if (m_playerController)
    {
        //Get the screen size and aspectRatio
        UGameViewportClient* viewPort = GetWorld()->GetGameViewport();
        AGameStateBase* GState = GetWorld()->GetGameState<AGameStateBase>();

        FVector2D screenSize;
        viewPort->GetViewportSize(screenSize);
        float aspectRatio = float(screenSize.X) / float(screenSize.Y);

        TSharedPtr<SHealthWidget> HealthWidget;
        FSlateRenderTransform removalTransform;

        for (APlayerState* PState : GState->PlayerArray)
        {
            //If the state belongs to the owning player continue
            if (PState == m_playerController->GetPlayerState<ACharacterState>())
                continue;

            ACharacterState* characterState = static_cast<ACharacterState*>(PState);
            if (characterState)
            {
                int oldHealth = playerHealthArray[characterState->PlayerIndex];
                if (characterState->HP != oldHealth)
                {
                    playerHealthArray[characterState->PlayerIndex] = characterState->HP;
                    FString enemyIndex = FString::FromInt(characterState->PlayerIndex); //Could get from the player state instead 

                    if (m_playersAreDrawn[characterState->PlayerIndex])
                    {
                        int playerNum = characterState->PlayerIndex;

                        //If the player's hearts are empty continue
                        if (m_enemyHearts[enemyIndex].empty())
                        {
                            continue;
                        }

                        if (characterState->HP <= 0) //Set the enemy for redraw
                        {
                            APlayerState* PStatee = m_playerController->GetPlayerState<APlayerState>();
                            ACharacterState* characterStateee = Cast<ACharacterState>(PStatee);

                            if (characterStateee->PlayerIndex == 1)
                            {
                                if (playerStockArray[playerNum] != characterState->StockCPP)
                                {
                                    if (!m_enemyScore[enemyIndex].empty())
                                    {
                              /*          playerStockArray[playerNum] = characterState->Stock;
                                        viewPort->RemoveViewportWidgetContent(m_enemyScore[enemyIndex].back().ToSharedRef());
                                        m_enemyScore[enemyIndex].pop_back();*/
                                    }
                                }
                            }


                            m_playersAreDrawn[characterState->PlayerIndex] = false;
                        }

                        int health = characterState->HP;
                        int removalNum = 1;
                        //Choosing how many heart elements should be removed

                        if (m_enemyHearts[enemyIndex].size() == 3 && characterState->HP <= 4)
                        {
                            removalNum = 3;
                        }
                        else if (m_enemyHearts[enemyIndex].size() == 2 && characterState->HP <= 4 || m_enemyHearts[enemyIndex].size() == 3 && characterState->HP <= 8)
                        {
                            removalNum = 2;
                        }

                        for (int j = 0; j < removalNum; j++)
                        {
                            if (!m_enemyHearts[enemyIndex].empty()) //max hp
                            {
                                viewPort->RemoveViewportWidgetContent(m_enemyHearts[enemyIndex].back().ToSharedRef());

                                for (int k = 0; k < m_HUDElements.size(); k++)
                                {
                                    if (m_HUDElements.at(k).m_HUDElement == m_enemyHearts[enemyIndex].back())
                                    {
                                        m_HUDElements.erase(m_HUDElements.begin() + k);
                                    }
                                }
                                m_enemyHearts[enemyIndex].pop_back();
                            }
                        }

                        health %= 4;

                        if (health == 3)
                        {
                            HealthWidget = SNew(SHealthWidget).HeartImage(m_threeQuarterHeartTemplate);
                        }
                        else if (health == 2)
                        {
                            HealthWidget = SNew(SHealthWidget).HeartImage(m_halfHeartTemplate);
                        }
                        else if (health == 1)
                        {
                            HealthWidget = SNew(SHealthWidget).HeartImage(m_quarterHeartTemplate);
                        }
                        else if (health == 0 && characterState->HP > 0) //Prevent the heart from being added if the player is dead
                        {
                            HealthWidget = SNew(SHealthWidget).HeartImage(m_heartTemplate);
                        }

                        if (HealthWidget)
                        {
                            int i = m_enemyHearts[enemyIndex].size();
                            HealthWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
                            removalTransform = FSlateRenderTransform(INITIAL_ENEMY_HEART_SCALE,
                                FVector2D(INITIAL_ENEMY_HEART_POS.X + DISTANCE_BETWEEN_ENEMY_HEARTS * i + enemyNumber[playerNum] * 500, INITIAL_ENEMY_HEART_POS.Y));

                            HealthWidget->SetRenderTransform(removalTransform);
                            m_enemyHearts[enemyIndex].push_back(HealthWidget);

                            HUDElements element;
                            element.m_HUDElement = HealthWidget;
                            element.minMaxPos = FVector2D(INITIAL_ENEMY_HEART_POS.Y, INITIAL_ENEMY_HEART_POS.Y - 200);
                            m_HUDElements.push_back(element);

                            viewPort->AddViewportWidgetContent(HealthWidget.ToSharedRef());
                        }
                    }
                }
            }
        }
    }
}

void AQuetzalHUD::addPlayerScore()
{
    //To be used in Score mode
    UGameViewportClient* viewPort = GetWorld()->GetGameViewport();

    FVector2D screenSize;
    viewPort->GetViewportSize(screenSize);
    float aspectRatio = float(screenSize.X) / float(screenSize.Y);

    if (m_playerScore < 5)
    {
        TSharedPtr<class SScoreWidget> ScoreWidget;

        ScoreWidget = SNew(SScoreWidget).ScoreImage(m_stockTemplate);
        ScoreWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
        FSlateRenderTransform scoreTransform(INITIAL_SCORE_SCALE, FVector2D(0, 0));

        //This sets the score pos differently based on if the rest of the HUD is on screen or not
        if (m_onScreen)
            scoreTransform.SetTranslation(FVector2D(INITIAL_SCORE_POS.X + DISTANCE_BETWEEN_SCORE * m_playerScore, INITIAL_SCORE_POS.Y));
        else
            scoreTransform.SetTranslation(FVector2D(INITIAL_SCORE_POS.X + DISTANCE_BETWEEN_SCORE * m_playerScore, -100));

        ScoreWidget->SetRenderTransform(scoreTransform);
        m_selectedPlayersScore.push_back(ScoreWidget);

        HUDElements element;
        element.m_HUDElement = ScoreWidget;
        element.minMaxPos = FVector2D(INITIAL_SCORE_POS.Y, -100);
        m_HUDElements.push_back(element);

        viewPort->AddViewportWidgetContent(ScoreWidget.ToSharedRef());

        m_playerScore++;
    }
    //When the score is going from 5 to 6, remove the tally and start counting score with numbers
    else if (m_playerScore == 5)
    {
        //Remove 4 of the 5 bottle counters
        for (int i = 0; i < m_playerScore - 1; i++)
        {
            viewPort->RemoveViewportWidgetContent(m_selectedPlayersScore.back().ToSharedRef());
            m_selectedPlayersScore.pop_back();
        }

        FVector2D pos = m_selectedPlayersScore.front().ToSharedRef()->GetRenderTransform()->GetTranslation();

        //Set the initial text score
        FText scoreText;
        scoreText = (NSLOCTEXT("Example", "O", "6"));

        //Make a new text widget
        m_selectedPlayerScoreText = SNew(STextWidget).ScoreNum(scoreText);
        //Put the score number next to the bottle on screen
        FSlateRenderTransform scoreTransform(FScale2f(1 / aspectRatio, 0.5), FVector2D(INITIAL_SCORE_TEXT_POS.X, INITIAL_SCORE_TEXT_POS.Y));
        m_selectedPlayerScoreText->SetRenderTransform(scoreTransform);
        //Set the text of the score 
        m_selectedPlayerScoreText->setText(scoreText, m_font, 60);

        HUDElements element;
        element.m_HUDElement = m_selectedPlayerScoreText;
        element.minMaxPos = FVector2D(INITIAL_SCORE_TEXT_POS.Y, -150);
        m_HUDElements.push_back(element);

        viewPort->AddViewportWidgetContent(m_selectedPlayerScoreText.ToSharedRef());
        m_playerScore++;
    }
    else if (m_playerScore > 5)
    {
        m_playerScore++;

        //Set the text of the score widget
        FString scoreNum = FString::FromInt(m_playerScore);
        const TCHAR* Lnum = &scoreNum[0];
        FText scoreText;
        scoreText = (NSLOCTEXT("Example", "O", num));

        m_selectedPlayerScoreText->setText(scoreText, m_font, 60);
    }
}

void AQuetzalHUD::addEnemyScore()
{
    //To be used in Score mode
    UGameViewportClient* viewPort = GetWorld()->GetGameViewport();

    FVector2D screenSize;
    viewPort->GetViewportSize(screenSize);
    float aspectRatio = float(screenSize.X) / float(screenSize.Y);

    int numEnemies = m_numPlayers - 1;

    for (int i = 0; i < numEnemies; i++)
    {
        FString enemyIndex = FString::FromInt(i);
        int enemyScore = m_enemyPoints; //characterState->Score
        if (enemyScore < 5)
        {
            TSharedPtr<class SScoreWidget> ScoreWidget;

            ScoreWidget = SNew(SScoreWidget).ScoreImage(m_stockTemplate);
            ScoreWidget->SetContentScale(FVector2D(1 / aspectRatio, 1));
            FSlateRenderTransform scoreTransform(INITIAL_ENEMY_SCORE_SCALE, FVector2D(0, 0));

            //This sets the score pos differently based on if the rest of the HUD is on screen or not
            FSlateRenderTransform trans6(INITIAL_ENEMY_SCORE_SCALE, FVector2D(INITIAL_ENEMY_SCORE_POS.X + DISTANCE_BETWEEN_ENEMY_SCORE * 0, INITIAL_ENEMY_SCORE_POS.Y));

            if (m_onScreen)
                scoreTransform.SetTranslation(FVector2D(INITIAL_ENEMY_SCORE_POS.X + DISTANCE_BETWEEN_ENEMY_SCORE * m_enemyScore[enemyIndex].size() + i + enemyScore, INITIAL_ENEMY_SCORE_POS.Y));
            else
                scoreTransform.SetTranslation(FVector2D(INITIAL_ENEMY_SCORE_POS.X + DISTANCE_BETWEEN_ENEMY_SCORE * m_enemyScore[enemyIndex].size() + i + enemyScore, -100));

            ScoreWidget->SetRenderTransform(scoreTransform);
            m_enemyScore[enemyIndex].push_back(ScoreWidget);

            HUDElements element;
            element.m_HUDElement = ScoreWidget;
            element.minMaxPos = FVector2D(INITIAL_ENEMY_SCORE_POS.Y, INITIAL_SCORE_POS.Y - 100);
            m_HUDElements.push_back(element);

            viewPort->AddViewportWidgetContent(ScoreWidget.ToSharedRef());

            m_enemyPoints++;
        }
        //When the score is going from 5 to 6, remove the tally and start counting score with numbers
        else if (enemyScore == 5)
        {
            //Remove 4 of the 5 bottle counters
            for (int j = 0; j < enemyScore - 1; j++)
            {
                viewPort->RemoveViewportWidgetContent(m_enemyScore[enemyIndex].back().ToSharedRef());
                m_enemyScore[enemyIndex].pop_back();
            }

            FVector2D pos = m_enemyScore[enemyIndex].front().ToSharedRef()->GetRenderTransform()->GetTranslation();

            //Set the initial text score
            FText scoreText;
            scoreText = (NSLOCTEXT("Example", "O", "6"));

            TSharedPtr<STextWidget> enemyText;

            //Make a new text widget
            enemyText = SNew(STextWidget).ScoreNum(scoreText);
            //Put the score number next to the bottle on screen
            FSlateRenderTransform scoreTransform(FScale2f(1 / aspectRatio, 0.5), FVector2D(pos.X + 30, pos.Y + 10));
            enemyText->SetRenderTransform(scoreTransform);
            //Set the text of the score 
            enemyText->setText(scoreText, m_font, 45);

            HUDElements element;
            element.m_HUDElement = enemyText;
            element.minMaxPos = FVector2D(pos.Y + 10, pos.Y - 90);
            m_HUDElements.push_back(element);

            viewPort->AddViewportWidgetContent(enemyText.ToSharedRef());
            m_enemiesScoreText.Add(enemyIndex, enemyText);

            m_enemyPoints++;
        }
        else if (enemyScore > 5)
        {
            //Set the text of the score widget
            m_enemyPoints++;

            FString scoreNum = FString::FromInt(m_enemyPoints);
            const TCHAR* Lnum = &scoreNum[0];
            FText scoreText;
            scoreText = (NSLOCTEXT("Example", "O", num));

            m_enemiesScoreText[enemyIndex]->setText(scoreText, m_font, 45);

        }
    }
}