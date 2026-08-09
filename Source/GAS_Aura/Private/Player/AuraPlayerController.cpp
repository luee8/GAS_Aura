// Luee


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	//设置AuraPlayerController是可复制的
	//复制是指将数据和命令从服务器（权威端）传递给连接的客户端，以确保所有玩家看到的游戏世界状态是一致的
	bReplicates = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTance();
	
	if (bAutoRunning)
	{
		AutoRun();
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount,ACharacter* TargetCharacter,bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter,DamageTextComponentClass);
		//将组件注册到所属 Actor（this）的组件数组，并完成与游戏世界的初始化 
		DamageText->RegisterComponent();
		
		//先附着再分离 获得角色的变换
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount,bBlockedHit,bCriticalHit);
	}
}

void AAuraPlayerController::AutoRun()
{
	//短按左键后的自动寻路移动
	if (APawn* ControlledPawn = GetPawn())
	{
		//找到Spline上距离当前位置最近的点
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		//计算方向
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);
		
		const float DistanceToDestination = (LocationOnSpline - CacheDestination).Length();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}


void AAuraPlayerController::CursorTance()
{
	//获得鼠标指向的actor
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit)
		return;
	
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();
	
	if (LastActor && LastActor != ThisActor) 
	{
		LastActor->UnHighLightActor();
	}
	if (ThisActor && ThisActor != LastActor) 
	{
		ThisActor->HighLightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	
	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}
	
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime<=ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath =  UNavigationSystemV1::FindPathToLocationSynchronously(this,ControlledPawn->GetActorLocation(),CacheDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);
				}
				CacheDestination = NavPath->PathPoints[NavPath->PathPoints.Num()-1];
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	
	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime +=GetWorld()->GetDeltaSeconds();
		
		if (CursorHit.bBlockingHit)
		{
			CacheDestination = CursorHit.ImpactPoint;
		}
		
		if(APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CacheDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
		
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}


void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//check()：断言（Assertion）
	//如果 MyPtr 为 null，程序会立即崩溃（在编辑器下会弹窗并中断到代码行）
	//在发行版本中，check 宏会被完全移除
	check(AuraContext);
	
	//LocalPlayer存在于 UGameInstance 中,跨关卡长存,因此不用多次配置IMC
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext,0); //第二个参数是优先级，由于区分在输入冲突时判断优先级
	}
	
	/*步骤：
	①准备阶段 (Setup)：
		游戏开始，LocalPlayer 创建。
		你在代码或蓝图中，通过 Subsystem 调用 AddMappingContext(IMC_Default)。此时，“说明书”被装载。
	②触发阶段 (Input)：
		你按下了键盘上的 W。
		Subsystem 接收到 W 信号，翻阅 IMC_Default，发现 W 映射到了 IA_Move。
	③路由阶段 (Routing)：
		Subsystem 将 IA_Move 的数值（比如 1.0）发送给 PlayerController。
		因为你的 PlayerController 已经 Possess（占有）了你的 Character，它会把这个指令传给 Character。
	④逻辑阶段 (Logic)：
		你的 Character 内部写了处理 IA_Move 的事件。
		角色执行 AddMovementInput，在屏幕上跑了起来。
	*/
	
	//是否显示鼠标
	bShowMouseCursor = true;
	//设置光标样式
	DefaultMouseCursor = EMouseCursor::Default;
	
	//创建一个配置对象，定义输入将如何分配给“游戏世界”和“UI 界面”，可以让光标与移动共存
	FInputModeGameAndUI InputModeDate;
	//控制鼠标是否可以离开游戏窗口（不会）
	InputModeDate.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	//决定当玩家点击视口（即“捕获”输入）时，光标是否消失
	InputModeDate.SetHideCursorDuringCapture(false);
	//应用到当前的玩家控制器
	SetInputMode(InputModeDate);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	//绑定函数
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Started,this,&AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Completed,this,&AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation =  GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
	
	//从YawRotation中提取出“正前方”的单位向量
	//把“水平旋转角度”转换成“水平面上的单位方向向量”
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//从YawRotation中提取出“正右方”的单位向量
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}


