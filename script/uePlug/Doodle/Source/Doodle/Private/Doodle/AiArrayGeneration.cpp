#include "Doodle/AiArrayGeneration.h"

#include "AI/NavigationSystemBase.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Doodle/DoodleEigenHelper.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NavigationSystem.h"
#include "UObject/ConstructorHelpers.h"

ADoodleAiArrayGeneration::ADoodleAiArrayGeneration() {
  PrimaryActorTick.bCanEverTick = true;
  SplineComponent               = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
  SplineComponent->Mobility     = EComponentMobility::Static;
  SplineComponent->SetGenerateOverlapEvents(false);
  SplineComponent->SetCanEverAffectNavigation(false);
  SplineComponent->SetClosedLoop(true);

  SplineComponent->SetSplinePoints(
      {FVector{100, 0, 0}, FVector{0, 100, 0}, FVector{-100, 0, 0}, FVector{0, -100, 0}}, ESplineCoordinateSpace::World
  );

  RootComponent = SplineComponent;
  SetHidden(true);
  SetCanBeDamaged(false);
  bCollideWhenPlacing          = true;
  SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
  Row                          = 10;
  Column                       = 10;

  SceneComponentTarget         = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
  SceneComponentTarget->SetupAttachment(RootComponent);
  SceneComponentTarget->SetRelativeLocation(FVector{200.0, 0.0, 0.0});

  static ConstructorHelpers::FObjectFinder<UStaticMesh> CraneBaseMesh(
      TEXT("/Engine/VREditor/BasicMeshes/SM_Cube_01.SM_Cube_01")
  );

  Target = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
  Target->SetupAttachment(SceneComponentTarget);
  Target->SetStaticMesh(CraneBaseMesh.Object);
#if WITH_EDITOR
  Target->SetIsVisualizationComponent(true);
#endif
  Target->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
  Target->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
  Target->SetCanEverAffectNavigation(false);
  Target->bHiddenInGame                = true;
  Target->CastShadow                   = false;

  Preview_InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedStaticMeshComponent");
  static ConstructorHelpers::FObjectFinder<UStaticMesh> Preview_InstancedStaticMeshComponent_Mesh(TEXT("/Niagara/DefaultAssets/S_Gnomon.S_Gnomon"));
  Preview_Mesh = Preview_InstancedStaticMeshComponent_Mesh.Object;
  Preview_InstancedStaticMeshComponent->SetStaticMesh(Preview_InstancedStaticMeshComponent_Mesh.Object);
#if WITH_EDITOR
  Preview_InstancedStaticMeshComponent->SetIsVisualizationComponent(true);
#endif
  Preview_InstancedStaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
  Preview_InstancedStaticMeshComponent->SetCanEverAffectNavigation(false);
  Preview_InstancedStaticMeshComponent->bHiddenInGame = true;
  Preview_InstancedStaticMeshComponent->CastShadow    = false;

  ClusterPointNum                                     = 5;
  ClusterIter                                         = 20;
}

void ADoodleAiArrayGeneration::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  if (!bCluster)
    if (Target_Transform != SceneComponentTarget->GetComponentTransform().GetLocation()) {
      for (auto i = 0; i < Points.Num(); ++i) {
        auto&& L_Tran = Points[i];
        L_Tran.SetRotation(GetRandomOrient(L_Tran.GetLocation()));
        Preview_InstancedStaticMeshComponent->UpdateInstanceTransform(i, L_Tran, true, true, true);
      }
      Target_Transform = SceneComponentTarget->GetComponentTransform().GetLocation();
    }
}

void ADoodleAiArrayGeneration::PostActorCreated() {
  Super::PostActorCreated();
  UMaterial* L_CraneBaseMaterial =
      LoadObject<UMaterial>(this, TEXT("/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01"));
  UMaterialInstanceDynamic* L_Material = UMaterialInstanceDynamic::Create(L_CraneBaseMaterial, this);
  L_Material->SetVectorParameterValue(TEXT("Color"), FColor::Red);
  Target->SetMaterial(0, L_Material);
  // GenPoint();
}

bool ADoodleAiArrayGeneration::ShouldTickIfViewportsOnly() const {
  return true;
}

void ADoodleAiArrayGeneration::BeginPlay() {
  if (AnimAssets.Num() == 0 || SkinAssets.Num() == 0)
    return;

  int32 L_Max_Anim = FMath::Max(0, AnimAssets.Num() - 1);
  int32 L_Max_Skin = FMath::Max(0, SkinAssets.Num() - 1);
  for (auto&& i : Points) {
    ASkeletalMeshActor* L_Actor      = GetWorld()->SpawnActor<ASkeletalMeshActor>(i.GetLocation(), i.GetRotation().Rotator());
    // L_Actor->SetActorTransform(i);
    UAnimationAsset* L_Anim          = AnimAssets[RandomStream_Anim.RandRange(0, L_Max_Anim)];
    USkeletalMesh* L_Skin            = SkinAssets[RandomStream_Skin.RandRange(0, L_Max_Skin)];
    USkeletalMeshComponent* L_Sk_Com = L_Actor->GetSkeletalMeshComponent();
    L_Sk_Com->SetSkeletalMesh(L_Skin);
    L_Sk_Com->PlayAnimation(L_Anim, true);
    // L_Sk_Com->LightingChannels = LightingChannels;
    L_Sk_Com->SetLightingChannels(LightingChannels.bChannel0, LightingChannels.bChannel1, LightingChannels.bChannel2);

    L_Sk_Com->AnimationData.SavedPlayRate = ((float)RandomStream_Anim_Rate.RandRange(950, 1050)) / 1000;
  }
}

void ADoodleAiArrayGeneration::GenPoint() {
  UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
  if (!NavSys) {
    return;
  }
  if (NavSys->MainNavData == nullptr)
    return;
  // RandomStream.Initialize();
  // RandomStream.RandRange(0, 10);
  Points.Empty(Row * Column);
  Preview_InstancedStaticMeshComponent->ClearInstances();
  Preview_InstancedStaticMeshComponent->SetStaticMesh(Preview_Mesh);

  FBox L_Box_                  = SplineComponent->Bounds.GetBox();
  FVector L_Row_Step           = L_Box_.GetSize();
  float L_Row_Step_X           = L_Row_Step.X / FMath::Clamp((double)Row, 1.0, 1000.0);
  float L_Row_Step_Y           = L_Row_Step.Y / FMath::Clamp((double)Column, 1.0, 1000.0);
  float L_Max                  = L_Row_Step.GetAbsMax() / FMath::Clamp((double)FMath::Max(Row, Column), 1.0, 1000.0);

  const FVector L_Preview_Size = FVector{L_Max, L_Max, L_Max} / 20;
  Target->SetRelativeScale3D(L_Preview_Size / 10);
  // UE_LOG(LogTemp, Log, TEXT("L_Preview_Size %s, L_Max %f"), *L_Preview_Size.ToString(), L_Max);

  // DrawDebugLine(GetWorld(), GetActorTransform().GetLocation(), GetActorTransform().GetLocation() + L_Vector * 50,
  // FColor::Red, false, 10.f);
  // DrawDebugBox(GetWorld(), L_Box_.GetCenter(), L_Box_.GetExtent(), FColor::Red, false, 10.f);
  for (auto x = 0.0f; x <= FMath::Clamp(Row, 1, 1000); ++x) {
    for (auto y = 0.0f; y <= FMath::Clamp(Column, 1, 1000); ++y) {
      FVector L_Point       = L_Box_.Min + FVector{L_Row_Step_X * x, L_Row_Step_Y * y, 0};
      const float L_Param   = SplineComponent->FindInputKeyClosestToWorldLocation(L_Point);
      FVector L_RightVector = SplineComponent->GetRightVectorAtSplineInputKey(L_Param, ESplineCoordinateSpace::World);
      FVector2D L_RightVector_2D{L_RightVector.X, L_RightVector.Y};
      L_RightVector_2D.Normalize();

      FVector L_Vector = SplineComponent->GetLocationAtSplineInputKey(L_Param, ESplineCoordinateSpace::World) - L_Point;
      FVector2D L_Vector_2D{L_Vector.X, L_Vector.Y};
      L_Vector_2D.Normalize();

      // DrawDebugLine(GetWorld(), FVector{L_Vector_2D, 0}, FVector{L_RightVector_2D * 100, 0}, FColor::Red,
      // false, 1.0f);
      if (FVector2D::DotProduct(L_Vector_2D, L_RightVector_2D) < 0.0) {
        FHitResult L_HitR{};
        if (UKismetSystemLibrary::LineTraceSingleForObjects(
                GetWorld(), L_Point + FVector{0, 0, 1000}, L_Point - FVector{0, 0, 1000},
                {UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)}, false, {},
                EDrawDebugTrace::None, L_HitR, true
            )) {
          FVector L_Point_Out{};
          // DrawDebugPoint(GetWorld(), L_HitR.ImpactPoint, 10.0f, FColor::Red, false, 1.0f);
          if (GetRandomPointInRadius(L_HitR.ImpactPoint, L_Point_Out)) {
            L_Point_Out.Z += OffsetValue;
            FTransform L_Ftran{GetRandomOrient(L_Point_Out), L_Point_Out};
            Points.Add(L_Ftran);

            Preview_InstancedStaticMeshComponent->AddInstanceWorldSpace(L_Ftran);
            // DrawDebugPoint(GetWorld(), L_Point_Out, 10.0f, FColor::Green, false, 1.0f);
          }
        }

        // DrawDebugPoint(GetWorld(), L_Point, 10.0f, FColor::Red, false, 1.0f);
      }
    }
  }
}

FQuat ADoodleAiArrayGeneration::GetRandomOrient(const FVector& In_Origin) {
  return GetRandomOrient(In_Origin, SceneComponentTarget->GetComponentTransform().GetLocation());
}

FQuat ADoodleAiArrayGeneration::GetRandomOrient(const FVector& In_Origin, const FVector& In_Look) {
  static constexpr float Pi = 3.1415926535897932384626433832795;
  FQuat L_R{FQuat::Identity};

  FVector L_Origin{In_Origin.X, In_Origin.Y, 0.0};
  FVector L_Loc = In_Look;
  L_Loc         = {L_Loc.X, L_Loc.Y, 0};
  L_Loc -= L_Origin;

  FQuat G_R{FVector::UpVector, -0.5 * Pi};

  G_R *= FQuat{FVector::UpVector, (((float)RandomStream_Orient.RandRange(RandomOrient.X * 100, RandomOrient.Y * 100)) / 100) * Pi};

  return G_R * L_Loc.ToOrientationQuat();
}

bool ADoodleAiArrayGeneration::GetRandomPointInRadius(const FVector& Origin, FVector& OutResult) {
  UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
  if (!NavSys) {
    return false;
  }

  FNavLocation Result;
  bool bSuccess = NavSys->GetRandomPointInNavigableRadius(Origin, RandomRadius, Result);
  // Out
  OutResult     = Result;

  return bSuccess;
}

void ADoodleAiArrayGeneration::OnConstruction(const FTransform& Transform) {
  Super::OnConstruction(Transform);
  // if (SplineComponent->bSplineHasBeenEdited)
  GenPoint();
  if (bCluster)
    K_Means_Clustering();

  Preview_InstancedStaticMeshComponent->SetLightingChannels(LightingChannels.bChannel0, LightingChannels.bChannel1, LightingChannels.bChannel2);
}

void ADoodleAiArrayGeneration::K_Means_Clustering() {
}

#if WITH_EDITOR
void ADoodleAiArrayGeneration::PostEditChangeProperty(
    FPropertyChangedEvent& PropertyChangeEvent
) {
  Super::PostEditChangeProperty(PropertyChangeEvent);
  // FName name2 = PropertyChangeEvent.GetPropertyName();
  // FName name  = PropertyChangeEvent.MemberProperty ? PropertyChangeEvent.MemberProperty->GetFName() : NAME_None;
  // UE_LOG(LogTemp, Log, TEXT("chick name: %s MemberProperty: %s"), *name2.ToString(), *name.ToString());
}
#endif  // WITH_EDITOR