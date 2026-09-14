// 프로젝트 설정의 Description 페이지에서 저작권 문구를 설정할 수 있습니다.

#pragma once

#include "CoreMinimal.h"
#include "OverPickupItem.h"
#include "OverLettuceItem.generated.h"

/** 도마에서 썰 수 있는 양상추 재료입니다. */
UCLASS(Blueprintable)
class OVERCOOKED_API AOverLettuceItem : public AOverPickupItem
{
	GENERATED_BODY()

public:
	// 양상추 원본 메시와 썰린 양상추 메시를 지정합니다.
	AOverLettuceItem();
};
