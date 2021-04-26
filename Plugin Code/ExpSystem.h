// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExpSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UIPLUGIN_API UExpSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UExpSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float MaxExp = 100.0f; //Stores max exp for each level

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float CurrentExp = 0.0f; //Stores current exp gained for the current level

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float PreviousCurrentExp = 0.0f; //Store exp before current gain

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float ExpPercentage = 0.0f; //holds current exp as a percentage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		int CurrentLevel = 1; //stores current level

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		int SkillPoints = 0; //stores current skill points

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		int LevelCap = 100; //stores level limit

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float ExpGrowth = 1.2f; //stores value for level growth needed to level up

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float ExpOverflow = 0.0f; //stores overflow exp from leveling up

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float ExpLeft; //stores exp needed to level up

	float ExpProgressData[2]; //Stores current and max exp
	int LevelProgressData[2]; //Stores current level and skillpints 

	// Get Exp
	UFUNCTION(BlueprintPure, Category = "Exp")
		float GetExp();

	// Get Current Exp Text
	UFUNCTION(BlueprintPure, Category = "Exp")
		FText GetExpIntText();

	// Get Current Level Text
	UFUNCTION(BlueprintPure, Category = "Exp")
		FText GetLevelIntText();

	// Get Current Skill Points Text
	UFUNCTION(BlueprintPure, Category = "Exp")
		FText GetSkillPointsIntText();

	// Get Saved Exp Data Text
	UFUNCTION(BlueprintPure, Category = "Exp")
		FText GetSaveExpIntText();

	// Get Exp Left to Level Up Text
	UFUNCTION(BlueprintPure, Category = "Exp")
		FText GetExpLeftText();

	// Prints Level Up message Text
	UFUNCTION(BlueprintCallable, Category = "Exp")
		void PrintLevelUpMessage();

	FTimerHandle _loopTimerHandle;

	// Add Exp
	UFUNCTION(BlueprintCallable, Category = "Exp")
		void UpdateExp(float ExpChange);

	UFUNCTION(BlueprintCallable, Category = "Exp")
		void AddExp();

	UFUNCTION(BlueprintCallable, Category = "Exp")
		void BufferExp();

	// Level up
	UFUNCTION(BlueprintCallable, Category = "Exp")
		void LevelUp();

	UFUNCTION(BlueprintCallable, Category = "Exp")
		void LevelCapped();

	// Exp Save System
	UFUNCTION(BlueprintCallable, Category = "Exp")
		void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Exp")
		void SaveData(float SCurrentExp, float SMaxExp, int SCurrentLevel, int SSkillpoints);

	UFUNCTION(BlueprintCallable, Category = "Exp")
		void LoadData();

	bool hasSaved = false;

};
