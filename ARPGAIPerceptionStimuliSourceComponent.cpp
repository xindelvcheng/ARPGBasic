// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGAIPerceptionStimuliSourceComponent.h"

#include "Perception/AISense_Sight.h"

UARPGAIPerceptionStimuliSourceComponent::UARPGAIPerceptionStimuliSourceComponent(
    const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
    bAutoRegisterAsSource = true;
    RegisterAsSourceForSenses = {UAISense_Sight::StaticClass()};
}
