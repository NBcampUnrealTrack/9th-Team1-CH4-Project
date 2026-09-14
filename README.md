# 담당: 게임모드·코어

## 구현 완료

- Listen Server 호스트 및 IP 접속
- 최대 4인 접속 제한
- 최소 2인 접속 시 라운드 자동 시작
- 플레이어 슬롯 0~3 서버 배정
- `PlayerStart1`~`PlayerStart4` 고정 스폰
- 캐릭터 이동 서버 동기화
- 고정 탑뷰 카메라 공유
- 경기 상태, 남은 시간, 점수, 결과 동기화
- 카운트다운, 라운드 시작·종료, 결과 이벤트

## 맵 설정

- `PlayerStart` 태그: `PlayerStart1`~`PlayerStart4`
- `OCSharedCameraActor` 1개 배치

## 연동 함수

### 네트워크

- `HostListenServer(MapPackagePath, MaxPlayers)`
- `JoinListenServer(ServerAddress)`
- `GetPlayMode()`
- `GetMaximumNetworkPlayers()`

### 게임모드

- `StartRound()`
- `AddScore(ScoreDelta)`
- `FinishRound()`
- `GetOCGameState()`

### 게임 상태

- `GetMatchPhase()`
- `GetRemainingTime()`
- `GetCurrentScore()`
- `GetMatchResult()`
- `GetParticipatingPlayerCount()`

### 플레이어

- `GetPlayerSlotIndex()`

## 델리게이트

- `OnPlayModeChanged`
- `OnMatchPhaseChanged`
- `OnRemainingTimeChanged`
- `OnScoreChanged`
- `OnMatchResultReady`
- `OnParticipatingPlayerCountChanged`
- `OnPlayerSlotChanged`
