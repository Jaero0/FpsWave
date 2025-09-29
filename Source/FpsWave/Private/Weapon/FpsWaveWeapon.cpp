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
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ItemMesh->SetupAttachment(BoxComponent);
}

// Called when the game starts or when spawned
void AFpsWaveWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFpsWaveWeapon::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AFpsWaveWeapon::OnOverlapEnd);
}

// Called every frame
void AFpsWaveWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFpsWaveWeapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
										 AActor* OtherActor,
										 UPrimitiveComponent* OtherComp,
										 int32 OtherBodyIndex,
										 bool bFromSweep,
										 const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(this->GetName()));

	if (AFpsWaveCharacter* CurrentPlayer = Cast<AFpsWaveCharacter>(OtherActor))
	{
		CurrentPlayer->SetDetectedWeapon(this);
		CurrentPlayer->SetOverlapDetectedType(EOverlapDetected::EOD_Weapon);
	}
}

void AFpsWaveWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AFpsWaveCharacter* CurrentPlayer = Cast<AFpsWaveCharacter>(OtherActor))
	{
		CurrentPlayer->SetOverlapDetectedType(EOverlapDetected::EOD_None);
		CurrentPlayer->SetDetectedWeapon(nullptr);
	}
}

