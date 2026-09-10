#include "OvercookedBaseWidget.h"

void UOvercookedBaseWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UOvercookedBaseWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}