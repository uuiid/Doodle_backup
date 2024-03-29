#include "DoodleAiSplineMoveToCom.h"

#include "AI/NavigationSystemBase.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"  // 蓝图ai帮助
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "DetourCrowdAIController.h"
#include "DoodleAIController.h"
#include "DoodleAiCrowd.h"
#include "DrawDebugHelpers.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "NavigationSystem.h"

#include "Kismet/KismetMathLibrary.h"  // 数学库
UDoodleAiSplineMoveToComponent::UDoodleAiSplineMoveToComponent() {
  PrimaryComponentTick.bCanEverTick = true;

  SplineCurve                       = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void UDoodleAiSplineMoveToComponent::BeginPlay() {
  Super::BeginPlay();

  // GoToRandomWaypoint();
  //  GetDistance() / ;

  if (!Actor || !AiController) {
    Actor = GetOwner<AActor>();
    if (!Actor) {
      UE_LOG(LogTemp, Log, TEXT("return Actor"));
      return;
    }
    AiController = Cast<ADoodleAIController>(UAIBlueprintHelperLibrary::GetAIController(Actor));
  }
  if (!Actor || !AiController) {
    UE_LOG(LogTemp, Log, TEXT("return Actor AiController"));
    return;
  }
  UWorld *L_World = GetWorld();
  if (!L_World) return;

  SpeedMax = GetOwner<APawn>()->FindComponentByClass<UMovementComponent>()->GetMaxSpeed();
}

void UDoodleAiSplineMoveToComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
  if (Actor && AiController) {
    FVector Result;
    FVector L_Loc = Actor->GetActorLocation();
    // do {
    if (!GetRandomPointInRadius(Actor->GetActorLocation(), Result)) {
      return;
    }
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(L_Loc, Result);
    LookAtRotation.Pitch    = 0.;
    LookAtRotation.Pitch    = 0.;
    FRotator InterpRotation = FMath::RInterpTo(Actor->GetActorRotation(), LookAtRotation, DeltaTime, 10.f);
    // FMath::Inerpto

    Actor->SetActorRotation(InterpRotation);

    UAIBlueprintHelperLibrary::SimpleMoveToLocation(AiController, Result);
  }
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// void UDoodleAiSplineMoveToComponent::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult) {
//   FVector Result;
//   if (!GetRandomPointInRadius(Actor->GetActorLocation(), Result)) {
//     return;
//   }
//   if (MovementResult == EPathFollowingResult::Success)
//     ++TimeToPoint;
//
//   if (TimeToPoint >= GetNumberOfSplinePoints()) TimeToPoint -= GetNumberOfSplinePoints();
//
//   FAIMoveRequest AIMoveRequest{Result};
//   AIMoveRequest.SetAcceptanceRadius(AcceptanceRadius);
//   AIMoveRequest.SetAllowPartialPath(true);
//
//   FPathFollowingRequestResult L_R = AiController->MoveTo(AIMoveRequest);
// }

void UDoodleAiSplineMoveToComponent::GoToRandomWaypoint() {
  // for (auto i = 1; i < SplineCurve->GetNumberOfSplinePoints(); ++i) {
  //   DrawDebugLine(GetWorld(), SplineCurve->GetWorldLocationAtSplinePoint(i - 1), SplineCurve->GetWorldLocationAtSplinePoint(i), FColor::Green, false, 1.0f);
  // }
  if (!Actor || !AiController) {
    Actor = GetOwner<AActor>();
    if (!Actor) {
      UE_LOG(LogTemp, Log, TEXT("return Actor"));
      return;
    }
    AiController = Cast<ADoodleAIController>(UAIBlueprintHelperLibrary::GetAIController(Actor));
  }
  if (!Actor || !AiController) {
    UE_LOG(LogTemp, Log, TEXT("return Actor AiController"));
    return;
  }
  UWorld *L_World = GetWorld();
  if (!L_World) return;

  FVector Result;

  TEnumAsByte<EPathFollowingRequestResult::Type> L_Code{};
  int L_TEST_Size{};
  // do {
  if (!GetRandomPointInRadius(Actor->GetActorLocation(), Result)) {
    return;
  }
  FAIMoveRequest AIMoveRequest{Result};
  AIMoveRequest.SetAcceptanceRadius(AcceptanceRadius);
  AIMoveRequest.SetAllowPartialPath(true);

  UAIBlueprintHelperLibrary::SimpleMoveToLocation(AiController, Result);
  // FPathFollowingRequestResult L_R = AiController->MoveTo(AIMoveRequest);
  // L_Code                          = L_R.Code;
  //++L_TEST_Size;
  //} while (L_TEST_Size < 100 && L_Code != EPathFollowingRequestResult::RequestSuccessful);
  L_World->GetTimerManager().SetTimer(
      TimerHandle, this, &UDoodleAiSplineMoveToComponent::GoToRandomWaypoint, 0.1f /*+ FMath::RandRange(-0.1f, 0.2f)*/,
      false
  );

  // switch (L_R.Code) {
  //   case EPathFollowingRequestResult::AlreadyAtGoal:
  //     ++TimeToPoint;
  //     L_World->GetTimerManager().SetTimer(
  //         TimerHandle, this, &UDoodleAiSplineMoveToComponent::GoToRandomWaypoint, 0.1f, false
  //     );
  //     break;
  //   case EPathFollowingRequestResult::Failed:
  //     L_World->GetTimerManager().SetTimer(
  //         TimerHandle, this, &UDoodleAiSplineMoveToComponent::GoToRandomWaypoint, 0.15f + FMath::RandRange(-0.1f,
  //         0.2f), false
  //     );
  //     break;
  //   case EPathFollowingRequestResult::RequestSuccessful:
  //     L_World->GetTimerManager().SetTimer(
  //         TimerHandle, this, &UDoodleAiSplineMoveToComponent::GoToRandomWaypoint, 0.15f + FMath::RandRange(-0.1f,
  //         0.2f), false
  //     );
  //     // AiController->ReceiveMoveCompleted.AddDynamic(this, &UDoodleAiSplineMoveToComponent::OnMoveCompleted);
  //     break;
  // }
}

void UDoodleAiSplineMoveToComponent::ReplaceSplineCurve(const TObjectPtr<USplineComponent> &In_Tmp, const FTransform &In_Loc) {
  USceneComponent *L_Parent = SplineCurve->GetAttachParent();
  SplineCurve->UnregisterComponent();
  SplineCurve->DestroyComponent();

  // SplineCurve = DuplicateObject<USplineComponent>(In_Tmp, this);
  SplineCurve = NewObject<USplineComponent>(this, NAME_None, RF_NoFlags, In_Tmp);
  SplineCurve->SetAbsolute(true, true, true);
  SplineCurve->SetWorldTransform(In_Loc);
  SplineCurve->SetMobility(EComponentMobility::Type::Movable);
  SplineCurve->RegisterComponent();
  SplineCurve->AttachToComponent(L_Parent, FAttachmentTransformRules::KeepWorldTransform);
  SplineCurve->UpdateSpline();
  // FTransform L_Tran = SplineCurve->GetComponentTransform();
  // UE_LOG(LogTemp, Log, TEXT("tran  %s"), *L_Tran.ToString());
  GoToRandomWaypoint();
}

bool UDoodleAiSplineMoveToComponent::GetRandomPointInRadius(const FVector &Origin, FVector &OutResult) {
  UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
  if (!NavSys) {
    return false;
  }
  FVector L_Actor_Loc = Actor->GetActorLocation();

  // FVector L_Forward   = SplineCurve->FindTangentClosestToWorldLocation(L_Actor_Loc, ESplineCoordinateSpace::World);
  float L_Key         = SplineCurve->FindInputKeyClosestToWorldLocation(L_Actor_Loc);
  float L_Nums        = SplineCurve->GetNumberOfSplinePoints();
  // L_Key               = (L_Nums - L_Key) > 0.2 ? L_Key : 0;

  L_Key += SpeedMax * 10 / SplineCurve->GetSplineLength();
  L_Key     = L_Key > L_Nums ? (L_Key - L_Nums) : L_Key;
  // UE_LOG(LogTemp, Log, TEXT("index key  %f"), L_Key);
  //  L_Forward *= 3.f;
  OutResult = SplineCurve->GetLocationAtSplineInputKey(L_Key, ESplineCoordinateSpace::World);
  // L_Forward = L_Actor_Loc + L_Forward;
  //  FVector L_point = GetLocationAtSplinePoint(TimeToPoint, ESplineCoordinateSpace::World);
  //  UE_LOG(LogTemp, Log, TEXT("index  %s"), *L_Forward.ToString());
  //   FNavLocation Result;
  //   bool bSuccess = NavSys->GetRandomReachablePointInRadius(L_point, RandomRadius, Result);
  //    Out
  // OutResult = SplineCurve->FindLocationClosestToWorldLocation(L_Forward, ESplineCoordinateSpace::World);
  // DrawDebugPoint(GetWorld(), OutResult, 10, FColor::Red, false, 1.0f);
  //DrawDebugSphere(GetWorld(), OutResult, 10.f, 3, FColor::Red, false, 1.0f);

  return true;
}
