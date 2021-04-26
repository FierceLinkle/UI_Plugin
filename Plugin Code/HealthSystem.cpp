// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthSystem.h"

// Sets default values for this component's properties
UHealthSystem::UHealthSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHealthSystem::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UHealthSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UHealthSystem::GetHealth() {
	return HealthPercentage;
}

FText UHealthSystem::GetHealthIntText() {
	FString H = FString::FromInt(CurrentHealth);
	FString HMAX = FString::FromInt(MaxHealth);
	FString HHUD = H + FString(TEXT(" / ")) + HMAX;
	FText HText = FText::FromString(HHUD);
	return HText;	
}

void UHealthSystem::UpdateHealth(float HealthChange) {
	PreviousCurrentHealth = CurrentHealth;
	CurrentHealth -= HealthChange;
	if (CurrentHealth <= 0.f) {
		CurrentHealth = 0.0f;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("GAME OVER"));
	}
	else if(CurrentHealth > MaxHealth) {
		CurrentHealth = MaxHealth;
	}
	HealthPercentage = CurrentHealth / MaxHealth;
}

void UHealthSystem::SaveGame() {
	SaveData(CurrentHealth, MaxHealth);
	hasSaved = true;
}

void UHealthSystem::SaveData(float SCurrentHealth, float SMaxHealth) {
	HealthProgressData[0] = SCurrentHealth;
	HealthProgressData[1] = SMaxHealth;
}

void UHealthSystem::LoadData() {
	if (hasSaved) {
		CurrentHealth = HealthProgressData[0];
		MaxHealth = HealthProgressData[1];
		HealthPercentage = HealthProgressData[0] / HealthProgressData[1];
	}
}
