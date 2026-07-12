// Luee


#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(100.f);
	InitMaxMana(100.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//GAS属性使用REPNOTIFY_Always
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//通知 AbilitySystemComponent 属性值从 OldHealth 变为了新值
	//单人模式下，不会调用OnRep函数,使用GE会自动调用OnAttributeChange 广播。
	//但是多人游戏中，服务局只是简单将新值拷贝到客户端，不是通过GE流程，因此需要额外调用这个宏，否则不会触发OnAttributeChange
	//同时，如果客户端的数据和服务器的数据有误，他还会使用服务器的数据校准客户端 AbilitySystemComponent 的值
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}
