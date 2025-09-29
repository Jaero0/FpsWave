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

UENUM(BlueprintType)
enum class EOverlapDetected : uint8
{
	EOD_None UMETA(DisplayName=None),
	EOD_Weapon UMETA(DisplayName=Weapon)
};

UENUM(BlueprintType)
enum class EPlayerEquipType : uint8
{
	EPE_Gun UMETA(DisplayName=Gun),
	EPE_Melee UMETA(DisplayName=Melee)
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Rifle = 0,
	Shotgun = 1,
	Katana = 2,
	Hammer = 3,
	MAX UMETA(Hidden)
};