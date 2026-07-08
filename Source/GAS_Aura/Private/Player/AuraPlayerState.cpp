 // Luee


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

 AAuraPlayerState::AAuraPlayerState()
 {
 	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
 	AbilitySystemComponent->SetIsReplicated(true);
 	//Mixed : 拥有该角色的客户端 接收 GE, 其他观察该角色的客户端 不接收GE
 	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
 	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
 	
 	NetUpdateFrequency = 100.f;
 }

 UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
 {
 	return AbilitySystemComponent;
 }
