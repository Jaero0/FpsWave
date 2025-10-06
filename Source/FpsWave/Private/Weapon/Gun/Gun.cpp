// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/Gun.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "HUD/Crosshair.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(GetItemMesh());
	MaxAccuracy = 100.f;
	AccuracyDecreasePerShot = (MaxAccuracy - MinAccuracy) / (MaxBulletCount - 8);
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
	GetWorld()->GetTimerManager().ClearTimer(AccuracyRecoveryTimer);
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
	//todo Accuracy 조정
	

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

	if (OnTriggerMontage.IsBound())
	{
		OnTriggerMontage.Broadcast();
	}
	
	FVector2D ViewPort;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewPort);

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

	//------------------------------------------------------
	// 🔫 Accuracy 기반 탄퍼짐 추가
	//------------------------------------------------------
	float MaxSpread = 2.f; // 최대 퍼짐 각도(도 단위)
	float ClampedAccuracy = FMath::Clamp(CurrentAccuracy, MinAccuracy, MaxAccuracy); // 60~100으로 제한
	float SpreadAngle = FMath::Lerp(0.f, MaxSpread, (MaxAccuracy - ClampedAccuracy) / 40.f);

	// Crosshair 방향 벡터를 중심으로 랜덤한 퍼짐 벡터 생성
	FVector RandomShootDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(
		CrosshairWorldDirection, SpreadAngle
	);
	//------------------------------------------------------

	// 카메라에서 쏘는 라인트레이스
	FVector Start = CrosshairWorldPosition;
	FVector End   = Start + RandomShootDirection * 10000.f; // 수정된 방향 사용

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
	// 🎯 기존 Accuracy 회복 타이머가 있다면 먼저 정리
	GetWorld()->GetTimerManager().ClearTimer(AccuracyRecoveryTimer);

	// 🎯 Accuracy 회복 시작
	GetWorld()->GetTimerManager().SetTimer(
		AccuracyRecoveryTimer,
		this,
		&AGun::RecoverAccuracy,
		0.016f,   // 60fps 기준 Tick 느낌
		true      // 반복 실행
	);
}

void AGun::RecoverAccuracy()
{
	// Accuracy를 천천히 MaxAccuracy로 회복
	//todo Crosshair interpspeed에 맞춰서 돌아오기
	CurrentAccuracy = FMath::FInterpTo(CurrentAccuracy, MaxAccuracy, 0.016f, 5.f);

	// 거의 회복됐으면 타이머 정지
	if (FMath::IsNearlyEqual(CurrentAccuracy, MaxAccuracy, 0.1f))
	{
		CurrentAccuracy = MaxAccuracy;
		GetWorld()->GetTimerManager().ClearTimer(AccuracyRecoveryTimer);
	}
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

