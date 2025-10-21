// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Melee/Melee.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AMelee::AMelee()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxComponent"));
	HitBox->SetupAttachment(GetRootComponent());
	TraceStartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Start"));
	TraceStartPoint->SetupAttachment(GetRootComponent());
	TraceEndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("End"));
	TraceEndPoint->SetupAttachment(GetRootComponent());

	CurrentComboCount = 1;
}

// Called when the game starts or when spawned
void AMelee::BeginPlay()
{
	Super::BeginPlay();

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AMelee::OnHitBoxOverlap);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &AMelee::OnHitBoxOverlapEnd);
}

void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMelee::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                            AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex,
                            bool bFromSweep,
                            const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMelee::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AMelee::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(OtherActor->GetName()));
	
	//world좌표
	const FVector Start = TraceStartPoint->GetComponentLocation();
	const FVector End = TraceEndPoint->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
	   this,
	   Start,
	   End,
	   FVector(10, 25, 10),
	   TraceStartPoint->GetComponentRotation(),
	   TraceTypeQuery1,
	   false,
	   ActorsToIgnore,
	   EDrawDebugTrace::ForDuration,
	   BoxHit,
	   true
	   );
}


void AMelee::OnHitBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	//todo hitbot boxtrace
}

void AMelee::Attack()
{
	if (bIsAttacking == true)
	{
		return;
	}
	if (OnAttackDelegate.IsBound())
	{
		OnAttackDelegate.Broadcast();
	}
	CurrentComboCount += 1;

	//todo hitbot boxtrace
}

void AMelee::AttackFinished()
{
	ResetComboCount();
}





int32 AMelee::GetCurrentComboCount()
{
	return CurrentComboCount;
}

float AMelee::GetAttackDelay()
{
	return AttackDelay;
}

void AMelee::ResetComboCount()
{
	CurrentComboCount = 1;
}

