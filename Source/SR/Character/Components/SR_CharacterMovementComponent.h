// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_CharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USR_CharacterMovementComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


/**
 * @description: Wall Run Props
 */
public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun")
	float WallJumpSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", meta = (ToolTip = "Angle maximum a partir duquel on peut faire un wall run"))
	float MaxAngleWallRun = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",
	meta = (ToolTip = "Durée maximale du wall run en secondes. Au-delà, le personnage tombe"))
	float MaxAngleBeforeStop = 120.f;
	/**
	* @description: Stop the wall jump when the character is already wall running 
	*/

	void PhysWallRun(float deltaTime, int32 Iterations);
	void StopWallJump();
private:
	void UpdateWallRunDirection(FHitResult& Hit);
    
	bool DetectNextWall(FHitResult& Hit);
	FVector m_WallRunDirection;
	FVector m_WallNormal;
/**
 *
 */

public:
	enum CustomMode
	{
		CUSTOM_None = 0,
		CUSTOM_WallRun = 1,
		CUSTOM_DASH = 2,
	};
	void SetCustomMode(int32 NewCustomMode)
	{
		m_CustomMovementMode = (CustomMode)NewCustomMode;
	}
private:
	CustomMode m_CustomMovementMode;
private:
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    virtual void PhysCustom(float deltaTime, int32 Iterations) override;
};
