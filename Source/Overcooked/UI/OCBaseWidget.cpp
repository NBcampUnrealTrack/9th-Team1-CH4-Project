#include "OCBaseWidget.h"

void UOCBaseWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UOCBaseWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}