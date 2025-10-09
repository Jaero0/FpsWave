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

	AttackDelay = 0.1f;
	ReloadSpeed = 2.f;
	MaxBulletCount = 30;
	CurrentBulletCount = MaxBulletCount;
	MaxAccuracy = 100.f;
	MinAccuracy = 80;
	DefaultAccuracy = 100.f;
	CurrentAccuracy = MaxAccuracy;
	MaxSpreadAngle = 20.f;
	MinSpreadAngle = 0.5f;
	AccuracyDecreasePerShot = (MaxAccuracy - MinAccuracy) / (MaxBulletCount);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	if (auto Cont = GetWorld()->GetFirstPlayerController<AFpsWaveCharacterController>())
	{
		PlayerController = Cont;
	}
}

void AGun::Attack()
{
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




	
	// 랜덤 탄착군 적용 (원뿔 형태로 퍼짐)
	CurrentAccuracy = FMath::Max(CurrentAccuracy - AccuracyDecreasePerShot, MinAccuracy);

	// MinAccuracy~MaxAccuracy 범위를 0~1로 정규화
	float NormalizedAccuracy = (CurrentAccuracy - MinAccuracy) / (MaxAccuracy - MinAccuracy);
	
	// 정확도가 낮을수록 큰 각도
	float SpreadAngle = FMath::Lerp(MaxSpreadAngle, MinSpreadAngle, NormalizedAccuracy);

	// 랜덤 방향으로 각도 오프셋 추가
	FVector SpreadDirection = FMath::VRandCone(
		CrosshairWorldDirection,
		FMath::DegreesToRadians(SpreadAngle));

	// 카메라에서 쏘는 라인트레이스
	FVector Start = CrosshairWorldPosition;
	FVector End = Start + (SpreadDirection * 10000.f);





	
	
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

	CurrentBulletCount--;
}

void AGun::AttackFinished()
{
	Super::AttackFinished();

	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
}

void AGun::RecoverAccuracy(float DeltaTime)
{
	// Accuracy를 천천히 MaxAccuracy로 회복
	CurrentAccuracy = FMath::FInterpTo(CurrentAccuracy, MaxAccuracy, DeltaTime, PlayerController->GetCrosshairObj()->GetAimDecreaseSpeed());

	// 거의 회복됐으면 타이머 정지
	if (FMath::IsNearlyEqual(CurrentAccuracy, MaxAccuracy, 0.1f))
	{
		CurrentAccuracy = MaxAccuracy;
	}
}

void AGun::Recoil()
{
	if (!PlayerController)
	{
		if (auto Cont = Cast<AFpsWaveCharacterController>(GetWorld()->GetFirstPlayerController()))
		{
			PlayerController = Cont;      
		}
	}

	float Pitch = FMath::FRandRange(4.0f, 6.f) * RecoilPitchMultiplier;
	float Yaw = FMath::FRandRange(-3.0f, 3.0f) * RecoilYawMultiplier;

	// 목표 반동 설정
	TargetRecoilRotation = FRotator(-Pitch, Yaw, 0.f);
	CurrentRecoilRotation = FRotator::ZeroRotator;
	RecoilProgress = 0.f;
	bIsRecoiling = true;
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFiring == true)
	{
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
	

	if (FMath::IsNearlyEqual(CurrentAccuracy, MaxAccuracy, 0.1f))
	{
		return;
	}

	RecoverAccuracy(DeltaTime);
	
}

float AGun::GetAttackDelay()
{
	return AttackDelay;
}

int32 AGun::GetMaxBulletCount()
{
	return MaxBulletCount;
}

void AGun::ResetCurrentBulletCountToMax()
{
	CurrentBulletCount = MaxBulletCount;
}

