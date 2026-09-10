// 프로젝트 설정의 Description 페이지에서 저작권 문구를 설정할 수 있습니다.

#pragma once

#include "CoreMinimal.h"
#include "OverPickupItem.h"
#include "OverCabbageItem.generated.h"

/** 도마에서 썰 수 있는 양배추 재료입니다. */
UCLASS(Blueprintable)
class OVERCOOKED_API AOverCabbageItem : public AOverPickupItem
{
	GENERATED_BODY()

public:
	// 양배추 원본 메시와 썰기 완료 후 사용할 잎채소 메시를 지정합니다.
	AOverCabbageItem();
};
