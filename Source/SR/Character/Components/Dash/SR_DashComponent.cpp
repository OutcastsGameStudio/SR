﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_DashComponent.h"


// Sets default values for this component's properties
USR_DashComponent::USR_DashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void USR_DashComponent::BeginPlay()
{
	Super::BeginPlay();

	// set the character movement component to the character movement component of the character
	CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	
}


// Called every frame
void USR_DashComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}


void USR_DashComponent::Dash(FVector DashDirection)
{	
	//set the movement mode to falling
	CharacterMovement->SetMovementMode(EMovementMode::MOVE_Falling);
	//set the gravity scale to 0
	CharacterMovement->GravityScale = 0.f;
	//set the braking deceleration falling to 0
	CharacterMovement->BrakingDecelerationFalling = 0.f;
	//set the velocity to the dash direction
	CharacterMovement->Velocity = DashDirection.GetSafeNormal() * 10000.f;
	//set a timer to end the dash
	GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &USR_DashComponent::EndDash, 0.2f, false);
}

void USR_DashComponent::EndDash()
{
	CharacterMovement->GravityScale = 2.f;
	CharacterMovement->BrakingDecelerationFalling = 1500.f;
}