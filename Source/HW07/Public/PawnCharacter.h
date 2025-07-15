#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCapsuleComponent;
struct FInputActionValue;

UCLASS()
class HW07_API APawnCharacter : public APawn
{
	GENERATED_BODY()

public:
	APawnCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UCapsuleComponent* RootCapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USkeletalMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone")
	UStaticMeshComponent* DroneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
	float MaxDroneDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
	float DroneMoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
	float DroneSensitivity;
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void DroneMoveXY(const FInputActionValue& Value);
	UFUNCTION()
	void DroneMoveZ(const FInputActionValue& Value);
	UFUNCTION()
	void DroneRotateXY(const FInputActionValue& Value);
	UFUNCTION()
	void DroneRotateZ(const FInputActionValue& Value);
	
	FVector LimitInScope(FVector RelativePos);
};
