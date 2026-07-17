// Luee


#include "Player/AuraPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	//设置AuraPlayerController是可复制的
	//复制是指将数据和命令从服务器（权威端）传递给连接的客户端，以确保所有玩家看到的游戏世界状态是一致的
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	CursorTance();
}

void AAuraPlayerController::CursorTance()
{
	FHitResult CursorHit;
	//获得鼠标指向的actor
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit)
		return;
	
	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();
	
	if (LastActor==nullptr)
	{
		if (ThisActor!=nullptr)
		{
			ThisActor->HighLightActor();
		}
		else
		{
			//do nothing
		}
	}
	else
	{
		if (ThisActor==nullptr)
		{
			LastActor->UnHighLightActor();
		}
		else
		{
			if (LastActor!=ThisActor)
			{
				LastActor->UnHighLightActor();
				ThisActor->HighLightActor();
			}
			else
			{
				//do nothing
			}
		}
	}
	/* 上面的逻辑判断同下
	if (LastActor && LastActor != ThisActor) 
	{
		LastActor->UnHighLightActor();
	}
	if (ThisActor && ThisActor != LastActor) 
	{
		ThisActor->HighLightActor();
	}
	*/
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
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	//绑定函数
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
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


