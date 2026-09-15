#include "OCConnectionWidget.h"

#include "OCMenuPlayerController.h"
#include "OCMenuWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"

void UOCConnectionWidget::InitializeConnectionScreen(UOCMenuWidget* InSourceMenu)
{
	SourceMenu = InSourceMenu;
	SetVisibility(ESlateVisibility::Visible);
	if (AddressPanel)
	{
		AddressPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UOCConnectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Root;

	UImage* Background = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	if (UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Overcooked/UI/Images/Backgrounds/T_Menu_Background.T_Menu_Background")))
	{
		Background->SetBrushFromTexture(Texture);
	}
	UCanvasPanelSlot* BackgroundSlot = Root->AddChildToCanvas(Background);
	BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	BackgroundSlot->SetOffsets(FMargin(0.0f));

	UVerticalBox* CenterBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UCanvasPanelSlot* CenterSlot = Root->AddChildToCanvas(CenterBox);
	CenterSlot->SetAnchors(FAnchors(0.5f));
	CenterSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CenterSlot->SetAutoSize(true);

	UHorizontalBox* ChoiceRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	CenterBox->AddChildToVerticalBox(ChoiceRow);

	UButton* HostButton = CreateTextButton(FText::FromString(TEXT("호스트")));
	HostButton->OnClicked.AddDynamic(this, &UOCConnectionWidget::OnHostClicked);
	ChoiceRow->AddChildToHorizontalBox(HostButton);

	USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
	Spacer->SetSize(FVector2D(24.0f, 1.0f));
	ChoiceRow->AddChildToHorizontalBox(Spacer);

	UButton* JoinButton = CreateTextButton(FText::FromString(TEXT("참가하기")));
	JoinButton->OnClicked.AddDynamic(this, &UOCConnectionWidget::OnJoinClicked);
	ChoiceRow->AddChildToHorizontalBox(JoinButton);

	AddressPanel = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	AddressPanel->SetVisibility(ESlateVisibility::Collapsed);
	CenterBox->AddChildToVerticalBox(AddressPanel);

	AddressInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass());
	AddressInput->SetHintText(FText::FromString(TEXT("호스트 IP 주소 (예: 192.168.0.10)")));
	AddressInput->SetMinDesiredWidth(420.0f);
	AddressPanel->AddChildToVerticalBox(AddressInput);

	UButton* ConnectButton = CreateTextButton(FText::FromString(TEXT("접속")));
	ConnectButton->OnClicked.AddDynamic(this, &UOCConnectionWidget::OnConnectClicked);
	AddressPanel->AddChildToVerticalBox(ConnectButton);

	UButton* BackButton = CreateTextButton(FText::FromString(TEXT("뒤로")));
	BackButton->OnClicked.AddDynamic(this, &UOCConnectionWidget::OnBackClicked);
	AddressPanel->AddChildToVerticalBox(BackButton);
}

UButton* UOCConnectionWidget::CreateTextButton(const FText& Label)
{
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	Button->SetBackgroundColor(FLinearColor(0.12f, 0.45f, 0.75f, 1.0f));

	UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Text->SetText(Label);
	Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	Text->SetJustification(ETextJustify::Center);
	FSlateFontInfo Font = Text->GetFont();
	Font.Size = 30;
	Text->SetFont(Font);
	if (UButtonSlot* TextSlot = Cast<UButtonSlot>(Button->AddChild(Text)))
	{
		TextSlot->SetPadding(FMargin(48.0f, 18.0f));
	}
	return Button;
}

void UOCConnectionWidget::OnHostClicked()
{
	if (AOCMenuPlayerController* MenuPC = Cast<AOCMenuPlayerController>(GetOwningPlayer()))
	{
		MenuPC->HostGame();
	}
}

void UOCConnectionWidget::OnJoinClicked()
{
	if (AddressPanel)
	{
		AddressPanel->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOCConnectionWidget::OnConnectClicked()
{
	if (AOCMenuPlayerController* MenuPC = Cast<AOCMenuPlayerController>(GetOwningPlayer()))
	{
		MenuPC->JoinGameByAddress(AddressInput ? AddressInput->GetText().ToString() : FString());
	}
}

void UOCConnectionWidget::OnBackClicked()
{
	RemoveFromParent();
	if (SourceMenu)
	{
		SourceMenu->SetVisibility(ESlateVisibility::Visible);
	}
}
