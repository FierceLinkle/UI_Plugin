// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ManaSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UIPLUGIN_API UManaSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UManaSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
		float MaxMana = 100.0f; //Stores value for full Mana

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
		float CurrentMana = 100.0f; //Stores current Mana

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
		float PreviousCurrentMana = 100.0f; //Stores previous amount of Mana

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
		float ManaPercentage = 100.0f; //Stores Mana as a percentage

	float ManaProgressData[2]; //Store current Mana progress as well as any health upgrades

	// Get Mana
	UFUNCTION(BlueprintPure, Category = "Mana")
		float GetMana();

	// Get Current Mana Text
	UFUNCTION(BlueprintPure, Category = "Mana")
		FText GetManaIntText();

	// Add Mana
	UFUNCTION(BlueprintCallable, Category = "Mana")
		void UpdateMana(float ManaChange);

	// Mana Save System
	UFUNCTION(BlueprintCallable, Category = "Mana")
		void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Mana")
		void SaveData(float SCurrentMana, float SMaxMana);

	UFUNCTION(BlueprintCallable, Category = "Mana")
		void LoadData();

	bool hasSaved = false;
};
