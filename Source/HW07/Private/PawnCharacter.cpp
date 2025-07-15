#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "MyPlayerController.h"
#include "PawnCharacter.h"

APawnCharacter::APawnCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	RootCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(RootCapsuleComp);
	RootCapsuleComp->SetCapsuleHalfHeight(90.0f);
	RootCapsuleComp->SetCapsuleRadius(20.0f);
	RootCapsuleComp->SetSimulatePhysics(false);
	
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootCapsuleComp);
	MeshComp->SetSimulatePhysics(false);
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootCapsuleComp);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	MoveSpeed = 300.0f;
	MouseSensitivity = 50.0f;

	// Drone Component Settings
	DroneComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneComp"));
	DroneComp->SetupAttachment(RootCapsuleComp);
	DroneComp->SetSimulatePhysics(false);
	DroneComp->SetCollisionProfileName(TEXT("NoCollision"));

	MaxDroneDistance = 100.0f;
	DroneMoveSpeed = 600.0f;
	DroneSensitivity = 100.0f;
}

void APawnCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
		{
			if (PC->MoveAction)
			{
				EnhancedInputComponent->BindAction(PC->MoveAction, ETriggerEvent::Triggered, this, &APawnCharacter::Move);
				EnhancedInputComponent->BindAction(PC->MoveAction, ETriggerEvent::Triggered, this, &APawnCharacter::DroneMoveXY);
			}

			if (PC->LookAction)
			{
				EnhancedInputComponent->BindAction(PC->LookAction, ETriggerEvent::Triggered, this, &APawnCharacter::Look);
				EnhancedInputComponent->BindAction(PC->LookAction, ETriggerEvent::Triggered, this, &APawnCharacter::DroneRotateXY);
			}

			if (PC->DroneMoveAction)
			{
				EnhancedInputComponent->BindAction(PC->DroneMoveAction, ETriggerEvent::Triggered, this, &APawnCharacter::DroneMoveZ);
			}

			if (PC->DroneRotateAction)
			{
				EnhancedInputComponent->BindAction(PC->DroneRotateAction, ETriggerEvent::Triggered, this, &APawnCharacter::DroneRotateZ);
			}
		}
	}
}

void APawnCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector2D InputValue = Value.Get<FVector2D>();
	FRotator ControlRotation = Controller->GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;

	if (!FMath::IsNearlyZero(InputValue.X))
	{
		const FVector ForwardVector = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::X);
		AddActorWorldOffset( ForwardVector * InputValue.X * DeltaTime * MoveSpeed);
	}
	
	if (!FMath::IsNearlyZero(InputValue.Y))
	{
		const FVector RightVector = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y);
		AddActorWorldOffset(RightVector * InputValue.Y * DeltaTime * MoveSpeed);
	}
}

void APawnCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector2D InputValue = Value.Get<FVector2D>();

	FRotator ControlRotation = Controller->GetControlRotation();
	ControlRotation.Yaw += InputValue.X * DeltaTime * MouseSensitivity;
	ControlRotation.Pitch = FMath::Clamp(ControlRotation.Pitch + InputValue.Y * DeltaTime * MouseSensitivity, -89.9f, 89.9f);
	
	Controller->SetControlRotation(ControlRotation);
}

void APawnCharacter::DroneMoveXY(const FInputActionValue& Value)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector2D InputValue = Value.Get<FVector2D>();

	if (!InputValue.IsNearlyZero())
	{
		FVector RelativePos = DroneComp->GetRelativeLocation() + FVector(InputValue.X * DeltaTime * DroneMoveSpeed, InputValue.Y * DeltaTime * DroneMoveSpeed,0.0f);
		RelativePos = LimitInScope(RelativePos);
		DroneComp->SetRelativeLocation(RelativePos);
	}
}

void APawnCharacter::DroneMoveZ(const FInputActionValue& Value)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const float InputValue = Value.Get<float>();

	if (!FMath::IsNearlyZero(InputValue))
	{
		FVector RelativePos = DroneComp->GetRelativeLocation() + FVector(0.0f, 0.0f, InputValue * DeltaTime * DroneMoveSpeed);
		RelativePos = LimitInScope(RelativePos);
		DroneComp->SetRelativeLocation(RelativePos);
	}
}

void APawnCharacter::DroneRotateXY(const FInputActionValue& Value)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector2D InputValue = Value.Get<FVector2D>();

	FRotator InputRotation(InputValue.Y * DeltaTime * DroneSensitivity, InputValue.X * DeltaTime * DroneSensitivity, 0.0f);

	FVector RelativePos = DroneComp->GetRelativeLocation();

	FVector RotatedPos = FRotationMatrix(InputRotation).TransformPosition(RelativePos);
	RotatedPos = LimitInScope(RotatedPos);
	DroneComp->SetRelativeLocation(RotatedPos);
}

void APawnCharacter::DroneRotateZ(const FInputActionValue& Value)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const float InputValue = Value.Get<float>();

	FRotator InputRotation(0.0f, 0.0f, InputValue * DeltaTime * DroneSensitivity);

	FVector RelativePos = DroneComp->GetRelativeLocation();
	
	FVector RotatedPos = FRotationMatrix(InputRotation).TransformPosition(RelativePos);
	RotatedPos = LimitInScope(RotatedPos);
	DroneComp->SetRelativeLocation(RotatedPos);
}

FVector APawnCharacter::LimitInScope(FVector RelativePos)
{
	if (RelativePos.Size() > MaxDroneDistance)
	{
		RelativePos = RelativePos.GetSafeNormal() * MaxDroneDistance;
	}
	
	return RelativePos;
}

