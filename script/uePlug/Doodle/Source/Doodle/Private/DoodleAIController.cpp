#include "DoodleAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "DoodleCurveCrowd.h"

ADoodleAIController::ADoodleAIController(
    const FObjectInitializer &ObjectInitializer)
    : AAIController(
          ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(
              TEXT("PathFollowingComponent")))
{
}

void ADoodleAIController::BeginPlay()
{
    CastChecked<UCrowdFollowingComponent>(GetPathFollowingComponent())->SetCrowdAvoidanceRangeMultiplier(1);

    GoToRandomWaypoint();
}

void ADoodleAIController::GoToRandomWaypoint()
{
    FVector Result;
    GetRandomPointInRadius(GetPawn()->GetActorLocation(), 600, Result);
    if (true)
    {
        FAIMoveRequest AIMoveRequest{Result};
        AIMoveRequest.SetAcceptanceRadius(50);
        AIMoveRequest.SetAllowPartialPath(true);

        MoveTo(AIMoveRequest);
    }
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ADoodleAIController::GoToRandomWaypoint, 5.0f + FMath::RandRange(-2.0f, 3.0f), false);
}

bool ADoodleAIController::GetRandomPointInRadius(const FVector &Origin, float Radius, FVector &OutResult)
{
    UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
    {
        return false;
    }

    FNavLocation Result;
    bool bSuccess = NavSys->GetRandomReachablePointInRadius(Origin, 600, Result);

    // Out
    OutResult = Result;
    ADoodleCurveCrowd *DoodleCurveCrowd = Cast<ADoodleCurveCrowd>(GetPawn());
    if (DoodleCurveCrowd)
    {
        OutResult += DoodleCurveCrowd->Direction;
    }

    return bSuccess;
}
void ADoodleAIController::OnMoveCompleted(FAIRequestID RequestID,
                                          const FPathFollowingResult &Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    //  UAIBlueprintHelperLibrary::
    // UE_LOG(LogTemp, Warning, TEXT("ADoodleAIController::OnMoveCompleted"));
    // GoToRandomWaypoint();
}