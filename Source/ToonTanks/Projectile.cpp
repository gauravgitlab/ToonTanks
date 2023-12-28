// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// mesh of projectile - static mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	// projectile movement component
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComp->MaxSpeed = 1300.f;
	ProjectileMovementComp->InitialSpeed = 1300.f;

	// projectile trail component ( particle system component )
	TrailParticleSystemComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle System Component"));
	TrailParticleSystemComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// add event for hit
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	// play launch sound
	if(LaunchSound != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* myOwner = GetOwner();
	if(myOwner == nullptr)
	{
		Destroy();
		return;
	}

	AController* myOwnerInstigator = myOwner->GetInstigatorController();

	// if we want to get the `UClass` of any particular class, such as UDamageType, 
	// we can call a function called `StaticClass`
	UClass* damageTypeClass = UDamageType::StaticClass();

	if(OtherActor && OtherActor != this && OtherActor != myOwner)
	{
		// we will generate the damage event, and on take any damage delegate will broadcast in response to this
		// which means in our health component damage taken will be called...
		UGameplayStatics::ApplyDamage(OtherActor, Damage, myOwnerInstigator, this, damageTypeClass);

		// spawn hit particles
		if(HitParticles != nullptr)
			UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());

		// play hit sound
		if(HitSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());

		// shake the Camera on hit
		if(HitCameraShakeClass != nullptr)
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);

		Destroy();
	}

}

