// Luee


#include "Character/AuraCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	//角色会自动旋转并面向它移动的方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//设置角色自动转向时的旋转速度
	GetCharacterMovement()->RotationRate = FRotator(0.f,400.f,0.f);
	//将角色的运动限制在一个特定的平面内
	GetCharacterMovement()->bConstrainToPlane = true;
	//当角色在世界中生成或游戏开始时，立即将其位置对齐（吸附）到上述定义的平面上
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	//角色模型不受控制器（Controller）旋转的影响
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}
