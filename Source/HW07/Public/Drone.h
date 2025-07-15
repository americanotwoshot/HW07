#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Drone.generated.h"

struct FInputActionValue;
class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class HW07_API ADrone : public APawn
{
	GENERATED_BODY()

public:
	ADrone();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UBoxComponent* RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float Gravity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float GroundCheckDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float VerticalVelocity;
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Rotate(const FInputActionValue& Value);
	UFUNCTION()
	void OnOffGravity(const FInputActionValue& Value);

	bool IsGrounded() const;
	void SetActorRotationAndLocation(float DeltaTime);
	
	bool bIsGrounded;
	bool bIsGravityOn;
	bool bIsLanding;
};
