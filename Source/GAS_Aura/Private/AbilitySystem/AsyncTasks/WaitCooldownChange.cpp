// Luee


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;
	
	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}
	
	// 当 Cooldown 结束 （Cooldown Tag Removed）
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag,EGameplayTagEventType::NewOrRemoved)
								.AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChanged);
	
	// 当 Cooldown Effect 被施加
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf
								.AddUObject(WaitCooldownChange,&UWaitCooldownChange::OnActiveEffectAdded);
	
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC))
		return;
	ASC->RegisterGameplayTagEvent(CooldownTag,EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// 获得 Ability 的资产标签
	FGameplayTagContainer AssetTagsContainer;
	SpecApplied.GetAllAssetTags(AssetTagsContainer);
	
	// 获得 Ability 身上被 GE 授予（Granted） 的标签
	FGameplayTagContainer GrantedTagsContainer;
	SpecApplied.GetAllGrantedTags(GrantedTagsContainer);
	
	if (AssetTagsContainer.HasTagExact(CooldownTag) || GrantedTagsContainer.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if (!TimesRemaining.IsEmpty())
		{
			float TimeRemaining = TimesRemaining[0];
			for (const int32& x:TimesRemaining)
			{
				TimeRemaining = FMath::Max(TimeRemaining, x);
			}
			
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
