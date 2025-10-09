#pragma once

UENUM(BlueprintType)
enum class EMoveState : uint8
{
	EMS_Idle UMETA(DisplayName=Idle),
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

enum class EKeyInput
{
	EKI_1,
	EKI_2,
	EKI_3,
	EKI_4
};

UENUM(BlueprintType)
enum class EPlayerWeaponType : uint8
{
	EPW_Rifle UMETA(DisplayName=Rifle),
	EPW_Shotgun UMETA(DisplayName=Shotgun),
	EPW_Katana UMETA(DisplayName=Katana),
	EPW_WarHammer UMETA(DisplayName=WarHammer)
};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	EAS_None UMETA(DisplayName=None),
	EAS_Attack UMETA(DisplayName=Attack),
	EAS_Reload UMETA(DisplayName=Reload),
	MAX UMETA(Hidden)
};