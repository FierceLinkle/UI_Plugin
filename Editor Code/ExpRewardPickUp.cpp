// Fill out your copyright notice in the Description page of Project Settings.


#include "ExpRewardPickUp.h"
#include "TAM_CWCharacter.h"

// Sets default values
AExpRewardPickUp::AExpRewardPickUp()
{
	OnActorBeginOverlap.AddDynamic(this, &AExpRewardPickUp::OnOverlap);

	ExpAdd = 40.0f;

}

//Check for overlap with player character
void AExpRewardPickUp::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor) {
	if (OtherActor != nullptr && OtherActor != this) {

		class ATAM_CWCharacter* MyCharacter = Cast<ATAM_CWCharacter>(OtherActor);

		if (MyCharacter)
		{
			MyCharacter->AddExp();
			Destroy();
		}
	}
}

