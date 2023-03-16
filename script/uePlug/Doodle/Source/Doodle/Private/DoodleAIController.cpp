#include "DoodleAIController.h"

#include "AI/NavigationSystemBase.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "DetourCrowdAIController.h"
#include "DoodleAiCrowd.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "NavigationSystem.h"
///@brief Actor子类AI控制器
ADoodleAIController::ADoodleAIController(
    const FObjectInitializer &ObjectInitializer
)
    : AAIController(
          ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(
              TEXT("PathFollowingComponent")
          )
      ) {
  // bAllowStrafe = true;
}
///@brief Actor子类控制器中的开始部件
void ADoodleAIController::BeginPlay() {
  CastChecked<UCrowdFollowingComponent>(GetPathFollowingComponent())->SetCrowdAvoidanceRangeMultiplier(1);
  ADoodleAiCrowd *DoodleCurveCrowd = Cast<ADoodleAiCrowd>(GetPawn());
  if (DoodleCurveCrowd && DoodleCurveCrowd->MoveTo) {
    GoToRandomWaypoint();
  }
}

void ADoodleAIController::GoToRandomWaypoint() {
  FVector Result;
  FVector In_Origin = GetPawn()->GetActorLocation();
  float Radius{600};
  ADoodleAiCrowd *DoodleCurveCrowd = Cast<ADoodleAiCrowd>(GetPawn());
  if (DoodleCurveCrowd) {
    In_Origin += DoodleCurveCrowd->Direction;
    Radius = DoodleCurveCrowd->Radius;
  }

  if (GetRandomPointInRadius(In_Origin, Radius, Result)) {
    FAIMoveRequest AIMoveRequest{Result};
    AIMoveRequest.SetAcceptanceRadius(50);
    AIMoveRequest.SetAllowPartialPath(true);

    MoveTo(AIMoveRequest);
  }
  GetWorldTimerManager().SetTimer(
      TimerHandle, this, &ADoodleAIController::GoToRandomWaypoint, 5.0f + FMath::RandRange(-3.0f, 2.0f), false
  );
}

bool ADoodleAIController::GetRandomPointInRadius(const FVector &Origin, float Radius, FVector &OutResult) {
  UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
  if (!NavSys) {
    return false;
  }

  FNavLocation Result;
  bool bSuccess = NavSys->GetRandomReachablePointInRadius(Origin, Radius, Result);

  // Out
  OutResult     = Result;

  return bSuccess;
}
void ADoodleAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult &Result) {
  Super::OnMoveCompleted(RequestID, Result);
  //  UAIBlueprintHelperLibrary::
  // UE_LOG(LogTemp, Warning, TEXT("ADoodleAIController::OnMoveCompleted"));
  // GoToRandomWaypoint();
}
