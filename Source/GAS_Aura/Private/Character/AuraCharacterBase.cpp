// Luee


#include "Character/AuraCharacterBase.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//创建骨骼网格体，并依附在父类网格体插槽上，并设置为“无碰撞”
	//注意要在父类网格体上创建一个名字为"WeaponHandSocket"的插槽
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	//GetMesh()返回主网格体，它是Character类自带的函数
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}


