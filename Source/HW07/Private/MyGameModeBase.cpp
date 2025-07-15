#include "HW07/Public/MyGameModeBase.h"
#include "HW07/Public/MyPlayerController.h"
#include "HW07/Public/PawnCharacter.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APawnCharacter::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
}
