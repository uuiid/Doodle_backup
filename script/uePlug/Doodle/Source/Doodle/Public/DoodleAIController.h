
#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"

//这个必须最后导入
#include "DoodleAIController.generated.h"

UCLASS()
class DOODLE_API ADoodleAIController : public AAIController
{
public:
    GENERATED_BODY()
    ADoodleAIController(
        const FObjectInitializer &ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay() override;

    virtual void OnMoveCompleted(FAIRequestID RequestID,
                                 const FPathFollowingResult &Result) override;

 
    void GoToRandomWaypoint();
    bool GetRandomPointInRadius(const FVector &Origin, float Radius, FVector &OutResult);

private:
    FTimerHandle TimerHandle;
};