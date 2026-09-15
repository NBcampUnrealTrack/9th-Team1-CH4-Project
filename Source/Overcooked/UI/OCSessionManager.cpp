#include "OCSessionManager.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"


void UOCSessionManager::Initialize()
{
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

    if (!OnlineSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("SESSION: OnlineSubsystem not found"));
        return;
    }

    SessionInterface = OnlineSubsystem->GetSessionInterface();

    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("SESSION: SessionInterface not valid"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("SESSION: Initialized"));
}

void UOCSessionManager::HostSession()
{
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("SESSION: Cannot host - invalid SessionInterface"));
        return;
    }

    FOnlineSessionSettings SessionSettings;

    SessionSettings.bIsLANMatch = true;
    SessionSettings.NumPublicConnections = 2;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bUsesPresence = false;

    CreateSessionCompleteHandle =
        SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
            FOnCreateSessionCompleteDelegate::CreateUObject(
                this,
                &UOCSessionManager::OnCreateSessionComplete
            )
        );

    const bool bStarted =
        SessionInterface->CreateSession(
            0,
            NAME_GameSession,
            SessionSettings
        );

    if (!bStarted)
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
            CreateSessionCompleteHandle
        );

        UE_LOG(LogTemp, Error, TEXT("SESSION: CreateSession failed to start"));
    }
}

void UOCSessionManager::OnCreateSessionComplete(
    FName SessionName,
    bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(
            CreateSessionCompleteHandle
        );
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("SESSION: CreateSession %s"),
        bWasSuccessful ? TEXT("SUCCESS") : TEXT("FAILED")
    );

    if (!bWasSuccessful)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("SESSION: Opening MenuMap as Listen Server"));

    UGameplayStatics::OpenLevel(
        World,
        FName(TEXT("/Game/Overcooked/Maps/MenuMap")),
        true,
        TEXT("listen")
    );
}
void UOCSessionManager::FindAndJoinSession()
{
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Error,
            TEXT("SESSION: Cannot Find - Interface invalid"));
        return;
    }

    SessionSearch = MakeShared<FOnlineSessionSearch>();

    SessionSearch->bIsLanQuery = true;
    SessionSearch->MaxSearchResults = 20;

    FindSessionsCompleteHandle =
        SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
            FOnFindSessionsCompleteDelegate::CreateUObject(
                this,
                &UOCSessionManager::OnFindSessionsComplete
            )
        );

    const bool bStarted =
        SessionInterface->FindSessions(
            0,
            SessionSearch.ToSharedRef()
        );

    if (!bStarted)
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
            FindSessionsCompleteHandle
        );

        UE_LOG(LogTemp, Error,
            TEXT("SESSION: FindSessions failed to start"));
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SESSION: Searching for LAN sessions..."));
    }
}

void UOCSessionManager::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
            FindSessionsCompleteHandle
        );
    }

    if (!bWasSuccessful || !SessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Error,
            TEXT("SESSION: FindSessions FAILED"));
        return;
    }
   
    UE_LOG(LogTemp, Warning,
        TEXT("SESSION: Found %d session(s)"),
        SessionSearch->SearchResults.Num());

    if (SessionSearch->SearchResults.Num() <= 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SESSION: No LAN sessions found"));
        return;
    }

    JoinSessionCompleteHandle =
        SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
            FOnJoinSessionCompleteDelegate::CreateUObject(
                this,
                &UOCSessionManager::OnJoinSessionComplete
            )
        );

    const bool bStarted =
        SessionInterface->JoinSession(
            0,
            NAME_GameSession,
            SessionSearch->SearchResults[0]
        );

    if (!bStarted)
    {
        SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
            JoinSessionCompleteHandle
        );

        UE_LOG(LogTemp, Error,
            TEXT("SESSION: JoinSession failed to start"));
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SESSION: Joining first LAN session..."));
    }
}
void UOCSessionManager::OnJoinSessionComplete(
    FName SessionName,
    EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid())
    {
        return;
    }

    SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(
        JoinSessionCompleteHandle
    );

    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("SESSION: JoinSession FAILED - Result=%d"),
            static_cast<int32>(Result)
        );
        return;
    }

    FString ConnectString;

    if (!SessionInterface->GetResolvedConnectString(
        SessionName,
        ConnectString))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("SESSION: Could not resolve connect string")
        );
        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("SESSION: Join SUCCESS - Address=%s"),
        *ConnectString
    );

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    APlayerController* PlayerController =
        World->GetFirstPlayerController();

    if (!PlayerController)
    {
        return;
    }

    PlayerController->ClientTravel(
        ConnectString,
        ETravelType::TRAVEL_Absolute
    );
}