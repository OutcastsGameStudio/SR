﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_DashComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USR_DashComponent::USR_DashComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Default values configuration
    DashDistance = 1000.0f;     // Dash distance
    DashSpeed = 3000.0f;         // Dash speed
    DashDuration = 0.2f;         // Dash duration
    bCanDash = true;             // Dash availability
    DashCooldown = 1.0f;         // Dash cooldown
}

void USR_DashComponent::BeginPlay()
{
    Super::BeginPlay();

    // Get the character movement component and the owner character
    CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
    OwnerCharacter = Cast<ACharacter>(GetOwner());

    if (!CharacterMovement || !OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("Dash Component: Failed to get Character or CharacterMovement"));
    }
}

void USR_DashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // dash update
    if (bIsDashing)
    {
        UpdateDash(DeltaTime);
    }

    // cooldown update
    if (!bCanDash)
    {
        CurrentCooldownTime += DeltaTime;
        if (CurrentCooldownTime >= DashCooldown)
        {
            bCanDash = true;
            CurrentCooldownTime = 0.0f;
        }
    }
}

void USR_DashComponent::Dash()
{   
    // check if the dash is available
    if (!bCanDash || !CharacterMovement || !OwnerCharacter) return;

    // Disable the dash
    bCanDash = false;
    bIsDashing = true;

    // Normalize the dash direction
    DashDirection.Normalize();

    // Stock the start location of the dash
    DashStartLocation = OwnerCharacter->GetActorLocation();

    // Reset the dash time
    CurrentDashTime = 0.0f;

    // Desactivate gravity
    CharacterMovement->GravityScale = 0.0f;
    CharacterMovement->BrakingDecelerationFalling = 0.0f;

    // Optional: Add a visual/sound effect at the start of the dash
    UE_LOG(LogTemp, Warning, TEXT("Dash Started!"));
}

void USR_DashComponent::UpdateDash(float DeltaTime)
{
    if (!bIsDashing || !OwnerCharacter) return;

    // Increment the dash time
    CurrentDashTime += DeltaTime;

    // Calculate the alpha value
    float Alpha = FMath::Clamp(CurrentDashTime / DashDuration, 0.0f, 1.0f);

    // Calculate the new location
    FVector NewLocation = DashStartLocation + (DashDirection * DashDistance * (1.0f - FMath::Pow(1.0f - Alpha, 3)));
    
    // Move the character
    OwnerCharacter->SetActorLocation(NewLocation, true);

    // check if the dash is finished
    if (Alpha >= 1.0f)
    {
        EndDash();
    }
}

void USR_DashComponent::EndDash()
{
    if (!CharacterMovement) return;

    // reset the character movement
    CharacterMovement->GravityScale = 1.0f;
    CharacterMovement->BrakingDecelerationFalling = 960.0f; // Valeur par défaut

    // Reset the flags
    bIsDashing = false;

    // Optional: Add a visual/sound effect at the end of the dash
    UE_LOG(LogTemp, Warning, TEXT("Dash Ended!"));
}