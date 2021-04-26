// Fill out your copyright notice in the Description page of Project Settings.


#include "ExpSystem.h"

// Sets default values for this component's properties
UExpSystem::UExpSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UExpSystem::BeginPlay()
{
	Super::BeginPlay();

	//Initialise variables
	ExpLeft = MaxExp;	
}


// Called every frame
void UExpSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UExpSystem::GetExp() {
	return ExpPercentage;
}

FText UExpSystem::GetExpIntText() {
	//int32 XP = FMath::RoundHalfFromZero(ExpPercentage * 100);
	FString XPS = FString::FromInt(CurrentExp);
	FString FullXPS = FString::FromInt(MaxExp);
	FString XPHUD = XPS + FString(TEXT("/")) + FullXPS;
	FText XPText = FText::FromString(XPHUD);
	return XPText;
}

FText UExpSystem::GetLevelIntText() {
	FString LV = FString::FromInt(CurrentLevel);
	FString LVHUD = FString(TEXT("Level: ")) + LV;
	FText LVText = FText::FromString(LVHUD);
	return LVText;
}

FText UExpSystem::GetSkillPointsIntText() {
	FString SP = FString::FromInt(SkillPoints);
	FString SPHUD = FString(TEXT("Skill Points: ")) + SP;
	FText SPText = FText::FromString(SPHUD);
	return SPText;
}

FText UExpSystem::GetSaveExpIntText() {
	FString CXP = FString::FromInt(ExpProgressData[0]);
	FString MAXXP = FString::FromInt(ExpProgressData[1]);
	FString CL = FString::FromInt(LevelProgressData[0]);
	FString SP = FString::FromInt(LevelProgressData[1]);
	//FString SDHUD = CXP + FString(TEXT(", ")) + MAXXP + FString(TEXT(", ")) + CL + FString(TEXT(", ")) + SP;
	FString SDHUD = FString(TEXT("Level: ")) + CL + FString(TEXT(" SkillPoints ")) + SP;
	FText SDText = FText::FromString(SDHUD);
	return SDText;
}


FText UExpSystem::GetExpLeftText() {
	FString EL = FString::FromInt(ExpLeft);
	FString ELHUD = FString(TEXT("Exp left to level up: ")) + EL;
	FText ELText = FText::FromString(ELHUD);
	return ELText;
}


void UExpSystem::PrintLevelUpMessage() {
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("Level Up!"));
}

// Adds Exp and calculates leftover exp for next level //

void UExpSystem::UpdateExp(float ExpChange) {
	if (CurrentLevel < LevelCap) {
		PreviousCurrentExp = CurrentExp;
		CurrentExp += ExpChange;
		ExpLeft = MaxExp - CurrentExp;
		//CurrentExp = FMath::Clamp(CurrentExp += ExpChange, 0.0f, MaxExp);
		ExpPercentage = CurrentExp / MaxExp;
	}
}

void UExpSystem::AddExp() {
	UpdateExp(20.0f);
}

void UExpSystem::BufferExp() {
	if (CurrentExp != MaxExp) {
		ExpOverflow = CurrentExp - MaxExp;
		CurrentExp = 0.0f + ExpOverflow;
		CurrentExp = round(CurrentExp);
		ExpOverflow = 0.0f;

	}
	else {
		CurrentExp = 0.0f;
	}
	ExpLeft = (MaxExp * ExpGrowth) - CurrentExp;
}

// Handles leveling up //

void UExpSystem::LevelUp() {

	CurrentLevel++;
	SkillPoints++;

	BufferExp();

	MaxExp *= ExpGrowth;
	ExpPercentage = CurrentExp / MaxExp;

	GetWorld()->GetTimerManager().SetTimer(_loopTimerHandle, this, &UExpSystem::PrintLevelUpMessage, 1.f, false, 0.f);
}

void UExpSystem::LevelCapped() {
	MaxExp = CurrentExp;
	ExpOverflow = CurrentExp;
}

// These functions store save data and load them back //

void UExpSystem::SaveGame() {
	SaveData(CurrentExp, MaxExp, CurrentLevel, SkillPoints);
	hasSaved = true;
}

void UExpSystem::SaveData(float SCurrentExp, float SMaxExp, int SCurrentLevel, int SSkillpoints) {
	ExpProgressData[0] = SCurrentExp;
	ExpProgressData[1] = SMaxExp;
	LevelProgressData[0] = SCurrentLevel;
	LevelProgressData[1] = SSkillpoints;
}

void UExpSystem::LoadData() {
	if (hasSaved) {
		CurrentExp = ExpProgressData[0];
		MaxExp = ExpProgressData[1];
		CurrentLevel = LevelProgressData[0];
		SkillPoints = LevelProgressData[1];

		ExpPercentage = ExpProgressData[0] / ExpProgressData[1];
		ExpLeft = MaxExp - CurrentExp;
	}
}


