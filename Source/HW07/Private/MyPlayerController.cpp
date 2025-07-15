#include "HW07/Public/MyPlayerController.h"
#include "Drone.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, LookAction(nullptr)
	, DroneMoveAction(nullptr)
	, DroneRotateAction(nullptr)
	, AirCraftInputMappingContext(nullptr)
	, AirCraftMoveAction(nullptr)
	, AirCraftRotateAction(nullptr)
	, AirCraftGravityAction(nullptr)
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (APawn* MyPawn = GetPawn())
			{
				if (MyPawn->IsA(ADrone::StaticClass()))
				{
					if (AirCraftInputMappingContext)
					{
						Subsystem->AddMappingContext(AirCraftInputMappingContext, 0);
					}
				}
				else
				{
					if (InputMappingContext)
					{
						Subsystem->AddMappingContext(InputMappingContext, 0);
					}
				}
			}
			
		}
	}
}
