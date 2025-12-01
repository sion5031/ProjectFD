## 프로젝트 개요
- 목적 : 언리얼 엔진을 활용한 멀티 플레이 루터슈터(TPS + RPG) 장르 구현
- 원작 : The FirstDescendant(2024)
- 개발 인원: 3인
- 개발 기간: 2025년 9월 1일 ~ 11월 7일


## 핵심 구현 내용
- 서상원(팀장) : 인벤토리 및 아이템, UI, 서버 환경 구축
- 이기호 : 캐릭터 기본 조작 및 기능 구현
- 김주신 : 보스 및 기본 몬스터 구현


## 기술 스택
**Core System**
- Dedicated Server 구성
- Replication(Properties & RPC)
- Seamless Travel(Lobby <-> In-Game)
- Server Travel
- PlayerState 복제 및 Custom Data Sync

**Gameplay Architecture**
- Gameplay Ability System(GAS)
  - GameplayEffect / AbilitySpec
  - AttributeSet Customization
- GameMode, PlayerState Custom Class 작성
- Enhanced Input System 적용
- Component 기반 설계
- FastArraySerializer 기반 인벤토리 설계

**Build & Deployment**
- Packaging(Shipping / Development)
- Config 관리(DefaultGame.ini, Engine.ini)

**Tools & Workflow**
- Perforce
- Notion


## 결과 및 성과(서상원)
- 시나리오 : 매칭 -> 전투 -> 보상(성장) -> 매칭
- 인벤토리 : 무기, 모듈 인벤토리 구현
- UI : 인벤토리, HUD 구현


## Contact
- 서상원 swseo97@naver.com
- 이기호 crystalwing@gmail.com
- 김주신 powe556@gamil.com
