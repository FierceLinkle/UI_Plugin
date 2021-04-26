INSTRUCTIONS FOR INSTALLING THE UI PLUGIN

-------------------------------------------------------------------------------------------------

How to make the HUD appear in the game world

1. Declare the HUD Widget Blueprint in the GameMode class 

(-Code-)
	static ConstructorHelpers::FClassFinder<UUserWidget> ExpBar(TEXT("/UIPlugin/ExpUI_Plugin"));
	HUDWidgetClass = ExpBar.Class;


	//Add exp bar UI to viewport
	if (HUDWidgetClass)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}

-------------------------------------------------------------------------------------------------

How to add the HUD classes to your character class

1. Include the class header files in the character class header file

(-Code-)
#include "ExpSystem.h"
#include "HealthSystem.h"
#include "ManaSystem.h"
#include "StaminaSystem.h"

2. Declare the class variables in the same class

(-Code-)
class UExpSystem;
class UHealthSystem;
class UManaSystem;
class UStaminaSystem;

3. Create variables of each class in the same class

(-Code-)
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UExpSystem* ExpComponent;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UHealthSystem* HealthComponent;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UManaSystem* ManaComponent;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UStaminaSystem* StaminaComponent;


4. Declare variables of these class types in the character cpp file


(-Code-)
	ExpComponent = CreateDefaultSubobject<UExpSystem>(FName("ExpComponent"));

	HealthComponent = CreateDefaultSubobject<UHealthSystem>(FName("HealthComponent"));

	ManaComponent = CreateDefaultSubobject<UManaSystem>(FName("ManaComponent"));

	StaminaComponent = CreateDefaultSubobject<UStaminaSystem>(FName("StaminaComponent"));

-------------------------------------------------------------------------------------------------



