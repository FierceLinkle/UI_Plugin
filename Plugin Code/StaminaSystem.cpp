// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaSystem.h"

// Sets default values for this component's properties
UStaminaSystem::UStaminaSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStaminaSystem::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UStaminaSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UStaminaSystem::GetStamina() {
	return StaminaPercentage;
}

FText UStaminaSystem::GetStaminaIntText() {
	//Decided not to use to avoid HUD clutter
	FString S = FString::FromInt(CurrentStamina);
	FString SHUD = FString(TEXT("Stamina: ")) + S;
	FText SText = FText::FromString(SHUD);
	return SText;
}

void UStaminaSystem::UpdateStamina(float StaminaChange) {
	PreviousCurrentStamina = CurrentStamina;
	CurrentStamina = CurrentStamina -= StaminaChange;
	StaminaPercentage = CurrentStamina / MaxStamina;
}

void UStaminaSystem::SaveGame() {
	SaveData(CurrentStamina, MaxStamina);
	hasSaved = true;
}

void UStaminaSystem::SaveData(float SCurrentStamina, float SMaxStamina) {
	StaminaProgressData[0] = SCurrentStamina;
	StaminaProgressData[1] = SMaxStamina;
}

void UStaminaSystem::LoadData() {
	if (hasSaved) {
		CurrentStamina = StaminaProgressData[0];
		MaxStamina = StaminaProgressData[1];
		StaminaPercentage = StaminaProgressData[0] / StaminaProgressData[1];
	}
}

