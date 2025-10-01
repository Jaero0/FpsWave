// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Projectile/Bullet.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetGenerateOverlapEvents(true);
	SetRootComponent(CapsuleComponent);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ABullet::OnOverlapEnd);
}

void ABullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(OtherActor->GetName()));

	//todo 부딪힐때 탄환 박히는 소리 재생
	this->Destroy();
}

void ABullet::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TObjectPtr<class UCapsuleComponent> ABullet::GetCapsuleComponent()
{
	return CapsuleComponent;
}

