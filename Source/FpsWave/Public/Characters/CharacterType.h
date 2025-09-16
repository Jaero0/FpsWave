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
	ETM_None UMETA(DisplayName=None),
	ETM_Toggle UMETA(DisplayName=Toggle),
};
