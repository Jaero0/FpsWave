// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/Gun.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"


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

	// 파티클 스폰
	if (GunFireParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(GunFireParticles,
			FirePoint, NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			true);
	}

	auto GameViewportClient = GetWorld()->GetGameViewport();
	FVector2D ViewPort;
	GameViewportClient->GetViewportSize(ViewPort);

	// 화면 중앙 좌표 (Crosshair)
	FVector2D CrosshairLocation(ViewPort.X / 2.f, ViewPort.Y / 2.f);
	CrosshairLocation.Y -= 50.f;

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 스크린 좌표 → 월드 좌표/방향 변환
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (!bScreenToWorld) return;

	// 카메라에서 쏘는 라인트레이스
	FVector Start = CrosshairWorldPosition;
	FVector End   = Start + CrosshairWorldDirection * 10000.f; // 사정거리

	FHitResult CrosshairHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	FVector BeamEndPoint = End; // 기본값 = 사정거리 끝점
	if (GetWorld()->LineTraceSingleByChannel(CrosshairHit, Start, End, ECC_Visibility, Params))
	{
		BeamEndPoint = CrosshairHit.ImpactPoint;
	}

	// 총구에서 다시 라인트레이스
	FHitResult WeaponTraceHit;
	FVector WeaponTraceStart = FirePoint->GetComponentLocation();
	FVector WeaponTraceEnd   = BeamEndPoint;

	if (GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility, Params))
	{
		BeamEndPoint = WeaponTraceHit.ImpactPoint;
	}

	// 파티클 스폰
	if (GunImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunImpactParticles, BeamEndPoint);
	}

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

float AGun::GetAttackDelay()
{
	return AttackDelay;
}

