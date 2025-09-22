#pragma once

UENUM(BlueprintType)
enum class EMoveState : uint8
{
	EMS_Walk UMETA(DisplayName=Walk),
	EMS_ZoomInWalk UMETA(DisplayName="Zoom In Walk"),
	EMS_Run UMETA(DisplayName=Run),
	EMS_Crouch UMETA(DisplayName=Crouch)
};

UENUM(BlueprintType)
enum class EToggleMode : uint8
{
	ETM_None UMETA(DisplayName=None),
	ETM_Toggle UMETA(DisplayName=Toggle),
};

UENUM(BlueprintType)
enum class EPointOfViewType : uint8
{
	EPT_FirstPersonView UMETA(DisplayName="First Person View"),
	EPT_ThirdPersonView UMETA(DisplayName="Third Person View"),
	EPT_ThirdPersonZoomInView UMETA(DisplayName="Third Person Zoom In View"),
	//EPT_FreeCameraView UMETA(DisplayName="Free Camera View")
};