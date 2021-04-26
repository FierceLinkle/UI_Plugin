// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaSystem.h"

// Sets default values for this component's properties
UManaSystem::UManaSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UManaSystem::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UManaSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UManaSystem::GetMana() {
	return ManaPercentage;
}

FText UManaSystem::GetManaIntText() {
	FString M = FString::FromInt(CurrentMana);
	FString MMAX = FString::FromInt(MaxMana);
	FString MHUD = M + FString(TEXT(" / ")) + MMAX;
	FText MText = FText::FromString(MHUD);
	return MText;
}

void UManaSystem::UpdateMana(float ManaChange) {
	PreviousCurrentMana = CurrentMana;
	CurrentMana -= ManaChange;
	if (CurrentMana < 0.f) {
		CurrentMana = 0.0f;
	}
	else if (CurrentMana > MaxMana) {
		CurrentMana = MaxMana;
	}
	ManaPercentage = CurrentMana / MaxMana;
}

void UManaSystem::SaveGame() {
	SaveData(CurrentMana, MaxMana);
	hasSaved = true;
}

void UManaSystem::SaveData(float SCurrentMana, float SMaxMana) {
	ManaProgressData[0] = SCurrentMana;
	ManaProgressData[1] = SMaxMana;
}

void UManaSystem::LoadData() {
	if (hasSaved) {
		CurrentMana = ManaProgressData[0];
		MaxMana = ManaProgressData[1];
		ManaPercentage = ManaProgressData[0] / ManaProgressData[1];
	}
}

