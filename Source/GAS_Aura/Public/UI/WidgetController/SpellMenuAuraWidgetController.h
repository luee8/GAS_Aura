// Luee

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuAuraWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_API USpellMenuAuraWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	
	virtual void BindCallbacksToDependencies() override;
};
