// Copyright Epic Games, Inc. All Rights Reserved.

#include "TAM_CWCharacter.h"
#include "TAM_CWProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include <cmath>
#include "Blueprint/UserWidget.h"
#include "TAM_CWGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ATAM_CWCharacter

ATAM_CWCharacter::ATAM_CWCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> PauseBar(TEXT("/Game/CW1_Content/PauseUI"));
	PauseWidgetClass = PauseBar.Class;

	ExpComponent = CreateDefaultSubobject<UExpSystem>(FName("ExpComponent"));

	HealthComponent = CreateDefaultSubobject<UHealthSystem>(FName("HealthComponent"));

	ManaComponent = CreateDefaultSubobject<UManaSystem>(FName("ManaComponent"));

	StaminaComponent = CreateDefaultSubobject<UStaminaSystem>(FName("StaminaComponent"));
}

void ATAM_CWCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

void ATAM_CWCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Checks for correct condition to level up
	if ((ExpComponent->CurrentExp >= ExpComponent->MaxExp) && (ExpComponent->CurrentLevel < ExpComponent->LevelCap)) {
		ExpComponent->LevelUp();
	}
	
	//Checks if level cap has been reached
	if (ExpComponent->CurrentLevel >= ExpComponent->LevelCap) {
		ExpComponent->LevelCapped();
	}

	//Checks if stamina bar should be decreasing or increasing according to players movement
	if (isRunning) {
		if (StaminaComponent->CurrentStamina > 0.0f) {
			StaminaComponent->UpdateStamina(DecraseSpeed);
		}
	}
	else {
		if (StaminaComponent->CurrentStamina < 100.0f) {
			StaminaComponent->UpdateStamina(IncreaseSpeed);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATAM_CWCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATAM_CWCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATAM_CWCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ATAM_CWCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATAM_CWCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATAM_CWCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATAM_CWCharacter::LookUpAtRate);

	//Debug actions
	PlayerInputComponent->BindAction("AddExp", IE_Pressed, this, &ATAM_CWCharacter::AddExp);
	//PlayerInputComponent->BindAction("AddExp", IE_Pressed, this, &UExpSystem::AddExp);
	PlayerInputComponent->BindAction("DamageSelf", IE_Pressed, this, &ATAM_CWCharacter::DamageSelf);

	//Save and load exp data
	PlayerInputComponent->BindAction("SaveExp", IE_Pressed, this, &ATAM_CWCharacter::SaveGame);
	PlayerInputComponent->BindAction("LoadExp", IE_Pressed, this, &ATAM_CWCharacter::LoadData);

	//Open pause menu
	PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &ATAM_CWCharacter::OpenPauseMenu);

	//Bind run events
	PlayerInputComponent->BindAction("Run", IE_Repeat, this, &ATAM_CWCharacter::Sprint);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ATAM_CWCharacter::SprintOff);
}

void ATAM_CWCharacter::OnFire()
{
	// try and fire a projectile
	if ((ProjectileClass) && (ManaComponent->CurrentMana > 0))
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ATAM_CWProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ATAM_CWProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}

		// try and play the sound if specified
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		ManaComponent->UpdateMana(20.0f);
	}
}

void ATAM_CWCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATAM_CWCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ATAM_CWCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (!TouchItem.bIsPressed)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ATAM_CWCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ATAM_CWCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}

}

void ATAM_CWCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ATAM_CWCharacter::Sprint() {
	isRunning = true;
	if (StaminaComponent->CurrentStamina > 0.0f) {
		//Change to run speed
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else {
		//Change back to walk speed
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ATAM_CWCharacter::SprintOff() {
	isRunning = false;
	//Change back to walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}


void ATAM_CWCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATAM_CWCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ATAM_CWCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATAM_CWCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ATAM_CWCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ATAM_CWCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void ATAM_CWCharacter::AddExp() {
	ExpComponent->UpdateExp(40.0f);
}

void ATAM_CWCharacter::DamageSelf() {
	HealthComponent->UpdateHealth(20.0f);
}

void ATAM_CWCharacter::SaveGame() {
	ExpComponent->SaveGame();
	HealthComponent->SaveGame();
	ManaComponent->SaveGame();
}

void ATAM_CWCharacter::LoadData() {
	ExpComponent->LoadData();
	HealthComponent->LoadData();
	ManaComponent->LoadData();
}

void ATAM_CWCharacter::RestoreHealth(float Change) {
	HealthComponent->UpdateHealth(Change);
}

void ATAM_CWCharacter::RestoreMana(float Change) {
	ManaComponent->UpdateMana(Change);
}

void ATAM_CWCharacter::ReceiveDamage(float Change) {
	HealthComponent->UpdateHealth(Change);
}

// Handles pause menu and it's features //

void ATAM_CWCharacter::OpenPauseMenu() {
	PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);

	//Add pause UI to viewport
	if (PauseWidgetClass)
	{
		if (PauseWidget)
		{
			PauseWidget->AddToViewport();
		}
	}

	PauseGame(true);
}

void ATAM_CWCharacter::ResumeGame() {
	PauseGame(false);
}

void ATAM_CWCharacter::PauseGame(bool isPaused) {
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;

	APlayerController* const MyChar = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));

	if (MyChar != NULL)
	{
		MyChar->SetPause(isPaused);
	}

	if (!isPaused) {
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;

		PauseWidget->RemoveFromParent();
		
	}
}

void ATAM_CWCharacter::SpendSkillPoints() {
	SkillPointsCost(1);
}

void ATAM_CWCharacter::SkillPointsCost(int Cost) {
	if (ExpComponent->SkillPoints > 0) {
		ExpComponent->SkillPoints -= Cost;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("No skill points to spend"));
	}
}


