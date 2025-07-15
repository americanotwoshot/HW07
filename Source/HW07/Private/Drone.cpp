#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Drone.h"
#include "MyPlayerController.h"

ADrone::ADrone()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(RootComp);
	RootComp->SetSimulatePhysics(false);
	RootComp->SetBoxExtent(FVector(45.0f, 80.0f, 10.0f));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComp);
	MeshComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComp);
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritRoll = true;
	SpringArmComp->bInheritYaw = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	MoveSpeed = 300.0f;
	MouseSensitivity = 20.0f;
	Gravity = -980.0f;
	GroundCheckDistance = 40.0f;
	VerticalVelocity = 0.0f;

	bIsGrounded = true;
	bIsGravityOn = false;
	bIsLanding = false;
}

void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
		{
			if (PC->AirCraftMoveAction)
			{
				EnhancedInputComponent->BindAction(PC->AirCraftMoveAction, ETriggerEvent::Triggered, this, &ADrone::Move);
			}

			if (PC->AirCraftRotateAction)
			{
				EnhancedInputComponent->BindAction(PC->AirCraftRotateAction, ETriggerEvent::Triggered, this, &ADrone::Rotate);
			}

			if (PC->AirCraftGravityAction)
			{
				EnhancedInputComponent->BindAction(PC->AirCraftGravityAction, ETriggerEvent::Started, this, &ADrone::OnOffGravity);
			}
		}
	}
}

void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsGrounded = IsGrounded();
	
	if (bIsGravityOn)
	{
		if (bIsGrounded)
		{
			VerticalVelocity = 0.0f;
		}
		else
		{
			VerticalVelocity += Gravity * DeltaTime;
			AddActorWorldOffset(FVector(0.0f, 0.0f, VerticalVelocity * DeltaTime / 2), true);
		}
	}
	
	if (bIsGrounded)
	{
		SetActorRotationAndLocation(DeltaTime);

		VerticalVelocity = 0.0f;
		MoveSpeed = 600.0f;
	}
	else
	{
		bIsLanding = false;
		MoveSpeed = 300.0f;
	}
	
}

void ADrone::Move(const FInputActionValue& Value)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector InputValue = Value.Get<FVector>();

	if (!FMath::IsNearlyZero(InputValue.X))
	{
		FVector ForwardVector = GetActorForwardVector();
		AddActorWorldOffset(ForwardVector * InputValue.X * DeltaTime * MoveSpeed);
	}
	if (!FMath::IsNearlyZero(InputValue.Y))
	{
		FVector RightVector = GetActorRightVector();
		AddActorWorldOffset(RightVector * InputValue.Y * DeltaTime * MoveSpeed);
	}
	if (!FMath::IsNearlyZero(InputValue.Z))
	{
		if (!bIsGrounded || InputValue.Z > 0.0f)
		{
			FVector UpVector = GetActorUpVector();
			AddActorWorldOffset(UpVector * InputValue.Z * DeltaTime * MoveSpeed);
		}
	}
}

void ADrone::Rotate(const FInputActionValue& Value)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector InputValue = Value.Get<FVector>();

	if (!InputValue.IsNearlyZero())
	{
		AddActorLocalRotation(FRotator(
			InputValue.Y * DeltaTime * MouseSensitivity,
			InputValue.X * DeltaTime * MouseSensitivity,
			InputValue.Z * DeltaTime * MouseSensitivity * 10.0f));
	}
}

void ADrone::OnOffGravity(const FInputActionValue& Value)
{
	if (bIsGravityOn)
	{
		bIsGravityOn = false;
	}
	else
	{
		bIsGravityOn = true;
	}
}

bool ADrone::IsGrounded() const
{
	FVector CurrentLocation = GetActorLocation();
	FVector EndLocation = CurrentLocation - FVector(0.0f, 0.0f, GroundCheckDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CurrentLocation, EndLocation, ECC_Visibility, Params);
	
	return bHit;
}

void ADrone::SetActorRotationAndLocation(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);
	if (bIsLanding)
	{
		SetActorRotation(TargetRotation);
	}
	else
	{
		FRotator InterpRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 2.0f);
		SetActorRotation(InterpRotation);

		if (FMath::Abs(InterpRotation.Pitch) < 0.5f && FMath::Abs(InterpRotation.Roll) < 0.5f)
		{
			bIsLanding = true;
		}
	}
		
	FVector CurrentLocation = GetActorLocation();
	SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, GroundCheckDistance));
}

