// Luee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	
	FEffectProperties(){}
	
	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	
	UPROPERTY()
	AController* SourceController = nullptr;
	
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
	
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	
	UPROPERTY()
	AController* TargetController = nullptr;
	
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

// typedef is specific to the FGameplayAttribute() signture,but TStaticFunPtr is generic to any signture chosen
//typedef TBaseStaticDelegateInstance<FGameplayAttribute(),FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<FGameplayAttribute(),FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 
 */
UCLASS()
class GAS_AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAuraAttributeSet();
	//通知引擎哪些变量需要同步（哪些属性要从服务器复制到客户端，使用 DOREPLIFETIME 宏注册属性）
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	TMap<FGameplayTag,TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	
	
	/*
	 * Primary Attributes
	 */
	
	//力量
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_Strength , Category="Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Strength);
	
	//智力
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_Intelligence , Category="Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Intelligence);
	
	//韧性
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_Resilience , Category="Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience);
	
	//活力
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_Vigor , Category="Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Vigor);
	
	/*
	 * Secondary Attribute
	 */
	
	//护甲
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_Armor , Category="Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Armor);
	
	//护甲穿透
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_ArmorPenetration , Category="Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ArmorPenetration);
	
	//格挡（半伤率）
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_BlockChance , Category="Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,BlockChance);
	
	//暴击率
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_CriticalHitChance , Category="Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitChance);
	
	//暴击伤害
   	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_CriticalHitDamage , Category="Secondary Attributes")
   	FGameplayAttributeData CriticalHitDamage;
   	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitDamage);
   	
	//抗爆率
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_CriticalHitResistance , Category="Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitResistance);
	
	//生命回复
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_HealthRegeneration , Category="Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,HealthRegeneration);
	
	//法力回复
   	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_ManaRegeneration , Category="Secondary Attributes")
   	FGameplayAttributeData ManaRegeneration;
   	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ManaRegeneration);
	
	//最大生命值
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_MaxHealth , Category="Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHealth);
	
	//最大法力值
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_MaxMana , Category="Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);
	
	/*
	 * Vital Attributes
	 */
	
	//ReplicatedUsing:  RepNotify（同步通知） 机制。表示：“这个变量需要在网络上同步。每当客户端从服务器接收到该变量的新值时，请立即自动调用 OnRep_Health 函数。”
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_Health , Category="Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Health);
	
	UPROPERTY(BlueprintReadOnly , ReplicatedUsing = OnRep_Mana , Category="Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);
	
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
	
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;
	
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	
	UFUNCTION()
    void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
    
    UFUNCTION()
    void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
    
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	
	UFUNCTION()
   	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
   	
   	UFUNCTION()
   	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

private:
	void SetEffectProperties(const  FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};


