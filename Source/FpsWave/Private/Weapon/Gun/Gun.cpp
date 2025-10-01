// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/Gun.h"

#include "Components/CapsuleComponent.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "Weapon/Projectile/Bullet.h"


// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(GetItemMesh());
	AttackDelay = 0.1f;
	ReloadSpeed = 2.f;
	MaxBulletCount = 30;
	CurrentBulletCount = MaxBulletCount;
	
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGun::Attack()
{
	// 이미 발사 중이면 리턴 (중복 방지)
	if (bIsFiring)
	{
		return;
	}
    
	// 자동 발사 시작
	StartAutoFire();
}

void AGun::StartAutoFire()
{
	bIsFiring = true;
    
	// 첫 번째 총알 즉시 발사 (지연 없음)
	FireSingleBullet();
    
	// 이후 FireRate 간격으로 연속 발사 (루프 설정)
	GetWorld()->GetTimerManager().SetTimer(
		AttackTimer,
		this,
		&AGun::FireSingleBullet,
		AttackDelay,
		true  // 중요: true = 무한 반복!
	);
}

void AGun::FireSingleBullet()
{
	//todo 총알발사소리, 총알 발사 지점 scenecomponent로 변경하기, projectile movement 사용

	auto GameViewportClient = GetWorld()->GetGameViewport();
	FVector2d ViewPort;
	GameViewportClient->GetViewportSize(ViewPort);
	FVector2D Crosshair;
}


void AGun::AttackFinished()
{
	Super::AttackFinished();

	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

