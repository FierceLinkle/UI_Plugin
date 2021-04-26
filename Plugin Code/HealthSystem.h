// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UIPLUGIN_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float MaxHealth = 100.0f; //Stores value for full health

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth = 100.0f; //Stores current health

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float PreviousCurrentHealth = 100.0f; //Stores previous amount of health

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthPercentage = 100.0f; //Stores health as a percentage

	float HealthProgressData[2]; //Store current health progress as well as any health upgrades

	// Get Health
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealth();

	// Get Current Health Text
	UFUNCTION(BlueprintPure, Category = "Health")
		FText GetHealthIntText();

	// Add Health
	UFUNCTION(BlueprintCallable, Category = "Health")
		void UpdateHealth(float HealthChange);

	// Health Save System
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SaveData(float SCurrentHealth, float SMaxHealth);

	UFUNCTION(BlueprintCallable, Category = "Health")
		void LoadData();

	bool hasSaved = false;
};
