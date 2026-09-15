 // Luee


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

 AAuraPlayerState::AAuraPlayerState()
 {
 	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
 	AbilitySystemComponent->SetIsReplicated(true);
 	//Mixed : 拥有该角色的客户端 接收 GE, 其他观察该角色的客户端 不接收GE
 	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
 	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
 	
 	NetUpdateFrequency = 100.f;
 }

 void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
 {
	 Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 	
 	DOREPLIFETIME(AAuraPlayerState,Level);
 	DOREPLIFETIME(AAuraPlayerState,XP);
 }

 UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
 {
 	return AbilitySystemComponent;
 }

 void AAuraPlayerState::AddToLevel(int32 NewLevel)
 {
 	Level += NewLevel;
 	OnLevelChangedDelegate.Broadcast(Level);
 }

 void AAuraPlayerState::SetLevel(int32 NewLevel)
 {
 	Level = NewLevel;
 	OnLevelChangedDelegate.Broadcast(Level);
 }

 void AAuraPlayerState::AddToXP(int32 InXP)
 {
 	XP += InXP;
 	OnXPChangedDelegate.Broadcast(XP);
 }

 void AAuraPlayerState::SetXP(int32 InXP)
 {
 	XP = InXP;
 	OnXPChangedDelegate.Broadcast(XP);
 }

 void AAuraPlayerState::OnRep_Level(int32 OldLevel)
 {
 	// 在 客户端 广播 ，更新 客户端 持有的 HUD
 	OnLevelChangedDelegate.Broadcast(Level);
 }

 void AAuraPlayerState::OnRep_XP(int32 OldXP)
 {
 	// 在 客户端 广播 ，更新 客户端 持有的 HUD
 	OnXPChangedDelegate.Broadcast(XP);
 }
