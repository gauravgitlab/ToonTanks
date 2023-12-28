// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ATank::ATank()
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);

    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
}


void ATank::HandleDestruction()
{
    Super::HandleDestruction();
    
    // hide the tank instead of destroying it,
    // so it will still have camera for visual
    SetActorHiddenInGame(true);

    // disable ticking
    SetActorTickEnabled(false);

    bAlive = false;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
    AController* controller = GetController();
    TankPlayerController = Cast<APlayerController>(controller);
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(TankPlayerController == nullptr)
        return;
    
    FHitResult hitResult;
    bool hit = TankPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, hitResult);
    if(hit)
    {
        //DrawDebugSphere(GetWorld(), hitResult.ImpactPoint, 25.f, 12, FColor::Red, false, -1.f);
        RotateTurret(hitResult.ImpactPoint);
    }
}

void ATank::Move(float Value)
{
    // get deltatime from Gameplay statics
    float deltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

    float move = Value * MoveSpeed * deltaTime;
    FVector deltaLocation = FVector(move, 0, 0);
    
    // AddActorLocalOffset is a method inside Actor class, to add delta location
    AddActorLocalOffset(deltaLocation, true);
}

void ATank::Turn(float Value)
{
    // get deltatime from Gameplay statics
    float deltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

    float rotation = Value * TurnRate * deltaTime;
    FRotator deltaRotation = FRotator(0, rotation, 0);

    AddActorLocalRotation(deltaRotation, true);
}