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
	MaxAccuracy = 100.f;
	MinAccuracy = 20.f;
	CurrentAccuracy = 60.f;
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
		float RandomScale = FMath::FRandRange(1.f, 1.4f);
		float SpreadAngleDeg = FMath::Lerp(MaxSpreadAngle, MinSpreadAngle, NormalizedAccuracy) * RandomScale;
		float SpreadAngleRad = FMath::DegreesToRadians(SpreadAngleDeg);

		// === 1️⃣ 카메라 기준 AimPoint 계산 ===
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * 10000.f;

		FHitResult CrosshairHit;
		FVector AimPoint = End;
		if (GetWorld()->LineTraceSingleByChannel(CrosshairHit, Start, End, ECC_Visibility, Params))
		{
			AimPoint = CrosshairHit.ImpactPoint;
		}

		// === 2️⃣ 총구 기준 방향 계산 ===
		FVector MuzzleLocation = FirePoint->GetComponentLocation();
		FVector MuzzleToAim = AimPoint - MuzzleLocation;
		float DistToAim = MuzzleToAim.Size();
		FVector MuzzleToAimDir = MuzzleToAim.GetSafeNormal();

		FVector BeamEndPoint = AimPoint;

		// === 3️⃣ 퍼짐 시작 전 구간 ===
		if (DistToAim <= SpreadStartDistance)
		{
			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, AimPoint, ECC_Visibility, Params))
			{
				BeamEndPoint = Hit.ImpactPoint;
			}
			else
			{
				BeamEndPoint = AimPoint;
			}
		}
		else
		{
			// === 4️⃣ 퍼짐 시작 이후 ===
			FVector SpreadOrigin = MuzzleLocation + MuzzleToAimDir * SpreadStartDistance;

			// 퍼짐 전 구간 충돌 체크
			FHitResult PreSpreadHit;
			if (GetWorld()->LineTraceSingleByChannel(PreSpreadHit, MuzzleLocation, SpreadOrigin, ECC_Visibility, Params))
			{
				BeamEndPoint = PreSpreadHit.ImpactPoint;
			}
			else
			{
				// 🔥 수정: SpreadOrigin에서 실제 퍼짐 적용
				FVector SpreadOriginToAim = (AimPoint - SpreadOrigin).GetSafeNormal();
    
				// SpreadOrigin부터 남은 거리 계산
				float RemainingDistance = (AimPoint - SpreadOrigin).Size();
				RemainingDistance = FMath::Min(RemainingDistance, 10000.f - SpreadStartDistance);
    
				// 해당 거리에서의 퍼짐 반경 계산
				float SpreadRadius = RemainingDistance * FMath::Tan(SpreadAngleRad);
    
				// 랜덤 오프셋 생성
				FVector RightVector = FVector::CrossProduct(SpreadOriginToAim, FVector::UpVector).GetSafeNormal();
				if (RightVector.IsNearlyZero())
				{
					RightVector = FVector::CrossProduct(SpreadOriginToAim, FVector::ForwardVector).GetSafeNormal();
				}
				FVector UpVector = FVector::CrossProduct(RightVector, SpreadOriginToAim).GetSafeNormal();
    
				float RandomAngle = FMath::FRandRange(0.f, 2.f * PI);
				float RandomRadius = FMath::Sqrt(FMath::FRand()) * SpreadRadius;
    
				FVector RandomOffset = (RightVector * FMath::Cos(RandomAngle) + UpVector * FMath::Sin(RandomAngle)) * RandomRadius;
    
				// 추가 랜덤성
				RandomOffset += FMath::VRand() * SpreadRadius * 0.3f;
    
				// SpreadOrigin에서 남은 거리만큼 나아간 지점 + 랜덤 오프셋
				FVector SpreadTarget = SpreadOrigin + SpreadOriginToAim * RemainingDistance + RandomOffset;
    
				FHitResult AfterSpreadHit;
				if (GetWorld()->LineTraceSingleByChannel(AfterSpreadHit, SpreadOrigin, SpreadTarget, ECC_Visibility, Params))
				{
					BeamEndPoint = AfterSpreadHit.ImpactPoint;
				}
				else
				{
					BeamEndPoint = SpreadTarget;
				}
			}
		}

		// === 5️⃣ 이펙트 스폰 ===
		if (GunImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunImpactParticles, BeamEndPoint);
		}

		// (선택) 디버그용
		DrawDebugLine(GetWorld(), MuzzleLocation, BeamEndPoint, FColor::Red, false, 1.f, 0, 0.5f);
	}
}

void AShotgun::AttackFinished()
{
	Super::AttackFinished();
	CurrentAccuracy = 60.f;
}

void AShotgun::Recoil()
{
	if (!PlayerController)
	{
		if (auto Cont = Cast<AFpsWaveCharacterController>(GetWorld()->GetFirstPlayerController()))
		{
			PlayerController = Cont;      
		}
	}

	float Pitch = FMath::FRandRange(4.0f, 6.f);
	float Yaw = FMath::FRandRange(-3.0f, 3.0f);

	// 목표 반동 설정
	TargetRecoilRotation = FRotator(-Pitch, Yaw, 0.f);
	CurrentRecoilRotation = FRotator::ZeroRotator;
	RecoilProgress = 0.f;
	bIsRecoiling = true;
}

float AShotgun::GetAttackDelay()
{
	return AttackDelay;
}

void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRecoiling)
	{
		// 진행도 업데이트
		RecoilProgress += DeltaTime / RecoilDuration;
		RecoilProgress = FMath::Clamp(RecoilProgress, 0.f, 1.f);
        
		// 부드러운 보간 (커브 적용)
		float Alpha = FMath::InterpEaseOut(0.f, 1.f, RecoilProgress, 2.0f);
		FRotator NewRecoil = FMath::Lerp(FRotator::ZeroRotator, TargetRecoilRotation, Alpha);
        
		// 이전 프레임과의 차이만 적용
		FRotator DeltaRecoil = NewRecoil - CurrentRecoilRotation;
        
		PlayerController->AddPitchInput(DeltaRecoil.Pitch);
		PlayerController->AddYawInput(DeltaRecoil.Yaw);
        
		CurrentRecoilRotation = NewRecoil;
        
		// 반동 완료
		if (RecoilProgress >= 1.f)
		{
			bIsRecoiling = false;
		}
	}
}