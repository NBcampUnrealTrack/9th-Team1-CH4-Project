#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OCSessionManager.generated.h"

UCLASS()
class OVERCOOKED_API UOCSessionManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "Overcooked|Session")
	void HostSession();

	UFUNCTION(BlueprintCallable, Category = "Overcooked|Session")
	void FindAndJoinSession();

private:
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FDelegateHandle CreateSessionCompleteHandle;
	FDelegateHandle FindSessionsCompleteHandle;
	FDelegateHandle JoinSessionCompleteHandle;

	void OnCreateSessionComplete(
		FName SessionName,
		bool bWasSuccessful
	);

	void OnFindSessionsComplete(
		bool bWasSuccessful
	);

	void OnJoinSessionComplete(
		FName SessionName,
		EOnJoinSessionCompleteResult::Type Result
	);
};