// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UIPLUGIN_API UStaminaSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaminaSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float MaxStamina = 100.0f; //Stores value for full Stamina

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float CurrentStamina = 100.0f; //Stores current Stamina

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float PreviousCurrentStamina = 100.0f; //Stores previous amount of Stamina

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float StaminaPercentage = 100.0f; //Stores Stamina as a percentage

	float StaminaProgressData[2]; //Store current Stamina progress as well as any health upgrades

	// Get Stamina
	UFUNCTION(BlueprintPure, Category = "Stamina")
		float GetStamina();

	// Get Current Stamina Text
	UFUNCTION(BlueprintPure, Category = "Stamina")
		FText GetStaminaIntText();

	// Add Stamina
	UFUNCTION(BlueprintCallable, Category = "Stamina")
		void UpdateStamina(float StaminaChange);

	// Stamina Save System
	UFUNCTION(BlueprintCallable, Category = "Stamina")
		void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Mana")
		void SaveData(float SCurrentStamina, float SMaxStamina);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
		void LoadData();

	bool hasSaved = false;

		
};
