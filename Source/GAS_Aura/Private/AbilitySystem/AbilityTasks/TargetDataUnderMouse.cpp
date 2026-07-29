// Luee


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"


UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		//在本地客户端执行的逻辑
		SendMouseCursorData();
	}
	else
	{
		//在服务器中执行的逻辑 
		
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		
		/** AbilityTargetDataSetDelegate() : 新建并返回给定 Ability、PredictionKey 的 TargetDataSet 委托 */
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle,ActivationPredictionKey)
										.AddUObject(this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		//如果来自客户端PRC的数据已经到了，该函数会直接触发绑定的 OnTargetDataReplicatedCallback，并返回 true；否则不触发函数并返回 false。
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,ActivationPredictionKey);
		
		if (!bCalledDelegate)
		{
			//挂起 Task 等待客户端的数据
			SetWaitingOnRemotePlayerData();
		}
		
	}
	
}

// 在本地执行
void UTargetDataUnderMouse::SendMouseCursorData()
{
	// 创建一个作用域窗口
	// 在这个花括号范围内产生的所有 GameplayEffect 或属性变化
	// 都会自动关联到这个 Key，从而实现网络同步
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	APlayerController* PC =Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);
	
	/*	ServerSetReplicatedTargetData() : Replicates targeting data to the server
	 *	服务器会在 Activate() 函数中接受这个 RPC

	####  GetAbilitySpecHandle() ： 技能实例的句柄
	####  GetActivationPredictionKey() ： 技能激活时的初始预测键。
		* 这两个参数 用于 找到对应的技能实例 ，匹配 对应的 AbilityTargetDataSetDelegate 
 
	####  DataHandle(FGameplayAbilityTargetDataHandle) ： 实际的TargetData
	####  FGameplayTag() ： 一个可选的标识符。用它来区分同一技能产生的不同类型的数据
		* 这两个参数会 传递给 AbilityTargetDataSetDelegate 绑定的回调函数，即 OnTargetDataReplicatedCallback
 
	####  AbilitySystemComponent->ScopedPredictionKey ：客户端当前的实时预测键
		*  当这个 Key 传给服务器后，服务器在处理 TargetData 时会开启一个同名的 PredictionWindow。
		*  这意味着在服务器收到数据并执行逻辑（例如产生伤害 GE）时，产生的状态改变会带上这个 Key。
		*  当服务器的状态更新传回客户端时，客户端会发现 Key 匹配，并接受这个权威结果，避免误以为是 服务器发起的状态更新并再次执行此逻辑。
	*/
	
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);
	
	// ShouldBroadcastAbilityTaskDelegates() : 确保该能力仍然有效。
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

//只在 服务器 执行
void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,FGameplayTag ActivationTag)
{
	//告诉 ASC : TargetData 已经接受，因此可以清除 缓存的 TargetData
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
