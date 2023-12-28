// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Projectile.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CreateDefaultSubobject is an Unreal generic method to create Constructor,
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));

	// assign Capsule Component as root component
	RootComponent = CapsuleComp;

	// create and construct UStaticMeshComponent named BaseMesh using CreateDefaultSubobject,
	// and make child of Capsule Component...
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	// similar approach for TurretMesh like we did for BaseMesh but make child of BaseMesh instead of Capsule Component
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

void ABasePawn::HandleDestruction()
{
	// play particle when destroyed
	if(DeathParticles != nullptr)
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());

	// play death sound
	if(DeathSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());	

	// shake the camera on death
	if(DeathCameraShakeClass != nullptr)
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
}

void ABasePawn::RotateTurret(FVector LookAtTarget)
{
	FVector toTarget = LookAtTarget - TurretMesh->GetComponentLocation();
	FRotator lookAtRotation = FRotator(0.f, toTarget.Rotation().Yaw, 0.f);

	float deltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

	TurretMesh->SetWorldRotation(FMath::RInterpTo(TurretMesh->GetComponentRotation(), 
										lookAtRotation, deltaTime, 25.f));
}

void ABasePawn::Fire()
{
	// create object at runtime- use spawnActor
	DrawDebugSphere(GetWorld(), ProjectileSpawnPoint->GetComponentLocation(), 10.f, 12, FColor::Blue, false, 1.f);
	AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, 
						ProjectileSpawnPoint->GetComponentLocation(), 
						ProjectileSpawnPoint->GetComponentRotation());

	projectile->SetOwner(this);
}