// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazard.h"
#include "TAM_CWCharacter.h"

// Sets default values
AHazard::AHazard()
{
	OnActorBeginOverlap.AddDynamic(this, &AHazard::OnOverlap);

	Damage = 30.0f;
}

//Check for overlap with player character
void AHazard::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor) {
	if (OtherActor != nullptr && OtherActor != this)
	{
		class ATAM_CWCharacter* MyCharacter = Cast<ATAM_CWCharacter>(OtherActor);

		if (MyCharacter)
		{
			MyCharacter->ReceiveDamage(Damage);
		}
	}
}

