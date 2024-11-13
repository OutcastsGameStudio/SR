// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_CharacterMovementComponent.h"

#include "CollisionDebugDrawingPublic.h"
#include "KismetTraceUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Logging/StructuredLog.h" 


// Sets default values for this component's properties
USR_CharacterMovementComponent::USR_CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USR_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterOwner()->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &USR_CharacterMovementComponent::OnHit);
}


// Called every frame
void USR_CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USR_CharacterMovementComponent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (MovementMode == MOVE_Falling && Velocity.Z < 0.f && FMath::IsNearlyZero(Hit.Normal.Z))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Start wall run detected !"));
		SetMovementMode(MOVE_Custom, 0);
		WallNormal = Hit.Normal;
		FVector WallDirection = FVector::CrossProduct(FVector::UpVector, Hit.Normal);
		WallRunDirection = WallDirection * (GetCharacterOwner()->GetActorForwardVector().Dot(WallDirection) > 0.f ? 1.f : -1.f);
	}
}

void USR_CharacterMovementComponent::WallJump()
{
	if (MovementMode == MOVE_Custom)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Wall jump"));
		Velocity = (WallRunDirection + WallNormal + FVector::UpVector).GetSafeNormal() * WallJumpSpeed;
		SetMovementMode(MOVE_Falling);
	}
}


void USR_CharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	LastUpdateVelocity = Velocity;
	Velocity = WallRunDirection * MaxWalkSpeed;

	FVector Delta = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

	if (Hit.IsValidBlockingHit() && Hit.Normal.Z > 0.f)
    {
    	if (Hit.Normal.Z > 0.5f)
    	{
    		SetMovementMode(MOVE_Walking, 0);
    		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("End wall run detected !"));
    		return;
    	}
    }
	else if (!DetectNextWall(Hit))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Next wall detected !"));
		SetMovementMode(MOVE_Falling);
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
	}
}

void USR_CharacterMovementComponent::UpdateWallRunDirection(FHitResult& Hit)
{
	FQuat Rotation = FQuat::FindBetweenNormals(WallNormal, Hit.Normal);
	WallNormal = Hit.Normal;
	WallRunDirection = Rotation.RotateVector(WallRunDirection);
}

bool USR_CharacterMovementComponent::DetectNextWall(FHitResult& Hit)
{
	UCapsuleComponent* Capsule = GetCharacterOwner()->GetCapsuleComponent();
	FName CollisionProfile = Capsule->GetCollisionProfileName();
	
	FVector Start = GetCharacterOwner()->GetActorLocation();
	FVector End = Start - WallNormal * Capsule->GetScaledCapsuleRadius();
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(GetCharacterOwner());
	
	GetWorld()->SweepSingleByProfile(Hit, Start, End, Capsule->GetComponentQuat(), CollisionProfile, Capsule->GetCollisionShape(), Params);

	return Hit.bBlockingHit;
}
