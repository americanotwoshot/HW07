#include "HW07/Public/MyPawnCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HW07/Public/MyPlayerController.h"

AMyPawnCharacter::AMyPawnCharacter()
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
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	MoveSpeed = 300.0f;
	MouseSensitivity = 50.0f;
}

void AMyPawnCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPawnCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPawnCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
		{
			if (PC->MoveAction)
			{
				EnhancedInputComponent->BindAction(PC->MoveAction, ETriggerEvent::Triggered, this, &AMyPawnCharacter::Move);
			}

			if (PC->LookAction)
			{
				EnhancedInputComponent->BindAction(PC->LookAction, ETriggerEvent::Triggered, this, &AMyPawnCharacter::Look);
			}
		}
	}
}

void AMyPawnCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;
	
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector2D InputValue = Value.Get<FVector2D>();
	FRotator ControlRotation = Controller->GetControlRotation();
	ControlRotation.Pitch = 0.0f;
	ControlRotation.Roll = 0.0f;
	
	if (!FMath::IsNearlyZero(InputValue.X))
	{
		FVector ForwardVector = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::X);
		AddActorWorldOffset( ForwardVector * InputValue.X * DeltaTime * MoveSpeed);
	}
	
	if (!FMath::IsNearlyZero(InputValue.Y))
	{
		FVector RightVector = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y);
		AddActorWorldOffset(RightVector * InputValue.Y * DeltaTime * MoveSpeed);
	}
}

void AMyPawnCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;
	
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector2D InputValue = Value.Get<FVector2D>();

	FRotator ControlRotation = Controller->GetControlRotation();
	ControlRotation.Yaw += InputValue.X * DeltaTime * MouseSensitivity;
	ControlRotation.Pitch = FMath::Clamp(ControlRotation.Pitch + InputValue.Y * DeltaTime * MouseSensitivity, -89.9f, 89.9f);
	
	Controller->SetControlRotation(ControlRotation);
}