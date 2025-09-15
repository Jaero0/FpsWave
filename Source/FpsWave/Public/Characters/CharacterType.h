#pragma once

UENUM(BlueprintType)
enum class EMoveState : uint8
{
	EMS_Walk UMETA(DisplayName=Walk),
	EMS_Run UMETA(DisplayName=Run),
	EMS_Crouch UMETA(DisplayName=Crouch)
};

UENUM(BlueprintType)
enum class EToggleMode : uint8
{
	ETM_ToggleNone UMETA(DisplayName=ToggleNone),
	ETM_ToggleRun UMETA(DisplayName=ToggleRun),
	ETM_ToggleCrouch UMETA(DisplayName=ToggleCrouch)
};
