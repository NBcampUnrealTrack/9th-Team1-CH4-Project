#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OCDebugHUD.generated.h"

UCLASS()
class OVERCOOKED_API AOCDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawCenteredText(const FString& Text, float ScreenY, float Scale, const FLinearColor& Color) const;
	void DrawBottomLeftText(const FString& Text, float BottomOffset, float Scale, const FLinearColor& Color) const;
	void DrawBottomRightText(const FString& Text, float Scale, const FLinearColor& Color) const;
	void DrawOrders(const class AOCGameState& State) const;

	int32 PreviousScore = 0;
	int32 DisplayedScoreDelta = 0;
	float ScoreDeltaDisplayEndTime = 0.0f;
	bool bHasInitializedScore = false;
};
