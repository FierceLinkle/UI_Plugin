// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaPickUp.h"
#include "TAM_CWCharacter.h"

// Sets default values
AManaPickUp::AManaPickUp()
{
	OnActorBeginOverlap.AddDynamic(this, &AManaPickUp::OnOverlap);

	ManaRestore = -60.0f;
}

//Check for overlap with player character
void AManaPickUp::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor) {
	if (OtherActor != nullptr && OtherActor != this) {
		
		class ATAM_CWCharacter* MyCharacter = Cast<ATAM_CWCharacter>(OtherActor);

		if (MyCharacter)
		{
			MyCharacter->RestoreMana(ManaRestore);
			Destroy();
		}
	}
}

