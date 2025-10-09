// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Gun/Shotgun.h"

#include "NiagaraFunctionLibrary.h"
#include "Controllers/FpsWaveCharacterController.h"
#include "HUD/Crosshair.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackDelay = 0.8f;
	ReloadSpeed = 3.f;
	MaxBulletCount = 8;
	CurrentBulletCount = MaxBulletCount;
	MaxAccuracy = 100.f;
	MinAccuracy = 20.f;
	DefaultAccuracy = 60.f;
	CurrentAccuracy = DefaultAccuracy;
	MaxSpreadAngle = 5.f;
	MinSpreadAngle = 3.f;
	AccuracyDecreasePerShot = (MaxAccuracy - MinAccuracy) / MaxBulletCount;
	BulletPerShot = 8;
	SpreadStartDistance = 100.f;
}

// Called when the game starts or when spawned
void AShotgun::BeginPlay()
{
	Super::BeginPlay();
}

void AShotgun::Attack()
{
	Super::Attack();
}

void AShotgun::StartAutoFire()
{
	Super::StartAutoFire();
}

void AShotgun::FireSingleBullet()
{
	if (CurrentBulletCount <= 0)
	{
		return;
	}
	
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

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, this->GetActorLocation());
	}

	if (OnTriggerMontage.IsBound())
	{
		OnTriggerMontage.Broadcast();
	}

	if (!PlayerController)
	{
		if (auto Cont = GetWorld()->GetFirstPlayerController<AFpsWaveCharacterController>())
		{
			PlayerController = Cont;
		}
	}

	//필수
	Recoil();

	FVector2D WidgetScreenPosition = FVector2d::ZeroVector;
	if (TObjectPtr<UCrosshair> Crosshair = PlayerController->GetCrosshairObj())
	{
		WidgetScreenPosition = Crosshair->GetAimLocation();
	}

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 스크린 좌표 → 월드 좌표/방향 변환
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,
		WidgetScreenPosition,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (!bScreenToWorld) return;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());


	for (int i = 0; i < BulletPerShot; ++i)
	{
		CurrentAccuracy = FMath::Max(CurrentAccuracy - AccuracyDecreasePerShot, MinAccuracy);
		float NormalizedAccuracy = (CurrentAccuracy - MinAccuracy) / (MaxAccuracy - MinAccuracy);
		float RandomScale = FMath::FRandRange(1.f, 1.6f);
		float SpreadAngleDeg = FMath::Lerp(MaxSpreadAngle, MinSpreadAngle, NormalizedAccuracy) * RandomScale;
		float SpreadAngleRad = FMath::DegreesToRadians(SpreadAngleDeg);

		// 1. 카메라 기준 AimPoint 계산
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * 10000.f;

		FHitResult CrosshairHit;
		FVector AimPoint = End;
		if (GetWorld()->LineTraceSingleByChannel(CrosshairHit, Start, End, ECC_Visibility, Params))
		{
			AimPoint = CrosshairHit.ImpactPoint;
		}

		// 2. 총구 기준 방향 계산
		FVector MuzzleLocation = FirePoint->GetComponentLocation();
		FVector MuzzleToAim = AimPoint - MuzzleLocation;
		FVector MuzzleToAimDir = MuzzleToAim.GetSafeNormal();

		FVector BeamEndPoint = AimPoint;

		// 3. 탄퍼짐
		FVector SpreadOrigin = MuzzleLocation + MuzzleToAimDir * SpreadStartDistance;

		// SpreadOrigin부터 남은 거리 계산
		float RemainingDistance = (AimPoint - SpreadOrigin).Size();
		RemainingDistance = FMath::Min(RemainingDistance, 10000.f - SpreadStartDistance);

		// 해당 거리에서의 퍼짐 반경 계산
		float SpreadRadius = RemainingDistance * FMath::Tan(SpreadAngleRad);

		// 4. 랜덤 오프셋 생성
		FVector RightVector = FVector::CrossProduct(MuzzleToAimDir, FVector::UpVector).GetSafeNormal();
		if (RightVector.IsNearlyZero())
		{
			RightVector = FVector::CrossProduct(MuzzleToAimDir, FVector::ForwardVector).GetSafeNormal();
		}
		FVector UpVector = FVector::CrossProduct(RightVector, MuzzleToAimDir).GetSafeNormal();

		float RandomAngle = FMath::FRandRange(0.f, 2.f * PI);
		float RandomRadius = FMath::Sqrt(FMath::FRand()) * SpreadRadius;

		FVector RandomOffset = (RightVector * FMath::Cos(RandomAngle) + UpVector * FMath::Sin(RandomAngle)) *
			RandomRadius;

		// 5. SpreadOrigin에서 남은 거리만큼 나아간 지점 + 랜덤 오프셋
		FVector SpreadTarget = SpreadOrigin + MuzzleToAimDir * RemainingDistance + RandomOffset;

		FHitResult AfterSpreadHit;
		if (GetWorld()->
			LineTraceSingleByChannel(AfterSpreadHit, SpreadOrigin, SpreadTarget, ECC_Visibility, Params))
		{
			BeamEndPoint = AfterSpreadHit.ImpactPoint;
		}
		else
		{
			BeamEndPoint = SpreadTarget;
		}
		
		

		// === 5️⃣ 이펙트 스폰 ===
		if (GunImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunImpactParticles, BeamEndPoint);
		}

		// (선택) 디버그용
		//DrawDebugLine(GetWorld(), MuzzleLocation, BeamEndPoint, FColor::Red, false, 1.f, 0, 0.5f);
	}
	
	CurrentBulletCount--;
}

void AShotgun::AttackFinished()
{
	Super::AttackFinished();
	CurrentAccuracy = DefaultAccuracy;
}

void AShotgun::Recoil()
{
	Super::Recoil();
}


float AShotgun::GetAttackDelay()
{
	return AttackDelay;
}

void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}