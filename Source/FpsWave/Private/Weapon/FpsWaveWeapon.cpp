// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/FpsWaveWeapon.h"

#include "Characters/FpsWaveCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AFpsWaveWeapon::AFpsWaveWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFpsWaveWeapon::OnCapsuleOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AFpsWaveWeapon::OnCapsuleOverlapEnd);
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ItemMesh->SetupAttachment(BoxComponent);
}

// Called when the game starts or when spawned
void AFpsWaveWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFpsWaveWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFpsWaveWeapon::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent,
										 AActor* OtherActor,
										 UPrimitiveComponent* OtherComp,
										 int32 OtherBodyIndex,
										 bool bFromSweep,
										 const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(OtherActor->GetActorLocation().ToString()));

	if (AFpsWaveCharacter* CurrentPlayer = Cast<AFpsWaveCharacter>(OtherActor))
	{
		CurrentPlayer->SetDetectedWeapon(this);
		CurrentPlayer->SetOverlapDetectedType(EOverlapDetected::EOD_Weapon);
	}
}

void AFpsWaveWeapon::OnCapsuleOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AFpsWaveCharacter* CurrentPlayer = Cast<AFpsWaveCharacter>(OtherActor))
	{
		Player->SetOverlapDetectedType(EOverlapDetected::EOD_Weapon);
		Player->SetDetectedWeapon(nullptr);
	}
}

