// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksGameMode.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AToonTanksGameMode::ActorDied(AActor* DeadActor)
{
    if(DeadActor == Tank)
    {
        Tank->HandleDestruction();
        if(ToonTanksPlayerController != nullptr)
        {
            ToonTanksPlayerController->SetPlayerEnabledState(false);
        }
        GameOver(false);
    }
    else if(ATower* destroyedTower = Cast<ATower>(DeadActor))
    {
        destroyedTower->HandleDestruction();

        --TargetTowers;
        if(TargetTowers <= 0)
            GameOver(true);
    }
}

// Called when the game starts or when spawned
void AToonTanksGameMode::BeginPlay()
{
	Super::BeginPlay();

    HandleGameStart();
}

void AToonTanksGameMode::HandleGameStart()
{
    // total towers, this is required to check if all tower destroyed to decide game is over or not
    TargetTowers = GetTargetTowerCount();

    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
    ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

    StartGame();

    // disable input for 3 seconds when start of the game
    // set a timer and enable the input after 3 seconds
    if(ToonTanksPlayerController != nullptr)
    {
        ToonTanksPlayerController->SetPlayerEnabledState(false);

        FTimerHandle playerEnableTimerHandle;
        FTimerDelegate playerEnableTimerDelegate = FTimerDelegate::CreateUObject(
            ToonTanksPlayerController,
            &AToonTanksPlayerController::SetPlayerEnabledState,
            true
        );
        GetWorldTimerManager().SetTimer(playerEnableTimerHandle, playerEnableTimerDelegate, StartDelay, false);
    }    
}

int32 AToonTanksGameMode::GetTargetTowerCount()
{
    TArray<AActor*> towers;
    UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), towers);
    return towers.Num();
}